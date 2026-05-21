# CMake 模块说明

本目录包含 llama.cpp 项目的 CMake 构建系统和模块配置文件。

## 目录结构

```
cmake/
├── arm64-apple-clang.cmake                    # Apple ARM64 Clang 工具链
├── arm64-linux-clang.cmake                    # Linux ARM64 Clang 工具链
├── arm64-windows-llvm.cmake                   # Windows ARM64 LLVM 工具链
├── build-info.cmake                           # 构建信息模块
├── common.cmake                               # 通用 CMake 配置
├── download-models.cmake                      # 模型下载模块
├── git-vars.cmake                             # Git 变量模块
├── license.cmake                              # 许可证信息模块
├── llama-config.cmake.in                      # LLaMA 配置模板
├── llama.pc.in                                # pkg-config 模板
├── riscv64-spacemit-linux-gnu-gcc.cmake       # RISC-V Spacemit GCC 工具链
└── x64-windows-llvm.cmake                     # Windows x64 LLVM 工具链
```

## 工具链文件

### 交叉编译工具链

这些文件用于交叉编译到特定平台：

| 文件 | 目标平台 | 编译器 |
|------|---------|--------|
| `arm64-apple-clang.cmake` | Apple ARM64 | Clang |
| `arm64-linux-clang.cmake` | Linux ARM64 | Clang |
| `arm64-windows-llvm.cmake` | Windows ARM64 | LLVM/Clang |
| `riscv64-spacemit-linux-gnu-gcc.cmake` | RISC-V Spacemit | GCC |
| `x64-windows-llvm.cmake` | Windows x64 | LLVM/Clang |

### 使用工具链文件

```bash
# ARM64 Apple
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-apple-clang.cmake ..

# ARM64 Linux
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-linux-clang.cmake ..

# RISC-V Spacemit
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/riscv64-spacemit-linux-gnu-gcc.cmake ..
```

## 核心模块

### common.cmake

通用 CMake 配置模块，包含：

1. **编译标志控制**
   - `LLAMA_FATAL_WARNINGS` - 将警告视为错误
   - `LLAMA_ALL_WARNINGS` - 启用所有警告

2. **消毒器支持**
   - `LLAMA_SANITIZE_THREAD` - 线程消毒器
   - `LLAMA_SANITIZE_ADDRESS` - 地址消毒器
   - `LLAMA_SANITIZE_UNDEFINED` - 未定义行为消毒器

3. **编译器优化**

示例：
```cmake
# 启用所有警告并视为错误
set(LLAMA_FATAL_WARNINGS ON)
set(LLAMA_ALL_WARNINGS ON)

# 启用地址消毒器
set(LLAMA_SANITIZE_ADDRESS ON)
```

### build-info.cmake

构建信息模块，提供：
- 构建时间戳
- 构建类型
- 编译器版本
- 平台信息

### git-vars.cmake

Git 变量模块，提供：
- Git 分支名称
- Git 提交哈希
- Git 标签
- 仓库状态

示例：
```cmake
include(git-vars)
message(STATUS "Git commit: ${GIT_COMMIT_HASH}")
message(STATUS "Git branch: ${GIT_BRANCH_NAME}")
```

### download-models.cmake

模型下载模块，用于自动下载测试模型。

示例：
```cmake
include(download-models)
download_model("tinyllama" "https://example.com/tinyllama.gguf")
```

### license.cmake

许可证信息模块，提供：
- 许可证文本
- 版权信息
- 第三方许可证

### llama-config.cmake.in

LLaMA CMake 配置模板，用于安装后的项目。

### llama.pc.in

pkg-config 模板，用于其他项目集成 llama.cpp。

## 构建选项

### 后端选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `LLAMA_CUDA` | 启用 CUDA 后端 | OFF |
| `LLAMA_METAL` | 启用 Metal 后端 | OFF |
| `LLAMA_ROCM` | 启用 ROCm 后端 | OFF |
| `LLAMA_SYCL` | 启用 SYCL 后端 | OFF |
| `LLAMA_VULKAN` | 启用 Vulkan 后端 | OFF |
| `LLAMA_OPENVINO` | 启用 OpenVINO 后端 | OFF |
| `LLAMA_CANN` | 启用 CANN 后端 | OFF |
| `LLAMA_MUSA` | 启用 MUSA 后端 | OFF |
| `LLAMA_OPENCL` | 启用 OpenCL 后端 | OFF |
| `LLAMA_WEBGPU` | 启用 WebGPU 后端 | OFF |

