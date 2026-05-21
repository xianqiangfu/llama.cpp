# 自动草稿/推测解码文档摘要

## 概述

llama.cpp 支持推测解码（自动草稿）技术，这是一种可以通过预测主模型之前的多个 token 来显著加速 token 生成的技术。推测解码利用了批量计算比顺序计算更高效的特性，通过快速生成草稿 token，然后使用目标模型在单个批次中验证它们，当草稿预测经常正确时，可以实现显著的加速。

## 自动草稿的原理

### 核心机制

1. **草稿生成阶段**：使用较小的模型或 n-gram 技术快速生成候选 token 序列
2. **验证阶段**：使用主模型在单个批次中并行验证这些候选 token
3. **接受/拒绝**：主模型接受的 token 直接输出，被拒绝的 token 需要重新生成
4. **加速效果**：当预测正确率较高时，可以显著减少主模型的推理次数

### 加速原理

- **批量处理效率**：提示处理中批量计算 n 个 token 比顺序生成 n 个 token 更高效
- **预测利用**：利用模型输出的概率分布进行预测，节省计算资源
- **并行验证**：同时验证多个草稿 token，减少串行计算

## 自动草稿的实现方法

### 1. 草稿模型方法 (Draft Model)

**特点**：
- 使用一个小得多的模型（草稿模型）生成候选 token
- 最常用的推测解码方法
- 可与无草稿解码混合使用

**优点**：
- 通用性强，适用于各种模型
- 预测质量较高
- 可灵活调整草稿模型大小

**缺点**：
- 需要加载两个模型，增加内存消耗
- 需要维护草稿模型和主模型的同步

### 2. n-gram 缓存方法 (ngram-cache)

**原理**：
- 维护短 n-gram 序列的统计信息
- 使用从统计信息得出的概率计算草稿
- 可从文件加载外部统计信息提高准确性

**特点**：
- 无需额外模型
- 基于历史模式预测
- 支持外部统计导入

### 3. n-gram 简单映射方法 (ngram-simple)

**原理**：
- 在历史记录中查找与当前 n-gram 匹配的最后一个 n-gram
- 使用匹配 n-gram 之后的 m 个 token 创建草稿
- 最简单的自推测方法，开销最小

**示例配置**：
```bash
llama-server [...] --spec-type ngram-simple --spec-draft-n-max 64
```

**特点**：
- 实现简单
- 计算开销小
- 适用于重复性强的文本

### 4. n-gram 映射密钥方法 (ngram-map-k)

**原理**：
- 在 token 历史中查找当前 n-gram（密钥）
- 如果密钥后跟随相同的 m 个 token 多次，使用这些 token 创建草稿
- 需要最小出现次数才能生成草稿

**特点**：
- 使用内部哈希映射
- 记录接受 token 数量
- 比简单映射更智能

**示例配置**：
```bash
llama-server [...] --spec-type ngram-map-k --spec-draft-n-max 64
```

### 5. n-gram 映射密钥-4-值方法 (ngram-map-k4v)

**原理**：
- 实验性实现
- 对每个密钥跟踪最多四个值（m-grams）
- 统计每个 mgram 的出现次数
- 选择最频繁的 mgram 作为草稿

**特点**：
- 更复杂的决策机制
- 可处理多种可能情况
- 适用于长重复模式

**示例配置**：
```bash
llama-server [...] --spec-type ngram-map-k4v --spec-ngram-map-k4v-size-n 8 \
  --spec-ngram-map-k4v-size-m 8 --spec-ngram-map-k4v-min-hits 2 --spec-draft-n-max 64
```

### 6. n-gram 模数方法 (ngram-mod)

**原理**：
- 使用 LCG 计算每个 ngram 的哈希
- 存储每个哈希的下一个 token
- 通过滚动哈希快速查找下一个 token

**特点**：
- 轻量级（约 16 MB）
- 恒定的内存和复杂度
- 可生成可变的草稿长度
- 哈希池在所有服务器插槽间共享

