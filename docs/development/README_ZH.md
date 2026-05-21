# 开发文档总览

本目录包含为 llama.cpp 开发者提供的指南和文档。

## 目录结构

```
docs/development/
├── HOWTO-add-model.md          # 添加新模型的指南
├── debugging-tests.md          # 测试调试指南
├── parsing.md                  # 解析器开发文档
├── token_generation_performance_tips.md  # Token 生成性能优化
└── llama-star/                 # llama-star 相关
```

## 文档说明

### HOWTO-add-model.md
[添加新模型指南](./HOWTO-add-model.md)

本指南详细介绍了如何将新的 LLM 架构添加到 llama.cpp 中：

**主要内容包括：**

1. **模型架构定义**
   - 在 `ggml` 中定义新的张量操作
   - 添加模型特定的参数
   - 实现前向传播逻辑

2. **分词器支持**
   - 添加分词器配置
   - 支持不同的分词方式
   - 测试分词器功能

3. **GGUF 格式**
   - 定义 GGUF 元数据
   - 添加模型特定的键值对
   - 量化支持

4. **模型注册**
   - 在主模型文件中注册新模型
   - 添加模型类型枚举
   - 配置加载参数

**适用对象：**
- 需要支持新模型架构的开发者
- 需要修改现有模型的开发者

### debugging-tests.md
[测试调试指南](./debugging-tests.md)

本指南提供了调试 llama.cpp 测试的方法和技巧：

**主要内容包括：**

1. **测试框架**
   - CTest 使用
   - 断言和验证
   - 测试组织结构

2. **调试技巧**
   - 使用 GDB 调试
   - 日志和输出分析
   - 内存问题检测

3. **常见问题**
   - 测试失败排查
   - 性能瓶颈分析
   - 平台特定问题

**适用对象：**
- 开发和修改测试的开发者
- 排查测试故障的开发者

### parsing.md
[解析器开发文档](./parsing.md)

本文档介绍了 llama.cpp 中的解析器系统：

**主要内容包括：**

1. **自动解析器 (Autoparser)**
   - 工作原理
   - 配置和使用
   - 性能考虑

2. **PEG 解析器**
   - Parsing Expression Grammar
   - GBNF 语法
   - 解析器实现

3. **聊天模板**
   - Jinja 模板
   - 模板语法
   - 自定义模板

**适用对象：**
- 需要修改解析器的开发者
- 需要添加新语法支持的开发者

### token_generation_performance_tips.md
[Token 生成性能优化](./token_generation_performance_tips.md)

本指南提供了优化 Token 生成性能的各种技巧：

**主要内容包括：**

1. **采样优化**
   - 采样算法选择
   - 热度调节
   - Top-k 和 Top-p

2. **内存管理**
   - KV 缓存优化
   - 内存分配策略
   - 量化影响

3. **批处理**
   - 批大小选择
   - 并行推理
   - 吞吐量优化

4. **硬件加速**
   - GPU 利用率
   - 多线程设置
   - 后端选择

**适用对象：**
- 追求最佳性能的开发者
- 需要优化推理速度的开发者

### llama-star/
[llama-star](./llama-star/)

llama-star 是 llama.cpp 的开发工具和测试框架。

**主要功能：**
- 自动化测试
- 性能基准测试
- 代码质量检查

## 开发流程

### 1. 环境设置

```bash
# 克隆仓库
git clone https://github.com/ggml-org/llama.cpp.git
cd llama.cpp

# 安装依赖 (参考 build.md)
cmake -DLLAMA_BUILD_TESTS=ON -DLLAMA_BUILD_EXAMPLES=ON ..
make
```

### 2. 开发新功能

```bash
# 创建新分支
git checkout -b feature/new-feature

# 进行修改
# ...

# 运行测试
ctest
```

### 3. 添加测试

参考 [testing-tests.md](./debugging-tests.md) 添加和调试测试。

### 4. 添加模型

参考 [HOWTO-add-model.md](./HOWTO-add-model.md) 添加新模型支持。

### 5. 优化性能

参考 [token_generation_performance_tips.md](./token_generation_performance_tips.md) 优化性能。

### 6. 提交代码

```bash
# 添加文件
git add .

# 提交
git commit -m "feat: add new feature"

# 推送
git push origin feature/new-feature

# 创建 Pull Request
```

## 代码规范

### C++ 代码

- 使用 C++17 标准
- 遵循现有代码风格
- 添加适当的注释
- 使用智能指针管理内存

### Python 代码

- 遵循 PEP 8 规范
- 添加类型提示
- 编写文档字符串

### 测试

- 测试覆盖率 >= 80%
- 每个功能都应该有测试
- 使用清晰的测试名称

## 贡献指南

1. **Fork 仓库** - 创建你的分支
2. **创建分支** - 为每个功能创建分支
3. **提交代码** - 清晰的提交信息
4. **推送到分支** - 推送到你的 fork
5. **创建 Pull Request** - 描述你的更改

## 相关资源

### 代码仓库

- [主仓库](https://github.com/ggml-org/llama.cpp)
- [Issues](https://github.com/ggml-org/llama.cpp/issues)
- [Discussions](https://github.com/ggml-org/llama.cpp/discussions)

### 相关文档

- [文档目录总览](../README_ZH.md)
- [构建指南](../build.md)
- [后端文档](../backend/README_ZH.md)
- [运维文档](../ops/README_ZH.md)

### 外部资源

- [GGML 文档](https://github.com/ggml-org/ggml)
- [GGUF 格式](https://github.com/ggml-org/ggml/pull/302)
- [GBNF 语法](../../grammars/README.md)

## 社区

- GitHub Discussions
- Discord 服务器
- Reddit r/llamacpp

## 常见问题

### Q: 如何开始贡献？

A: 从修复小 bug 或添加文档开始，然后逐步参与更大项目。

### Q: 如何添加新的硬件后端？

A: 参考现有后端实现，在 `ggml/src/` 中添加新后端代码。

### Q: 测试在哪里？

A: 测试在 `tests/` 目录，参考 [调试测试指南](./debugging-tests.md)。

### Q: 性能基准如何运行？

A: 使用 `tools/llama-bench` 进行基准测试。

## 联系方式

如有问题，请通过以下方式联系：

- GitHub Issues - 报告问题
- GitHub Discussions - 讨论和疑问
- Pull Request - 代码审查