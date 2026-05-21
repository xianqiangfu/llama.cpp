# ggml-musa - MUSA 后端

## 概述

ggml-musa 是基于 Moore Threads MUSA (Moore Threads Unified System Architecture) 的 GGML 后端实现，支持 Moore Threads GPU 上的矩阵运算。MUSA 是 Moore Threads 开发的计算平台，与 CUDA 兼容。

## 文件结构

```
ggml/src/ggml-musa/
├── CMakeLists.txt          # CMake 构建配置
├── mudnn.cu                # MUDNN 库接口实现
└── mudnn.cuh               # MUDNN 库接口头文件
```

## 实现原理

### 架构设计

MUSA 后端提供 MUDNN (Moore Threads Deep Neural Network) 库的封装接口：

```cpp
namespace mudnn = musa::dnn;
```

### 核心功能

#### 错误处理

```cpp
const char* mudnnGetErrorString(mudnn::Status err) {
    switch (err) {
        case mudnn::Status::SUCCESS:           return "Success";
        case mudnn::Status::INVALID_PARAMETER: return "Invalid parameter";
        case mudnn::Status::NOT_INITIALIZED:    return "Not initialized";
        case mudnn::Status::ALLOC_FAILED:       return "Allocation failed";
        case mudnn::Status::NOT_SUPPORTED:      return "Not supported";
        case mudnn::Status::INTERNAL_ERROR:    return "Internal error";
        case mudnn::Status::ARCH_MISMATCH:     return "Architecture mismatch";
        case mudnn::Status::EXECUTION_FAILED:  return "Execution failed";
        default:                               return "Unknown mudnn status";
    }
}
```

#### 句柄缓存

```cpp
static std::unordered_map<int, std::unique_ptr<mudnn::Handle>> handle_cache;

static mudnn::Handle* get_cached_handle(int device_id) {
    std::lock_guard<std::mutex> lock(handle_cache_mutex);
    auto it = handle_cache.find(device_id);
    if (it != handle_cache.end()) {
        return it->second.get();
    }
    auto handle = std::make_unique<mudnn::Handle>(device_id);
    handle_cache[device_id] = std::move(handle);
    return handle_cache[device_id].get();
}
```

#### 张量信息提取

```cpp
int get_ggml_dims_and_strides(
    const ggml_tensor* tensor,
    std::vector<int64_t>& dims,
    std::vector<int64_t>& strides
) {
    const int ndims = ggml_n_dims(tensor);
    const size_t element_size = ggml_element_size(tensor);

    dims.resize(ndims);
    strides.resize(ndims);

    for (int i = 0; i < ndims; ++i) {
        dims[i] = tensor->ne[i];
        strides[i] = tensor->nb[i] / static_cast<int64_t>(element_size);
    }
    return ndims;
}
```

#### 类型转换

```cpp
mudnn::Tensor::Type ggml_type_to_mudnn_type(ggml_type type) {
    switch (type) {
        case GGML_TYPE_F32: return mudnn::Tensor::Type::FLOAT;
        case GGML_TYPE_F16: return mudnn::Tensor::Type::HALF;
        default: MUDNN_CHECK(mudnn::Status::NOT_SUPPORTED);
    }
}
```

### 异步内存复制

```cpp
musaError_t mudnnMemcpyAsync(
    ggml_backend_cuda_context& ctx,
    const ggml_tensor* dst,
    const ggml_tensor* src
) {
    mudnn::Tensor tensor_dst, tensor_src;

    MUDNN_CHECK(tensor_dst.SetType(ggml_type_to_mudnn_type(dst->type)));
    MUDNN_CHECK(tensor_src.SetType(ggml_type_to_mudnn_type(src->type)));

    // 设置张量信息
    const int ndims = get_ggml_dims_and_strides(src, dims, strides);
    MUDNN_CHECK(tensor_dst.SetNdInfo(ndims, dims.data(), strides.data()));
    MUDNN_CHECK(tensor_src.SetNdInfo(ndims, dims.data(), strides.data()));

    MUDNN_CHECK(tensor_dst.SetAddr(dst->data));
    MUDNN_CHECK(tensor_src.SetAddr(src->data));

    // 使用 IDENTITY 操作执行复制
    mudnn::Unary op;
    MUDNN_CHECK(op.SetMode(mudnn::Unary::Mode::IDENTITY));
    MUDNN_CHECK(op.SetAlpha(0.0f));
    MUDNN_CHECK(op.SetBeta(0.0f));

    mudnn::Handle* handle = get_cached_handle(ctx.device);
    MUDNN_CHECK(handle->SetStream(ctx.stream()));
    MUDNN_CHECK(op.Run(*handle, tensor_dst, tensor_src));

    return musaSuccess;
}
```

### 构建配置

#### 依赖库

```cmake
find_package(musa REQUIRED)
find_package(mudnn REQUIRED)
target_link_libraries(ggml-musa PRIVATE
    ggml-base
    musa::musa
    mudnn::mudnn
)
```

### 支持的数据类型

当前支持的类型：

| GGML 类型 | MUDNN 类型 |
|-----------|------------|
| F32 | FLOAT |
| F16 | HALF |

### 使用场景

MUSA 后端适用于：

1. **Moore Threads GPU 用户**: 运行在 Moore Threads GPU 上
2. **低成本推理**: 利用 MUDNN 库的优化实现
3. **兼容 CUDA**: MUSA 与 CUDA API 兼容，便于移植

## 使用方法

### 编译

```bash
cmake -DGGML_MUSA=ON ..
make
```

### 运行

```bash
./llama-cli --model model.gguf --backend musa
```

### 环境变量

| 变量 | 说明 |
|------|------|
| `MUSA_VISIBLE_DEVICES` | 可见 GPU 列表 |
| `MUSA_LAUNCH_BLOCKING` | 阻塞执行模式 (调试) |

## 限制

- 仅支持 F32 和 F16 数据类型
- 其他数据类型尚未实现
- 需要 Moore Threads GPU 和驱动
- 需要安装 MUDNN 库

## 相关链接

- [Moore Threads 官网](https://www.mthreads.com/)
- [MUSA 文档](https://www.mthreads.com/)
- [MUDNN 文档](https://www.mthreads.com/)