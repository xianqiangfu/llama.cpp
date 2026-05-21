# llama.cpp/examples/retrieval

基于余弦相似度的简单检索增强技术演示。

## 简介

本示例演示了如何使用文本嵌入和余弦相似度来实现基本的检索增强生成（RAG）系统。系统将文档分块并嵌入，然后根据查询相似度返回最相关的文档片段。

## 技术详情

更多信息请参考：[PR #6193](https://github.com/ggml-org/llama.cpp/pull/6193)

## 如何使用

`retrieval.cpp` 有以下专用参数：

| 参数 | 说明 |
|------|------|
| `--context-file` | 要嵌入的文件 - 可以多次指定此选项以嵌入多个文件 |
| `--chunk-size` | 每个文本块的最小大小 |
| `--chunk-separator` | 用于分隔文本块的字符串，默认为换行符 |

## 快速开始

```bash
llama-retrieval --model ./models/bge-base-en-v1.5-f16.gguf --top-k 3 --context-file README.md --context-file License --chunk-size 100 --chunk-separator .
```

这会对所有给定文件进行分块和嵌入，并启动一个循环请求查询输入：

```
Enter query:
```

## 示例输出

```
Enter query: describe the mit license
batch_decode: n_tokens = 6, n_seq = 1
Top 3 similar chunks:
filename: README.md
filepos: 119
similarity: 0.762334
textdata:
png)

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

[Roadmap](https://github.
--------------------
filename: License
filepos: 0
similarity: 0.725146
textdata:
MIT License

Copyright (c) 2023 Georgi Gerganov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
--------------------
filename: README.md
filepos: 9178
similarity: 0.621722
textdata:
com/cztomsik/ava) (MIT)
- [ptsochantaris/emeltal](https://github.com/ptsochantaris/emeltal)
- [pythops/tenere](https://github.
--------------------
```

## 工作流程

1. **文档预处理**：
   - 读取指定的文本文件
   - 根据分隔符将文本分割成块
   - 过滤掉过小的文本块

2. **嵌入生成**：
   - 使用嵌入模型为每个文本块生成向量
   - 存储嵌入向量和元数据（文件名、位置、原始文本）

3. **查询处理**：
   - 接收用户查询输入
   - 生成查询的嵌入向量

4. **相似度计算**：
   - 计算查询嵌入与所有文档嵌入的余弦相似度
   - 按相似度排序

5. **结果返回**：
   - 返回最相似的 K 个文档片段
   - 显示相似度分数和上下文信息

## 参数说明

| 参数 | 说明 |
|------|------|
| `--model` | 嵌入模型路径（必需） |
| `--top-k` | 返回的最相似片段数量 |
| `--context-file` | 要索引的文件路径（可多次指定） |
| `--chunk-size` | 文本块的最小大小（字符数） |
| `--chunk-separator` | 文本块分隔符（默认换行符） |
| `-c, --ctx-size` | 上下文窗口大小 |
| `-ngl, --n-gpu-layers` | GPU 层数 |
| `-t, --threads` | 使用的线程数 |

## 使用场景

- **文档问答**：基于文档内容回答问题
- **知识库搜索**：在大量文档中查找相关信息
- **代码搜索**：在代码库中搜索相关代码片段
- **内容推荐**：基于内容相似度推荐相关文档
- **上下文增强**：为大语言模型提供相关上下文

## 模型要求

- **嵌入专用模型**：使用经过嵌入训练的模型
- **推荐的嵌入模型**：
  - bge-base-en-v1.5
  - bge-small-en-v1.5
  - e5-base-v2
  - 其他 GGUF 格式的嵌入模型

## 配置建议

| 用途 | Top-K | Chunk Size | Chunk Separator |
|------|-------|------------|-----------------|
| 代码搜索 | 3 | 50-100 | \n |
| 文档问答 | 5 | 100-200 | .\n |
| 知识库搜索 | 10 | 200-500 | \n\n |
| 通用检索 | 5 | 150 | \n |

## 高级用法

### 多文件索引

```bash
llama-retrieval --model model.gguf \
  --context-file README.md \
  --context-file docs/*.md \
  --context-file src/*.cpp \
  --top-k 5
```

### 自定义分隔符

```bash
llama-retrieval --model model.gguf \
  --context-file data.txt \
  --chunk-separator "---" \
  --chunk-size 200
```

### 与 LLM 结合

检索到的片段可以用作大语言模型的上下文：

1. 使用 retrieval 示例获取相关文档
2. 将片段作为上下文传递给 LLM
3. 让 LLM 基于检索到的内容生成答案

## 性能优化

- **批处理嵌入**：批量处理多个文本块
- **GPU 加速**：使用 GPU 层加速嵌入计算
- **缓存机制**：缓存已计算的嵌入向量
- **索引优化**：使用高效的相似度搜索算法

## 相关示例

- [embedding](../embedding/README_ZH.md) - 嵌入生成示例
- [simple-chat](../simple-chat/README_ZH.md) - 聊天示例

## 注意事项

- 需要足够的内存存储所有嵌入向量
- 文本块大小影响检索质量和性能
- 相似度阈值需要根据具体任务调整
- 嵌入模型的质量直接影响检索效果

## 扩展功能

可以基于此示例实现：
- 向量数据库集成
- 增量索引更新
- 混合检索（关键词+语义）
- 重排序（reranking）机制
- 多语言支持