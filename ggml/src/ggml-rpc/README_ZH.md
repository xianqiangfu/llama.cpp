# ggml-rpc - RPC 后端

## 概述

ggml-rpc 是基于远程过程调用 (Remote Procedure Call) 的 GGML 后端实现，允许通过网络在远程服务器上执行 GGML 操作。这支持分布式计算和设备卸载场景。

## 文件结构

```
ggml/src/ggml-rpc/
├── CMakeLists.txt          # CMake 构建配置
├── ggml-rpc.cpp            # RPC 后端主实现
└── transport.h             # 传输层抽象接口
```

## 实现原理

### 架构设计

RPC 后端采用客户端-服务器架构：

```
客户端                          服务器
  │                              │
  ├─ 连接服务器                  ├─ 监听端口
  │                              │
  ├─ 序列化张量                  ├─ 反序列化张量
  │                              │
  ├─ 发送 RPC 请求               ├─ 执行 GGML 操作
  │                              │
  └─ 接收 RPC 响应               └─ 发送响应
```

### 核心组件

#### RPC 协议

RPC 命令类型：

```cpp
enum rpc_cmd {
    RPC_CMD_ALLOC_BUFFER = 0,
    RPC_CMD_GET_ALIGNMENT,
    RPC_CMD_GET_MAX_SIZE,
    RPC_CMD_BUFFER_GET_BASE,
    RPC_CMD_FREE_BUFFER,
    RPC_CMD_BUFFER_CLEAR,
    RPC_CMD_SET_TENSOR,
    RPC_CMD_SET_TENSOR_HASH,  // 哈希优化传输
    RPC_CMD_GET_TENSOR,
    RPC_CMD_COPY_TENSOR,
    RPC_CMD_GRAPH_COMPUTE,
    RPC_CMD_GET_DEVICE_MEMORY,
    RPC_CMD_INIT_TENSOR,
    RPC_CMD_GET_ALLOC_SIZE,
    RPC_CMD_HELLO,             // 握手
    RPC_CMD_DEVICE_COUNT,
    RPC_CMD_GRAPH_RECOMPUTE,   // 图重计算
    RPC_CMD_COUNT,
};
```

#### 张量序列化

```cpp
struct rpc_tensor {
    uint64_t id;
    uint32_t type;
    uint64_t buffer;
    uint32_t ne[GGML_MAX_DIMS];
    uint32_t nb[GGML_MAX_DIMS];
    uint32_t op;
    int32_t  op_params[GGML_MAX_OP_PARAMS / sizeof(int32_t)];
    int32_t  flags;
    uint64_t src[GGML_MAX_SRC];
    uint64_t view_src;
    uint64_t view_offs;
    uint64_t data;
    char name[GGML_MAX_NAME];
};
```

#### 传输层

```cpp
struct socket_t {
    ~socket_t();

    bool send_data(const void * data, size_t size);
    bool recv_data(void * data, size_t size);

    socket_ptr accept();
    static socket_ptr create_server(const char * host, int port);
    static socket_ptr connect(const char * host, int port);
};
```

### 通信流程

#### 握手协议

```cpp
static bool negotiate_hello(
    const std::shared_ptr<socket_t> & sock
) {
    rpc_msg_hello_req request = {};
    rpc_msg_hello_rsp response = {};

    sock->get_caps(request.conn_caps);

    bool status = send_rpc_cmd(
        sock, RPC_CMD_HELLO,
        &request, sizeof(request),
        &response, sizeof(response)
    );

    if (response.major != RPC_PROTO_MAJOR_VERSION ||
        response.minor > RPC_PROTO_MINOR_VERSION) {
        GGML_LOG_ERROR("RPC server version mismatch\n");
        return false;
    }

    sock->update_caps(response.conn_caps);
    return true;
}
```

#### RPC 请求格式

```
| rpc_cmd (1 byte) | request_size (8 bytes) | request_data (request_size bytes) |
```

#### RPC 响应格式

```
| response_size (8 bytes) | response_data (response_size bytes) |
```

### 性能优化

#### 哈希优化传输

对于大数据（>10MB），使用 FNV-1a 哈希避免重复传输：

```cpp
const size_t HASH_THRESHOLD = 10 * 1024 * 1024;

if (size > HASH_THRESHOLD) {
    uint64_t hash = fnv_hash((const uint8_t*)data, size);
    rpc_msg_set_tensor_hash_rsp response;
    bool status = send_rpc_cmd(sock, RPC_CMD_SET_TENSOR_HASH, &request, sizeof(request), &response, sizeof(response));
    
    if (response.result) {
        // 服务器已有相同数据，无需传输
        return;
    }
}
```

