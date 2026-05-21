# llama.cpp 模型加载常见问题

本文档总结 llama.cpp 项目中模型加载的常见问题及其解决方案。

## 1. 模型文件问题

### 1.1 文件不存在

**问题**：
```
error: failed to load model: file not found: model.gguf
```

**原因**：模型文件路径不正确

**解决方案**：
```cpp
// 检查文件是否存在
#include <fstream>
if (!std::ifstream(model_path).good()) {
    throw std::runtime_error("model file not found: " + model_path);
}

// 使用绝对路径
std::string model_path = "/absolute/path/to/model.gguf";

// 或检查相对路径
std::ifstream test(model_path);
if (!test.good()) {
    std::cerr << "Model file not found: " << model_path << std::endl;
    return false;
}
```

### 1.2 文件格式错误

**问题**：
```
error: invalid file format: unknown model type
```

**原因**：模型文件格式不正确或损坏

**解决方案**：
```cpp
// 检查文件扩展名
if (model_path.substr(model_path.find_last_of('.') + 1) != "gguf") {
    throw std::runtime_error("model file must be in GGUF format");
}

// 检查文件头
auto file = llama_file(fname, "rb");
uint32_t magic = read_u32();
if (magic != GGUF_MAGIC) {
    throw std::runtime_error("invalid GGUF file");
}
```

### 1.3 文件损坏

**问题**：
```
error: failed to read model: I/O error
```

**原因**：模型文件损坏或不完整

**解决方案**：
```bash
# 重新下载模型文件
# 验证文件完整性
sha256sum model.gguf
# 与预期哈希值比较

# 检查文件大小
ls -lh model.gguf
```

## 2. 内存问题

### 2.1 内存不足

**问题**：
```
error: failed to allocate memory: out of memory
```

**原因**：系统内存不足以加载模型

**解决方案**：
```cpp
// 检查可用内存
size_t get_available_memory() {
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys;
#else
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
#endif
}

// 估算模型所需内存
size_t estimate_memory_size(const std::string & model_path) {
    struct llama_file file(model_path.c_str(), "rb");
    return file.size() * 2;  // 估算值
}

// 检查是否有足够内存
size_t available = get_available_memory();
size_t required = estimate_memory_size(model_path);
if (available < required) {
    throw std::runtime_error("insufficient memory: need " +
                           std::to_string(required) + " bytes, have " +
                           std::to_string(available) + " bytes");
}

// 使用量化减少内存使用
std::string quantized_model = "model-q4_k_m.gguf";
```

### 2.2 32 位系统限制

**问题**：
```
error: model too large for 32-bit system
```

**原因**：32 位系统内存地址空间限制

**解决方案**：
```bash
# 使用 64 位系统
# 或使用更小的模型

# 检查系统架构
uname -m
# x86_64 或 aarch64 表示 64 位系统
```

## 3. 模型参数问题

### 3.1 参数不兼容

**问题**：
```
error: model parameters not compatible with context
```

**原因**：模型参数与上下文参数不匹配

**解决方案**：
```cpp
// 检查模型参数
auto hparams = model.hparams;

// 设置兼容的上下文参数
llama_context_params ctx_params = llama_context_default_params();
ctx_params.n_ctx = std::min(ctx_params.n_ctx, hparams.n_ctx_train);
ctx_params.n_batch = std::min(ctx_params.n_batch, hparams.n_ctx_train);

// 创建上下文
auto ctx = llama_new_context_with_model(model, ctx_params);
if (!ctx) {
    throw std::runtime_error("failed to create context");
}
```

### 3.2 上下文长度不匹配

**问题**：
```
error: context length exceeds model's maximum context length
```

**原因**：请求的上下文长度超过模型支持的最大长度

**解决方案**：
```cpp
// 获取模型的最大上下文长度
uint32_t max_ctx = llama_n_ctx_train(model);

// 调整上下文长度
uint32_t requested_ctx = 8192;
uint32_t actual_ctx = std::min(requested_ctx, max_ctx);

if (actual_ctx < requested_ctx) {
    LLAMA_LOG_WARN("Requested context length %u exceeds model's maximum %u, using %u\n",
                   requested_ctx, max_ctx, actual_ctx);
}

// 设置上下文参数
llama_context_params params = llama_context_default_params();
params.n_ctx = actual_ctx;
```

### 3.3 量化类型不支持

**问题**：
```
error: unsupported quantization type
```

