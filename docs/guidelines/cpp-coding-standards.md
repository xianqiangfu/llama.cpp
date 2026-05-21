# llama.cpp C/C++ 编码规范与代码风格

本文档总结 llama.cpp 项目的 C/C++ 编码规范与代码风格，供开发者参考。

## 1. 代码风格概述

llama.cpp 项目采用现代 C/C++ 编码风格，注重代码的可读性、可维护性和性能。项目主要使用 C++17 标准，但也保持与 C 语言的兼容性。

## 2. 命名规范

### 2.1 函数命名

- **公共 API 函数**：使用 `llama_` 前缀（C 接口）
  ```c
  const char * llama_flash_attn_type_name(enum llama_flash_attn_type flash_attn_type);
  void llama_backend_init(void);
  ```

- **内部函数**：使用小写字母和下划线分隔
  ```cpp
  static llm_graph_type ctx_type_to_graph_type(llama_context_type ctx_type);
  size_t llama_max_devices(void);
  ```

- **类成员函数**：使用小写字母和下划线分隔
  ```cpp
  void llama_context::attach_threadpool(
          ggml_threadpool_t threadpool,
          ggml_threadpool_t threadpool_batch);
  ```

### 2.2 变量命名

- **局部变量**：使用小写字母和下划线分隔
  ```cpp
  const auto & hparams = model.hparams;
  int64_t t_start_us = 0;
  ```

- **成员变量**：使用小写字母和下划线分隔
  ```cpp
  struct llama_context {
      llama_cparams cparams;
      llama_adapter_cvec_ptr cvec;
      std::unique_ptr<llama_memory_i> memory;
  };
  ```

- **结构体成员**：使用小写字母和下划线分隔
  ```cpp
  struct common_cpu_params {
      int      n_threads                   = -1;
      bool     cpumask[GGML_MAX_N_THREADS] = {false};
      enum ggml_sched_priority  priority   = GGML_SCHED_PRIO_NORMAL;
  };
  ```

### 2.3 类型命名

- **枚举类型**：使用大写字母和下划线分隔
  ```cpp
  enum llama_vocab_type {
      LLAMA_VOCAB_TYPE_NONE   = 0,
      LLAMA_VOCAB_TYPE_SPM    = 1,
      LLAMA_VOCAB_TYPE_BPE    = 2,
  };
  ```

- **结构体**：使用小写字母和下划线分隔，`struct` 关键字
  ```cpp
  struct llama_model;
  struct llama_context;
  struct llama_sampler;
  ```

- **类**：使用大写字母和下划线分隔，或使用 Pimpl 模式
  ```cpp
  class llama_batch_allocr;
  class llama_io_read_i;
  class llama_io_write_i;
  ```

### 2.4 常量命名

- **宏定义**：使用大写字母和下划线分隔
  ```cpp
  #define LLAMA_DEFAULT_SEED 0xFFFFFFFF
  #define LLAMA_TOKEN_NULL -1
  #define LLAMA_FILE_MAGIC_GGLA 0x67676c61u
  ```

- **常量变量**：使用小写字母和下划线分隔
  ```cpp
  static const bool SUPPORTED;
  ```

### 2.5 文件命名

- **头文件**：使用小写字母和连字符或下划线分隔
  ```
  llama.h
  llama-context.h
  llama-mmap.h
  ```

- **源文件**：使用小写字母和连字符或下划线分隔
  ```
  llama.cpp
  llama-context.cpp
  llama-mmap.cpp
  ```

## 3. 注释规范

### 3.1 文件头注释

每个源文件和头文件都应包含描述性注释：
```cpp
// llama.cpp - LLaMA模型主接口实现
// 本文件实现了LLaMA模型的主要公共接口函数，包括后端初始化、设备支持查询、上下文管理等
```

```cpp
// llama.h - LLaMA C接口头文件
// 本文件提供了LLaMA模型的C语言公共API接口，支持C和C++调用
```

### 3.2 函数注释

公共 API 函数应包含详细的注释：
```cpp
// 获取Flash Attention类型名称
const char * llama_flash_attn_type_name(enum llama_flash_attn_type flash_attn_type);
```

### 3.3 结构体/类注释

