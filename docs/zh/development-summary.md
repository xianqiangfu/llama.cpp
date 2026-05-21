# GGML 开发文档摘要

## 概述

GGML 项目的开发文档位于 `docs/development/` 目录，包含添加新模型、调试测试、解析器使用等开发指南。

## 文档列表

### 1. HOWTO-add-model.md - 如何添加新模型架构

**概述**：详细说明了向 llama.cpp 添加新模型架构的完整流程。

**主要步骤**：

1. **将模型转换为 GGUF 格式**
   - 使用 Python 转换脚本
   - 依赖 gguf 库
   - 定义模型架构注册
   - 配置 GGUF 张量布局
   - 映射原始张量名称

2. **在 llama.cpp 中定义模型架构**
   - 在 `src/llama-arch.h` 中定义新架构枚举
   - 在 `src/llama-arch.cpp` 中添加架构名称
   - 实现模型特定逻辑

3. **构建 GGML 图实现**
   - 在 `src/llama-models.h` 中实现前向传播
   - 定义张量图构建逻辑
   - 实现模型特定操作

4. **多模态编码器实现（可选）**
   - 实现视觉/音频编码器
   - 集成到模型流程

**验证工具**：
- cli - 命令行工具
- completion - 补全工具
- imatrix - 重要性矩阵
- quantize - 量化工具
- server - HTTP 服务器

### 2. debugging-tests.md - 测试调试技巧

**概述**：提供了调试特定测试的方法，保持快速的反馈循环。

**使用方法**：

```bash
# 运行特定测试
./scripts/debug-test.sh test-tokenizer

# 在 GDB 中调试
./scripts/debug-test.sh -g test-tokenizer

# 直接运行测试编号
./scripts/debug-test.sh test 23
```

**调试流程**：

1. **重置并设置构建目录**
   ```bash
   rm -rf build-ci-debug && mkdir build-ci-debug && cd build-ci-debug
   ```

2. **构建测试二进制文件**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug -DLLAMA_CUDA=1 -DLLAMA_FATAL_WARNINGS=ON ..
   make -j
   ```

3. **查找测试**
   ```bash
   ctest -R "test-tokenizer" -V -N
   ```

4. **在 GDB 中运行**
   ```bash
   gdb --args ${Test Binary} ${Test GGUF Model}
   ```

### 3. parsing.md - 模型输出解析

**概述**：介绍了使用 PEG 解析器解析模型输出的方法。

**解析器类型**：
- `common_peg_*` - 通用解析器
- `common_chat_peg_*` - 聊天专用解析器

**特性**：
- 流式输入部分解析
- 内置 JSON 解析器
- AST 生成

**基本匹配器**：
- `eps()` - 空匹配
- `start()` / `end()` - 开始/结束锚点
- `literal(string)` - 字面量匹配
- `any()` - 任意字符

**组合器**：
- `sequence(...)` - 顺序匹配
- `choice(...)` - 选择第一个成功的
- `one_or_more(p)` - 一次或多次重复
- `zero_or_more(p)` - 零次或多次重复
- `optional(p)` - 可选匹配
- `repeat(p, min, max)` - 限定次数重复

**JSON 解析器**：
- `json()` - 完整 JSON
- `json_object()` - JSON 对象
- `json_array()` - JSON 数组
- `json_string()` - JSON 字符串
- `json_number()` - JSON 数字

### 4. token_generation_performance_tips.md - Token 生成性能提示

**概述**：提供了优化 token 生成性能的建议。

**主要优化方向**：
- 算法优化
- 内存优化
- 批处理优化
- 后端优化

## 开发环境配置

### 必需工具

- CMake 3.14+
- C/C++ 编译器 (GCC/Clang/MSVC)
- Python 3.10+

### 可选依赖

- CUDA 工具包 (NVIDIA GPU)
- Metal SDK (Apple Silicon)
- oneAPI (Intel GPU)
- ROCm (AMD GPU)

### 构建类型

- `Debug` - 调试模式
- `Release` - 发布模式
- `RelWithDebInfo` - 带调试信息的发布模式

## 代码规范

### 命名规范

- 函数：`snake_case`
- 变量：`snake_case`
- 结构体/类：`snake_case`
- 常量：`UPPER_CASE`
- 宏：`UPPER_CASE`

### 文件组织

```
src/
├── llama-*.h          # 核心头文件
├── llama-models.h     # 模型实现
└── llama-vocab.h      # 词表处理
```

## 测试要求

### 单元测试

- 使用自定义测试框架
- 测试文件位于 `tests/` 目录
- 测试命名：`test-*.cpp`

### 集成测试

- 验证端到端功能
- 测试模型加载和推理

### 性能测试

- 使用 `llama-bench` 工具
- 测试不同配置的性能

## 贡献流程

1. Fork 项目
2. 创建特性分支
3. 实现更改
4. 添加测试
5. 运行测试
6. 提交 Pull Request

## 相关资源

- 添加模型指南：`docs/development/HOWTO-add-model.md`
- 测试调试：`docs/development/debugging-tests.md`
- 解析器使用：`docs/development/parsing.md`
- 性能优化：`docs/development/token_generation_performance_tips.md`