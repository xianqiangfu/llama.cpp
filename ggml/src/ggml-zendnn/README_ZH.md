# ZenDNN 后端说明

## 概述

ZenDNN (Zen Deep Neural Network) 后端是 llama.cpp 中基于 AMD ZenDNN 库的 CPU 加速实现。ZenDNN 是 AMD 专门为 Zen 架构处理器优化的深度学习推理库，利用了 AMD 处理器的硬件特性（如 AVX-512、VNNI 等）来加速神经网络计算。

## 目录结构

```
ggml/src/ggml-zendnn/
├── CMakeLists.txt                    # CMake 构建配置
└── ggml-zendnn.cpp                   # ZenDNN 后端核心实现 (24KB)
```

## 实现原理

### 1. ZenDNN 架构

ZenDNN 后端采用适配器架构：

```
应用程序层
    ↓
GGML 前端
    ↓
ZenDNN 后端接口
    ↓
ZenDNN 库
    ↓
AMD Zen 处理器优化
    ↓
硬件指令
```

### 2. 核心组件

#### 2.1 上下文管理

```cpp
struct ggml_backend_zendnn_context {
    int n_threads = GGML_DEFAULT_N_THREADS;  // 线程数
    std::unique_ptr<char[]> work_data;        // 工作数据缓冲区
    size_t work_size = 0;                     // 工作缓冲区大小
};
```

#### 2.2 数据类型映射

ZenDNN 后端支持以下数据类型：

| GGML 类型 | ZenDNN 类型 | 说明 |
|-----------|-------------|------|
| GGML_TYPE_F32 | `f32` | 单精度浮点 |
| GGML_TYPE_BF16 | `bf16` | 脑浮点 |

#### 2.3 矩阵乘法实现

ZenDNN 矩阵乘法遵循以下约定：

```
C = B * A

其中：
- A: 权重矩阵，形状 (k, m)，列主序
- B: 输入矩阵，形状 (n, k)，行主序
- C: 输出矩阵，形状 (n, m)，行主序

维度：
- m = 输出特征（C 的列数，A 的列数）
- n = 批次大小（C 的行数，B 的行数）
- k = 内部维度（B 的列数，A 的行数）
```

#### 2.4 后端操作

ZenDNN 后端实现以下操作：

1. **矩阵乘法**：
   - FP32 × FP32 → FP32
   - BF16 × BF16 → BF16
   - BF16 × BF16 → FP32

2. **计算图执行**：
   - 同步计算
   - 异步计算支持
   - 内存管理

### 3. ZenDNN 库依赖

#### 3.1 ZenDNN 库组成

ZenDNN 库包含以下组件：

- **zendnnl**：核心 ZenDNN 库
- **aoclutils**：AMD OpenCL 实用工具
- **aocldlp**：AMD DLP 加速库
- **onednn**：oneDNN 集成
- **libxsmm**：小矩阵加速库

#### 3.2 依赖项详情

| 库 | 用途 | 说明 |
|----|------|------|
| zendnnl | 核心功能 | 主要推理引擎 |
| aoclutils | 工具函数 | CPU 信息查询 |
| aocldlp | DLP 加速 | 深度学习加速 |
| onednn | oneDNN | Intel oneDNN 集成 |
| libxsmm | 矩阵乘法 | 小矩阵优化 |

### 4. 性能优化

#### 4.1 硬件优化

- **AVX-512**：512 位向量指令
- **VNNI**：向量神经网络指令
- **BF16**：Brain Float 16 支持
- **缓存优化**：L1/L2/L3 缓存利用

#### 4.2 线程优化

- **多线程并行**：支持多线程计算
- **NUMA 感知**：NUMA 架构优化
- **负载均衡**：高效的工作分配

#### 4.3 算法优化

- **分块算法**：适应缓存层次结构
- **融合操作**：减少内存访问
- **量化感知**：支持量化推理

### 5. 工作流程

#### 5.1 初始化流程

