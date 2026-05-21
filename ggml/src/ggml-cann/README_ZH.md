# CANN 后端说明

GGML CANN 后端通过华为 CANN (Compute Architecture for Neural Networks) 框架提供昇腾 AI 加速卡支持。

## 概述

CANN 后端利用华为昇腾系列 AI 处理器（如 Ascend 910）的硬件加速能力，为 llama.cpp 提供 GPU 级别的推理性能。

## 系统要求

### 硬件
- 华为昇腾 AI 处理器（Ascend 910、310 等）
- 服务器/工作站支持 CANN 环境

### 软件
- CANN Toolkit（推荐版本 5.0+）
- AscendCL 驱动
- 相关依赖库

## 编译配置

### CMake 选项

```cmake
# 启用 CANN 后端
GGML_CANN=ON

# CANN 安装路径
CANN_ROOT=/usr/local/Ascend/ascend-toolkit/latest

# 启用 ACL 图模式（图编译优化）
USE_ACL_GRAPH=ON
```

### 环境变量

```bash
# CANN 环境设置
export ASCEND_HOME=/usr/local/Ascend/ascend-toolkit/latest
export LD_LIBRARY_PATH=$ASCEND_HOME/lib64:$LD_LIBRARY_PATH

# 设置图缓存容量
export GGML_CANN_GRAPH_CACHE_CAPACITY=12

# 启用/禁用图模式
export GGML_CANN_ACL_GRAPH=on
```

## 目录结构

```
ggml-cann/
├── CMakeLists.txt       # 构建配置
├── common.h             # 公共定义和工具函数
├── acl_tensor.h/cpp     # Tensor 封装
├── aclnn_ops.h/cpp      # ACLNN 操作实现
├── ggml-cann.cpp        # 后端主实现
└── ...
```

## 核心组件

### 1. 设备管理

`ggml_cann_device_info` 结构提供设备信息：

```cpp
struct cann_device_info {
    int    cc;              // 计算能力
    size_t smpb;            // 单块最大共享内存
    bool   vmm;             // 虚拟内存支持
    size_t vmm_granularity; // 虚拟内存粒度
    size_t total_vram;      // 总显存
};
```

### 2. 内存管理

#### 内存池

`ggml_cann_pool` 抽象内存池接口：
- `alloc()` - 分配内存
- `free()` - 释放内存
- 支持多种内存池实现策略

#### RAII 包装

`ggml_cann_pool_alloc` 提供自动内存管理：
- 构造时分配
- 析构时自动释放
- 防止资源泄漏

### 3. 计算图优化

#### ACL 图模式

`ggml_cann_graph` 封装 CANN 计算图：

```cpp
struct ggml_cann_graph {
    aclmdlRI graph;
    std::vector<ggml_graph_node_properties> ggml_graph_properties;
    
    // 创建 CANN 图
    static ggml_cann_graph * create_from_cgraph(ggml_cgraph * cgraph);
    
    // 匹配检查
    bool matches_cgraph(ggml_cgraph * cgraph);
};
```

#### LRU 缓存

`ggml_cann_graph_lru_cache` 实现计算图缓存：
- 最多缓存 N 个计算图（默认 12）
- 自动驱逐最少使用的图
- 提高重复推理性能

### 4. 缓存机制

#### RoPE 缓存

`ggml_cann_rope_cache` 缓存旋转位置编码：
- sin/cos 预计算
- 支持各种 RoPE 变体（NEOX、M-RoPE、iM-RoPE）
- YaRN 扩展支持

#### 张量缓存

`ggml_cann_tensor_cache` 缓存常用张量：
- RMS Norm 的 ones/zeros 张量
- 减少重复分配

### 5. ACLNN 操作

`aclnn_ops.cpp/h` 实现 CANN 神经网络操作：

| 操作类型 | 说明 |
|---------|------|
| 矩阵乘法 | MatMul、BatchMatMul |
| 卷积 | Conv2d、Conv1d |
| 池化 | MaxPool、AvgPool |
| 激活 | ReLU、GELU、SiLU 等 |
| 归一化 | LayerNorm、RMSNorm |
| 注意力 | FlashAttention |
| 其他 | Softmax、Dropout 等 |

## 执行模式

### 图模式 (Graph Mode)

**优点：**
- 算子融合优化
- 减少内核启动开销
- 更高的吞吐量

**缺点：**
- 首次编译耗时
- 灵活性较低

### 急切模式 (Eager Mode)

**优点：**
- 即时执行
- 更好的调试体验
- 动态形状支持

**缺点：**
- 吞吐量较低

## 性能优化

### 1. 流水线并行

支持最多 8 个流 (`GGML_CANN_MAX_STREAMS`)：
```cpp
aclrtStream streams[GGML_CANN_MAX_STREAMS];
```

### 2. 内存复用

通过内存池减少分配开销：
- 预分配大块内存
- 按需分配和释放
- 支持虚拟内存

### 3. 矩阵优化

- 矩阵行填充（512 字节对齐）
- 优化数据布局
- 利用 Ascend Cube 单元

## 错误处理

使用宏 `ACL_CHECK` 自动检查错误：

```cpp
#define ACL_CHECK(stmt) ACL_CHECK_GEN(stmt, 0, aclGetRecentErrMsg)

void ggml_cann_error(const char * stmt, const char * func, 
                     const char * file, int line, const char * msg);
```

## 使用示例

```cpp
// 初始化后端
ggml_backend_t backend = ggml_backend_cann_init(0);  // 设备 0

// 创建缓冲区
ggml_backend_buffer_type_t buft = ggml_backend_cann_buffer_type(0);
ggml_backend_buffer_t buffer = ggml_backend_buffer_type(buft);

// 执行计算
// ... 标准 GGML API ...

// 清理
ggml_backend_free(backend);
```

## 限制与注意事项

1. **显存限制**：需要在昇腾设备显存范围内运行
2. **模型大小**：大模型可能需要模型并行
3. **调试**：图模式调试相对困难
4. **兼容性**：需要匹配的 CANN 版本

## 性能建议

1. **启用图模式**：生产环境推荐使用图模式
2. **调整缓存**：根据内存情况调整图缓存容量
3. **多流并行**：利用多流提高吞吐
4. **内存池**：使用内存池减少分配开销

## 相关文档

- [../README_ZH.md](../README_ZH.md) - GGML 库总览
- [../ggml-cpu/README_ZH.md](../ggml-cpu/README_ZH.md) - CPU 后端说明
- 华为 CANN 官方文档