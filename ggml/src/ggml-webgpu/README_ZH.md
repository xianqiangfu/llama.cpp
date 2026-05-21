# WebGPU 后端说明

## 概述

WebGPU 后端是 llama.cpp 中基于 WebGPU 标准的 GPU 加速实现。WebGPU 是一个现代的 Web 图形和计算 API，提供对 GPU 的高效访问和跨平台支持。该后端使得 llama.cpp 能够在浏览器、移动设备以及支持 WebGPU 的桌面平台上运行，极大地扩展了其应用场景。

## 目录结构

```
ggml/src/ggml-webgpu/
├── CMakeLists.txt                    # CMake 构建配置
├── ggml-webgpu.cpp                    # WebGPU 后端核心实现 (225KB)
├── ggml-webgpu-shader-lib.hpp        # 着色器库 (135KB)
├── pre_wgsl.hpp                       # 预处理 WGSL 头文件
├── wgsl-shaders/                     # WGSL 着色器目录
│   ├── embed_wgsl.py                 # WGSL 嵌入脚本
│   ├── common_decls.tmpl             # 通用声明模板 (73KB)
│   ├── mul_mat_decls.tmpl            # 矩阵乘法声明模板 (49KB)
│   ├── mul_mat_vec_acc.tmpl          # 矩阵向量累积模板 (63KB)
│   ├── unary.wgsl                    # 一元运算着色器
│   ├── binary.wgsl                   # 二元运算着色器
│   ├── mul_mat.wgsl                  # 矩阵乘法 (29KB)
│   ├── mul_mat_vec.wgsl              # 矩阵向量乘法
│   ├── mul_mat_id.wgsl               # 矩阵乘法 ID
│   ├── mul_mat_id_gather.wgsl        # 矩阵乘法 ID gather
│   ├── mul_mat_id_vec.wgsl           # 矩阵乘法 ID 向量
│   ├── mul_mat_subgroup_matrix.wgsl  # 子群矩阵乘法
│   ├── mul_mat_reg_tile.wgsl         # 寄存器平铺矩阵乘法
│   ├── flash_attn.wgsl               # Flash Attention (29KB)
│   ├── flash_attn_tile.wgsl          # Flash Attention 平铺
│   ├── flash_attn_vec_blk.wgsl       # Flash Attention 向量块
│   ├── flash_attn_vec_reduce.wgsl    # Flash Attention 向量归约
│   ├── flash_attn_vec_split.wgsl     # Flash Attention 向量分割 (27KB)
│   ├── rope.wgsl                     # RoPE 位置编码
│   ├── soft_max.wgsl                 # Softmax
│   ├── rms_norm_mul.wgsl             # RMS 归一化乘法
│   ├── row_norm.wgsl                 # 行归一化
│   ├── get_rows.wgsl                 # 获取行 (29KB)
│   ├── set_rows.wgsl                 # 设置行
│   ├── glu.wgsl                      # GLU 激活
│   ├── gated_delta_net.wgsl          # 门控 Delta 网络
│   ├── ssm_conv.wgsl                  # SSM 卷积
│   ├── ssm_scan.wgsl                 # SSM 扫描
│   ├── conv2d.wgsl                   # 2D 卷积
│   ├── im2col.wgsl                   # 图像到列变换
│   ├── argsort.wgsl                  # 参数排序
│   ├── argsort_merge.wgsl             # 排序合并
│   ├── argmax.wgsl                   # 参数最大值
│   ├── cumsum.wgsl                   // 累积求和
│   ├── concat.wgsl                   // 张量拼接
│   ├── repeat.wgsl                   // 重复
│   ├── pad.wgsl                      // 填充
│   ├── upscale.wgsl                  // 上采样 (8KB)
│   ├── set.wgsl                      // 设置操作
│   ├── memset.wgsl                   // 内存设置
│   ├── scale.wgsl                    // 缩放
│   ├── cpy.wgsl                      // 复制
│   └── solve_tri.wgsl                // 三角求解
└── (在构建目录生成)
    └── ggml-wgsl-shaders.hpp         // 生成的 WGSL 着色器头文件
```

## 实现原理

