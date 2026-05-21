# ggml-openvino - OpenVINO 后端

## 概述

ggml-openvino 是基于 Intel OpenVINO (Open Visual Inference and Neural Network Optimization) 的 GGML 后端实现，支持 Intel CPU、GPU 和 NPU 上的高性能推理。OpenVINO 是 Intel 推出的深度学习推理优化工具包。

## 文件结构

```
ggml/src/ggml-openvino/
├── .clang-format           # Clang 格式配置
├── CMakeLists.txt          # CMake 构建配置
├── ggml-decoder.cpp        # 解码器实现
├── ggml-decoder.h          # 解码器头文件
├── ggml-openvino.cpp       # OpenVINO 后端主实现
├── ggml-openvino.h         # OpenVINO 后端头文件
├── ggml-openvino-extra.cpp # OpenVINO 额外功能
├── ggml-openvino-extra.h   # OpenVINO 额外功能头文件
├── ggml-quants.cpp         # 量化操作实现
├── ggml-quants.h           # 量化操作头文件
├── utils.cpp               # 工具函数
└── utils.h                 # 工具函数头文件
```

## 实现原理

### 架构设计

OpenVINO 后端采用混合设计：

1. **权重张量**: 存储预构建的 `ov::op::v0::Constant`，避免运行时复制
2. **KV Cache/计算张量**: 存储 `ov::Tensor`，可直接传递给推理请求
3. **GPU/NPU 优化**: 支持 `ov::RemoteTensor` 进行零拷贝操作

### 核心组件

#### 缓冲区上下文

```cpp
struct ggml_backend_openvino_buffer_context {
    int device;
    std::string name;
    size_t id;

    void * data;
    size_t size;
    bool is_remote;

    std::shared_ptr<ov::Tensor> ov_buffer;

    // 跟踪所有 extra 用于清理
    std::map<ggml_tensor *, ggml_openvino_extra_base *> tensor_extras;

    void * data_prev;  // 用于 KV cache 的重新分配
};
```

#### 运行时上下文

```cpp
struct ov_runtime_context {
    std::string device;
    bool stateful;
    std::map<std::string, std::shared_ptr<ov::Model>> model_cache;
    std::map<std::string, std::shared_ptr<ov::CompiledModel>> compiled_model_cache;
    std::map<std::string, std::shared_ptr<ov::InferRequest>> infer_request_cache;
    int backend_count;
};
```

### 量化处理

#### 量化数据提取

```cpp
struct ggml_openvino_extracted_layout {
    size_t total_size;       // 总大小
    size_t weights_size;     // 权重大小
    size_t scales_size;      // 缩放因子大小
    size_t zp_size;          // 零点大小
    bool is_u4;              // 是否为 u4 量化
    bool is_u8;              // 是否为 u8 量化
    size_t weights_per_block; // 每块权重数
    std::optional<ggml_type> requant_type; // 重量化类型
};
```

#### 权重张量创建

对于量化权重，创建包含权重、缩放因子和零点的常量节点：

```cpp
extra = new ggml_openvino_quantized_weight_extra(
    std::move(result.weights),
    std::move(result.scales),
    std::move(result.zp),
    result.weight_node
);
```

### 内存管理

#### 主机缓冲区

```cpp
static void * data = ggml_aligned_malloc(size);
memset(data, 0, size);
ov_buffer = std::make_shared<ov::Tensor>(
    ov::element::u8,
    ov::Shape{size},
    data
);
```

#### 设备缓冲区

对于 GPU KV Cache：

```cpp
auto gpu_context = remote_context->as<ov::intel_gpu::ocl::ClContext>();
ov::intel_gpu::ocl::USMTensor usm_tensor =
    gpu_context.create_usm_device_tensor(
        ov::element::u8,
        ov::Shape{size}
    );
data = usm_tensor.get();
```

### 操作支持

#### 支持的操作类型

| 操作 | 说明 |
|------|------|
| ADD, MUL | 加法、乘法 |
| MUL_MAT | 矩阵乘法 |
| FLASH_ATTN_EXT | Flash Attention |
| RMS_NORM | RMS 归一化 |
| GELU, SILU | 激活函数 |
| GLU (SWIGLU, GEGLU) | 门控线性单元 |
| ROPE | 旋转位置编码 |
| PERMUTE, TRANSPOSE | 张量变换 |
| GET_ROWS, SET_ROWS | 行操作 |

#### 支持的数据类型