**示例配置**：
```bash
llama-server ... --spec-type ngram-mod --spec-ngram-mod-n-match 24 \
  --spec-ngram-mod-n-min 48 --spec-ngram-mod-n-max 64
```

**应用场景**：
- 迭代文本/代码块处理
- 推理模型（需要重复思考过程）
- 摘要生成

## 不同实现方法的对比

| 方法 | 内存消耗 | 计算复杂度 | 预测质量 | 适用场景 |
|------|---------|-----------|---------|---------|
| draft-model | 高（双模型） | 中 | 高 | 通用场景 |
| ngram-cache | 中 | 低 | 中 | 有统计数据的场景 |
| ngram-simple | 低 | 很低 | 中低 | 强重复性文本 |
| ngram-map-k | 中 | 低 | 中 | 模式匹配场景 |
| ngram-map-k4v | 中高 | 中 | 中高 | 复杂重复模式 |
| ngram-mod | 低（16MB） | 低 | 中 | 多会话共享场景 |

## 配置参数详解

### 通用参数

- `--spec-type`：选择推测解码类型（none|draft-simple|draft-mtp|ngram-cache|ngram-simple|ngram-map-k|ngram-map-k4v|ngram-mod）
- `--spec-default`：使用默认配置（启用 ngram-mod）
- `--spec-draft-n-max`：最大草稿 token 数量（默认：3）
- `--spec-draft-n-min`：最小草稿 token 数量（默认：0）
- `--spec-draft-p-split`：分割概率（默认：0.10）
- `--spec-draft-p-min`：最小概率（默认：0.00）

### n-gram 参数

- `--spec-ngram-*-size-n`：查找 n-gram 的长度（决定向后查看多少 token）
- `--spec-ngram-*-size-m`：草稿 m-gram 的长度（决定生成多少 token）
- `--spec-ngram-*-min-hits`：最小命中数（默认：1）

### 性能调优建议

1. **模型选择**：
   - MoE 模型需要更长的草稿
   - 密集模型可以减少 n-gram 参数

2. **参数调整**：
   - 增加 `size-m` 可提高速度但降低接受率
   - 增加 `min-hits` 可提高预测准确性但减少草稿数量

3. **硬件优化**：
   - GPU 层数配置：`--spec-draft-ngl`
   - CPU 亲和性：`--spec-draft-cpu-mask`
   - 线程配置：`--spec-draft-threads`

## 统计信息解读

### 关键指标

- `#calls(b,g,a)`：调用次数（开始、生成、累积）
- `#gen drafts`：生成的草稿数量
- `#acc drafts`：被接受的草稿数量
- `#gen tokens`：生成的 token 总数（包括被拒绝的）
- `#acc tokens`：被接受的 token 数量
- `dur(b,g,a)`：各阶段的持续时间

### 接受率计算

```
draft acceptance rate = #acc tokens / #gen tokens
```

理想情况下，接受率应该在 0.5-0.8 之间，超过 0.8 可能预测过于保守，低于 0.5 可能预测质量不够。

## 最佳实践

### 1. 场景选择

- **代码生成**：使用 ngram-simple 或 ngram-map-k
- **推理任务**：使用 ngram-mod
- **通用对话**：使用草稿模型
- **摘要任务**：使用 ngram-mod 或混合方法

### 2. 参数调优

- 从默认参数开始
- 根据接受率调整 `size-m` 和 `min-hits`
- 监控内存使用情况

### 3. 性能监控

- 关注接受率
- 监控生成速度
- 检查内存占用

### 4. 故障排除

- 接受率过低：降低 `size-m`，提高 `min-hits`
- 内存不足：切换到轻量级方法（如 ngram-mod）
- 速度慢：增加草稿长度，调整 n-gram 参数

## 结论

推测解码（自动草稿）是 llama.cpp 中一项重要的性能优化技术。通过选择合适的实现方法和配置参数，可以在保持模型输出质量的同时显著提升推理速度。建议根据具体应用场景和硬件资源选择最适合的方案，并通过持续监控和调优来获得最佳性能。