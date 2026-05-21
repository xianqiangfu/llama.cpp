# 后端文档总览

本目录包含 llama.cpp 各硬件后端的使用文档和配置指南。

## 支持的后端

llama.cpp 支持多种硬件后端，以在不同平台上实现高性能推理：

| 后端 | 支持的硬件 | 文档 |
|------|-----------|------|
| CUDA | NVIDIA GPU | [CUDA 文档](../CUDA-FEDORA.md) |
| Metal | Apple Silicon (M1/M2/M3) | - |
| Vulkan | 跨平台 GPU | [OpenCL 文档](../OPENCL.md) |
| OpenCL | 各种 OpenCL 设备 | [OpenCL 文档](../OPENCL.md) |
| WebGPU | 浏览器/Web | [Vulkan 文档](../VirtGPU.md) |
| SYCL | Intel GPU | [SYCL 文档](../SYCL.md) |
| BLIS | CPU (BLIS 库) | [BLIS 文档](../BLIS.md) |
| CANN | 华为昇腾 NPU | [CANN 文档](../CANN.md) |
| ZenDNN | AMD GPU | [ZenDNN 文档](../ZenDNN.md) |
| zDNN | IBM Z | [zDNN 文档](../zDNN.md) |
| VirtGPU | 虚拟 GPU | [VirtGPU 文档](../VirtGPU.md) |

## 后端文档

### BLIS
[BLIS.md](../BLIS.md)

BLIS (Basic Linear Algebra Subprograms Language and Infrastructure) 是一个高性能线性代数库。llama.cpp 可以使用 BLIS 来加速 CPU 上的矩阵运算。

**适用场景：**
- 需要 CPU 加速但平台特定库不可用
- 跨平台 CPU 性能优化

### CANN
[CANN.md](../CANN.md)

CANN (Compute Architecture for Neural Networks) 是华为提供的神经网络计算架构，用于昇腾系列 AI 芯片。

**适用场景：**
- 华为昇腾 AI 加速卡
- 昇腾处理器服务器

### CUDA
[CUDA-FEDORA.md](../CUDA-FEDORA.md)

CUDA 是 NVIDIA 的并行计算平台和编程模型，支持在 NVIDIA GPU 上进行高性能计算。

**适用场景：**
- NVIDIA GPU (RTX 系列、Tesla 系列、Ampere 系列、Hopper 系列)
- Linux 和 Windows 平台

### OpenCL
[OPENCL.md](../OPENCL.md)

OpenCL 是一个开放标准，用于在各种异构设备上进行并行计算。

**适用场景：**
- AMD GPU
- Intel GPU
- 其他 OpenCL 兼容设备

### SYCL
[SYCL.md](../SYCL.md)

SYCL 是 Intel 提供的跨平台抽象层，基于 Khronos 标准，简化异构计算编程。

**适用场景：**
- Intel GPU
- Intel CPU 和 FPGA
- 跨平台开发

### VirtGPU
[VirtGPU.md](../VirtGPU.md)

VirtGPU 是虚拟化 GPU 后端，支持在虚拟环境中使用 GPU 加速。

**适用场景：**
- 虚拟机环境
- 云端 GPU 虚拟化

### ZenDNN
[ZenDNN.md](../ZenDNN.md)

ZenDNN 是 AMD 提供的深度神经网络库，用于 AMD GPU 加速。

**适用场景：**
- AMD GPU
- ROCm 平台

### zDNN
[zDNN.md](../zDNN.md)

zDNN 是为 IBM Z 系列优化的深度神经网络库。

**适用场景：**
- IBM Z 服务器
- Linux on Z

### Snapdragon
[snapdragon/](./snapdragon/)

高通骁龙系列处理器的专用支持。

**适用场景：**
- 安卓设备
- Snapdragon 平板

## 如何选择后端

### 桌面/工作站

| 硬件 | 推荐后端 |
|------|----------|
| NVIDIA GPU | CUDA |
| AMD GPU | OpenCL / ZenDNN |
| Intel GPU | SYCL / OpenCL |
| Apple Silicon | Metal |
| 通用 CPU | BLIS |

### 服务器/云端

| 环境 | 推荐后端 |
|------|----------|
| NVIDIA 云 GPU | CUDA |
| 昇腾 NPU | CANN |
| IBM Z | zDNN |
| 虚拟化环境 | VirtGPU |

### 移动/嵌入式

| 平台 | 推荐后端 |
|------|----------|
| iOS | Metal |
| Android | Snapdragon / CPU |
| 嵌入式 Linux | OpenCL / CPU |

## 构建配置

### 启用后端

使用 CMake 构建时，可以指定要启用的后端：

```bash
# CUDA
cmake -DLLAMA_CUBLAS=ON ..

# Metal
cmake -DLLAMA_METAL=ON ..

# Vulkan
cmake -DLLAMA_VULKAN=ON ..

# OpenCL
cmake -DLLAMA_CLBLAST=ON ..

# SYCL
cmake -DLLAMA_SYCL=ON ..

# CANN
cmake -DLLAMA_CANN=ON ..

# BLIS
cmake -DLLAMA_BLIS=ON ..
```

### 多后端支持

可以同时启用多个后端：

```bash
cmake -DLLAMA_CUBLAS=ON -DLLAMA_METAL=ON ..
```

### 自动检测

llama.cpp 会自动检测可用的后端：

```bash
cmake -DLLAMA_BACKEND_AUTO_DETECT=ON ..
```

## 性能优化

### 1. 批处理

启用批处理可以提高吞吐量：

```bash
./llama-cli -m model.gguf -n 512 -b 512
```

### 2. 线程数调整

根据硬件配置调整线程数：

```bash
./llama-cli -m model.gguf -t 8
```

### 3. 内存优化

使用 `mmap` 加载大模型：

```bash
./llama-cli -m model.gguf --mmap
```

## 故障排查

### 后端加载失败

检查是否正确安装了后端依赖：

```bash
# CUDA
nvidia-smi

# Metal
system_profiler SPDisplaysDataType

# OpenCL
clinfo
```

### 性能问题

1. 检查硬件是否被正确识别
2. 验证驱动版本是否最新
3. 调整批处理大小和线程数
4. 使用量化模型

### 内存不足

1. 使用量化模型 (Q4, Q5 等)
2. 减小上下文长度 (`n_ctx`)
3. 启用 `mmap` 加载
4. 使用分层卸载

## 基准测试

各后端的基准测试数据可在 [ops/](../ops/) 目录中查看：

- [CPU.csv](../ops/CPU.csv) - CPU 后端性能
- [CUDA.csv](../ops/CUDA.csv) - CUDA 后端性能
- [Metal.csv](../ops/Metal.csv) - Metal 后端性能
- [Vulkan.csv](../ops/Vulkan.csv) - Vulkan 后端性能
- 等等...

## 相关文档

- [文档目录总览](../README_ZH.md)
- [构建指南](../build.md)
- [运维文档](../ops/README_ZH.md)
- [开发文档](../development/README_ZH.md)

## 贡献

欢迎为新的硬件平台添加后端支持。请参考：

1. 现有后端的实现
2. 后端接口定义
3. 提供完整的文档和测试