# llama.cpp 构建文档摘要

## 项目概述

llama.cpp 的主要产品是 `llama` 库，其 C 风格接口位于 [include/llama.h](../include/llama.h)。

项目还包括许多使用 `llama` 库的示例程序和工具，范围从简单的最小代码片段到复杂的子项目，如 OpenAI 兼容的 HTTP 服务器。

## 获取代码

```bash
git clone https://github.com/ggml-org/llama.cpp
cd llama.cpp
```

## 构建方式概览

支持以下后端：
- CPU Build
- BLAS Build（Accelerate、OpenBLAS、BLIS、Intel oneMKL）
- Metal Build（macOS 默认启用）
- SYCL（支持 Intel GPU）
- CUDA（支持 NVIDIA GPU）
- MUSA（支持 Moore Threads GPU）
- HIP（支持 AMD GPU）
- Vulkan
- CANN（支持 Ascend NPU）
- Arm® KleidiAI™
- OpenCL（支持 Adreno GPU）
- Android
- OpenVINO
- WebGPU（进行中）
- IBM Z & LinuxONE

---

## 通用编译优化

### 加速编译
- 添加 `-j` 参数并行运行多个作业，或使用自动执行此操作的生成器（如 Ninja）
  - 示例：`cmake --build build --config Release -j 8`
