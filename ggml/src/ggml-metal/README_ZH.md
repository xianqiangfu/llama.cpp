# ggml-metal - Metal 后端

## 概述

ggml-metal 是基于 Apple Metal Performance Shaders (MPS) 的 GGML 后端实现，专为 macOS 和 Apple Silicon 平台优化。Metal 提供了低延迟的 GPU 计算能力，并支持 Apple 的统一内存架构。

## 文件结构

```
ggml/src/ggml-metal/
├── CMakeLists.txt              # CMake 构建配置
├── ggml-metal.cpp              # Metal 后端主实现文件
├── ggml-metal.m                # Metal 后端 Objective-C 实现
├── ggml-metal.h                # Metal 后端头文件
├── ggml-metal.metal            # Metal Shading Language 内核
├── ggml-metal-common.cpp       # Metal 后端通用函数
├── ggml-metal-common.h         # Metal 后端通用函数头文件
├── ggml-metal-context.h        # Metal 上下文管理
├── ggml-metal-context.m        # Metal 上下文 Objective-C 实现
├── ggml-metal-device.cpp       # Metal 设备管理
├── ggml-metal-device.h         # Metal 设备管理头文件
├── ggml-metal-device.m         # Metal 设备 Objective-C 实现
├── ggml-metal-impl.h           # Metal 后端内部接口
├── ggml-metal-ops.cpp          # Metal 操作实现
└── ggml-metal-ops.h            # Metal 操作头文件
```

## 实现原理

### 架构设计

Metal 后端采用分层架构：

1. **设备层**: 管理 MTLDevice 和 Metal 特定功能
2. **上下文层**: 管理 MTLCommandQueue 和计算状态
3. **缓冲区层**: 提供 Shared/Private/Mapped 三种缓冲区类型
4. **操作层**: 实现 GGML 操作到 Metal 的映射

### 核心组件

#### 缓冲区类型

Metal 后端提供三种缓冲区类型：

```cpp
// 共享缓冲区 (默认)
ggml_backend_buffer_type_t ggml_backend_metal_buffer_type_shared(int device);

// 私有缓冲区
ggml_backend_buffer_type_t ggml_backend_metal_buffer_type_private(int device);

// 映射缓冲区
ggml_backend_buffer_type_t ggml_backend_metal_buffer_type_mapped(int device);
```

**共享缓冲区**: 与 CPU 共享内存，通过 `IOSurface` 实现
**私有缓冲区**: GPU 专用内存，不与 CPU 共享
**映射缓冲区**: 从主机指针映射，适合零拷贝场景

#### 设备管理

```cpp
struct ggml_metal_device {
    id<MTLDevice> device;
    MTLDeviceProperties * props;
    struct ggml_metal_context * ctx;
    int device_id;
};
```

设备属性包括：
- 最大缓冲区大小
- 计算单元数量
- 操作卸载最小批次大小

#### Metal Shading Language 内核

ggml-metal.metal 包含 GPU 计算内核：

```metal
// Flash Attention 内核
kernel void kernel_flash_attn_ext(
    device const  char * src0,
    device const  char * src1,
    device const  char * src2,
    device        float * dst,
    ...
);

// 矩阵乘法内核
template <typename type0, typename type1>
kernel void kernel_mul_mat(
    device const type0 * src0,
    device const type1 * src1,
    device        float * dst,
    ...
);
```

### 内存管理

#### 统一内存架构

Metal 的统一内存架构允许 CPU 和 GPU 共享物理内存：

```cpp
void * base = ggml_metal_buffer_get_base(buffer);
// CPU 和 GPU 都可以访问同一内存
```

#### 延迟映射

对于 KV Cache 等大量数据，使用延迟映射减少内存占用：

```cpp
if (strncmp(tensor->name, "cache_", 6) == 0) {
    // 使用远程缓冲区
}
```

### 操作支持

#### 支持的操作

- 矩阵乘法 (MUL_MAT, MUL_MAT_ID)
- Flash Attention (FLASH_ATTN_EXT)
- 归一化 (RMS_NORM, LAYER_NORM)
- 激活函数 (GELU, SILU, SWIGLU, GEGLU)
- RoPE 位置编码
- 量化/反量化

#### 量化支持

Metal 后端支持多种量化格式：

- IQ2_XXS, IQ2_XS, IQ2_S, IQ3_XXS, IQ3_S, IQ4_XS, IQ4_NL
- Q4_0, Q4_1, Q4_K, Q5_K, Q6_K, Q8_0
- F16, BF16, F32

### 性能优化

#### 操作批处理

```cpp
static bool ggml_backend_metal_device_offload_op(
    ggml_backend_dev_t dev, const ggml_tensor * op
) {
    // 仅对大批次操作卸载到 GPU
    return (op->op == GGML_OP_MUL_MAT ||
            op->op == GGML_OP_MUL_MAT_ID) &&
           get_op_batch_size(op) >= op_offload_min_batch_size;
}
```

#### 自适应批处理大小

```cpp
static void ggml_backend_metal_set_n_cb(
    ggml_backend_t backend, int n_cb
) {
    // 根据设备能力调整批处理大小
    ggml_metal_set_n_cb(ctx, n_cb);
}
```

#### 内存对齐

```cpp
static size_t ggml_backend_metal_buffer_type_get_alignment(
    ggml_backend_buffer_type_t buft
) {
    return 32;  // Metal 要求 32 字节对齐
}
```

### 事件同步

Metal 后端支持事件机制用于跨设备同步：

```cpp
struct ggml_metal_event {
    id<MTLEvent> event;
    struct ggml_backend_device * dev;
};
```

## 使用方法

### 编译

```bash
# 基本 Metal 支持
cmake -DGGML_METAL=ON ..

# 启用 Metal Performance Primitives
cmake -DGGML_METAL=ON -DGGML_METAL_MPS=ON ..

# 嵌入 Metal 库
cmake -DGGML_METAL=ON -DGGML_METAL_EMBED_LIBRARY=ON ..
```

### 环境变量

| 变量 | 说明 |
|------|------|
| `GGML_METAL_NUNITS` | Metal 线程数 |
| `GGML_METAL_NCB` | 命令缓冲区数量 |
| `GGML_METAL_DEVICES` | 模拟设备数量 (调试) |

### 运行

```bash
./llama-cli --model model.gguf --backend metal
```

### 内存模式选择

```bash
# 共享内存模式 (默认)
./llama-cli --model model.gguf

# 私有内存模式
./llama-cli --model model.gguf --metal-malloc private

# 映射内存模式
./llama-cli --model model.gguf --metal-malloc mapped
```

## 性能调优

### 批处理大小调整

```bash
# 设置较大的批处理大小
export GGML_METAL_NCB=4
```

### 内存模式选择

- **Shared**: 适合小模型，内存占用低
- **Private**: 适合大模型，性能更高
- **Mapped**: 适合已有数据，零拷贝

### 设备选择

```bash
# 选择特定 GPU
export GGML_METAL_DEVICES=0

# 多 GPU 并行
export GGML_METAL_DEVICES=0,1
```

## 限制

- 仅支持 macOS 和 iOS
- 需要 Metal 2.0+ (macOS 10.13+)
- 某些量化格式需要特定 GPU 支持
- 内存大小受限于统一内存架构

## 相关链接

- [Metal Programming Guide](https://developer.apple.com/metal/)
- [Metal Performance Shaders](https://developer.apple.com/metal/mps/)
- [Metal Shading Language](https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf)