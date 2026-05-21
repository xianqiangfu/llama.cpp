# CUDA 后端说明

GGML CUDA 后端通过 NVIDIA CUDA 框架提供 GPU 加速支持，是 llama.cpp 最常用的推理加速方案。

## 概述

CUDA 后端利用 NVIDIA GPU 的并行计算能力，提供高性能的张量运算，支持多种计算能力和优化技术。

## 目录结构

```
ggml-cuda/
├── *.cu                  # CUDA 内核实现
├── *.cuh                 # CUDA 内核头文件
├── vendors/              # 厂商适配层
│   ├── cuda.h            # CUDA 接口
│   ├── hip.h             # HIP/ROCm 接口
│   └── musa.h            # MUSA (摩尔线程) 接口
└── ...
```

## 支持的计算能力

### NVIDIA GPU

| 架构 | 计算能力 | 代表产品 |
|------|---------|---------|
| Pascal | 600 | GTX 1080、Tesla P100 |
| Volta | 700 | V100、Titan V |
| Turing | 750 | RTX 20 系列、T4 |
| Ampere | 800 | RTX 30 系列、A100 |
| Ada Lovelace | 890 | RTX 40 系列、L40 |
| Hopper | 900 | H100、GH200 |
| Blackwell | 1200 | B100、B200 |
| DGX Spark | 1210 | 专用服务器 GPU |
| Rubin | 1300 | 下一代架构 |

### AMD GPU（通过 HIP）

| 架构 | 计算能力 | 代表产品 |
|------|---------|---------|
| GCN4 | 0x803 | Tonga、Fiji、Polaris |
| Vega | 0x900 | Vega 56/64 |
| Vega20 | 0x906 | MI50、Radeon VII |
| CDNA1 | 0x908 | MI100 |
| CDNA2 | 0x90a | MI210 |
| CDNA3 | 0x942 | MI300 |
| CDNA4 | 0x950 | MI350X/MI355X |
| RDNA1 | 0x1010 | RX 5000 系列 |
| RDNA2 | 0x1030 | RX 6000 系列 |
| RDNA3 | 0x1100 | RX 7000 系列 |
| RDNA3.5 | 0x1150 | AI 370、AI Max 395 |
| RDNA4 | 0x1200 | RX 9000 系列 |

## 核心操作实现

### 1. 矩阵运算

| 操作 | 文件 | 说明 |
|------|------|------|
| 矩阵乘法 | `*.cu` | GEMM 优化实现 |
| 加法/减法 | `acc.cu` | 元素级运算 |
| 复制操作 | `cpy.cu` | 数据传输 |
| 广播运算 | `binbcast.cu` | 广播和逐元素运算 |
| 拼接 | `concat.cu` | 张量拼接 |
| 转置 | `*.cu` | 矩阵转置 |

### 2. 卷积运算

| 操作 | 文件 | 说明 |
|------|------|------|
| 1D 卷积 | `conv-transpose-1d.cu` | 1D 卷积 |
| 2D 卷积 | `conv2d.cu` | 2D 卷积 |
| 深度卷积 | `conv2d-dw.cu` | Depthwise 卷积 |
| 转置卷积 | `conv2d-transpose.cu` | 转置卷积 |

### 3. 激活函数

| 操作 | 文件 | 说明 |
|------|------|------|
| ReLU | `*.cu` | ReLU 激活 |
| GELU | `*.cu` | GELU 激活 |
| SiLU | `*.cu` | SiLU 激活 |
| Tanh | `*.cu` | Tanh 激活 |

### 4. 归一化

| 操作 | 文件 | 说明 |
|------|------|------|
| Layer Norm | `*.cu` | 层归一化 |
| RMS Norm | `*.cu` | RMS 归一化 |
| Group Norm | `*.cu` | 组归一化 |

### 5. 注意力机制

| 操作 | 文件 | 说明 |
|------|------|------|
| Flash Attention | `*.cu` | Flash Attention 2 实现 |
| MQA | `*.cu` | Multi-Query Attention |
| GQA | `*.cu` | Grouped Query Attention |
| RoPE | `*.cu` | 旋转位置编码 |

### 6. 其他操作

| 操作 | 文件 | 说明 |
|------|------|------|
| Softmax | `*.cu` | Softmax |
| Argmax | `argmax.cu` | 最大值索引 |
| Argsort | `argsort.cu` | 排序 |
| Clamp | `clamp.cu` | 裁剪 |
| Cumsum | `cumsum.cu` | 累积和 |
| 交叉熵 | `cross-entropy-loss.cu` | 交叉熵损失 |

