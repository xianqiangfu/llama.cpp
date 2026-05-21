# ggml-hip - HIP 后端

## 概述

ggml-hip 是基于 AMD ROCm HIP (Heterogeneous-Compute Interface for Portability) 的 GGML 后端实现，支持 AMD GPU 上的高性能矩阵运算。HIP 提供与 CUDA 类似的 API，使代码可以在 AMD GPU 上运行。

## 文件结构

```
ggml/src/ggml-hip/
└── CMakeLists.txt          # CMake 构建配置
```

HIP 后端共享 CUDA 后端的实现，从 `ggml/src/ggml-cuda/` 目录导入代码：

- `*.cu`: CUDA/HIP 源文件
- `*.cuh`: CUDA/HIP 头文件
- `template-instances/`: 模板实例化文件

## 实现原理

### 架构设计

HIP 后端复用 CUDA 后端的大部分实现，通过编译宏实现代码共享：

```cpp
#define GGML_USE_CUDA  // 启用 CUDA 代码路径
#define GGML_USE_HIP   // 启用 HIP 特定功能
```

### 核心组件

#### 依赖库

| 库 | 说明 |
|----|------|
| `hip::host` | HIP 主机运行时 |
| `hip::device` | HIP 设备运行时 |
| `roc::rocblas` | ROCm BLAS 库 |
| `roc::hipblas` | HIP BLAS 库 |
| `roc::rccl` | RCCL (可选，多GPU通信) |

#### 编译选项

| 选项 | 说明 |
|------|------|
| `GGML_HIP_GRAPHS` | 启用 HIP 图捕获 |
| `GGML_HIP_NO_VMM` | 禁用虚拟内存管理 |
| `GGML_HIP_ROCWMMA_FATTN` | 使用 ROCWMMA 实现 Flash Attention |
| `GGML_HIP_NO_MMQ_MFMA` | 禁用 MFMA 实现 MMQ |
| `GGML_HIP_RCCL` | 启用 RCCL 支持 |
| `GGML_HIP_EXPORT_METRICS` | 导出性能指标 |

### ROCm 架构支持

通过 `CMAKE_HIP_ARCHITECTURES` 或 `GPU_TARGETS` 指定目标架构：

```
gfx803  # Vega 10 (RX 580)
gfx900  # Vega 10
gfx906  # Vega 20 (RX 5700 XT)
gfx90a  # Aldebaran (MI200)
gfx90c  # Aldebaran (MI200)
gfx940  # Aldebaran (MI300)
gfx941  # Aldebaran (MI300)
gfx942  # Aldebaran (MI300)
gfx1030 # RDNA 2
gfx1100 # RDNA 3
```

### 模板实例化

支持多种精度和量化组合的 Flash Attention 实现：

- `fattn-tile*.cu`: 基于 Tile 的实现
- `fattn-mma*.cu`: 基于 MMA (Matrix Multiply Accumulate) 的实现
- `fattn-vec*.cu`: 基于向量的实现
- `mmq*.cu`: 矩阵乘量化
- `mmf*.cu`: 矩阵乘法浮点

### 代码共享策略

HIP 后端通过文件重用实现与 CUDA 后端的代码共享：

```cmake
file(GLOB   GGML_SOURCES_ROCM "../ggml-cuda/*.cu")
file(GLOB   SRCS "../ggml-cuda/template-instances/fattn-tile*.cu")
list(APPEND GGML_SOURCES_ROCM ${SRCS})
```

### ROCWMMA 优化

ROCWMMA (Wave Matrix Multiply Accumulate) 是 AMD 的矩阵乘法加速指令：

- 在支持的 GPU 上提供更高的矩阵乘法性能
- 用于 Flash Attention 和矩阵量化操作
- 需要特定架构支持 (gfx90a, gfx940+)

### 构建配置

#### ROCm 路径

```cmake
if (NOT EXISTS $ENV{ROCM_PATH})
    if (NOT EXISTS /opt/rocm)
        set(ROCM_PATH /usr)
    else()
        set(ROCM_PATH /opt/rocm)
    endif()
endif()
```

#### 版本要求

```cmake
if (${hip_VERSION} VERSION_LESS 6.1)
    message(FATAL_ERROR "At least ROCM/HIP V6.1 is required")
endif()
```

### 性能优化

1. **矩阵乘法**: 使用 rocBLAS 和 hipBLAS
2. **Flash Attention**: 支持多种实现方式 (Tile, MMA, Vec)
3. **量化运算**: 优化的 MMQ/MMF 实现
4. **多 GPU**: RCCL 支持

### 限制

- 不支持静态链接
- 需要 ROCm 6.1 或更高版本
- 仅支持 AMD GPU
- Windows 支持有限

## 使用方法

### 编译

```bash
# 基本编译
cmake -DGGML_HIP=ON ..

# 指定 ROCm 路径
cmake -DGGML_HIP=ON -DROCM_PATH=/opt/rocm ..

# 指定 GPU 架构
cmake -DGGML_HIP=ON -DGPU_TARGETS=gfx906,gfx90a ..

# 启用 RCCL
cmake -DGGML_HIP=ON -DGGML_HIP_RCCL=ON ..
```

### 运行时

```bash
# 设置设备
export HIP_VISIBLE_DEVICES=0

# 运行程序
./llama-cli --model model.gguf --backend hip
```

### 环境变量

| 变量 | 说明 |
|------|------|
| `ROCM_PATH` | ROCm 安装路径 |
| `HIP_VISIBLE_DEVICES` | 可见 GPU 列表 |
| `HIP_LAUNCH_BLOCKING` | 阻塞执行模式 (调试) |

## 性能调优

### 内存管理

- 使用 VMM (虚拟内存管理) 减少内存碎片
- 启用异步操作提高吞吐量

### 图优化

- 启用 HIP 图捕获减少内核启动开销
- 批量执行相似操作

### 精度选择

- 使用 BF16/F16 提高吞吐量
- 根据模型要求选择量化级别

## 相关链接

- [ROCm 文档](https://rocm.docs.amd.com/)
- [HIP 文档](https://rocm.docs.amd.com/projects/HIP/en/latest/)
- [rocBLAS](https://rocm.docs.amd.com/projects/rocBLAS/en/latest/)
- [RCCL](https://rocm.docs.amd.com/projects/rccl/en/latest/)