**原因**：模型使用了不支持的量化类型

**解决方案**：
```bash
# 检查支持的量化类型
./tools/quantize/quantize --help

# 转换为支持的量化类型
./tools/quantize/quantize model.gguf model-q4_k_m.gguf Q4_K_M

# 查看支持的类型
# Q4_K_M, Q5_K_M, Q6_K, Q8_0 等
```

## 4. 后端问题

### 4.1 GPU 后端初始化失败

**问题**：
```
error: failed to initialize GPU backend
```

**原因**：GPU 后端初始化失败

**解决方案**：
```cpp
// 检查 GPU 支持
if (!llama_supports_gpu_offload()) {
    LLAMA_LOG_WARN("GPU offload not supported, using CPU\n");
    params.n_gpu_layers = 0;
}

// 减少 GPU 层数
if (params.n_gpu_layers > max_gpu_layers) {
    LLAMA_LOG_WARN("Requested %d GPU layers, maximum is %d\n",
                   params.n_gpu_layers, max_gpu_layers);
    params.n_gpu_layers = max_gpu_layers;
}
```

### 4.2 GPU 内存不足

**问题**：
```
error: GPU out of memory
```

**原因**：GPU 内存不足以加载模型

**解决方案**：
```cpp
// 检查 GPU 内存大小
size_t get_gpu_memory() {
    // 实现获取 GPU 内存大小
    return gpu_memory_size;
}

// 估算 GPU 内存需求
size_t estimate_gpu_memory(int n_gpu_layers, const llama_model & model) {
    size_t layer_memory = calculate_layer_memory(model);
    return n_gpu_layers * layer_memory;
}

// 调整 GPU 层数
size_t available_gpu_memory = get_gpu_memory();
size_t required_gpu_memory = estimate_gpu_memory(params.n_gpu_layers, model);

if (required_gpu_memory > available_gpu_memory) {
    params.n_gpu_layers = available_gpu_memory / calculate_layer_memory(model);
    LLAMA_LOG_WARN("Reducing GPU layers to %d due to insufficient memory\n",
                   params.n_gpu_layers);
}
```

### 4.3 后端不支持

**问题**：
```
error: requested backend not available
```

**原因**：请求的后端不可用

**解决方案**：
```cpp
// 检查后端支持
bool llama_supports_mmap(void);
bool llama_supports_mlock(void);
bool llama_supports_gpu_offload(void);

// 根据支持情况调整参数
llama_model_params model_params = llama_model_default_params();
if (!llama_supports_mmap()) {
    model_params.use_mmap = false;
}

if (!llama_supports_gpu_offload()) {
    model_params.n_gpu_layers = 0;
}
```

## 5. 加载性能问题

### 5.1 加载时间长

**问题**：
模型加载时间过长

**原因**：
- 模型文件大
- 存储速度慢
- 内存映射未启用

**解决方案**：
```cpp
// 启用内存映射
llama_model_params model_params = llama_model_default_params();
model_params.use_mmap = true;

// 预取数据
llama_mmap mmap(file, size, false);

// 使用更快的存储
// SSD > HDD
// 本地存储 > 网络存储
```

### 5.2 分段加载

**问题**：
大模型加载后性能下降

**原因**：
大模型导致频繁的内存访问

**解决方案**：
```cpp
// 使用分段加载
llama_model_params model_params = llama_model_default_params();
model_params.split_mode = LLAMA_SPLIT_MODE_LAYER;  // 按层分割

// 或按行分割
model_params.split_mode = LLAMA_SPLIT_MODE_ROW;

// 使用多 GPU
// 分层加载到不同的 GPU
```

## 6. 模型版本问题

### 6.1 版本不兼容

**问题**：
```
error: model version not supported
```

**原因**：模型版本与当前代码不兼容

**解决方案**：
```cpp
// 检查模型版本
uint32_t version = read_u32();
if (version < MIN_VERSION || version > MAX_VERSION) {
    throw std::runtime_error("model version " + std::to_string(version) +
                           " not supported (requires " + std::to_string(MIN_VERSION) +
                           " to " + std::to_string(MAX_VERSION) + ")");
}

// 更新代码到最新版本
git pull
cmake -B build
cmake --build build
```

### 6.2 架构不兼容

**问题**：
```
error: model architecture not supported
```

**原因**：模型架构不被支持

**解决方案**：
```bash
# 检查支持的架构
./build/bin/llama-cli --help | grep "arch"

# 使用支持的架构
# LLaMA, GPT, GPT-NeoX, Falcon 等
```

