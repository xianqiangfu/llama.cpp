# Vulkan 后端说明

## 概述

Vulkan 后端是 llama.cpp 中基于 Vulkan 图形计算 API 的 GPU 加速实现。Vulkan 是一个低开销、跨平台的图形和计算 API，提供对 GPU 的细粒度控制和高性能计算能力。该后端通过计算着色器实现各种张量运算，特别适合在支持 Vulkan 的各种设备上运行大语言模型推理。

## 目录结构

```
ggml/src/ggml-vulkan/
├── CMakeLists.txt                    # CMake 构建配置
├── ggml-vulkan.cpp                    # Vulkan 后端核心实现 (911KB)
├── vulkan-shaders/                   # Vulkan 计算着色器目录
│   ├── CMakeLists.txt                # 着色器编译配置
│   ├── vulkan-shaders-gen.cpp        # 着色器代码生成器 (63KB)
│   ├── feature-tests/                # 功能测试着色器
│   │   ├── coopmat.comp              # 协作矩阵测试
│   │   ├── coopmat2.comp             # 协作矩阵2测试
│   │   ├── integer_dot.comp          # 整数点积测试
│   │   └── bfloat16.comp             # BF16 测试
│   ├── types.glsl                    # 类型定义 (126KB)
│   ├── utils.glsl                    # 工具函数
│   ├── dequant_funcs.glsl            # 反量化函数 (27KB)
│   ├── dequant_funcs_cm2.glsl        # 反量化函数 CM2 版本 (26KB)
│   ├── dequant_head.glsl             # 反量化头部
│   ├── mul_mm_funcs.glsl             # 矩阵乘法函数 (33KB)
│   ├── mul_mm_cm2.comp               # 矩阵乘法 CM2 版本
│   ├── mul_mmq_funcs.glsl            # MMQ 矩阵乘法函数 (18KB)
│   ├── mul_mat_vecq_funcs.glsl       # 矩阵向量乘法函数 (22KB)
│   ├── mul_mat_vec_base.glsl         # 矩阵向量乘法基础 (8KB)
│   ├── flash_attn.comp               # Flash Attention (32KB)
│   ├── flash_attn_cm1.comp           # Flash Attention CM1 版本
│   ├── flash_attn_cm2.comp           # Flash Attention CM2 版本
│   ├── flash_attn_base.glsl          # Flash Attention 基础函数 (9KB)
│   ├── flash_attn_dequant.glsl       # Flash Attention 反量化 (9KB)
│   ├── flash_attn_mmq_funcs.glsl    # Flash Attention MMQ 函数 (9KB)
│   ├── rope_funcs.glsl               # RoPE 位置编码函数 (8KB)
│   └── *.comp                        # 其他计算着色器（100+个）
└── (在构建目录生成)
    ├── ggml-vulkan-shaders.hpp        # 生成的着色器头文件
    ├── *.spv                         # 编译后的 SPIR-V 着色器
    └── *.cpp                         # 着色器封装代码
```

## 实现原理

### 1. Vulkan 架构

Vulkan 后端采用标准 Vulkan 计算流水线架构：

```
应用程序层
    ↓
GGML 前端
    ↓
Vulkan 后端接口
    ↓
Vulkan 实例和设备
    ↓
计算着色器 (SPIR-V)
    ↓
GPU 执行
```

### 2. 核心组件

#### 2.1 Vulkan 设备管理

- **实例创建**：初始化 Vulkan 实例
- **物理设备选择**：枚举并选择合适的 GPU
- **逻辑设备创建**：创建逻辑设备和队列
- **命令池和缓冲**：管理命令执行

#### 2.2 内存管理

- **缓冲分配**：在设备内存中分配缓冲
- **内存类型选择**：根据使用需求选择合适的内存类型
- **内存映射**：主机到设备的数据传输
- **内存屏障**：确保内存访问的正确同步

#### 2.3 着色器系统

##### 着色器编译流程

1. **GLSL 源码**：以 `.comp` 扩展名的 GLSL 计算着色器源文件
2. **GLSL 编译**：使用 `glslc` 编译器将 GLSL 编译为 SPIR-V 中间表示
3. **代码生成**：`vulkan-shaders-gen` 工具生成 C++ 封装代码
4. **头文件生成**：生成 `ggml-vulkan-shaders.hpp` 包含所有着色器数据
5. **运行时加载**：后端在运行时加载并创建着色器模块

##### 主要着色器类别

1. **矩阵运算**
   - `mul_mm.comp`：矩阵乘法
   - `mul_mat_vec.comp`：矩阵向量乘法
   - `mul_mmq.comp`：量化矩阵乘法

2. **量化/反量化**
   - `dequant_*.comp`：各种格式的反量化（Q1_0, Q2_K, Q3_K, Q4_K, Q5_K, Q6_K, IQ1_*, IQ2_*, IQ3_* 等）
   - `quantize_q8_1.comp`：量化为 Q8_1 格式
   - `copy_from_quant.comp`：从量化格式复制
   - `copy_to_quant.comp`：复制到量化格式