1. 检查 ZenDNN 库可用性
2. 创建后端上下文
3. 初始化线程池
4. 分配工作缓冲区

#### 5.2 计算流程

1. 准备输入数据
2. 调用 ZenDNN 矩阵乘法
3. 等待计算完成
4. 获取输出结果

## 编译配置

### CMake 选项

#### ZENDNN_ROOT 设置

```bash
# 使用系统安装的 ZenDNN
cmake -DZENDNN_ROOT=/path/to/zendnn ..

# 自动下载并编译 ZenDNN
cmake ..
# 或
cmake -DZENDNN_ROOT=OFF ..
```

#### 自动下载和编译

如果未设置 `ZENDNN_ROOT`，CMake 会自动：

1. 从 GitHub 克隆 ZenDNN 仓库
2. 检出指定版本（ZenDNN-2026-WW17, commit: ac9e580）
3. 编译 ZenDNN 及其依赖项
4. 安装到构建目录

### ZenDNN 编译选项

自动编译时使用的选项：

```cmake
-DCMAKE_BUILD_TYPE=Release
-DCMAKE_INSTALL_PREFIX=${ZENDNN_INSTALL_DIR}
-DZENDNNL_BUILD_EXAMPLES=OFF
-DZENDNNL_BUILD_DOXYGEN=OFF
-DZENDNNL_BUILD_GTEST=OFF
-DZENDNNL_BUILD_BENCHDNN=OFF
-DZENDNNL_DEPENDS_FBGEMM=OFF
-DZENDNNL_LIB_BUILD_ARCHIVE=${ZENDNN_ARCHIVE_LIB}
-DZENDNNL_LIB_BUILD_SHARED=${ZENDNN_SHARED_LIB}
-DZENDNNL_DEPENDS_AOCLDLP=ON
-DZENDNNL_DEPENDS_ONEDNN=ON
-DZENDNNL_DEPENDS_LIBXSMM=ON
```

### 库类型选择

```cmake
# 构建静态库（默认）
cmake -DBUILD_SHARED_LIBS=OFF ..

# 构建共享库
cmake -DBUILD_SHARED_LIBS=ON ..
```

### OpenMP 支持

```bash
# 启用 OpenMP
cmake -DGGML_OPENMP=ON ..
```

## 硬件要求

### 推荐硬件

- **处理器**：AMD Ryzen™ 或 EPYC™ 处理器（Zen 2 或更新架构）
- **指令集**：支持 AVX-512 或 AVX2
- **缓存**：至少 16MB L3 缓存

### 支持的处理器系列

| 系列 | 代数 | AVX-512 | 备注 |
|------|------|---------|------|
| AMD Ryzen 7000 | Zen 4 | 部分 | 部分型号支持 |
| AMD Ryzen 5000 | Zen 3 | 无 | AVX2 |
| AMD Ryzen 3000 | Zen 2 | 部分 | 部分型号支持 |
| AMD EPYC 7004 | Zen 4 | 是 | 完全支持 |
| AMD EPYC 7003 | Zen 3 | 否 | AVX2 |
| AMD EPYC 7002 | Zen 2 | 部分 | 部分型号支持 |

## 使用示例

### 基本使用

```cpp
// 初始化 ZenDNN 后端
ggml_backend_t backend = ggml_backend_zendnn_init();

// 创建缓冲
ggml_backend_buffer_t buffer = ggml_backend_alloc_buffer(backend, size);

// 构建计算图
ggml_tensor* a = ggml_new_tensor_2d(ctx, GGML_TYPE_F32, k, m);
ggml_tensor* b = ggml_new_tensor_2d(ctx, GGML_TYPE_F32, n, k);
ggml_tensor* result = ggml_mul_mat(ctx, b, a);  // C = B * A

// 分配内存
ggml_backend_buffer_alloc_tensor(buffer, a);
ggml_backend_buffer_alloc_tensor(buffer, b);
ggml_backend_buffer_alloc_tensor(buffer, result);

// 执行计算
ggml_backend_graph_compute(backend, graph);
```