### 1. WebGPU 架构

WebGPU 后端基于 WebGPU 标准架构：

```
应用程序层
    ↓
GGML 前端
    ↓
WebGPU 后端接口
    ↓
WebGPU API
    ↓
适配层
    ├─ wgpu-native (桌面)
    ├─ Dawn (Chrome)
    └─ emdawnwebgpu (Emscripten)
    ↓
图形 API
    ├─ Vulkan
    ├─ Metal
    ├─ Direct3D 12
    └─ OpenGL ES
    ↓
GPU 执行
```

### 2. 核心组件

#### 2.1 WebGPU 设备管理

- **适配器选择**：枚举并选择合适的 GPU 适配器
- **设备创建**：创建 WebGPU 设备和队列
- **配置管理**：管理设备限制和功能

#### 2.2 内存和缓冲管理

- **缓冲创建**：在 GPU 内存中创建缓冲
- **内存映射**：主机到设备的数据传输
- **绑定组**：管理资源绑定

#### 2.3 计算流水线

- **着色器模块**：加载和编译 WGSL 着色器
- **计算流水线**：创建和管理计算流水线
- **命令编码**：记录和执行计算命令

### 3. WGSL 着色器系统

#### 3.1 着色器生成流程

1. **WGSL 源码**：以 `.wgsl` 扩展名的着色器源文件
2. **模板处理**：使用 Python 脚本处理模板文件
3. **代码嵌入**：将 WGSL 代码嵌入到 C++ 头文件
4. **运行时编译**：WebGPU 在运行时编译着色器

#### 3.2 主要着色器类别

1. **矩阵运算**
   - `mul_mat.wgsl`：通用矩阵乘法
   - `mul_mat_vec.wgsl`：矩阵向量乘法
   - `mul_mat_subgroup_matrix.wgsl`：子群矩阵优化
   - `mul_mat_reg_tile.wgsl`：寄存器平铺优化

2. **Flash Attention**
   - `flash_attn.wgsl`：标准 Flash Attention
   - `flash_attn_tile.wgsl`：平铺优化版本
   - `flash_attn_vec_blk.wgsl`：向量块版本
   - `flash_attn_vec_reduce.wgsl`：向量归约版本
   - `flash_attn_vec_split.wgsl`：向量分割版本

3. **位置编码**
   - `rope.wgsl`：RoPE 位置编码

4. **归一化**
   - `rms_norm_mul.wgsl`：RMS 归一化乘法
   - `row_norm.wgsl`：行归一化

5. **激活函数**
   - `unary.wgsl`：一元运算（ReLU, GELU, SiLU, Tanh, Sigmoid 等）
   - `binary.wgsl`：二元运算（加、减、乘、除等）
   - `glu.wgsl`：GLU 激活函数

6. **特殊模型操作**
   - `gated_delta_net.wgsl`：门控 Delta 网络
   - `ssm_conv.wgsl`：状态空间模型卷积
   - `ssm_scan.wgsl`：状态空间模型扫描

7. **卷积操作**
   - `conv2d.wgsl`：2D 卷积
   - `im2col.wgsl`：图像到列变换

8. **注意力机制**
   - `soft_max.wgsl`：Softmax

9. **行操作**
   - `get_rows.wgsl`：获取行（支持多种量化格式）
   - `set_rows.wgsl`：设置行

10. **排序和搜索**
    - `argsort.wgsl`：参数排序
    - `argsort_merge.wgsl`：排序合并
    - `argmax.wgsl`：参数最大值索引

11. **归约操作**
    - `sum_rows.wgsl`：行求和
    - `cumsum.wgsl`：累积求和

12. **张量操作**
    - `concat.wgsl`：张量拼接
    - `repeat.wgsl`：重复
    - `pad.wgsl`：填充
    - `upscale.wgsl`：上采样

13. **工具操作**
    - `set.wgsl`：设置操作
    - `memset.wgsl`：内存设置
    - `scale.wgsl`：缩放
    - `cpy.wgsl`：复制
    - `solve_tri.wgsl`：三角求解

### 4. 平台适配

#### 4.1 桌面平台

