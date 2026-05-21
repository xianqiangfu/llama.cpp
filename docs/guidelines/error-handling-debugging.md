# llama.cpp 错误处理与调试注意事项

本文档总结 llama.cpp 项目的错误处理和调试注意事项，帮助开发者处理错误和调试问题。

## 1. 错误处理机制

### 1.1 错误类型

llama.cpp 使用多种错误处理方式：

#### 1.1.1 返回值

```cpp
// 布尔返回值
bool llama_supports_mmap(void);
bool llama_supports_mlock(void);

// 整数返回值（状态码）
int encode(const llama_batch & batch_inp);
int decode(const llama_batch & batch_inp);
```

#### 1.1.2 异常

```cpp
// 使用 std::runtime_error 抛出异常
if (cparams.n_seq_max > LLAMA_MAX_SEQ) {
    throw std::runtime_error("n_seq_max must be <= " + std::to_string(LLAMA_MAX_SEQ));
}

// 使用 GGML_ABORT 终止程序
switch (flash_attn_type) {
    case LLAMA_FLASH_ATTN_TYPE_AUTO:
        return "auto";
    case LLAMA_FLASH_ATTN_TYPE_DISABLED:
        return "disabled";
    case LLAMA_FLASH_ATTN_TYPE_ENABLED:
        return "enabled";
}
GGML_ABORT("fatal error");
```

#### 1.1.3 状态枚举

```cpp
// GGML 状态
enum ggml_status {
    GGML_STATUS_SUCCESS = 0,
    GGML_STATUS_FAILED = 1,
    GGML_STATUS_ABORTED = 2,
};

// 状态检查
ggml_status ret = ggml_backend_sched_graph_compute_async(sched.get(), gf);
if (ret != GGML_STATUS_SUCCESS) {
    // 处理错误
}
```

### 1.2 错误检测

#### 1.2.1 参数验证

```cpp
// 检查参数有效性
if (cparams.n_seq_max > LLAMA_MAX_SEQ) {
    throw std::runtime_error("n_seq_max must be <= " + std::to_string(LLAMA_MAX_SEQ));
}

if (params.n_threads <= 0) {
    throw std::runtime_error("n_threads must be positive");
}

if (!params.model) {
    throw std::runtime_error("model cannot be null");
}
```

#### 1.2.2 运行时检查

```cpp
// 检查初始化状态
if (!initialized) {
    throw std::runtime_error("not initialized");
}

// 检查资源可用性
if (!buf_output) {
    throw std::runtime_error("output buffer not allocated");
}

// 检查状态一致性
if (n_outputs > output_ids.size()) {
    throw std::runtime_error("output count mismatch");
}
```

### 1.3 错误恢复

#### 1.3.1 中止回调

```cpp
ggml_abort_callback abort_callback      = nullptr;
void *              abort_callback_data = nullptr;

void set_abort_callback(
        bool (*abort_callback)(void * data),
        void * abort_callback_data) {
    this->abort_callback      = abort_callback;
    this->abort_callback_data = abort_callback_data;
}

// 在计算过程中检查中止
if (abort_callback && abort_callback(abort_callback_data)) {
    return GGML_STATUS_ABORTED;
}
```

#### 1.3.2 状态重置

```cpp
// 重置性能统计
void perf_reset() {
    t_start_us  = 0;
    t_load_us   = 0;
    t_p_eval_us = 0;
    t_eval_us   = 0;
    n_p_eval = 0;
    n_eval   = 0;
    n_reused = 0;
}

// 重置图重用
has_evaluated_once = false;
```

## 2. 调试方法

### 2.1 日志记录

#### 2.1.1 日志级别

```cpp
// 日志级别定义
#define LLAMA_LOG_LEVEL_DEBUG  0
#define LLAMA_LOG_LEVEL_INFO   1
#define LLAMA_LOG_LEVEL_WARN   2
#define LLAMA_LOG_LEVEL_ERROR  3

// 设置日志级别
int llama_log_set(llama_log_callback log_callback, void * user_data);
```

#### 2.1.2 日志宏

```cpp
// 信息日志
LLAMA_LOG_INFO("%s: constructing llama_context\n", __func__);

// 调试日志
LLAMA_LOG_DEBUG("%s: n_rs_seq=%u requested but model arch does not support recurrent partial rollback; clamping to 0\n",
                __func__, cparams.n_rs_seq);

// 警告日志
LLAMA_LOG_WARN("%s: large context size may cause performance issues\n", __func__);

// 错误日志
LLAMA_LOG_ERROR("%s: failed to allocate memory\n", __func__);
```

#### 2.1.3 自定义日志