- 安装 [ccache](https://ccache.dev/) 加速重复编译

### Debug 构建
1. 单配置生成器（如默认 = `Unix Makefiles`）：
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ```

2. 多配置生成器（`-G` 参数设置为 Visual Studio、XCode...）：
   ```bash
   cmake -B build -G "Xcode"
   cmake --build build --config Debug
   ```

### 静态构建
```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
```

---

## 各平台构建方法

### CPU 构建

#### 基本构建
```bash
cmake -B build
cmake --build build --config Release
```

#### Windows 构建
- 安装 Visual Studio 2022
  - 工作负载标签：使用 C++ 进行桌面开发
  - 组件标签（通过搜索快速选择）：Windows 的 C++ CMake 工具、Windows 的 Git、Windows 的 C++ Clang 编译器、LLVM 工具集（clang）的 MS-Build 支持
- 始终使用 VS2022 的开发者命令提示符/PowerShell

#### ARM64 构建
```bash
cmake --preset arm64-windows-llvm-release -D GGML_OPENMP=OFF
cmake --build build-arm64-windows-llvm-release
```

#### HTTPS/TLS 支持
需要安装 OpenSSL 开发库：
- Debian/Ubuntu：`sudo apt-get install libssl-dev`
- Fedora/RHEL/Rocky/Alma：`sudo dnf install openssl-devel`
- Arch/Manjaro：`sudo pacman -S openssl`

---

### BLAS 构建

BLAS 支持可以在批处理大小大于 32（默认为 512）时提高提示词处理性能。不影响生成性能。

#### Accelerate Framework（仅 Mac）
默认启用，使用正常构建命令即可。

#### OpenBLAS（CPU 加速）
```bash
cmake -B build -DGGML_BLAS=ON -DGGML_BLAS_VENDOR=OpenBLAS
cmake --build build --config Release
```

#### BLIS
参考 [BLIS.md](./backend/BLIS.md)。

#### Intel oneMKL
不支持 Intel GPU，需要 oneAPI 编译器。

手动安装：
```bash
source /opt/intel/oneapi/setvars.sh
cmake -B build -DGGML_BLAS=ON -DGGML_BLAS_VENDOR=Intel10_64lp -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx -DGGML_NATIVE=ON
cmake --build build --config Release
```

使用 Docker：
```bash
docker run -it --rm intel/oneapi-basekit
# 然后使用上述命令
```

---

### Metal 构建（macOS）

默认启用，计算在 GPU 上运行。要在编译时禁用：
```bash
cmake -B build -DGGML_METAL=OFF
```

运行时禁用 GPU 推理：
```bash
--n-gpu-layers 0
```

---

### SYCL 构建（Intel GPU）

支持 Intel GPU（Data Center Max 系列、Flex 系列、Arc 系列、内置 GPU 和 iGPU）。

详细说明：[llama.cpp for SYCL](./backend/SYCL.md)

---

### CUDA 构建（NVIDIA GPU）

#### 基本构建
```bash
cmake -B build -DGGML_CUDA=ON
cmake --build build --config Release
```

#### 非原生构建
为所有 CUDA GPU 构建：
```bash
cmake -B build -DGGML_CUDA=ON -DGGML_NATIVE=OFF
```

#### 覆盖计算能力规格
1. 记录 NVIDIA 设备的 `Compute Capability`：[CUDA: Your GPU Compute Capability](https://developer.nvidia.com/cuda-gpus)
   ```
   GeForce RTX 4090      8.9
   GeForce RTX 3080 Ti   8.6
   GeForce RTX 3070      8.6
   ```

2. 在 `CMAKE_CUDA_ARCHITECTURES` 列表中手动列出每个不同的 `Compute Capability`：
   ```bash
   cmake -B build -DGGML_CUDA=ON -DCMAKE_CUDA_ARCHITECTURES="86;89"
   ```

#### 覆盖 CUDA 版本
```bash
cmake -B build -DGGML_CUDA=ON -DCMAKE_CUDA_COMPILER=/opt/cuda-11.7/bin/nvcc -DCMAKE_INSTALL_RPATH="/opt/cuda-11.7/lib64;$ORIGIN" -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON
```

#### CUDA 运行时环境变量

**CUDA_VISIBLE_DEVICES**
```bash
CUDA_VISIBLE_DEVICES="-0" ./build/bin/llama-server --model /srv/models/llama.gguf
```

**CUDA_SCALE_LAUNCH_QUEUES**
控制 CUDA 命令缓冲区的大小，设置为 `4x` 可以增加 4 倍缓冲区大小，特别有利于多 GPU 管道并行设置。

**GGML_CUDA_FORCE_CUBLAS_COMPUTE_32F**
在 FP16 cuBLAS 中使用 FP32 计算类型，防止可能的数值溢出，但提示词处理较慢。

**GGML_CUDA_FORCE_CUBLAS_COMPUTE_16F**
在 FP16 cuBLAS 中强制使用 FP16 计算类型（适用于 V100、CDNA 和 RDNA4）。

**GGML_CUDA_ENABLE_UNIFIED_MEMORY=1**
在 Linux 中启用统一内存，允许在 GPU VRAM 耗尽时交换到系统 RAM。

**GGML_CUDA_P2P**
在多个 GPU 之间启用点对点访问。

#### CUDA 编译选项

| 选项 | 值 | 默认 | 描述 |
|------|-----|------|------|
| GGML_CUDA_FORCE_MMQ | 布尔值 | false | 强制对量化模型使用自定义矩阵乘法内核 |
| GGML_CUDA_FORCE_CUBLAS | 布尔值 | false | 强制对量化模型使用 FP16 cuBLAS |
| GGML_CUDA_PEER_MAX_BATCH_SIZE | 正整数 | 128 | 启用多 GPU 点对点访问的最大批处理大小 |
| GGML_CUDA_FA_ALL_QUANTS | 布尔值 | false | 编译 FlashAttention CUDA 内核的所有 KV 缓存量化类型 |

---

### MUSA 构建（Moore Threads GPU）

#### 基本构建
```bash
cmake -B build -DGGML_MUSA=ON
cmake --build build --config Release
```

#### 覆盖计算能力规格
```bash
cmake -B build -DGGML_MUSA=ON -DMUSA_ARCHITECTURES="21"
cmake --build build --config Release
```

#### MUSA 运行时环境变量
```bash
MUSA_VISIBLE_DEVICES="-0" ./build/bin/llama-server --model /srv/models/llama.gguf
```

---

### HIP 构建（AMD GPU）

#### Linux 构建
```bash
HIPCXX="$(hipconfig -l)/clang" HIP_PATH="$(hipconfig -R)" \
    cmake -S . -B build -DGGML_HIP=ON -DGPU_TARGETS=gfx1030 -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --config Release -- -j 16
```

启用 RDNA3+ 或 CDNA 架构的 flash attention 性能：
```bash
-DGGML_HIP_ROCWMMA_FATTN=ON
```

#### Windows 构建
```bash
set PATH=%HIP_PATH%\bin;%PATH%
cmake -S . -B build -G Ninja -DGPU_TARGETS=gfx1100 -DGGML_HIP=ON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### HIP 环境变量
- `HIP_VISIBLE_DEVICES`：指定要使用的 GPU
- `HSA_OVERRIDE_GFX_VERSION`：将不受支持的 GPU 映射到类似 GPU（例如，RDNA2 上为 10.3.0，RDNA3 上为 11.0.0）

---

### Vulkan 构建

#### Windows - w64devkit
```bash
# 复制 Vulkan 依赖后
cmake -B build -DGGML_VULKAN=ON
cmake --build build --config Release
```

#### Windows - Git Bash MINGW64
```bash
cmake -B build -DGGML_VULKAN=ON
cmake --build build --config Release
```

#### MSYS2
```bash
pacman -S git \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-vulkan-devel \
    mingw-w64-ucrt-x86_64-shaderc \
    mingw-w64-ucrt-x86_64-spirv-headers

cmake -B build -DGGML_VULKAN=ON
cmake --build build --config Release
```

#### Linux
```bash
# 使用 LunarG Vulkan SDK
source /path/to/vulkan-sdk/setup-env.sh

# 或使用系统包
sudo apt-get install libvulkan-dev glslc spirv-headers

# 验证安装
vulkaninfo

# 构建
cmake -B build -DGGML_VULKAN=1
cmake --build build --config Release

# 测试
./build/bin/llama-cli -m "PATH_TO_MODEL" -p "Hi you how are you" -ngl 99
```

#### macOS
```bash
# MoltenVK（默认）
source /path/to/vulkan-sdk/setup-env.sh

# KosmicKrisp
export VK_ICD_FILENAMES=$VULKAN_SDK/share/vulkan/icd.d/libkosmickrisp_icd.json
export VK_DRIVER_FILES=$VULKAN_SDK/share/vulkan/icd.d/libkosmickrisp_icd.json

# 构建
cmake -B build -DGGML_VULKAN=1 -DGGML_METAL=OFF
cmake --build build --config Release
```

#### Docker
```bash
docker build -t llama-cpp-vulkan --target light -f .devops/vulkan.Dockerfile .
docker run -it --rm -v "$(pwd):/app:Z" --device /dev/dri/renderD128:/dev/dri/renderD128 --device /dev/dri/card1:/dev/dri/card1 llama-cpp-vulkan -m "/app/models/YOUR_MODEL_FILE" -p "Building a website can be done in 10 simple steps:" -n 400 -e -ngl 33
```

---

### CANN 构建（Ascend NPU）

```bash
cmake -B build -DGGML_CANN=on -DCMAKE_BUILD_TYPE=release
cmake --build build --config release

# 测试
./build/bin/llama-cli -m PATH_TO_MODEL -p "Building a website can be done in 10 steps:" -ngl 32
```

详细说明：[llama.cpp for CANN](./backend/CANN.md)

---

### ZenDNN 构建（AMD EPYC™ CPU）

```bash
# 自动构建（首次 5-10 分钟）
cmake -B build -DGGML_ZENDNN=ON
cmake --build build --config Release

# 自定义安装
cmake -B build -DGGML_ZENDNN=ON -DZENDNN_ROOT=/path/to/zendnn/install
cmake --build build --config Release
```

详细说明：[llama.cpp for ZenDNN](./backend/ZenDNN.md)

---

### Arm® KleidiAI™ 构建

```bash
cmake -B build -DGGML_CPU_KLEIDIAI=ON
cmake --build build --config Release

# 验证
./build/bin/llama-cli -m PATH_TO_MODEL -p "What is a car?"
# 输出应包含：load_tensors: CPU_KLEIDIAI model buffer size =  3474.00 MiB
```

#### SME 控制
- 未设置：自动启用 SME（如果支持并检测到）
- `GGML_KLEIDIAI_SME=0`：禁用 SME
- `GGML_KLEIDIAI_SME=<n>`：启用 SME 并假设有 n 个可用 SME 单元

---

### OpenCL 构建

详细说明：[OPENCL.md](./backend/OPENCL.md)

#### Android
```bash
# 先安装 OpenCL 头文件和 ICD 加载器
# 然后构建
cmake .. -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-28 \
  -DBUILD_SHARED_LIBS=OFF \
  -DGGML_OPENCL=ON
ninja
```

#### Windows Arm64
```powershell
cmake .. -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE="$HOME/dev/llm/llama.cpp/cmake/arm64-windows-llvm.cmake" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH="$HOME/dev/llm/opencl" `
  -DBUILD_SHARED_LIBS=OFF `
  -DGGML_OPENCL=ON
ninja
```

---

### Android 构建

详细说明：[android.md](./android.md)

---

### WebGPU 构建（进行中）

依赖 [Dawn](https://dawn.googlesource.com/dawn)。

```bash
cmake -B build -DGGML_WEBGPU=ON
cmake --build build --config Release
```

#### 浏览器支持
使用 [Emscripten](https://emscripten.org/) 将 ggml 的 WebGPU 后端编译为 WebAssembly。

---

### OpenVINO 构建

[OpenVINO](https://docs.openvino.ai/) 是一个开源工具包，用于优化和部署高性能 AI 推理，专为 Intel 硬件（CPU、GPU 和 NPU）设计。

详细说明：[OPENVINO.md](backend/OPENVINO.md)

---

### IBM Z & LinuxONE 构建

详细说明：[build-s390x.md](./build-s390x.md)

---

## GPU 加速后端注意事项

1. 使用 `-ngl 0` 选项时，GPU 可能仍用于加速部分计算。要完全禁用 GPU 加速，请使用 `--device none`。

2. 在大多数情况下，可以同时构建和使用多个后端。例如，可以使用 `-DGGML_CUDA=ON -DGGML_VULKAN=ON` 选项使用 CUDA 和 Vulkan 支持构建 llama.cpp。

3. 在运行时，使用 `--device` 选项指定要使用的后端设备。使用 `--list-devices` 选项查看可用设备列表。

4. 后端可以构建为在运行时动态加载的动态库。这允许在不同的机器上使用不同的 GPU 使用相同的 llama.cpp 二进制文件。要启用此功能，在构建时使用 `GGML_BACKEND_DL` 选项。

---

## 编译器支持

- GCC/Clang：主要支持
- MSVC（Windows）：支持 x86、x64 和 arm64
- LLVM/Clang（Windows）：支持通过预配置（preset）构建

---

## 关键 CMake 选项总结

| 选项 | 描述 |
|------|------|
| `-DGGML_BLAS=ON` | 启用 BLAS 支持 |
| `-DGGML_BLAS_VENDOR=<vendor>` | 指定 BLAS 供应商 |
| `-DGGML_METAL=ON/OFF` | 启用/禁用 Metal（macOS） |
| `-DGGML_CUDA=ON` | 启用 CUDA 支持 |
| `-DGGML_MUSA=ON` | 启用 MUSA 支持 |
| `-DGGML_HIP=ON` | 启用 HIP 支持 |
| `-DGGML_VULKAN=ON` | 启用 Vulkan 支持 |
| `-DGGML_CANN=ON` | 启用 CANN 支持 |
| `-DGGML_ZENDNN=ON` | 启用 ZenDNN 支持 |
| `-DGGML_CPU_KLEIDIAI=ON` | 启用 KleidiAI 支持 |
| `-DGGML_OPENCL=ON` | 启用 OpenCL 支持 |
| `-DGGML_WEBGPU=ON` | 启用 WebGPU 支持 |
| `-DGGML_NATIVE=ON/OFF` | 启用/禁用原生优化 |
| `-DBUILD_SHARED_LIBS=ON/OFF` | 构建共享/静态库 |
| `-DCMAKE_BUILD_TYPE=Debug/Release` | 构建类型 |
| `-DCMAKE_CUDA_ARCHITECTURES="list"` | CUDA 计算能力列表 |
| `-GGML_BACKEND_DL=ON` | 启用动态后端加载 |