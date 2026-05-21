# GGML 各硬件后端文档摘要

## 概述

GGML 项目支持多种硬件后端，文档位于 `docs/backend/` 目录。了解各后端的配置方法和特性是正确部署和优化推理服务的基础。

## 支持的后端列表

### 1. BLIS 后端 (BLIS.md)

**简介**：BLIS (BLAS-like Library Instantiation Software) 是一个高性能线性代数库。

**特点**：
- 基于框架的线性代数计算
- 优化的内核生成器
- 支持多种 CPU 架构

**适用场景**：
- 需要高性能矩阵运算的 CPU 环境
- 替代传统 BLAS 库

### 2. CANN 后端 (CANN.md)

**简介**：华为 CANN (Compute Architecture for Neural Networks) 是华为昇腾 AI 处理器的计算框架。

**特点**：
- 专为昇腾芯片优化
- 支持华为 GPU
- 与华为云平台深度集成

**适用场景**：
- 使用华为昇腾 GPU 的环境
- 华为云 AI 服务

### 3. CUDA 后端 (CUDA-FEDORA.md)

**简介**：NVIDIA CUDA GPU 后端，提供最完整的 GPU 加速支持。

**特点**：
- Flash Attention 支持
- 多 GPU 并行
- 量化支持完整
- 性能最优

**适用场景**：
- NVIDIA GPU 环境
- 高性能推理服务

**配置要点**：
- CUDA 11.0+ 版本要求
- 启用 cuBLAS 或 cuDNN 可获得更好性能

### 4. OpenCL 后端 (OPENCL.md)

**简介**：OpenCL 跨平台并行计算后端。

**特点**：
- 跨 GPU 厂商支持
- 兼容性好
- 性能中等

**适用场景**：
- 多种 GPU 混合环境
- 不依赖特定 GPU 厂商

### 5. OpenVINO 后端 (OPENVINO.md)

**简介**：Intel OpenVINO 工具套件后端。

**特点**：
- Intel GPU 优化
- 模型格式转换
- 边缘设备支持

**适用场景**：
- Intel GPU 环境
- 边缘计算设备

### 6. SYCL 后端 (SYCL.md)

**简介**：Intel SYCL (SYCLomatic) 后端，提供跨平台并行计算能力。

**特点**：
- Intel GPU 原生支持
- CUDA 到 SYCL 迁移
- Level Zero 集成
- USM (统一共享内存) 支持

**适用场景**：
- Intel GPU 环境
- 需要跨平台支持

**配置要点**：
- oneAPI 安装
- DPC++ 编译器
- Level Zero 运行时

### 7. VirtGPU 后端 (VirtGPU.md)

**简介**：虚拟 GPU 后端，支持虚拟化环境中的 GPU 访问。

**特点**：
- 虚拟机 GPU 直通
- 云环境支持
- 资源隔离

**适用场景**：
- 虚拟化环境
- 云 GPU 服务

### 8. ZenDNN 后端 (ZenDNN.md)

**简介**：AMD ZenDNN 后端，针对 AMD GPU 优化。

**特点**：
- AMD GPU 原生支持
- RDNA 架构优化
- ROCm 集成

**适用场景**：
- AMD GPU 环境
- ROCm 生态

### 9. zDNN 后端 (zDNN.md)

**简介**：IBM zDNN 后端，针对 IBM Z 系列大型机优化。

**特点**：
- IBM Z 系列支持
- 企业级环境
- 高可靠性要求

**适用场景**：
- IBM Z 大型机环境
- 金融行业

## 其他支持的后端

### CUDA 后端（主分支）
- 完整的 CUDA 支持
- 最成熟稳定
- 社区支持最好

### Metal 后端
- Apple Silicon 原生支持
- M1/M2/M3 芯片优化
- Unified Memory

### Vulkan 后端
- 跨 GPU 厂商
- Windows/Linux 支持
- 移动设备支持

### HIP 后端
- AMD GPU 支持
- CUDA 兼容层
- ROCm 生态

## 后端性能比较

| 后端 | GPU 厂商 | 性能 | 兼容性 | 推荐度 |
|------|----------|------|--------|--------|
| CUDA | NVIDIA | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Metal | Apple | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| SYCL | Intel | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| ROCm/HIP | AMD | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| Vulkan | 跨厂商 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| OpenCL | 跨厂商 | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| CANN | 华为 | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ |

## 选择建议

### 根据硬件选择

1. **NVIDIA GPU** → CUDA 后端
2. **Apple Silicon** → Metal 后端
3. **Intel GPU** → SYCL 后端
4. **AMD GPU** → ROCm/HIP 后端
5. **跨厂商环境** → Vulkan 后端

### 根据需求选择

1. **最高性能** → CUDA/Metal（对应硬件）
2. **最广兼容** → Vulkan/OpenCL
3. **企业级** → CANN/zDNN

### 根据环境选择

1. **云端** → CUDA/Metal/SYCL
2. **边缘设备** → OpenVINO/Vulkan
3. **虚拟化** → VirtGPU

## 配置检查清单

- [ ] 检查 GPU 驱动版本
- [ ] 验证后端编译选项
- [ ] 测试推理性能
- [ ] 检查操作支持状态（参考 docs/ops.md）
- [ ] 配置正确的量化格式

## 相关资源

- 操作支持状态：`docs/ops.md`
- 构建文档：`docs/build.md`
- 多 GPU 文档：`docs/multi-gpu.md`