复杂的数据结构应包含注释说明：
```cpp
// "memory" as in abstract memory for the context
struct llama_memory_i;
struct llama_memory_context_i;

// stores copy of the memory in device buffer. used for fast state save/load
struct llama_memory_buffer {
    int n_tensors = 0;
    size_t total_size = 0;
    ggml_backend_buffer_ptr buf;
    ggml_context_ptr ctx;
    std::vector<ggml_tensor *> org;
    std::vector<ggml_tensor *> cpy;
};
```

### 3.4 内联注释

重要代码段应包含解释性注释：
```cpp
// Initialize backend samplers here so they are part of the sampling graph
// before the reserve passes run later in this function. This avoids a later
// re-reserve when graph nodes change.
if (params.samplers != nullptr && params.n_samplers > 0) {
    // ...
}

// TODO warning when creating llama_context with awkward ctx size that is not a power of 2,
//     may need to be backend-dependent
```

## 4. 格式规范

### 4.1 缩进

- 使用 4 个空格缩进
- 不使用制表符（Tab）

### 4.2 括号

- 左括号另起一行（函数定义）或同行（条件语句）
- 代码块使用统一风格
```cpp
if (condition) {
    // code
}

llama_context::llama_context(
        const llama_model & model,
              llama_context_params params) :
    model(model),
    cvec(std::make_unique<llama_adapter_cvec>()) {
    // code
}
```

### 4.3 空格

- 操作符两侧使用空格
- 逗号后使用空格
- 括号内部不使用空格（除非需要分隔参数）
```cpp
cparams.n_seq_max = std::max(1u, params.n_seq_max);
cparams.n_threads        = params.n_threads;
cparams.n_threads_batch  = params.n_threads_batch;
```

### 4.4 行长度

- 建议每行不超过 100 字符
- 超长行适当换行，保持对齐
```cpp
LLAMA_LOG_DEBUG("%s: n_rs_seq=%u requested but model arch does not support recurrent partial rollback; clamping to 0\n",
                __func__, cparams.n_rs_seq);
```

### 4.5 空行

- 函数之间使用空行分隔
- 逻辑块之间使用空行分隔
- 相关的声明和实现分组放置

## 5. 代码组织规范

### 5.1 头文件包含

- 使用相对路径或绝对路径包含
- 系统头文件在前，项目头文件在后
- 按字母顺序排列
```cpp
#include <cinttypes>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

#include "ggml.h"
#include "llama-arch.h"
#include "llama-graph.h"
#include "llama-impl.h"
```

### 5.2 命名空间

- 项目使用 C 接口，通常不使用命名空间
- 内部实现可以使用匿名命名空间
```cpp
namespace {
    // internal functions
}
```

### 5.3 前置声明

- 尽量使用前置声明减少依赖
- 头文件中使用前置声明代替完整包含
```cpp
struct llama_model;
class llama_batch_allocr;
class llama_io_read_i;
```

### 5.4 Pimpl 模式

- 复杂类使用 Pimpl 模式隐藏实现细节
```cpp
struct llama_file {
    // public interface
private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
```

## 6. 类型使用规范

### 6.1 基本类型

- 使用精确大小的类型定义
- 优先使用标准类型
```cpp
typedef int32_t llama_pos;
typedef int32_t llama_token;
typedef int32_t llama_seq_id;
```

### 6.2 智能指针

- 优先使用 `std::unique_ptr` 管理资源
- 适当使用 `std::shared_ptr` 和 `std::weak_ptr`
```cpp
std::unique_ptr<llama_memory_i> memory;
std::unique_ptr<llama_adapter_cvec> cvec;
std::unique_ptr<llama_adapter_loras> loras;
std::unique_ptr<llama_batch_allocr> balloc;
```

### 6.3 容器

- 使用标准容器
```cpp
std::vector<int32_t> output_ids;
std::map<llama_seq_id, llama_sampler *> samplers;
std::vector<std::pair<ggml_backend_t, ggml_backend_set_n_threads_t>> set_n_threads_fns;
```

### 6.4 类型别名

- 使用 `using` 定义类型别名
```cpp
using llama_files  = std::vector<std::unique_ptr<llama_file>>;
using llama_mmaps  = std::vector<std::unique_ptr<llama_mmap>>;
using llama_mlocks = std::vector<std::unique_ptr<llama_mlock>>;
using llama_tokens = std::vector<llama_token>;
```

