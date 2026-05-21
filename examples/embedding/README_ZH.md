# llama.cpp/example/embedding

本示例演示如何使用 llama.cpp 为给定文本生成高维嵌入向量。

## 快速开始

运行以下命令，确保使用正确的模型路径：

### 基于 Unix 的系统（Linux、macOS 等）：

```bash
./llama-embedding -m ./path/to/model --pooling mean --log-disable -p "Hello World!" 2>/dev/null
```

### Windows：

```powershell
llama-embedding.exe -m ./path/to/model --pooling mean --log-disable -p "Hello World!" 2>$null
```

上述命令将输出以空格分隔的浮点数值。

## 附加参数

### --embd-normalize $integer$

| $integer$ | 说明 | 公式 |
|-----------|------|------|
| $-1$      | 不归一化 | - |
| $0$       | 最大绝对值归一化到 int16 | $\Large{{32760 * x_i} \over\max \lvert x_i\rvert}$ |
| $1$       | 曼哈顿距离归一化（taxicab） | $\Large{x_i \over\sum \lvert x_i\rvert}$ |
| $2$       | 欧几里得距离归一化（默认） | $\Large{x_i \over\sqrt{\sum x_i^2}}$ |
| $>2$      | p-范数归一化 | $\Large{x_i \over\sqrt[p]{\sum \lvert x_i\rvert^p}}$ |

### --embd-output-format $'string'$

| $'string'$ | 说明 | 格式 |
|------------|------|------|
| ''         | 保持原样 | （默认） |
| 'array'    | 单个嵌入 | $[[x_1,...,x_n]]$ |
|            | 多个嵌入 | $[[x_1,...,x_n],[x_1,...,x_n],...,[x_1,...,x_n]]$ |
| 'json'     | OpenAI 风格 | JSON 格式 |
| 'json+'    | 添加余弦相似度矩阵 | JSON 格式 + 相似度 |
| 'raw'      | 纯文本输出 | 纯文本 |

### --embd-separator $"string"$

| $"string"$   | 说明 |
|--------------|------|
| "\n"         | （默认换行符） |
| "<#embSep#>" | 自定义分隔符示例 1 |
| "<#sep#>"    | 自定义分隔符示例 2 |

## 使用示例

### 基于 Unix 的系统（Linux、macOS 等）：

```bash
./llama-embedding -p 'Castle<#sep#>Stronghold<#sep#>Dog<#sep#>Cat' --pooling mean --embd-separator '<#sep#>' --embd-normalize 2 --embd-output-format '' -m './path/to/model.gguf' --n-gpu-layers 99 --log-disable 2>/dev/null
```

### Windows：

```powershell
llama-embedding.exe -p 'Castle<#sep#>Stronghold<#sep#>Dog<#sep#>Cat' --pooling mean --embd-separator '<#sep#>' --embd-normalize 2 --embd-output-format '' -m './path/to/model.gguf' --n-gpu-layers 99 --log-disable 2>$null
```

## 功能特点

- **文本嵌入**：将文本转换为高维向量表示
- **批量处理**：支持同时处理多个文本
- **多种归一化**：支持多种向量归一化方法
- **多种输出格式**：支持数组、JSON、纯文本等格式
- **相似度计算**：支持余弦相似度矩阵计算

## 使用场景

- 语义搜索
- 文本相似度计算
- 聚类分析
- 推荐系统
- 文本分类
- 检索增强生成（RAG）

## 模型要求

- 需要使用嵌入专用模型（如 bge-base-en-v1.5）
- 支持 GGUF 格式的嵌入模型
- 模型应经过训练以生成高质量的嵌入向量

## 相关示例

- [retrieval](../retrieval/README_ZH.md) - 基于嵌入的检索增强示例

## 注意事项

- 不同模型的嵌入维度可能不同
- 归一化方法的选择会影响相似度计算结果
- 批量处理可以提高效率，但会增加内存使用