## 7. 调试模型加载

### 7.1 详细日志

```cpp
// 设置详细日志级别
llama_log_set(log_callback, nullptr);

// 或使用环境变量
export LLAMA_LOG_LEVEL=0  # DEBUG
export LLAMA_LOG_LEVEL=1  # INFO
export LLAMA_LOG_LEVEL=2  # WARN
export LLAMA_LOG_LEVEL=3  # ERROR
```

### 7.2 模型信息查询

```cpp
// 查询模型信息
auto hparams = llama_model_hparams(model);

printf("Model info:\n");
printf("  vocab size: %d\n", hparams.n_vocab);
printf("  context length: %d\n", hparams.n_ctx_train);
printf("  embedding dimension: %d\n", hparams.n_embd);
printf("  number of layers: %d\n", hparams.n_layer);
printf("  number of heads: %d\n", hparams.n_head);
```

### 7.3 内存使用统计

```cpp
// 获取内存使用信息
size_t memory_usage = llama_model_memory_size(model);
size_t memory_used = llama_model_memory_used(model);

printf("Memory usage: %zu MB / %zu MB\n",
       memory_used / (1024 * 1024),
       memory_usage / (1024 * 1024));
```

## 8. 常见解决方案

### 8.1 使用正确的模型文件

```bash
# 确保使用 GGUF 格式
# 旧格式需要转换
python3 convert.py --outfile model.gguf model.bin

# 量化模型
./tools/quantize/quantize model.gguf model-q4_k_m.gguf Q4_K_M
```

### 8.2 调整上下文参数

```cpp
// 根据模型调整参数
llama_context_params params = llama_context_default_params();
params.n_ctx = 2048;          // 上下文长度
params.n_batch = 512;         // 批处理大小
params.n_threads = 4;         // 线程数
params.n_gpu_layers = 20;      // GPU 层数
```

### 8.3 使用内存映射

```cpp
llama_model_params model_params = llama_model_default_params();
model_params.use_mmap = true;
model_params.use_mlock = true;  // 如果支持
```

### 8.4 分层加载

```cpp
// 部分层加载到 GPU
llama_model_params model_params = llama_model_default_params();
model_params.n_gpu_layers = 10;  // 前 10 层到 GPU
```

## 9. 最佳实践

### 9.1 检查模型文件

```cpp
// 加载前检查
bool validate_model(const std::string & model_path) {
    // 检查文件存在
    if (!std::ifstream(model_path).good()) {
        return false;
    }

    // 检查文件格式
    llama_file file(model_path.c_str(), "rb");
    uint32_t magic = read_u32();
    if (magic != GGUF_MAGIC) {
        return false;
    }

    return true;
}
```

### 9.2 处理错误

```cpp
// 正确处理错误
auto model = llama_load_model_from_file(model_path.c_str(), model_params);
if (!model) {
    LLAMA_LOG_ERROR("Failed to load model from %s\n", model_path.c_str());
    return false;
}

// 尝试备用模型
auto model = llama_load_model_from_file(model_path.c_str(), model_params);
if (!model) {
    model = llama_load_model_from_file(fallback_model_path.c_str(), model_params);
}
```

### 9.3 资源管理

```cpp
// 使用 RAII 管理模型资源
class ModelHolder {
private:
    llama_model_ptr model_;
public:
    ModelHolder(const std::string & path, llama_model_params params)
        : model_(llama_load_model_from_file(path.c_str(), params)) {
        if (!model_) {
            throw std::runtime_error("failed to load model");
        }
    }

    ~ModelHolder() {
        if (model_) {
            llama_free_model(model_);
        }
    }

    llama_model_ptr get() const { return model_; }
};

// 使用
try {
    ModelHolder model("model.gguf", params);
    // 使用模型
} catch (const std::exception & e) {
    LLAMA_LOG_ERROR("Error loading model: %s\n", e.what());
}
```

## 10. 总结

llama.cpp 模型加载常见问题要点：

1. **文件检查**：验证文件存在、格式正确
2. **内存管理**：检查可用内存，使用量化
3. **参数匹配**：确保参数与模型兼容
4. **后端支持**：检查后端可用性，调整配置
5. **性能优化**：使用内存映射、分层加载
6. **错误处理**：正确处理错误，提供回退方案

遵循这些指南可以避免常见的模型加载问题。