```cpp
// 自定义日志回调
void my_log_callback(ggml_log_level level, const char * text, void * user_data) {
    FILE * file = (FILE *)user_data;
    fprintf(file, "[%c] %s", "DIWE"[level], text);
    fflush(file);
}

// 设置自定义日志
FILE * log_file = fopen("llama.log", "w");
llama_log_set(my_log_callback, log_file);
```

### 2.2 断言

#### 2.2.1 运行时断言

```cpp
#include <cassert>

// 断言检查
assert(n_tokens > 0);
assert(batch.n_tokens <= batch.n_tokens_max);
assert(model.hparams.n_vocab > 0);
```

#### 2.2.2 条件断言

```cpp
// 仅在调试模式下检查
#ifndef NDEBUG
    assert(condition);
#endif
```

### 2.3 调试宏

```cpp
// 调试输出
#ifdef DEBUG
    printf("debug: value = %d\n", value);
#endif

// 跟踪函数调用
#define TRACE() printf("TRACE: %s\n", __func__)

// 跟踪变量
#define TRACE_VAR(x) printf("TRACE: %s = %d\n", #x, x)
```

## 3. 常见错误类型

### 3.1 内存错误

#### 3.1.1 内存不足

```cpp
// 问题：内存分配失败
void * ptr = malloc(size);
if (!ptr) {
    throw std::runtime_error("out of memory");
}
```

#### 3.1.2 访问越界

```cpp
// 问题：数组越界访问
if (index >= array.size()) {
    throw std::runtime_error("index out of bounds");
}

// 访问前检查
if (i < logits_count.size() && j < logits_count[i]) {
    // 安全访问
}
```

#### 3.1.3 野指针

```cpp
// 问题：使用空指针
if (!tensor) {
    throw std::runtime_error("null tensor pointer");
}

// 检查指针有效性
if (!ctx || !ctx->data) {
    throw std::runtime_error("invalid context");
}
```

### 3.2 文件错误

#### 3.2.1 文件不存在

```cpp
// 问题：文件不存在
struct llama_file file(fname, "rb", false);
if (!file.file_id()) {
    throw std::runtime_error("file not found: " + std::string(fname));
}
```

#### 3.2.2 文件读取错误

```cpp
// 问题：文件读取错误
void read_raw(void * ptr, size_t len) {
    size_t nread = fread(ptr, 1, len, file);
    if (nread != len) {
        throw std::runtime_error("file read error");
    }
}
```

#### 3.2.3 文件格式错误

```cpp
// 问题：文件格式不正确
uint32_t magic = read_u32();
if (magic != EXPECTED_MAGIC) {
    throw std::runtime_error("invalid file format");
}
```

### 3.3 模型错误

#### 3.3.1 模型加载失败

```cpp
// 问题：模型加载失败
auto model = llama_model_load_from_file(fname, params);
if (!model) {
    throw std::runtime_error("failed to load model");
}
```

#### 3.3.2 模型不兼容

```cpp
// 问题：模型版本不兼容
if (version != EXPECTED_VERSION) {
    throw std::runtime_error("model version mismatch");
}
```

#### 3.3.3 模型参数错误

```cpp
// 问题：模型参数无效
if (n_vocab <= 0 || n_embd <= 0) {
    throw std::runtime_error("invalid model parameters");
}
```

### 3.4 后端错误

#### 3.4.1 后端初始化失败

```cpp
// 问题：后端初始化失败
ggml_backend_t backend = ggml_backend_reg_init_backend(reg, params);
if (!backend) {
    throw std::runtime_error("failed to initialize backend");
}
```

#### 3.4.2 GPU 内存不足

```cpp
// 问题：GPU 内存不足
ggml_backend_buffer_t buffer = ggml_backend_alloc_buffer(backend, size);
if (!buffer) {
    throw std::runtime_error("GPU out of memory");
}
```

#### 3.4.3 计算失败

```cpp
// 问题：计算失败
ggml_status ret = ggml_backend_sched_graph_compute_async(sched, gf);
if (ret != GGML_STATUS_SUCCESS) {
    throw std::runtime_error("computation failed");
}
```

### 3.5 参数错误

#### 3.5.1 无效参数

```cpp
// 问题：参数超出范围
if (n_threads < 1 || n_threads > 256) {
    throw std::runtime_error("n_threads must be between 1 and 256");
}
```

#### 3.5.2 参数不匹配

```cpp
// 问题：参数不匹配
if (batch.n_tokens != n_ctx) {
    throw std::runtime_error("batch size mismatch");
}
```

## 4. 调试工具

### 4.1 编译器调试选项

#### 4.1.1 Debug 构建

```bash
# Debug 构建
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

#### 4.1.2 调试符号

```cmake
# 确保包含调试符号
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
```

#### 4.1.3 优化级别

```bash
# 禁用优化以简化调试
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

### 4.2 内存调试工具

#### 4.2.1 AddressSanitizer

