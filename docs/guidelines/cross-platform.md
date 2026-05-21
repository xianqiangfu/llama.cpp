# llama.cpp 跨平台兼容性注意事项

本文档总结 llama.cpp 项目的跨平台兼容性注意事项，帮助开发者编写跨平台兼容的代码。

## 1. 支持的平台

llama.cpp 支持多种平台：
- **Linux** (x86_64, ARM64, RISC-V, S390X)
- **Windows** (x86_64, ARM64)
- **macOS** (x86_64, ARM64)
- **Android** (ARM64, x86_64)
- **WebAssembly** (via Emscripten)
- **iOS** (ARM64)

## 2. 平台差异处理

### 2.1 目录分隔符

```cpp
// 目录分隔符定义
#ifdef _WIN32
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif // _WIN32
```

### 2.2 动态库导出/导入

```cpp
// 共享库导入/导出宏定义
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

**注意事项**：
- Windows 使用 `__declspec(dllexport/import)`
- 其他平台使用 `__attribute__ ((visibility ("default")))`
- 静态库不需要这些宏

### 2.3 弃用函数标记

```cpp
// 弃用函数标记宏
#ifdef __GNUC__
#    define DEPRECATED(func, hint) func __attribute__((deprecated(hint)))
#elif defined(_MSC_VER)
#    define DEPRECATED(func, hint) __declspec(deprecated(hint)) func
#else
#    define DEPRECATED(func, hint) func
#endif
```

### 2.4 编译器特定警告

```cpp
// MSVC 警告控制
#if defined(_MSC_VER)
#pragma warning(disable: 4244 4267) // possible loss of data
#endif

// MSVC 编译选项
if (MSVC) {
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/bigobj>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/bigobj>")
}