## 优化特性

### 1. Tensor Core 加速

利用 Tensor Core 进行矩阵乘法加速：
- FP16/BF16 混合精度
- INT8 量化加速
- TF32 加速（Ampere+）

### 2. Flash Attention 2

高效的注意力计算：
- 减少内存访问
- 分块计算
- 更好的缓存利用

### 3. Split-K 矩阵乘法

- 并行归约
- 更好的负载均衡
- 减少同步开销

### 4. 多 GPU 支持

- 数据并行
- 模型并行
- 层并行
- AllReduce 操作 (`allreduce.cu`)

### 5. 内存优化

- CUDA Graph（图执行）
- 内存池
- 异步内存传输

## 编译配置

### CMake 选项

```cmake
# 启用 CUDA 后端
GGML_CUDA=ON

# CUDA 版本
CMAKE_CUDA_ARCHITECTURES="native"    # 自动检测
CMAKE_CUDA_ARCHITECTURES="80;90"    # 指定架构

# CUDA Toolkit 路径
CUDAToolkit_ROOT=/usr/local/cuda

# 启用优化
GGML_CUDA_FA_ALL_QUANTS=ON          # Flash Attention 所有量化
GGML_CUDA_PEER_MAX_MB=128           # GPU 间通信带宽

# 量化支持
GGML_CUDA_MMV=ON                    # 量化矩阵向量乘
```

### 环境变量

```bash
# CUDA 可见性
export CUDA_VISIBLE_DEVICES=0,1

# CUDA 缓存
export CUDA_CACHE_MAXSIZE=1G

# CUDA 统计
export CUDA_LAUNCH_BLOCKING=0       # 异步执行
```

## 性能调优

### 1. 选择合适的计算能力

```bash
# 自动检测（推荐）
CMAKE_CUDA_ARCHITECTURES="native"

# 或指定架构
CMAKE_CUDA_ARCHITECTURES="86"       # RTX 30 系列
CMAKE_CUDA_ARCHITECTURES="89"       # RTX 40 系列
CMAKE_CUDA_ARCHITECTURES="90"       # H100
```

### 2. 层级设置

```bash
# 设置卸载到 GPU 的层数
export N_GPU_LAYERS=35
```

### 3. 量化优化

```bash
# 使用 Q4_K 量化
# 编译时启用所有量化支持
cmake -DGGML_CUDA_FA_ALL_QUANTS=ON ..
```

### 4. 多 GPU 配置

```bash
# 指定使用的 GPU
CUDA_VISIBLE_DEVICES=0,1 ./main ...

# 设置层拆分模式
LLAMA_SPLIT_MODE=layer              # 层并行
LLAMA_SPLIT_MODE=row                # 行并行
```

## Flash Attention

### 工作原理

Flash Attention 通过重新计算和内存优化，减少 HBM 访问次数：

1. 分块计算注意力
2. 使用 SRAM/SMEM 缓存
3. 在线 Softmax 更新
4. 避免存储完整注意力矩阵

### 启用方式

```cpp
// 自动启用（如果支持）
struct llama_context_params params = llama_context_default_params();
params.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_ENABLED;
```

## 混合精度

### FP16/BF16

```cmake
# 启用混合精度
GGML_CUDA_F16=ON
```

### TF32（Ampere+）

```cmake
# 启用 TF32
GGML_CUDA_TF32=ON
```

## 限制与注意事项

1. **显存限制**：需要在 GPU 显存范围内运行
2. **计算能力**：需要计算能力 ≥ 6.0（Pascal）
3. **CUDA 版本**：需要 CUDA 11.0+（推荐 12.0+）
4. **同步问题**：异步执行需注意同步点

## 性能建议

1. **使用合适的量化格式**
   - Q4_K_M：最佳平衡
   - Q5_K_M：更高精度
   - Q8_0：接近全精度

2. **优化层卸载**
   - 小模型：全卸载到 GPU
   - 大模型：部分卸载

3. **利用多 GPU**
   - 数据并行提高吞吐
   - 模型并行运行大模型

4. **启用 Flash Attention**
   - 显著提升注意力计算速度
   - 减少显存占用

## 相关文档

- [../../README_ZH.md](../../README_ZH.md) - GGML 库总览
- [../ggml-cpu/README_ZH.md](../ggml-cpu/README_ZH.md) - CPU 后端说明
- NVIDIA CUDA 编程指南
- AMD ROCm 编程指南