```bash
# 编译时启用 AddressSanitizer
cmake -B build -DLLAMA_SANITIZE_ADDRESS=ON
cmake --build build

# 运行程序
./build/your_program
```

#### 4.2.2 Valgrind

```bash
# 使用 Valgrind 检测内存错误
valgrind --leak-check=full --show-leak-kinds=all ./your_program

# 使用 Helgrind 检测线程错误
valgrind --tool=helgrind ./your_program

# 使用 Drd 检测数据竞争
valgrind --tool=drd ./your_program
```

### 4.3 性能调试工具

#### 4.3.1 perf (Linux)

```bash
# 记录性能数据
perf record -g ./your_program

# 查看性能报告
perf report

# 查看火焰图
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg
```

#### 4.3.2 NVIDIA Nsight

```bash
# 记录 GPU 性能数据
nsys profile --stats=true ./your_program

# 查看性能报告
nsight-sys ./your_program
```

#### 4.3.3 Intel VTune

```bash
# 使用 VTune 分析
vtune -collect hotspots ./your_program
```

## 5. 常见调试场景

### 5.1 内存泄漏

**检测方法**：
```bash
# 使用 Valgrind
valgrind --leak-check=full ./your_program

# 使用 AddressSanitizer
cmake -B build -DLLAMA_SANITIZE_ADDRESS=ON
./build/your_program
```

**调试步骤**：
1. 运行程序
2. 检查泄漏报告
3. 找到泄漏位置
4. 修复泄漏

### 5.2 崩溃

**检测方法**：
```bash
# 使用 GDB
gdb ./your_program
(gdb) run
(gdb) backtrace

# 使用 LLDB
lldb ./your_program
(lldb) run
(lldb) bt
```

**调试步骤**：
1. 使用调试器运行
2. 触发崩溃
3. 查看堆栈跟踪
4. 分析崩溃原因

### 5.3 数据竞争

**检测方法**：
```bash
# 使用 ThreadSanitizer
cmake -B build -DLLAMA_SANITIZE_THREAD=ON
./build/your_program

# 使用 Helgrind
valgrind --tool=helgrind ./your_program
```

**调试步骤**：
1. 运行检测工具
2. 查看竞争报告
3. 分析竞争位置
4. 添加同步机制

### 5.4 性能问题

**检测方法**：
```bash
# 使用性能分析工具
perf record -g ./your_program
perf report
```

**调试步骤**：
1. 记录性能数据
2. 找到热点函数
3. 分析热点原因
4. 优化热点代码

## 6. 错误预防

### 6.1 代码审查

- 审查错误处理逻辑
- 检查边界条件
- 验证资源管理
- 确保异常安全

### 6.2 单元测试

```cpp
// 测试错误处理
TEST(ErrorHandling, InvalidInput) {
    EXPECT_THROW(create_context(invalid_params), std::runtime_error);
}

TEST(ErrorHandling, MemoryAllocation) {
    EXPECT_THROW(allocate_too_much_memory(), std::runtime_error);
}
```

### 6.3 静态分析

```bash
# 使用 Clang Static Analyzer
scan-build make

# 使用 Cppcheck
cppcheck --enable=all your_code.cpp

# 使用 Clang-Tidy
clang-tidy your_code.cpp
```

## 7. 最佳实践

### 7.1 错误处理

```cpp
// 好的做法：检查所有错误
int result = function_that_might_fail();
if (result != SUCCESS) {
    handle_error(result);
}

// 避免：忽略错误
function_that_might_fail();  // 危险！
```

### 7.2 资源管理

```cpp
// 好的做法：使用 RAII
{
    Resource r = acquire_resource();
    use_resource(r);
}  // 自动释放

// 避免：手动管理
Resource * r = acquire_resource();
use_resource(r);
// 忘记释放 r
```

### 7.3 日志记录

```cpp
// 好的做法：记录关键事件
LLAMA_LOG_INFO("model loaded successfully\n");
LLAMA_LOG_ERROR("failed to load model: %s\n", error_message.c_str());

// 避免：过度日志
LLAMA_LOG_DEBUG("variable x = %d\n", x);  // 太多调试日志
```

### 7.4 断言使用

```cpp
// 好的做法：断言不变量
assert(n_tokens > 0);
assert(buffer != nullptr);

// 避免：断言条件性代码
assert(allocate_memory());  // 不要在断言中执行副作用
```

## 8. 总结

llama.cpp 错误处理与调试要点：

1. **错误检测**：检查参数、状态、资源
2. **错误处理**：返回值、异常、状态码
3. **错误恢复**：中止回调、状态重置
4. **日志记录**：适当级别的日志
5. **调试工具**：调试器、内存检测、性能分析
6. **预防措施**：代码审查、单元测试、静态分析

遵循这些注意事项可以有效处理错误和调试问题，提高代码质量和稳定性。