3. **Flash Attention**
   - `flash_attn.comp`：标准 Flash Attention
   - `flash_attn_cm1.comp`：协作矩阵版本 1
   - `flash_attn_cm2.comp`：协作矩阵版本 2
   - `flash_attn_mask_opt.comp`：掩码优化版本

4. **位置编码**
   - `rope_*.comp`：各种 RoPE 变体
     - `rope_multi.comp`：多头 RoPE
     - `rope_neox.comp`：NeoX 风格 RoPE
     - `rope_norm.comp`：标准化 RoPE
     - `rope_vision.comp`：视觉模型 RoPE

5. **激活函数**
   - `gelu.comp`, `gelu_erf.comp`, `gelu_quick.comp`：GELU 变体
   - `relu.comp`：ReLU
   - `silu.comp`, `silu_back.comp`：SiLU 及其反向
   - `swiglu.comp`, `swiglu_oai.comp`：SwiGLU 变体
   - `tanh.comp`, `sigmoid.comp`：双曲函数
   - `hardswish.comp`, `hardsigmoid.comp`：Hard 激活函数

6. **归一化**
   - `norm.comp`：层归一化
   - `rms_norm.comp`：RMS 归一化
   - `rms_norm_back.comp`：RMS 归一化反向
   - `rms_norm_partials.comp`：RMS 归一化部分计算
   - `group_norm.comp`：组归一化

7. **注意力机制**
   - `soft_max.comp`：Softmax
   - `soft_max_large*.comp`：大序列 Softmax（多通道版本）
   - `soft_max_back.comp`：Softmax 反向

8. **卷积操作**
   - `conv2d_dw.comp`：深度可分离卷积
   - `conv2d_mm.comp`：矩阵乘法实现的卷积
   - `im2col.comp`：图像到列变换
   - `im2col_3d.comp`：3D 图像到列变换
   - `pool2d.comp`：2D 池化

9. **优化器**
   - `opt_step_adamw.comp`：AdamW 优化器步进
   - `opt_step_sgd.comp`：SGD 优化器步进

10. **特殊操作**
    - `wkv6.comp`, `wkv7.comp`：RWKV 状态空间模型
    - `ssm_conv.comp`, `ssm_scan.comp`：状态空间模型操作
    - `gated_delta_net.comp`：门控 Delta 网络
    - `topk_moe.comp`：TopK 混合专家
    - `count_experts.comp`：专家计数

11. **工具操作**
    - `copy.comp`, `copy_transpose.comp`：复制和转置
    - `concat.comp`：张量拼接
    - `repeat.comp`, `repeat_back.comp`：重复操作
    - `roll.comp`：元素滚动
    - `pad.comp`：填充
    - `upscale.comp`：上采样

12. **归约操作**
    - `sum_rows.comp`：行求和
    - `acc.comp`：累加
    - `l2_norm.comp`：L2 范数

13. **排序和搜索**
    - `argsort.comp`, `argsort_large.comp`：参数排序
    - `argmax.comp`：参数最大值索引
    - `topk_argsort.comp`：TopK 排序

### 3. 数据类型支持

#### 3.1 量化格式

Vulkan 后端支持多种量化格式以减少内存使用和提升性能：

- **Q 格式**：Q1_0, Q2_K, Q3_K, Q4_K, Q5_K, Q6_K, Q8_0
- **IQ 格式**：IQ1_S, IQ1_M, IQ2_S, IQ2_XS, IQ2_XXS, IQ3_S, IQ3_XXS, IQ4_NL, IQ4_XS
- **其他格式**：MXFP4, NVFP4

#### 3.2 浮点格式

- **FP32**：单精度浮点
- **FP16**：半精度浮点
- **BF16**：脑浮点（如果硬件支持）

### 4. 性能优化技术

#### 4.1 协作矩阵 (Cooperative Matrix)

- 使用 `GL_KHR_cooperative_matrix` 和 `GL_NV_cooperative_matrix2` 扩展
- 硬件加速的矩阵乘法
- 自动检测和启用硬件支持

#### 4.2 整数点积

- 使用 `GL_EXT_integer_dot_product` 扩展
- 加速整数运算
- 提升量化模型性能

#### 4.3 Flash Attention

- 内存高效的注意力机制实现
- 多种优化变体
- 支持量化输入

#### 4.4 MMQ (Mixed Matrix Quantization)

- 混合矩阵量化技术
- 在精度和性能之间取得平衡
- 针对不同硬件的优化路径

### 5. 工作流程

#### 5.1 初始化流程

1. 创建 Vulkan 实例
2. 枚举并选择物理设备
3. 创建逻辑设备和队列
4. 测试着色器扩展支持
5. 加载并编译着色器
6. 初始化内存池

#### 5.2 计算流程

1. 准备输入数据并上传到 GPU
2. 创建描述符集绑定资源
3. 记录命令缓冲
4. 提交命令队列执行
5. 等待完成并获取结果
6. 清理资源

### 6. 扩展支持

Vulkan 后端动态检测和使用以下扩展：

- **GL_KHR_cooperative_matrix**：协作矩阵操作
- **GL_NV_cooperative_matrix2**：NVIDIA 协作矩阵扩展
- **GL_EXT_integer_dot_product**：整数点积
- **GL_EXT_bfloat16**：BF16 数据类型

