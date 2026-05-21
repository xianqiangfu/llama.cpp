# CPU 后端说明

GGML CPU 后端是 GGML 的默认后端，提供跨平台的 CPU 张量计算支持，并针对不同架构进行了优化。

## 概述

CPU 后端通过高度优化的 SIMD 指令集和多线程并行，在 CPU 上提供高效的张量计算能力，是 GGML 最通用和最稳定的后端。

## 目录结构

```
ggml-cpu/
├── CMakeLists.txt        # 构建配置
├── ggml-cpu.c/h          # CPU 后端接口
├── ggml-cpu.cpp          # CPU 后端实现
├── ggml-cpu-impl.h       # 内部实现定义
├── ops.c/h               # 操作实现
├── ops.cpp               # 操作优化
├── quants.c/h            # 量化实现
├── repack.cpp/h          # 数据重排
├── binary-ops.cpp/h      # 二元操作
├── unary-ops.cpp         # 一元操作
├── simd-gemm.h           # SIMD 矩阵乘法
├── simd-mappings.h       # SIMD 映射
├── common.h              # 公共定义
├── traits.cpp/h          # 类型特征
├── hbm.cpp/h             # HBM (High Bandwidth Memory) 支持
│
├── amx/                  # Intel AMX 指令集优化
├── arch/                 # CPU 架构适配
│   ├── x86_64/           # x86 架构
│   ├── arm/              # ARM 架构
│   └── ...
├── kleidiai/             # 特定处理器优化
├── llamafile/            # llamafile 专用优化
└── spacemit/             # Spacemit RISC-V 优化
```

## 核心组件

### 1. 操作实现 (`ops.c/h`)

CPU 后端实现了丰富的操作集合：

| 操作类别 | 主要操作 |
|---------|---------|
| **基础运算** | add、sub、mul、div |
| **矩阵运算** | matmul、gemm、outer |
| **卷积运算** | conv1d、conv2d、conv-transpose |
| **池化运算** | pool1d、pool2d |
| **归一化** | layer_norm、rms_norm、group_norm |
| **激活函数** | relu、gelu、silu、tanh、sigmoid |
| **注意力** | flash_attn、scaled_dot_product |
| **RoPE** | 旋转位置编码（各种变体） |
| **其他** | softmax、argmax、argsort、concat |

### 2. 量化实现 (`quants.c/h`)

支持多种量化格式的高效实现：

| 格式 | 比特数 | 特点 |
|------|--------|------|
| Q4_0 | 4-bit | 基础量化 |
| Q4_1 | 4-bit | 带偏移 |
| Q4_K | 4-bit | K-means 聚类 |
| Q4_K_S | 4-bit | K-means 小块 |
| Q4_K_M | 4-bit | K-means 中块 |
| Q5_K | 5-bit | K-means 聚类 |
| Q8_0 | 8-bit | 8-bit 基础量化 |
| IQ2_XXS | 2-bit | 极限量化 |
| IQ3_XXS | 3-bit | 3-bit 量化 |
| IQ1_S | 1-bit | 1-bit 量化 |

### 3. SIMD 优化

#### x86 架构
- **AVX2**：256 位向量指令
- **AVX512**：512 位向量指令
- **AMX**：矩阵扩展指令（至强专用）

#### ARM 架构
- **NEON**：ARM SIMD 指令集
- **SVE**：可变长度 SIMD（ARMv9）

#### 其他架构
- **RISC-V**：向量扩展
- **PowerPC**：Altivec/VSX

### 4. 架构适配 (`arch/`)

不同 CPU 架构的特定优化：

```
arch/
├── x86_64/
│   ├── avx.c/h          # AVX 实现
│   ├── avx2.c/h         # AVX2 实现
│   ├── avx512.c/h       # AVX512 实现
│   └── ...
├── arm/
│   ├── neon.c/h         # NEON 实现
│   └── ...
└── ...
```

### 5. 矩阵乘法优化 (`simd-gemm.h`)

高度优化的 GEMM 实现：
- 分块算法
- 缓存友好布局
- 向量化内循环
- 多线程并行

## 多线程支持

### 线程池

```cpp
// 线程池创建
ggml_threadpool_t pool = ggml_threadpool_new(n_threads);

// 关联到上下文
ggml_attach_threadpool(ctx, pool, pool_batch);
```

### 任务并行

- OpenMP 并行（如果启用）
- C++ std::async 并行
- 细粒度任务调度

## 内存管理

### 1. HBM 支持 (`hbm.cpp/h`)

针对高频内存的优化：
- 减少内存访问延迟
- 提高缓存利用率

### 2. 数据重排 (`repack.cpp/h`)

- 量化数据重排
- 优化内存布局
- 提高缓存命中率

## 性能优化技巧

### 1. 矩阵乘法优化

```cpp
// 启用优化的 GEMM
GGML_CUBLAS=ON           # BLAS 加速
GGML_AVX512=ON           # AVX512 优化
GGML_AMX=ON              # AMX 矩阵加速
```

### 2. 线程数配置

```bash
# 设置 CPU 线程数
GGML_NUM_THREADS=8
```

### 3. 内存对齐

- 64 字节对齐
- 缓存行对齐
- NUMA 优化

## 编译选项

### CMake 配置

```cmake
# AVX 优化
GGML_AVX=ON              # AVX
GGML_AVX2=ON             # AVX2
GGML_AVX512=ON           # AVX512
GGML_AVX512_VBMI=ON      # AVX512 VBMI
GGML_AVX512_VNNI=ON      # AVX512 VNNI

# ARM 优化
GGML_NEON=ON             # NEON
GGML_SVE=ON              # SVE

# 其他优化
GGML_AMX=ON              # AMX
GGML_LLAMAFILE=ON        # llamafile 优化
GGML_KLEIDIAI=ON         # Kleidiai 优化

# 量化支持
GGML_QK_8_0=ON
GGML_QK_4_0=ON
GGML_QK_4_1=ON
```

## 特殊功能

### 1. Llamafile 优化

专为 llamafile 格式的优化：
- 内置模型加载
- 单文件执行
- 快速启动

### 2. AMX 加速

Intel AMX（高级矩阵扩展）：
- 矩阵乘法加速
- 大幅提升推理性能
- 适用于至强处理器

### 3. Spacemit RISC-V

针对 Spacemit 处理器的优化：
- RISC-V 架构适配
- 特定指令集优化

## 性能特点

### 优势
- 跨平台支持（x86、ARM、RISC-V 等）
- 无需 GPU 即可运行
- 高度优化的 SIMD 代码
- 灵活的量化支持

### 局限
- 相比 GPU，推理速度较慢
- 受 CPU 核心数限制
- 内存带宽瓶颈

## 使用建议

1. **启用所有可用 SIMD**
   ```bash
   cmake -DGGML_AVX2=ON -DGGML_AVX512=ON ..
   ```

2. **合理设置线程数**
   ```bash
   export GGML_NUM_THREADS=$(nproc)
   ```

3. **选择合适量化**
   - Q4_K_M：速度和精度平衡
   - Q5_K_M：更高精度
   - Q8_0：接近全精度

4. **结合 BLAS**
   ```bash
   cmake -DGGML_BLAS=ON ..
   ```

## 相关文档

- [../../README_ZH.md](../../README_ZH.md) - GGML 库总览
- [../ggml-cuda/README_ZH.md](../ggml-cuda/README_ZH.md) - CUDA 后端说明
- [../ggml-blas/README_ZH.md](../ggml-blas/README_ZH.md) - BLAS 后端说明