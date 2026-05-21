# GGML 张量计算库总览

GGML (General Gpu Machine Learning) 是一个轻量级的张量计算库，为 llama.cpp 提供底层计算支持。

## 目录结构

```
ggml/
├── src/
│   ├── ggml-*.c/h          # 核心计算库文件
│   ├── ggml-backend-*.cpp/h # 后端接口
│   ├── ggml-alloc.c        # 内存分配器
│   ├── ggml-opt.cpp        # 优化器
│   ├── ggml-quants.c/h     # 量化实现
│   ├── ggml-blas/          # BLAS 后端（详见后端文档）
│   ├── ggml-cann/          # 华为 CANN 后端
│   ├── ggml-cpu/           # CPU 后端
│   ├── ggml-cuda/          # CUDA 后端
│   ├── ggml-metal/         # Apple Metal 后端
│   ├── ggml-hip/           # AMD ROCm 后端
│   ├── ggml-opencl/        # OpenCL 后端
│   ├── ggml-sycl/          # Intel SYCL 后端
│   ├── ggml-openvino/      # Intel OpenVINO 后端
│   ├── ggml-rpc/           # RPC 远程后端
│   └── ggml-hexagon/       # Qualcomm Hexagon 后端
└── ...
```

## 核心组件

### 1. 张量计算 (ggml.c/h)
- 定义张量数据结构和基本操作
- 支持的数据类型：F32、F16、Q4_0、Q4_K、Q8_0 等
- 提供矩阵乘法、卷积、注意力等基础运算

### 2. 后端系统 (ggml-backend*.cpp/h)
- 统一的后端接口，支持多硬件平台
- 后端注册和调度机制
- 设备管理和内存分配

### 3. 内存管理 (ggml-alloc.c)
- 高效的内存池分配器
- 支持计算图内存复用
- 内存碎片整理

### 4. 量化支持 (ggml-quants.c/h)
- 多种量化格式实现
- 量化和反量化操作
- 量化矩阵乘法优化

### 5. 优化器 (ggml-opt.cpp)
- 模型训练优化
- 支持 Adam、SGD 等优化算法
- 学习率调度

## 支持的后端

| 后端 | 硬件平台 | 特点 |
|------|----------|------|
| ggml-cpu | x86/ARM CPU | 基础后端，支持 SIMD 优化 |
| ggml-cuda | NVIDIA GPU | 高性能，支持 FP16/BF16 |
| ggml-metal | Apple Silicon | macOS/M 系列芯片优化 |
| ggml-hip | AMD GPU | ROCm 生态支持 |
| ggml-opencl | OpenCL 设备 | 跨平台支持 |
| ggml-sycl | Intel GPU | oneAPI 支持 |
| ggml-blas | CPU/GPU | BLAS 库接口 |
| ggml-cann | 华为昇腾 | 昇腾芯片支持 |
| ggml-openvino | Intel CPU/GPU | 推理加速 |
| ggml-rpc | 远程设备 | 网络远程计算 |
| ggml-hexagon | Qualcomm NPU | 移动端加速 |

## CPU 优化

### SIMD 指令集支持
- AVX2、AVX512 (Intel/AMD)
- NEON (ARM)
- AMX (Intel 至强)
- 特殊指令集（如 SPACEMIT、LLAMAFILE 等）

### 架构适配
- x86_64
- ARM64
- RISC-V
- PowerPC

## CUDA 优化特性

- Flash Attention 2 实现
- FP16/BF16 混合精度
- Multi-Query Attention (MQA) 优化
- Grouped Query Attention (GQA) 优化
- Split-K 矩阵乘法
- Tensor Core 加速

## 量化格式

| 格式 | 比特数 | 说明 |
|------|--------|------|
| Q4_0 | 4-bit | 基础 4-bit 量化 |
| Q4_1 | 4-bit | 带偏移的 4-bit 量化 |
| Q4_K | 4-bit | K-means 聚类 4-bit 量化 |
| Q5_K | 5-bit | K-means 聚类 5-bit 量化 |
| Q8_0 | 8-bit | 8-bit 量化 |
| IQ2_XXS | 2-bit | 极限 2-bit 量化 |
| TQ1_0 | 1-bit | Ternary 量化 |

## 计算图

GGML 使用计算图来优化计算流程：
- 自动内存管理
- 算子融合
- 并行执行
- 图缓存和复用

## 相关文档

- [src/ggml-cpu/README_ZH.md](src/ggml-cpu/README_ZH.md) - CPU 后端详细说明
- [src/ggml-cuda/README_ZH.md](src/ggml-cuda/README_ZH.md) - CUDA 后端详细说明
- [src/ggml-blas/README_ZH.md](src/ggml-blas/README_ZH.md) - BLAS 后端详细说明
- [src/ggml-cann/README_ZH.md](src/ggml-cann/README_ZH.md) - CANN 后端详细说明