- **wgpu-native**：Rust 实现的原生 WebGPU
- **Dawn**：Chrome 的 WebGPU 实现
- **后端映射**：
  - Linux: Vulkan
  - Windows: Direct3D 12 / Vulkan
  - macOS: Metal

#### 4.2 Web 平台

- **Emscripten**：WebAssembly 编译
- **emdawnwebgpu**：Dawn 的 WebAssembly 移植
- **浏览器支持**：Chrome、Edge、Firefox

#### 4.3 移动平台

- 通过 WebGPU API 访问移动 GPU
- 支持 iOS 和 Android

### 5. WGSL 语言特性

#### 5.1 数据类型

- **整型**：i32, u32, vec2<i32>, vec3<i32>, vec4<i32>
- **浮点型**：f32, vec2<f32>, vec3<f32>, vec4<f32>
- **矩阵型**：mat2x2<f32>, mat3x3<f32>, mat4x4<f32>
- **原子类型**：atomic<i32>, atomic<u32>

#### 5.2 量化支持

WebGPU 后端支持多种量化格式：

- **Q 格式**：Q2_K, Q3_K, Q4_K, Q5_K, Q6_K, Q8_0
- **IQ 格式**：IQ1_S, IQ2_S, IQ2_XS, IQ2_XXS, IQ3_S, IQ3_XXS, IQ4_NL, IQ4_XS

#### 5.3 子群操作

- **子群内建函数**：subgroupAdd, subgroupMul, subgroupMin, subgroupMax
- **子群矩阵**：加速矩阵运算
- **负载均衡**：高效的工作分配

### 6. 性能优化技术

#### 6.1 子群矩阵

- 使用 WGSL 子群矩阵操作
- 硬件加速的矩阵乘法
- 自动检测和启用硬件支持

#### 6.2 平铺优化

- 寄存器平铺减少内存访问
- 缓存局部性优化
- 共享内存利用

#### 6.3 Flash Attention

- 内存高效的注意力机制
- 多种优化变体
- 支持不同的输入大小

#### 6.4 向量化

- 使用向量化数据类型
- SIMD 指令利用
- 提升计算吞吐量

### 7. 工作流程

#### 7.1 初始化流程

1. 初始化 WebGPU 实例
2. 请求 GPU 适配器
3. 创建 WebGPU 设备和队列
4. 加载并编译着色器
5. 创建计算流水线
6. 初始化内存池

#### 7.2 计算流程

1. 准备输入数据并上传到 GPU
2. 创建绑定组绑定资源
3. 编码计算命令
4. 提交命令缓冲执行
5. 等待完成并获取结果

### 8. 模板系统

WebGPU 后端使用 Python 模板系统生成着色器代码：

- **common_decls.tmpl**：通用声明模板
  - 类型定义
  - 工具函数
  - 常量定义

- **mul_mat_decls.tmpl**：矩阵乘法声明模板
  - 不同数据类型的矩阵乘法函数
  - 量化支持

- **mul_mat_vec_acc.tmpl**：矩阵向量累积模板
  - 向量累积优化
  - 性能调优选项

## 编译选项

### CMake 选项

- `GGML_WEBGPU_DEBUG`：启用调试输出
- `GGML_WEBGPU_CPU_PROFILE`：启用 CPU 性能分析
- `GGML_WEBGPU_GPU_PROFILE`：启用 GPU 性能分析

### Emscripten 选项

- `GGML_WEBGPU_JSPI`：启用 JSPI (JavaScript Promise Integration)
- `EMDAWNWEBGPU_DIR`：自定义 emdawnwebgpu 路径

### 默认配置

- **Exceptions**：默认启用异常处理
- **Asyncify**：Emscripten 异步处理
- **Assertion Level**：调试模式下启用详细断言

## 依赖项

### 桌面平台

- **Dawn**：Chrome 的 WebGPU 实现
- **Vulkan SDK**：Linux/Windows 上作为后端

### Web 平台

- **Emscripten**：WebAssembly 编译工具链
- **emdawnwebgpu**：Dawn 的 WebAssembly 移植

### 构建工具

