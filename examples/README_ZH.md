# llama.cpp 示例程序总览

本目录包含 llama.cpp 库的各种示例程序，展示了如何使用 llama.cpp 实现不同的功能。

## 示例列表

### 基础示例

- **[simple](./simple/README_ZH.md)** - 展示 llama.cpp 的最小化文本生成用法
- **[simple-chat](./simple-chat/README_ZH.md)** - 使用 GGUF 文件中的聊天模板创建简单聊天程序
- **[embedding](./embedding/README_ZH.md)** - 使用 llama.cpp 为给定文本生成高维嵌入向量

### 高级功能示例

- **[batched](./batched/README_ZH.md)** - 演示批处理文本生成
- **[batched.swift](./batched.swift/README_ZH.md)** - Swift 版本的批处理示例
- **[parallel](./parallel/README_ZH.md)** - 模拟并行处理传入请求
- **[retrieval](./retrieval/README_ZH.md)** - 基于余弦相似度的简单检索增强技术

### 性能优化示例

- **[speculative](./speculative/README_ZH.md)** - 推测解码和基于树的推测解码技术
- **[speculative-simple](./speculative-simple/README_ZH.md)** - 基础贪婪推测解码演示
- **[lookahead](./lookahead/README_ZH.md)** - Lookahead 解码技术演示

## 编译示例

所有示例都可以通过 CMake 构建系统编译。在项目根目录运行：

```bash
cmake -B build
cmake --build build
```

编译后的可执行文件将位于 `build/bin/` 目录下。

## 通用参数

大多数示例程序支持以下通用参数：

- `-m, --model` - 模型文件路径（必需）
- `-p, --prompt` - 输入提示词
- `-c, --ctx-size` - 上下文窗口大小
- `-ngl, --n-gpu-layers` - GPU 层数
- `-t, --threads` - 使用的线程数
- `-b, --batch-size` - 批处理大小

## 模型要求

- 使用 GGUF 格式的模型文件
- 支持的模型包括 LLaMA、Qwen、Mistral 等
- 不同示例可能需要特定类型的模型（如嵌入模型使用专门的嵌入版本）

## 获取帮助

每个示例程序都支持 `-h` 或 `--help` 参数以查看完整的参数列表和说明。

```bash
./llama-simple --help
```

## 更多资源

- [llama.cpp 主仓库](https://github.com/ggml-org/llama.cpp)
- [项目文档](../README.md)
- [Pull Requests](https://github.com/ggml-org/llama.cpp/pulls)