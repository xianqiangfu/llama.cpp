# llama.cpp/examples/speculative-simple

基础贪婪推测解码演示。

## 简介

本示例演示了基础的贪婪推测解码（Greedy Speculative Decoding）技术。这是 [speculative](../speculative/README_ZH.md) 示例的简化版本，专注于最简单但有效的推测解码实现。

## 快速开始

```bash
./bin/llama-speculative-simple \
    -m  ../models/qwen2.5-32b-coder-instruct/ggml-model-q8_0.gguf \
    -md ../models/qwen2.5-1.5b-coder-instruct/ggml-model-q4_0.gguff \
    -f test.txt -c 0 -ngl 99 --color on \
    --sampling-seq k --top-k 1 -fa on --temp 0.0 \
    -ngld 99 --spec-draft-n-max 16 --spec-draft-n-draft-min 5 --draft-p-min 0.9
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `-m, --model` | 主模型路径（必需） |
| `-md, --model-draft` | 草稿模型路径（必需） |
| `-f, --file` | 输入文件路径 |
| `-c, --ctx-size` | 上下文窗口大小（0 表示使用模型默认值） |
| `-ngl, --n-gpu-layers` | 主模型 GPU 层数 |
| `-ngld, --n-gpu-layers-draft` | 草稿模型 GPU 层数 |
| `--color` | 是否启用彩色输出 |
| `--sampling-seq k` | 采样序列（贪婪采样） |
| `--top-k 1` | Top-K 采样参数（贪婪采样） |
| `-fa, --flash-attn` | 是否启用 Flash Attention |
| `--temp 0.0` | 温度参数（0 表示贪婪采样） |
| `--spec-draft-n-max` | 最大草稿 token 数量 |
| `--spec-draft-n-draft-min` | 最小草稿 token 数量 |
| `--draft-p-min` | 草稿接受概率阈值 |

## 工作原理

1. **贪婪草稿**：草稿模型使用贪婪采样预测多个 token
2. **并行验证**：主模型并行验证这些预测
3. **接受/拒绝**：接受验证通过的 token，拒绝失败的 token
4. **继续生成**：使用接受的 token 继续下一轮

## 性能优化

- **贪婪采样**：减少采样开销
- **Flash Attention**：加速注意力计算
- **GPU 加速**：主模型和草稿模型都可以使用 GPU
- **并行验证**：一次验证多个预测

## 使用场景

- 代码生成任务
- 结构化文本生成
- 需要确定性的场景
- 快速推理需求

## 模型建议

- **主模型**：Qwen2.5-32B-Coder 或类似高质量模型
- **草稿模型**：Qwen2.5-1.5B-Coder 或相同系列的较小模型
- 推荐使用 Q8_0 量化的主模型
- 草稿模型可以使用 Q4_0 量化以节省内存

## 配置建议

- `--spec-draft-n-max`：通常设置为 8-16
- `--spec-draft-n-draft-min`：通常设置为 4-8
- `--draft-p-min`：通常设置为 0.8-0.95
- 草稿模型应比主模型小 10-30 倍

## 相关示例

- [speculative](../speculative/README_ZH.md) - 完整推测解码实现
- [lookahead](../lookahead/README_ZH.md) - Lookahead 解码技术

## 注意事项

- 确保主模型和草稿模型来自同一系列
- 草稿模型质量对加速效果影响很大
- 温度设置为 0 以确保贪婪采样
- 需要足够的 GPU 内存支持两个模型