# ggml-hexagon - Hexagon DSP 后端

## 概述

ggml-hexagon 是基于 Qualcomm Hexagon DSP 的 GGML 后端实现，专为在移动设备和嵌入式系统中运行 AI 模型而设计。Hexagon 是 Qualcomm 的数字信号处理器架构，提供高效的向量计算能力。

## 文件结构

```
ggml/src/ggml-hexagon/
├── CMakeLists.txt          # CMake 构建配置
├── ggml-hexagon.cpp        # Hexagon 后端主实现文件
├── htp-drv.cpp            # Hexagon TSP 驱动接口实现
├── htp-drv.h              # Hexagon TSP 驱动接口头文件
├── libdl.h                # 动态加载库抽象接口
├── libggml-htp.inf        # Windows 驱动安装配置
└── op-desc.h              # 操作描述符定义
```

## 实现原理

### 架构设计

Hexagon 后端采用分层架构：

1. **驱动层 (htp-drv)**: 提供与 Qualcomm Hexagon TSP (Tensor/Spatial Processor) 的底层通信
2. **缓冲层**: 管理 DSP 与主处理器之间的共享内存
3. **操作层**: 将 GGML 操作映射到 Hexagon DSP 操作

### 核心特性

#### 共享缓冲区管理

- 使用 `rpcmem_alloc2` 分配共享内存
- 通过 `fastrpc_mmap` 映射到 DSP 地址空间
- 支持固定和延迟映射模式

#### 量化支持

支持多种量化格式并自动转换：

- **Q4_0x4x2**: 4位量化，专为 Hexagon HVX 优化
- **Q8_0x4x2**: 8位量化
- **MXFP4x4x2**: 混合精度浮点4位格式

转换函数包括：
- `repack_q4_0_q4x4x2()`: Q4_0 格式转换为 Q4_0x4x2
- `repack_q8_0_q8x4x2()`: Q8_0 格式转换为 Q8_0x4x2
- `repack_mxfp4_mxfp4x4x2()`: MXFP4 格式转换

#### 操作批处理

- **opbatch**: 将多个操作分组为批次
- **opqueue**: 管理操作队列，支持异步执行
- 支持操作批量和深度的可配置参数

#### 会话管理

```cpp
struct ggml_hexagon_session {
    std::string      name;
    remote_handle64  handle;      // Hexagon 会话句柄
    dspqueue_t       queue;       // DSP 队列
    uint32_t         session_id;
    uint32_t         domain_id;
    uint64_t         queue_id;
    int              dev_id;
    std::atomic<int> op_pending;  // 待处理操作计数
};
```

### 后端接口实现

#### 缓冲区接口

```cpp
static ggml_backend_buffer_i ggml_backend_hexagon_buffer_interface = {
    .free_buffer     = ggml_backend_hexagon_buffer_free_buffer,
    .get_base        = ggml_backend_hexagon_buffer_get_base,
    .init_tensor     = ggml_backend_hexagon_buffer_init_tensor,
    .set_tensor      = ggml_backend_hexagon_buffer_set_tensor,
    .get_tensor      = ggml_backend_hexagon_buffer_get_tensor,
    .cpy_tensor      = ggml_backend_hexagon_buffer_cpy_tensor,
    .clear           = ggml_backend_hexagon_buffer_clear,
};
```

#### 设备能力

- HVX (Hexagon Vector eXtensions) 向量指令集
- HMX (Hexagon Matrix eXtensions) 矩阵计算单元
- 异步操作批处理
- 性能计数器和 PMU 事件

### 配置选项

环境变量控制：

| 变量 | 说明 |
|------|------|
| `GGML_HEXAGON_ARCH` | 目标架构版本 (0=自动检测) |
| `GGML_HEXAGON_NDEV` | 设备数量 |
| `GGML_HEXAGON_NHVX` | HVX 线程数 (0=全部) |
| `GGML_HEXAGON_USE_HMX` | 启用 HMX (默认1) |
| `GGML_HEXAGON_VMEM` | 最大虚拟内存 (默认 1GB) |
| `GGML_HEXAGON_MBUF` | 最大缓冲区大小 |
| `GGML_HEXAGON_VERBOSE` | 详细日志 |
| `GGML_HEXAGON_PROFILE` | 性能分析模式 |

### 性能优化

1. **量化优化**: 使用 Hexagon 特定的量化格式
2. **批处理**: 将多个操作合并为批次执行
3. **内存重用**: 缓冲区池化和重用
4. **操作过滤**: 通过正则表达式过滤不需要的操作

### 限制

- 静态链接不支持
- 需要 Qualcomm Hexagon SDK
- 仅在支持的 Qualcomm 平台上可用
- VTCM (Vector Tightly Coupled Memory) 大小限制

## 使用方法

### 编译

```bash
cmake -DGGML_HEXAGON=ON ..
make
```

### 运行时要求

- Qualcomm Snapdragon 处理器
- Adreno GPU (部分功能)
- Hexagon SDK 库
- Linux/Android 操作系统

### 初始化

```cpp
ggml_backend_t backend = ggml_backend_hexagon_init(0);
```

## 相关链接

- [Qualcomm Hexagon SDK](https://developer.qualcomm.com/software/hexagon-dsp-sdk)
- [Hexagon Vector eXtensions](https://developer.qualcomm.com/software/hexagon-dsp-sdk/tools)
- [FastRPC](https://source.codeaurora.org/quic/le/kernel/fastrpc)