## 7. 错误处理规范

### 7.1 返回值

- 使用布尔返回值表示成功/失败
- 使用枚举或整数返回状态码
```cpp
bool llama_supports_mmap(void);
bool llama_supports_mlock(void);
int encode(const llama_batch & batch_inp);
int decode(const llama_batch & batch_inp);
```

### 7.2 异常

- 使用 `std::runtime_error` 抛出异常
- 提供有意义的错误消息
```cpp
if (cparams.n_seq_max > LLAMA_MAX_SEQ) {
    throw std::runtime_error("n_seq_max must be <= " + std::to_string(LLAMA_MAX_SEQ));
}
```

### 7.3 日志

- 使用日志宏记录信息
- 根据日志级别选择合适的宏
```cpp
LLAMA_LOG_INFO("%s: constructing llama_context\n", __func__);
LLAMA_LOG_DEBUG("%s: n_rs_seq=%u requested but model arch does not support recurrent partial rollback; clamping to 0\n",
                __func__, cparams.n_rs_seq);
```

## 8. 性能相关规范

### 8.1 引用传递

- 大对象使用引用传递避免拷贝
- 使用 `const` 引用传递只读对象
```cpp
llama_context::llama_context(
        const llama_model & model,
              llama_context_params params);

const llama_model & get_model() const;
```

### 8.2 移动语义

- 使用移动语义优化资源转移
```cpp
llama_mmap(struct llama_mmap &&) = default;
llama_mmap & operator=(struct llama_mmap &&) = default;
```

### 8.3 内联

- 小函数使用 `inline` 关键字
- 热路径函数可能需要内联
```cpp
static inline llm_graph_type ctx_type_to_graph_type(llama_context_type ctx_type);
```

## 9. 平台相关规范

### 9.1 条件编译

- 使用宏进行平台相关编译
```cpp
#ifdef LLAMA_SHARED
#    if defined(_WIN32) && !defined(__MINGW32__)
#        ifdef LLAMA_BUILD
#            define LLAMA_API __declspec(dllexport)
#        else
#            define LLAMA_API __declspec(dllimport)
#        endif
#    else
#        define LLAMA_API __attribute__ ((visibility ("default")))
#    endif
#else
#    define LLAMA_API
#endif
```

### 9.2 编译器特定

- 使用宏处理编译器差异
```cpp
#ifdef __GNUC__
#    define DEPRECATED(func, hint) func __attribute__((deprecated(hint)))
#elif defined(_MSC_VER)
#    define DEPRECATED(func, hint) __declspec(deprecated(hint)) func
#else
#    define DEPRECATED(func, hint) func
#endif
```

### 9.3 C/C++ 兼容

- 使用 `extern "C"` 包装 C 接口
```cpp
#ifdef __cplusplus
extern "C" {
#endif

    // C language interface

#ifdef __cplusplus
}
#endif
```

## 10. 最佳实践

### 10.1 代码复用

- 提取公共函数避免重复代码
- 使用模板实现泛型算法

### 10.2 资源管理

- 使用 RAII 原则管理资源
- 优先使用智能指针

### 10.3 const 正确性

- 使用 `const` 修饰不修改的对象
- 成员函数使用 `const` 修饰表示不修改对象状态
```cpp
const llama_model & get_model() const;
const llama_cparams & get_cparams() const;
ggml_backend_sched_t get_sched() const;
```

### 10.4 初始化

- 使用初始化列表初始化成员变量
- 使用 `= default` 或 `= delete` 显式声明特殊成员函数
```cpp
llama_mmap(const llama_mmap &) = delete;
llama_mmap & operator=(const llama_mmap &) = delete;

~llama_mmap();
```

## 11. 总结

llama.cpp 项目的编码规范注重：

1. **可读性**：清晰的命名、适当的注释、合理的代码组织
2. **可维护性**：模块化设计、清晰的接口、良好的错误处理
3. **性能**：高效的资源管理、合理的类型选择、优化的代码结构
4. **兼容性**：跨平台支持、C/C++ 兼容、多编译器支持

遵循这些规范有助于编写高质量、易于维护的代码，同时保持项目的性能和兼容性。