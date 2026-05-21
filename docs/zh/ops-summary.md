# GGML 运维操作文档摘要

## 概述

GGML 操作列表文档 (docs/ops.md) 定义了所有 GGML 操作及其在不同硬件后端的支持状态。这对于运维人员选择合适的硬件后端、配置推理环境、排查性能问题至关重要。

## 图例说明

- ✅ 完全支持
- 🟡 部分支持
- ❌ 不支持

## 支持的后端

| 后端 | 描述 |
|------|------|
| BLAS | 基础线性代数子程序库后端 |
| CANN | 华为 CANN 后端 |
| CPU | CPU 通用计算后端 |
| CUDA | NVIDIA CUDA GPU 后端 |
| MTL | Apple Metal 后端 |
| OpenCL | OpenCL 跨平台后端 |
| SYCL | Intel SYCL 后端 |
| Vulkan | Vulkan GPU 后端 |
| WebGPU | WebGPU 后端 |
| ZenDNN | AMD ZenDNN 后端 |
| zDNN | IBM zDNN 后端 |

## 核心操作支持分析

### 矩阵运算（核心推理操作）

| 操作 | CUDA | MTL | CPU | SYCL | 说明 |
|------|------|------|------|------|------|
| MUL_MAT | 🟡 | 🟡 | 🟡 | 🟡 | 矩阵乘法 |
| MUL_MAT_ID | ✅ | 🟡 | ✅ | 🟡 | 矩阵乘法（带 ID） |
| SOFT_MAX | ✅ | ✅ | ✅ | ✅ | Softmax |
| FLASH_ATTN_EXT | 🟡 | 🟡 | ✅ | 🟡 | Flash Attention |

### 激活函数

各后端对激活函数的支持情况良好，包括：
- RELU, SILU, GELU, TANH, SIGMOID 等常见激活函数
- GEGLU, REGU, SWIGLU 等门控机制函数

### 注意力机制

| 操作 | 说明 |
|------|------|
| ROPE | 旋转位置编码 |
| FLASH_ATTN_EXT | 扩展 Flash Attention |
| DIAG_MASK_INF | 对角掩码负无穷 |

### 量化相关

| 操作 | 说明 |
|------|------|
| IM2COL | 图像转列（量化计算） |
| GET_ROWS | 获取行（量化矩阵乘法） |
| DEQUANTIZE | 反量化 |

### 卷积运算

| 操作 | 说明 |
|------|------|
| CONV_2D | 2D 卷积 |
| CONV_2D_DW | 2D 深度卷积 |
| CONV_3D | 3D 卷积 |
| CONV_TRANSPOSE_1D | 1D 转置卷积 |
| CONV_TRANSPOSE_2D | 2D 转置卷积 |

## 运维要点

### 后端选择建议

1. **NVIDIA GPU 环境**：使用 CUDA 后端，支持最完整
2. **Apple Silicon**：使用 Metal 后端，性能优秀
3. **Intel GPU**：使用 SYCL 后端，跨平台性好
4. **通用环境**：CPU 后端兜底，支持所有操作

### 性能优化配置

1. 启用支持的操作以获得最佳性能
2. 对于部分支持的操作，回退到 CPU 可能影响性能
3. 检查操作支持状态以排查性能问题

### 监控与调试

1. 使用 `test-backend-ops support --output csv` 测试后端支持
2. 检查操作支持状态表以验证配置
3. 关注 🟡 部分支持的操作可能影响推理效果

## 操作统计

- 总操作数：约 123 个
- CPU 支持：几乎全部（100%）
- CUDA 支持：约 95%（部分核心操作 🟡）
- Metal 支持：约 90%
- SYCL 支持：约 85%

## 文件位置

完整操作列表：`docs/ops.md`
后端文档：`docs/backend/`
生成脚本：`scripts/create_ops_docs.py`