#### 图重计算

对于重复的图，使用重计算命令避免重复传输：

```cpp
bool reuse = cgraph->uid != 0 && rpc_dev_ctx->last_graph_uid == cgraph->uid;
if (reuse) {
    rpc_msg_graph_recompute_req request;
    request.device = rpc_ctx->device;
    send_rpc_cmd(sock, RPC_CMD_GRAPH_RECOMPUTE, &request, sizeof(request));
} else {
    rpc_dev_ctx->last_graph_uid = cgraph->uid;
    serialize_graph(rpc_ctx->device, cgraph, input);
    send_rpc_cmd(sock, RPC_CMD_GRAPH_COMPUTE, input.data(), input.size());
}
```

### 缓存机制

#### 服务器缓存

```cpp
static bool get_cached_file(uint64_t hash, std::vector<uint8_t> & data) {
    if (!cache_dir) return false;
    
    char hash_str[17];
    snprintf(hash_str, sizeof(hash_str), "%016" PRIx64, hash);
    
    fs::path cache_file = fs::path(cache_dir) / hash_str;
    
    std::ifstream ifs(cache_file, std::ios::binary);
    if (!ifs) return false;
    
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    data.resize(size);
    ifs.read((char *)data.data(), size);
    return true;
}
```

#### 连接池

```cpp
static std::unordered_map<std::string, std::weak_ptr<socket_t>> sockets;

static std::shared_ptr<socket_t> get_socket(const std::string & endpoint) {
    auto it = sockets.find(endpoint);
    if (it != sockets.end()) {
        if (auto sock = it->second.lock()) {
            return sock;
        }
    }
    // 创建新连接
    auto sock = socket_t::connect(host.c_str(), port);
    sockets[endpoint] = sock;
    return sock;
}
```

### 传输能力

```cpp
static constexpr size_t RPC_CONN_CAPS_SIZE = 24;

struct socket_t {
    void get_caps(uint8_t * local_caps);
    void update_caps(const uint8_t * remote_caps);
};
```

支持的功能协商：
- 基础传输
- RDMA (可选)
- 压缩 (可选)

## 使用方法

### 服务器启动

```bash
# 启动 RPC 服务器
./llama-server \
    --host 0.0.0.0 \
    --port 8080 \
    --backend cuda \
    --n-gpu 1

# 带缓存目录的启动
./llama-server \
    --port 8080 \
    --rpc-cache-dir /tmp/rpc-cache \
    --n-threads 4
```

### 客户端连接

```bash
# 连接到 RPC 服务器
./llama-cli \
    --model model.gguf \
    --backend rpc \
    --endpoint localhost:8080 \
    --device 0
```

### 多设备支持

```bash
# 服务器支持多设备
./llama-server --n-gpu 2 --port 8080

# 客户端指定设备
./llama-cli --endpoint localhost:8080 --device 0  # 使用第一个 GPU
./llama-cli --endpoint localhost:8080 --device 1  # 使用第二个 GPU
```

### 环境变量

| 变量 | 说明 |
|------|------|
| `GGML_RPC_DEBUG` | 启用调试日志 |
| `GGML_RPC_CACHE_DIR` | 缓存目录路径 |

## 性能调优

### 批处理

```bash
# 服务器启用批处理
./llama-server --n-gpu 1 --n-server 4
```

### 缓存策略

```bash
# 使用缓存减少传输
./llama-server --rpc-cache-dir /path/to/cache

# 清理旧缓存
find /path/to/cache -mtime +7 -delete
```

### 网络优化

1. **使用 RDMA**: 减少延迟和 CPU 开销
2. **启用压缩**: 减少网络带宽使用
3. **连接池**: 重用连接避免握手开销

## 限制

- 网络延迟影响性能
- 依赖网络稳定性
- 序列化/反序列化开销
- 需要同步服务端和客户端版本

## 适用场景

1. **分布式计算**: 多个客户端共享计算资源
2. **设备卸载**: 本地设备无法运行，远程服务器执行
3. **资源共享**: 高性能服务器集群
4. **边缘计算**: 边缘设备连接到云服务器

## 相关链接

- [RPC 协议](https://en.wikipedia.org/wiki/Remote_procedure_call)
- [TCP/IP 编程](https://beej.us/guide/bgnet/html/)