```cpp
static std::set<ggml_type> supported_types{
    GGML_TYPE_F32, GGML_TYPE_F16, GGML_TYPE_BF16,
    GGML_TYPE_I64, GGML_TYPE_I32,
    GGML_TYPE_Q4_0, GGML_TYPE_Q4_1,
    GGML_TYPE_Q4_K, GGML_TYPE_Q5_K,
    GGML_TYPE_Q8_0, GGML_TYPE_Q6_K
};
```

### 限制处理

OpenVINO 后端对某些操作模式有限制：

```cpp
static bool is_op_unsupported_case(const ggml_tensor * op) {
    switch (op->op) {
    case GGML_OP_SOFT_MAX:
        // 不支持 sinks
        if (op->src[2] != nullptr) return true;
        // 不支持 max_bias > 0
        if (max_bias > 0) return true;
        break;
    case GGML_OP_FLASH_ATTN_EXT:
        // 不支持 sinks
        if (op->src[4] != nullptr) return true;
        // 不支持 max_bias > 0
        if (max_bias > 0) return true;
        break;
    }
    return false;
}
```

### 设备支持

#### 设备类型

```cpp
// CPU
"GPU"  // Intel 集成/独立显卡
"NPU"  // Intel 神经处理器
```

#### 设备初始化

```cpp
void ggml_openvino_init_device_config() {
    const char * env = getenv("GGML_OPENVINO_DEVICE");
    if (env) {
        g_device_name = std::string(env);
    } else {
        // 自动选择设备
        if (std::filesystem::exists("/dev/intel/nci")) {
            g_device_name = "NPU";
        } else {
            g_device_name = "CPU";
        }
    }
}
```

### 性能优化

#### 状态执行

```cpp
static bool is_stateful_enabled() {
    static const auto * stateful = getenv("GGML_OPENVINO_STATEFUL_EXECUTION");
    return stateful && *stateful != '\0' && strcmp(stateful, "0") != 0;
}
```

状态执行模式可以重用模型状态，减少内存分配和初始化开销。

#### 图缓存

```cpp
std::map<std::string, std::shared_ptr<ov::Model>> model_cache;
std::map<std::string, std::shared_ptr<ov::CompiledModel>> compiled_model_cache;
std::map<std::string, std::shared_ptr<ov::InferRequest>> infer_request_cache;
```

缓存编译后的模型和推理请求，避免重复编译。

### 环境变量

| 变量 | 说明 |
|------|------|
| `GGML_OPENVINO_DEVICE` | 目标设备 (CPU/GPU/NPU) |
| `GGML_OPENVINO_STATEFUL_EXECUTION` | 启用状态执行模式 |
| `OPENVINO_DEVICE` | OpenVINO 设备选择 |

## 使用方法

### 编译

```bash
# 基本 OpenVINO 支持
cmake -DGGML_OPENVINO=ON ..

# 指定 OpenVINO 路径
cmake -DGGML_OPENVINO=ON -DOpenVINO_DIR=/path/to/openvino ..
```

### 运行

```bash
# CPU 推理
./llama-cli --model model.gguf --backend openvino

# GPU 推理
./llama-cli --model model.gguf --backend openvino --device GPU

# NPU 推理
./llama-cli --model model.gguf --backend openvino --device NPU
```

### 性能模式

```bash
# 状态执行模式 (推荐)
export GGML_OPENVINO_STATEFUL_EXECUTION=1
./llama-cli --model model.gguf --backend openvino
```

## 性能调优

### 设备选择

1. **CPU**: 通用性好，支持所有操作
2. **GPU**: 高吞吐量，适合大规模推理
3. **NPU**: 低功耗，适合边缘设备

### 精度选择

```bash
# BF16 精度 (GPU/NPU)
export GGML_OPENVINO_ENABLE_BF16=1

# FP16 精度
export GGML_OPENVINO_ENABLE_F16=1
```

### 批处理

调整批处理大小提高吞吐量：

```bash
./llama-cli --model model.gguf --backend openvino --batch-size 8
```

## 限制

- 部分操作模式不支持
- 某些量化格式需要特殊处理
- GPU 功能需要特定驱动支持
- NPU 仅在支持的平台可用

## 适用场景

1. **Intel CPU**: 利用 AVX-512 等指令集
2. **Intel GPU**: 利用 Intel Arc Graphics
3. **Intel NPU**: 利用 AI 加速器
4. **低功耗推理**: 边缘设备和移动平台

## 相关链接

- [OpenVINO 官网](https://www.intel.com/content/www/us/en/developer/tools/openvino-toolkit.html)
- [OpenVINO 文档](https://docs.openvino.ai/)
- [Intel GPU](https://www.intel.com/content/www/us/en/products/details/discrete-gpus/arc/overview.html)