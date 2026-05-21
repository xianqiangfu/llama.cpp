# llama.cpp/examples/speculative

推测解码和基于树的推测解码技术演示。

## 简介

推测解码（Speculative Decoding）是一种加速文本生成的技术，通过使用较小的草稿模型预测多个 token，然后由主模型验证这些预测，从而减少计算时间。

基于树的推测解码（Tree-based Speculative Decoding）是推测解码的扩展，支持在树结构中并行验证多个候选序列。

## 技术详情

更多技术信息请参考以下 Pull Request：

- [PR #2926](https://github.com/ggml-org/llama.cpp/pull/2926) - 初始推测解码实现
- [PR #3624](https://github.com/ggml-org/llama.cpp/pull/3624) - 改进和优化
- [PR #5625](https://github.com/ggml-org/llama.cpp/pull/5625) - 基于树的推测解码

## 工作原理

1. **草稿阶段**：使用较小的草稿模型快速预测 N 个候选 token
2. **验证阶段**：主模型并行验证这些预测
3. **接受阶段**：接受验证通过的 token，拒绝失败的 token
4. **重复**：继续这个过程直到完成

## 性能提升

- 在合适的主模型和草稿模型组合下，可以实现 2-3 倍的速度提升
- 草稿模型应比主模型小 10-100 倍
- 适用于高质量的草稿模型

## 使用场景

- 需要快速文本生成的场景
- 服务器端推理加速
- 实时对话系统
- 长文本生成任务

## 模型选择

- **主模型**：较大、高质量的目标模型
- **草稿模型**：较小但与主模型架构相同的模型
- 推荐使用相同系列或兼容的模型组合

## 相关示例

- [speculative-simple](../speculative-simple/README_ZH.md) - 基础贪婪推测解码演示
- [lookahead](../lookahead/README_ZH.md) - Lookahead 解码技术

## 注意事项

- 需要选择合适的主模型和草稿模型组合
- 草稿模型的质量直接影响加速效果
- 不适合所有场景，需要根据具体任务测试效果

## 参数说明

具体参数请参考示例的 `--help` 输出，常见参数包括：
- `-m, --model` - 主模型路径
- `-md, --model-draft` - 草稿模型路径
- `--spec-draft-n-max` - 最大草稿 token 数量
- `--draft-p-min` - 草稿接受概率阈值