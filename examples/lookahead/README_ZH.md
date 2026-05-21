# llama.cpp/examples/lookahead

Lookahead 解码技术演示。

## 简介

Lookahead 解码是一种先进的文本生成优化技术，通过预测多个未来 token 并并行验证它们来加速生成过程。这种技术特别适合代码生成和结构化文本生成任务。

## 技术详情

更多技术信息请参考：

- [Lookahead Decoding 博客](https://lmsys.org/blog/2023-11-21-lookahead-decoding/)
- [PR #4207](https://github.com/ggml-org/llama.cpp/pull/4207) - llama.cpp 实现

## 快速开始

```bash
llama-lookahead -hf ggml-org/Qwen2.5-Coder-3B-Q8_0-GGUF -p "// network server implemented in C\n// author: Peter Hacker\n\n#include" -e -ngl 99 -t 4 -n 512 -c 4096 -kvu
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `-hf, --hf-model` | Hugging Face 模型名称或路径 |
| `-p, --prompt` | 输入提示词 |
| `-e, --escape` | 转义特殊字符 |
| `-ngl, --n-gpu-layers` | GPU 层数 |
| `-t, --threads` | 使用的线程数 |
| `-n, --n-predict` | 生成的 token 数量 |
| `-c, --ctx-size` | 上下文窗口大小 |
| `-kvu, --kv-unified` | 使用统一的 KV 缓存 |

## 工作原理

Lookahead 解码的核心思想：

1. **预测多个分支**：为每个可能的下一个 token 预测未来的 token 序列
2. **构建 N-gram 图**：构建所有可能的未来序列图
3. **并行验证**：并行验证多个可能的路径
4. **贪婪选择**：贪婪地选择验证通过的路径
5. **继续扩展**：基于选择的路径继续预测

## 性能特点

- **加速比**：在某些任务上可达 2-4 倍加速
- **内存开销**：需要额外的内存存储预测图
- **适用场景**：特别适合代码生成和结构化文本
- **延迟降低**：减少了串行解码的延迟

## 适用场景

- **代码生成**：代码具有很强的局部性
- **结构化文本**：JSON、XML 等格式化文本
- **编程助手**：IDE 中的代码补全
- **API 生成**：API 响应生成

## 模型建议

- 适合中小型模型（1B-7B）
- 代码专用模型效果最佳
- Qwen2.5-Coder、CodeLlama 等

## 配置建议

- **上下文大小**：根据任务需求调整
- **线程数**：设置为 CPU 核心数
- **GPU 层数**：尽可能使用 GPU 加速
- **生成长度**：适合中等长度的生成任务

## 与推测解码的区别

| 特性 | Lookahead | Speculative Decoding |
|------|-----------|---------------------|
| 草稿模型 | 不需要单独的草稿模型 | 需要小的草稿模型 |
| 预测策略 | N-gram 图预测 | 线性预测 |
| 内存使用 | 较高 | 较低 |
| 适用场景 | 代码、结构化文本 | 通用文本 |
| 加速比 | 2-4x | 2-3x |

## 相关示例

- [speculative](../speculative/README_ZH.md) - 推测解码技术
- [speculative-simple](../speculative-simple/README_ZH.md) - 基础推测解码

## 注意事项

- 需要额外的内存存储预测图
- 不适合所有任务，需要测试效果
- 对于随机性强的文本，效果可能不佳
- 需要权衡内存使用和性能提升

## 优化建议

- 启用 KV 缓存统一（`-kvu`）
- 使用 GPU 加速（`-ngl 99`）
- 调整线程数以匹配硬件
- 根据任务调整上下文大小