### 使用 BF16

```cpp
// 创建 BF16 张量
ggml_tensor* a = ggml_new_tensor_2d(ctx, GGML_TYPE_BF16, k, m);
ggml_tensor* b = ggml_new_tensor_2d(ctx, GGML_TYPE_BF16, n, k);
ggml_tensor* result = ggml_new_tensor_2d(ctx, GGML_TYPE_F32, n, m);

// 计算将输出 FP32
ggml_tensor* mul = ggml_mul_mat(ctx, b, a);
ggml_tensor* cast = ggml_cast(mul, result);
```

### 多线程配置

```cpp
// 初始化时设置线程数
ggml_backend_zendnn_context* ctx = new ggml_backend_zendnn_context();
ctx->n_threads = 8;  // 使用 8 个线程

// 使用环境变量
setenv("OMP_NUM_THREADS", "8", 1);
```

## 性能特性

### 性能优势

1. **硬件优化**：针对 AMD Zen 架构专门优化
2. **指令级并行**：充分利用 AVX-512 指令
3. **缓存友好**：优化数据访问模式
4. **多线程扩展**：良好的多核扩展性

### 性能对比

与通用 CPU 后端相比：

| 场景 | 性能提升 | 说明 |
|------|----------|------|
| 矩阵乘法 | 2-4x | 大矩阵优势明显 |
| 小批量 | 1.5-2x | 开销相对较大 |
| 多线程 | 良好 | 线性扩展 |
| BF16 | 2x | 内存带宽优势 |

### 最佳实践

1. **使用适当的数据类型**：
   - FP32 用于需要高精度的场景
   - BF16 用于内存受限场景

2. **优化批次大小**：
   - 较大批次提高吞吐量
   - 避免极小批次

3. **线程配置**：
   - 根据物理核心数设置线程
   - 考虑超线程的影响

4. **内存布局**：
   - 使用连续内存
   - 避免频繁的数据重排

## 故障排查

### 常见问题

#### 1. ZenDNN 库未找到

**错误信息**：
```
zendnn: include directory not found, please set ZENDNN_ROOT
```

**解决方案**：
- 设置正确的 `ZENDNN_ROOT`
- 或让 CMake 自动下载编译

#### 2. 不支持的数据类型

**错误信息**：
```
ZenDNN matmul failed
```

**解决方案**：
- 检查使用的数据类型
- 确保使用 F32 或 BF16

#### 3. 性能不如预期

**可能原因**：
- 硬件不支持 AVX-512
- 线程配置不当
- 数据布局不优化

**解决方案**：
- 检查硬件支持
- 调整线程数
- 优化数据布局

#### 4. 编译错误

**错误信息**：
```
undefined reference to zendnnl::...
```

**解决方案**：
- 确认 ZenDNN 库正确链接
- 检查库类型（静态/共享）匹配
- 验证 `BUILD_SHARED_LIBS` 设置

### 调试工具

- **性能分析**：AMD uProf
- **性能计数器**：性能监控工具
- **日志输出**：启用详细日志

## 限制和注意事项

### 当前限制

1. **数据类型支持有限**：仅支持 F32 和 BF16
2. **操作支持有限**：主要支持矩阵乘法
3. **仅限 AMD 硬件**：其他处理器性能不佳

### 注意事项

1. **内存对齐**：确保数据正确对齐
2. **线程安全**：注意并发访问
3. **错误处理**：检查返回值

## 相关链接

- [AMD ZenDNN GitHub](https://github.com/amd/ZenDNN)
- [AMD ROCm](https://rocm.docs.amd.com/)
- [oneDNN 文档](https://oneapi-src.github.io/oneDNN/)
- [libxsmm](https://github.com/hfp/libxsmm)

## 许可证

遵循 llama.cpp 项目许可证。ZenDNN 库遵循其自身的许可证。