## 编译选项

### CMake 选项

- `GGML_VULKAN_CHECK_RESULTS`：启用结果检查（调试用）
- `GGML_VULKAN_DEBUG`：启用调试输出
- `GGML_VULKAN_MEMORY_DEBUG`：启用内存调试
- `GGML_VULKAN_SHADER_DEBUG_INFO`：在着色器中包含调试信息
- `GGML_VULKAN_VALIDATE`：启用 Vulkan 验证层
- `GGML_VULKAN_RUN_TESTS`：运行 Vulkan 测试

### 交叉编译

- `GGML_VULKAN_SHADERS_GEN_TOOLCHAIN`：着色器生成工具链文件（交叉编译时使用）

## 依赖项

### 必需依赖

- **Vulkan SDK**：Vulkan 开发工具包
  - `Vulkan::Vulkan`：Vulkan 库
  - `Vulkan::glslc`：GLSL 编译器
- **SPIRV-Headers**：SPIR-V 头文件

### 可选依赖

- **Vulkan 验证层**：用于调试和验证

## 平台支持

### Windows

- 完全支持
- 需要 Vulkan SDK
- 支持主要 GPU 厂商驱动（NVIDIA, AMD, Intel）

### Linux

- 完全支持
- 需要 Mesa 或厂商驱动
- 广泛的硬件兼容性

### Android

- 通过 Dawn WebGPU 间接支持
- 需要额外的配置

### 其他平台

- 通过 WebGPU 间接支持

## 使用示例

### 基本使用

```cpp
// 初始化 Vulkan 后端
ggml_backend_t backend = ggml_backend_vulkan_init(0);

// 创建缓冲
ggml_backend_buffer_t buffer = ggml_backend_alloc_buffer(backend, size);

// 构建计算图
ggml_tensor* a = ggml_new_tensor_2d(ctx, GGML_TYPE_F32, rows, cols);
ggml_tensor* b = ggml_new_tensor_2d(ctx, GGML_TYPE_F32, cols, rows);
ggml_tensor* result = ggml_mul_mat(ctx, a, b);

// 分配内存到 GPU
ggml_backend_buffer_alloc_tensor(buffer, a);
ggml_backend_buffer_alloc_tensor(buffer, b);
ggml_backend_buffer_alloc_tensor(buffer, result);

// 执行计算
ggml_backend_graph_compute(backend, graph);

// 获取结果
ggml_backend_tensor_get_async(buffer, result, output_ptr, size);
```

### 选择特定设备

```cpp
// 列出所有 Vulkan 设备
int num_devices = ggml_backend_vulkan_get_device_count();
for (int i = 0; i < num_devices; i++) {
    ggml_backend_device_desc desc = ggml_backend_vulkan_get_device_desc(i);
    printf("Device %d: %s\n", i, desc.description);
}

// 使用特定设备初始化后端
ggml_backend_t backend = ggml_backend_vulkan_init(device_index);
```

## 性能调优

### 1. 批量大小

- 增大批量大小以提高 GPU 利用率
- 避免太小导致 kernel 启动开销过大

### 2. 内存分配

- 重用缓冲区减少分配开销
- 使用流缓冲进行异步传输

### 3. 着色器选择

- 根据硬件自动选择最优着色器
- 手动指定量化格式以平衡精度和速度

### 4. 扩展利用

- 启用协作矩阵以获得最佳性能
- 使用 BF16 减少内存带宽需求

## 故障排查

### 常见问题

1. **初始化失败**
   - 检查 Vulkan 驱动是否正确安装
   - 验证 Vulkan SDK 版本
   - 检查 GPU 是否支持计算

2. **着色器编译错误**
   - 确保 GLSL 编译器路径正确
   - 检查着色器扩展支持
   - 查看编译器输出日志

3. **内存不足**
   - 减大批次大小
   - 使用量化格式
   - 清理未使用的缓冲区

4. **性能不佳**
   - 启用性能分析工具
   - 检查 GPU 利用率
   - 验证扩展是否被正确使用

### 调试工具

- **Vulkan Validation Layers**：验证 API 使用正确性
- **RenderDoc**：图形调试和分析工具
- **Nsight Systems**：性能分析（NVIDIA）
- **Radeon GPU Profiler**：性能分析（AMD）

## 性能对比

Vulkan 后端在不同场景下的性能特点：

| 场景 | 优势 | 劣势 |
|------|------|------|
| 推理 | 高性能，低延迟 | 初始化较慢 |
| 小批量 | 良好的扩展性 | Kernel 开销 |
| 大批次 | 优秀吞吐量 | 内存需求高 |
| 量化模型 | 广泛支持 | 精度损失 |

## 相关链接

- [Vulkan 官方网站](https://www.vulkan.org/)
- [Vulkan 规范](https://registry.khronos.org/vulkan/)
- [SPIR-V 规范](https://registry.khronos.org/SPIR-V/)
- [Vulkan SDK](https://vulkan.lunarg.com/)
- [GLSL 规范](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf)

## 许可证

遵循 llama.cpp 项目许可证。