### 优化选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `LLAMA_ALL_WARNINGS` | 启用所有警告 | OFF |
| `LLAMA_FATAL_WARNINGS` | 将警告视为错误 | OFF |
| `LLAMA_SANITIZE_THREAD` | 线程消毒器 | OFF |
| `LLAMA_SANITIZE_ADDRESS` | 地址消毒器 | OFF |
| `LLAMA_SANITIZE_UNDEFINED` | 未定义行为消毒器 | OFF |

### 功能选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `LLAMA_BENCH` | 构建基准测试工具 | OFF |
| `LLAMA_SERVER` | 构建服务器 | ON |
| `LLAMA_TESTS` | 构建测试 | OFF |
| `LLAMA_EXAMPLES` | 构建示例 | ON |

## 构建示例

### 基本构建

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 启用 CUDA

```bash
cmake -DLLAMA_CUDA=ON ..
cmake --build .
```

### 启用 Metal (macOS)

```bash
cmake -DLLAMA_METAL=ON ..
cmake --build .
```

### 启用所有警告

```bash
cmake -DLLAMA_ALL_WARNINGS=ON -DLLAMA_FATAL_WARNINGS=ON ..
cmake --build .
```

### 使用地址消毒器

```bash
cmake -DLLAMA_SANITIZE_ADDRESS=ON ..
cmake --build .
```

### 交叉编译到 ARM64

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-linux-clang.cmake ..
cmake --build .
```

## 安装和集成

### 安装

```bash
cmake --install . --prefix /usr/local
```

安装后，其他项目可以通过以下方式使用 llama.cpp：

```cmake
find_package(llama REQUIRED)
target_link_libraries(myapp llama::llama)
```

### 使用 pkg-config

```bash
pkg-config --cflags --libs llama
```

## 编译标志

### GCC/Clang 标志

`common.cmake` 中定义的编译标志：

```cmake
# 基础警告标志
-Wshadow -Wstrict-prototypes -Wpointer-arith -Wmissing-prototypes
-Werror=implicit-int -Werror=implicit-function-declaration

# C++ 特定
-Wmissing-declarations -Wmissing-noreturn

# 通用警告
-Wall -Wextra -Wpedantic -Wcast-qual -Wno-unused-function
```

### MSVC 标志

对于 MSVC 编译器，使用 `/WX` 标志将警告视为错误。

## 交叉编译

### ARM64 Linux

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-linux-clang.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

### RISC-V Spacemit

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/riscv64-spacemit-linux-gnu-gcc.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

### Windows ARM64

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-windows-llvm.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

## 依赖管理

### 自动依赖下载

CMake 配置会自动下载和配置必要的依赖：

- GGML（核心库）
- 不需要额外的外部依赖

### 可选依赖

某些后端需要额外的依赖：

- CUDA: CUDA Toolkit
- Metal: Xcode 和 Metal SDK
- ROCm: ROCm Toolkit
- SYCL: Intel oneAPI
- Vulkan: Vulkan SDK
- OpenVINO: OpenVINO Toolkit
- CANN: CANN Toolkit
- MUSA: MUSA Toolkit

## 自定义构建

### 自定义安装前缀

```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

### 自定义构建类型

```bash
# Debug 构建
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release 构建
cmake -DCMAKE_BUILD_TYPE=Release ..

# RelWithDebInfo 构建
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### 控制构建并行度

```bash
cmake --build . -j$(nproc)
```

## 故障排除

### 问题：找不到编译器

解决方案：
- 指定 C/C++ 编译器
```bash
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
```

### 问题：后端依赖未找到

解决方案：
- 设置环境变量指向依赖路径
```bash
export CUDA_HOME=/usr/local/cuda
cmake -DLLAMA_CUDA=ON ..
```

### 问题：工具链文件错误

解决方案：
- 验证工具链文件路径
- 检查工具链文件内容

## 相关文档

- [llama.cpp 主 README](../README.md)
- [CMake 文档](https://cmake.org/documentation/)
- [DevOps 配置](../.devops/README_ZH.md)
- [CI 配置](../ci/README_ZH.md)