- **Python 3**：着色器模板处理
- **CMake 3.13+**：构建系统

## 平台支持

### 浏览器支持

| 浏览器 | 支持状态 | 说明 |
|--------|----------|------|
| Chrome | 完全支持 | 主要开发平台 |
| Edge | 完全支持 | 与 Chrome 相同 |
| Firefox | 部分支持 | 需要启用标志 |
| Safari | 部分支持 | macOS/iOS 限制 |

### 桌面平台

| 平台 | 后端 | 支持状态 |
|------|------|----------|
| Linux | Vulkan | 完全支持 |
| Windows | D3D12/Vulkan | 完全支持 |
| macOS | Metal | 完全支持 |

### 移动平台

| 平台 | 支持状态 |
|------|----------|
| iOS | 实验性 |
| Android | 部分支持 |

## 使用示例

### 基本使用

```cpp
// 初始化 WebGPU 后端
ggml_backend_t backend = ggml_backend_webgpu_init(0);

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
```

### 在浏览器中使用

```javascript
// 在浏览器中使用 WebAssembly + WebGPU
const { createWebGPUContext } = await import('./llama-webgpu.js');

// 初始化 WebGPU 上下文
const context = await createWebGPUContext();

// 加载模型并运行
const model = await context.loadModel(modelPath);
const result = await model.generate(prompt, options);
```

## 性能特性

### 优势

1. **跨平台统一**：同一套代码在多个平台运行
2. **低开销**：比 WebGL 更高效
3. **现代 API**：支持现代 GPU 特性
4. **沙箱安全**：浏览器环境安全执行

### 局限性

1. **性能差异**：相比原生后端有一定性能损失
2. **内存限制**：浏览器内存限制严格
3. **功能限制**：某些高级功能不可用
4. **兼容性**：浏览器支持仍在发展中

### 性能优化建议

1. **批量处理**：尽可能使用大批量
2. **减少传输**：减少 CPU-GPU 数据传输
3. **重用资源**：重用着色器和缓冲区
4. **启用子群**：利用子群操作加速

## 调试和性能分析

### 浏览器调试

- **Chrome DevTools**：GPU 性能分析
- **Firefox DevTools**：WebGPU 检查器
- **WebGPU Inspector**：专用调试工具

### 性能分析

- `GGML_WEBGPU_CPU_PROFILE`：CPU 性能分析
- `GGML_WEBGPU_GPU_PROFILE`：GPU 性能分析
- 浏览器性能分析器

## 常见问题

### 1. WebGPU 不可用

**原因**：浏览器不支持或未启用 WebGPU

**解决方案**：
- 更新到最新版 Chrome/Edge
- 在 Firefox 中启用 WebGPU 标志
- 检查系统 GPU 支持

### 2. 着色器编译失败

**原因**：WGSL 语法错误或不支持的操作

**解决方案**：
- 检查浏览器 WebGPU 支持级别
- 启用 `GGML_WEBGPU_DEBUG` 查看详细错误
- 检查着色器模板生成

### 3. 内存不足

**原因**：浏览器内存限制或缓冲区过大

**解决方案**：
- 减小批次大小
- 使用量化格式
- 清理未使用的缓冲区

### 4. 性能不佳

**原因**：不合理的计算或内存使用

**解决方案**：
- 启用性能分析
- 检查 GPU 利用率
- 优化计算图结构

## 未来发展

### 计划中的特性

1. **更多量化格式**：扩展量化支持
2. **高级子群操作**：更好的硬件利用
3. **异步计算**：提高并发性能
4. **多 GPU 支持**：并行计算

### 社区参与

- 贡献新的着色器优化
- 报告兼容性问题
- 提供性能反馈

## 相关链接

- [WebGPU 规范](https://www.w3.org/TR/webgpu/)
- [WGSL 规范](https://www.w3.org/TR/WGSL/)
- [WebGPU 示例](https://webgpu.dev/)
- [wgpu-native](https://github.com/gfx-rs/wgpu-native)
- [Dawn 项目](https://dawn.googlesource.com/dawn)
- [Emscripten](https://emscripten.org/)

## 许可证

遵循 llama.cpp 项目许可证。