// Windows 安全警告
if (WIN32) {
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
}
```

## 3. 构建系统兼容性

### 3.1 CMake 最低版本

```cmake
cmake_minimum_required(VERSION 3.14...3.28)
project("llama.cpp" C CXX)
```

### 3.2 构建类型

```cmake
if (NOT XCODE AND NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
```

### 3.3 共享库控制

```cmake
option(BUILD_SHARED_LIBS "build shared libraries" ${BUILD_SHARED_LIBS_DEFAULT})

if (EMSCRIPTEN)
    set(BUILD_SHARED_LIBS_DEFAULT OFF)
elseif (MINGW)
    set(BUILD_SHARED_LIBS_DEFAULT OFF)
else
    set(BUILD_SHARED_LIBS_DEFAULT ON)
endif()
```

### 3.4 WebAssembly 特殊配置

```cmake
if (EMSCRIPTEN)
    # 使用 64 位内存
    if (LLAMA_WASM_MEM64)
      add_compile_options("-sMEMORY64=1")
      add_link_options("-sMEMORY64=1")
    endif()
    add_link_options("-sALLOW_MEMORY_GROWTH=1")

    option(LLAMA_WASM_SINGLE_FILE "llama: embed WASM inside the generated llama.js" OFF)
    option(LLAMA_BUILD_HTML "llama: build HTML file" ON)
    if (LLAMA_BUILD_HTML)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()
endif()
```

### 3.5 iOS 特殊配置

```cmake
if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(LLAMA_TOOLS_INSTALL_DEFAULT OFF)
else()
    set(LLAMA_TOOLS_INSTALL_DEFAULT ${LLAMA_STANDALONE})
endif()
```

## 4. 文件系统兼容性

### 4.1 路径最大长度

```cpp
size_t llama_path_max();
```

**注意事项**：
- Windows: 260 字符（UNICODE 版本 32767 字符）
- Linux: PATH_MAX（通常 4096 字节）
- macOS: PATH_MAX（通常 1024 字节）

### 4.2 内存映射支持

```cpp
struct llama_mmap {
    llama_mmap(struct llama_file * file, size_t prefetch = (size_t) -1, bool numa = false);

    static const bool SUPPORTED;

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
```

**注意事项**：
- Windows 支持
- Linux 支持
- macOS 支持
- 某些嵌入式系统可能不支持

### 4.3 内存锁定支持

```cpp
struct llama_mlock {
    llama_mlock();
    ~llama_mlock();

    void init(void * ptr);
    void grow_to(size_t target_size);

    static const bool SUPPORTED;

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
```

**注意事项**：
- 需要 root 权限（Linux）
- Windows 支持
- macOS 支持
- 某些系统可能不支持

## 5. 后端兼容性

### 5.1 Metal (macOS/iOS)

```cmake
option(GGML_METAL "ggml: use Metal" ON)
```

**注意事项**：
- 仅支持 macOS 和 iOS
- Apple Silicon (M1/M2/M3) 支持最佳
- Intel Mac 也支持

### 5.2 CUDA (NVIDIA)

```cmake
option(GGML_CUDA "ggml: use CUDA" ON)
```

**注意事项**：
- 需要 CUDA Toolkit
- 支持 Linux、Windows
- 需要 NVIDIA GPU

### 5.3 Vulkan

```cmake
option(GGML_VULKAN "ggml: use Vulkan" ON)
```

**注意事项**：
- 跨平台支持
- 需要 Vulkan 驱动

### 5.4 OpenCL

```cmake
option(GGML_OPENCL "ggml: use OpenCL" ON)
```

**注意事项**：
- 跨平台支持
- 需要OpenCL 驱动

### 5.5 SYCL (Intel)

```cmake
option(GGML_SYCL "ggml: use SYCL" ON)
```

**注意事项**：
- 支持 Intel GPU
- 需要 oneAPI 工具链

## 6. 代码风格兼容性

### 6.1 头文件保护

```cpp
#ifndef LLAMA_H
#define LLAMA_H

// 内容

#endif // LLAMA_H
```

或者使用 `#pragma once`：

```cpp
#pragma once

// 内容
```

### 6.2 C/C++ 兼容

```cpp
#ifdef __cplusplus
extern "C" {
#endif

    // C 语言接口

#ifdef __cplusplus
}
#endif
```

### 6.3 类型定义

```cpp
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
```

**注意事项**：
- 使用标准 C 头文件
- 避免平台特定的类型
- 使用固定大小的整数类型

## 7. 硬件特性检测

### 7.1 CPU 特性

```cpp
// CPU 检测
bool has_avx = ggml_cpu_has_avx();
bool has_avx2 = ggml_cpu_has_avx2();
bool has_avx512 = ggml_cpu_has_avx512();
bool has_neon = ggml_cpu_has_neon();
bool has_riscv_v = ggml_cpu_has_riscv_v();
```

### 7.2 GPU 支持

```cpp
bool llama_supports_mmap(void);
bool llama_supports_mlock(void);
bool llama_supports_gpu_offload(void);
bool llama_supports_rpc(void);

size_t llama_max_devices(void);
```

## 8. 平台特定代码

### 8.1 Windows 特定

```cpp
// Windows 特定代码
#ifdef _WIN32
    // Windows 代码
    HANDLE hFile = CreateFileA(filename, ...);
    if (hFile == INVALID_HANDLE_VALUE) {
        // 错误处理
    }
    CloseHandle(hFile);
#else
    // Unix 代码
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        // 错误处理
    }
    close(fd);
#endif
```

### 8.2 Unix 特定

```cpp
#ifndef _WIN32
    #include <unistd.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
#endif
```

### 8.3 macOS 特定

```cpp
#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        // macOS 代码
    #endif
    #if TARGET_OS_IPHONE
        // iOS 代码
    #endif
#endif
```

## 9. 编译选项

### 9.1 编译器标志

```cmake
# 警告选项
option(LLAMA_ALL_WARNINGS           "llama: enable all compiler warnings"                   ON)
option(LLAMA_ALL_WARNINGS_3RD_PARTY "llama: enable all compiler warnings in 3rd party libs" OFF)
option(LLAMA_FATAL_WARNINGS         "llama: enable -Werror flag"                            OFF)

# Sanitizer 选项
option(LLAMA_SANITIZE_THREAD    "llama: enable thread sanitizer"    OFF)
option(LLAMA_SANITIZE_ADDRESS   "llama: enable address sanitizer"   OFF)
option(LLAMA_SANITIZE_UNDEFINED "llama: enable undefined sanitizer" OFF)
```

### 9.2 平台特定选项

```cmake
# Windows 特定
if (MSVC)
    # MSVC 选项
endif()

# Linux 特定
if (UNIX AND NOT APPLE)
    # Linux 选项
endif()

# macOS 特定
if (APPLE)
    # macOS 选项
endif()
```

## 10. 常见问题与解决方案

### 10.1 编译错误

**问题**：Windows 上出现编译错误

**解决方案**：
```cmake
# 使用 MSVC 编译器
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release

# 或者使用 clang-cl
cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -B build
cmake --build build
```

### 10.2 链接错误

**问题**：出现未定义的符号错误

**解决方案**：
- 检查链接库
- 检查符号可见性
- 检查 ABI 兼容性

### 10.3 运行时错误

**问题**：程序在不同平台行为不一致

**解决方案**：
- 检查整数大小
- 检查字节序
- 检查对齐方式
- 检查浮点精度

### 10.4 性能差异

**问题**：不同平台性能差异很大

**解决方案**：
- 检查硬件特性
- 优化平台特定代码
- 使用 SIMD 指令
- 调整编译选项

## 11. 测试与验证

### 11.1 多平台测试

```bash
# Linux 测试
cmake -B build && cmake --build build
ctest --test-dir build

# Windows 测试
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release
ctest --test-dir build -C Release

# macOS 测试
cmake -B build && cmake --build build
ctest --test-dir build
```

### 11.2 CI/CD

使用 GitHub Actions 进行多平台测试：

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release]
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@v3
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
      - name: Build
        run: cmake --build build --config ${{ matrix.build_type }}
      - name: Test
        run: ctest --test-dir build -C ${{ matrix.build_type }}
```

## 12. 最佳实践

### 12.1 条件编译

```cpp
// 使用平台宏进行条件编译
#if defined(_WIN32)
    // Windows 代码
#elif defined(__APPLE__)
    // macOS/iOS 代码
#elif defined(__linux__)
    // Linux 代码
#endif
```

### 12.2 类型定义

```cpp
// 使用标准类型
typedef int32_t llama_token;
typedef int32_t llama_seq_id;

// 避免平台特定类型
// 不要使用 long、size_t 等（除非必要）
```

### 12.3 字符串处理

```cpp
// 使用 UTF-8
// 避免使用平台特定的编码

// Windows 编码设置
#if defined(_MSC_VER)
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
#endif
```

### 12.4 文件操作

```cpp
// 使用跨平台的文件操作
struct llama_file {
    llama_file(const char * fname, const char * mode, bool use_direct_io = false);

    size_t tell() const;
    size_t size() const;
    void seek(size_t offset, int whence) const;
    void read_raw(void * ptr, size_t len);
    void write_raw(const void * ptr, size_t len) const;
};
```

## 13. 总结

llama.cpp 跨平台兼容性要点：

1. **使用条件编译**：正确处理平台差异
2. **标准类型**：使用标准类型定义
3. **抽象接口**：提供统一的接口
4. **测试验证**：在多平台上测试
5. **文档说明**：记录平台限制
6. **优雅降级**：不支持的功能应优雅处理

遵循这些注意事项可以编写出跨平台兼容的代码，使项目能够在多种平台上正常运行。