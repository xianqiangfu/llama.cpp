# cvector-generator

此示例演示如何使用 GGUF 模型生成控制向量。

相关 PR：
- [添加控制向量支持](https://github.com/ggml-org/llama.cpp/pull/5970)
- (Issue) [使用 llama.cpp 生成控制向量](https://github.com/ggml-org/llama.cpp/issues/6880)
- [添加 cvector-generator 示例](https://github.com/ggml-org/llama.cpp/pull/7514)

## 示例

```sh
# 仅使用 CPU
./cvector-generator -m ./llama-3.Q4_K_M.gguf

# 使用 GPU
./cvector-generator -m ./llama-3.Q4_K_M.gguf -ngl 99

# 使用高级选项
./cvector-generator -m ./llama-3.Q4_K_M.gguf -ngl 99 --pca-iter 2000 --pca-batch 100

# 使用均值而不是 PCA
./cvector-generator -m ./llama-3.Q4_K_M.gguf --method mean

# 查看帮助消息
./cvector-generator -h
# 然后，查看 "cvector" 部分
```

## 命令行选项

### 基本选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-m, --model PATH` | 模型文件路径 | - |
| `-o, --output PATH` | 输出文件路径 | control_vector.gguf |
| `-p, --positive PATH` | 正向提示词文件 | positive.txt |
| `-n, --negative PATH` | 负向提示词文件 | negative.txt |
| `-h, --help` | 显示帮助信息 | - |

### 方法选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `--method METHOD` | 计算方法（mean/pca） | pca |
| `--pca-iter N` | PCA 迭代次数 | 1000 |
| `--pca-batch N` | PCA 批处理大小 | 512 |

### 采样选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-t, --threads N` | 线程数 | CPU 核心数 |
| `-ngl, --n-gpu-layers N` | GPU 层数 | 0 |
| `--temperature N` | 采样温度 | 1.0 |
| `--top-p N` | Top-p 采样参数 | 0.9 |
| `--top-k N` | Top-k 采样参数 | 40 |

### 输出选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `--verbose` | 启用详细输出 | false |
| `--quiet` | 禁用输出 | false |

## 工作原理

### 什么是控制向量？

控制向量是一种通过修改模型激活来控制模型行为的技术。它允许：

- 改变模型输出的风格
- 调整模型的行为模式
- 实现特定任务定制

### 生成过程

1. **正向提示词**：定义您希望模型表现的行为
2. **负向提示词**：定义您希望模型避免的行为
3. **向量计算**：比较正向和负向提示词的激活差异
4. **PCA 降维**：使用主成分分析降低维度（可选）
5. **输出向量**：生成可用于推理的控制向量

### 计算方法

#### Mean 方法

- 计算正向和负向提示词激活的平均值
- 取两者之间的差异作为控制向量
- 简单快速，但可能不够精确

#### PCA 方法

- 使用主成分分析提取主要特征
- 保留最重要的方向
- 通常提供更好的控制效果

## 使用技巧

如果您在提示词中有多行，可以转义换行符（将其更改为 `\n`）。例如：

```
<|im_start|>system\nAct like a person who is extremely happy.<|im_end|>
<|im_start|>system\nYou are in a very good mood today<|im_end|>
```

### 使用 llama-cli 的输出文件示例：

（提示：控制向量在应用于高于 10 的层时效果更好）

```sh
./llama-cli -m ./llama-3.Q4_K_M.gguf -p "<|start_header_id|>system<|end_header_id|>\n\nYou are a helpful assistant<|eot_id|><|start_header_id|>user<|end_header_id|>\n\nSing a song<|im_end|><|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n" --special --control-vector-scaled ./control_vector.gguf 0.8 --control-vector-layer-range 10 31
```

## 提示词设计

### 好的提示词特征

1. **明确具体**：清楚地描述期望的行为
2. **对比明显**：正负向提示词应该有明显区别
3. **一致性强**：使用相似的格式和风格
4. **充分覆盖**：涵盖您想要控制的各种方面

### 示例提示词

#### 正向提示词示例 (positive.txt)

```
You are an enthusiastic and energetic assistant.
Your responses are always positive and motivating.
You use emojis and exclamation marks frequently.
You express excitement about helping users.
```

#### 负向提示词示例 (negative.txt)

```
You are a calm and formal assistant.
Your responses are neutral and objective.
You avoid using emojis or exclamation marks.
You maintain a professional tone at all times.
```

## 应用场景

### 1. 性格调整

- 让模型更友好或更正式
- 调整热情程度
- 控制语言风格

### 2. 语气控制

- 改变回答的语气
- 调整情感表达
- 控制敏感度

### 3. 行为引导

- 引导模型关注特定方面
- 改变回答风格
- 调整详细程度

## 性能优化

### GPU 加速

```bash
./cvector-generator -m model.gguf -ngl 99
```

### 线程优化

```bash
./cvector-generator -m model.gguf -t 8
```

### 方法选择

- 对于快速测试，使用 `--method mean`
- 对于更好的质量，使用 `--method pca`

## 故障排除

### 常见问题

1. **向量效果不明显**
   - 检查正负向提示词是否有足够区别
   - 尝试增加提示词的多样性
   - 调整缩放因子

2. **生成时间过长**
   - 使用 GPU 加速
   - 减少 PCA 迭代次数
   - 使用 mean 方法

3. **内存不足**
   - 使用量化模型
   - 减少 GPU 层数
   - 使用较小的批处理大小

## 最佳实践

1. **测试缩放因子**：从 0.5 开始，根据效果调整
2. **选择合适的层**：通常 10-30 层效果较好
3. **迭代改进**：根据结果调整提示词
4. **备份向量**：保存成功的控制向量以供复用
5. **文档记录**：记录每个向量的用途和效果

## 相关资源

- [控制向量原理](https://arxiv.org/abs/2308.04823)
- [llama.cpp 控制向量支持](https://github.com/ggml-org/llama.cpp/pull/5970)
- [向量操作文档](../../docs/control-vectors.md)

## 注意事项

1. **不保证效果**：控制向量可能不适用于所有模型或任务
2. **需要测试**：建议先在小规模测试
3. **权衡考虑**：更强的控制可能影响生成质量
4. **模型依赖**：不同模型可能需要不同的控制向量