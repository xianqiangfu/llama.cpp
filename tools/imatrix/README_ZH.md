# llama.cpp/tools/imatrix

为模型和给定文本数据集计算重要性矩阵。可以在量化期间使用，以增强量化模型的质量。
更多信息请参阅 <https://github.com/ggml-org/llama.cpp/pull/4861>。

## 使用方法

```
./llama-imatrix \
    -m model.gguf -f some-text.txt [-o imatrix.gguf] [--output-format {gguf,dat}] [--no-ppl] \
    [--process-output] [--chunk 123] [--save-frequency 0] [--output-frequency 10] \
    [--in-file imatrix-prev-0.gguf --in-file imatrix-prev-1.gguf ...] [--parse-special] \
    [--show-statistics] [...]
```

这里 `-m | --model` 指定模型名称和 `-f | --file` 指定包含校准数据（例如 `wiki.train.raw`）的文件是必需的。
方括号中的参数是可选的，具有以下含义：

* `-h | --help` 显示使用信息并退出。
* `-lv | --verbosity` 指定详细级别。如果设置为 `0`，除了已处理块的困惑度之外，不会生成任何其他输出。如果设置为 `1`，每次保存结果时都会向 `stderr` 写入一条消息。如果 `>=2`，每次为任何张量收集数据时都会输出一条消息。默认详细级别为 `1`。
* `-o | --output-file` 指定要存储计算数据的文件名。如果缺少，则使用 `imatrix.gguf`。
* `-ofreq | --output-frequency` 指定将到目前为止计算的结果保存到磁盘的频率。默认为 10（即，每 10 个块）
* `--output-format` 指定生成的 imatrix 文件的输出格式。可以是 "gguf" 或 "dat"（传统格式）。默认为 "gguf"。
* `--save-frequency` 指定在单独的文件中保存 imatrix 副本的频率。默认为 0（即，从不）
* `--process-output` 指定是否将为 `output.weight` 张量收集数据。通常，最好不要在量化 `output.weight` 时使用重要性矩阵，因此默认情况下将其设置为 `false`。
* `--in-file` 一个或多个现有的 imatrix 文件以加载和组合。对于从多次运行/数据集合并文件很有用。
* `--parse-special` 启用特殊 token 解析（例如，某些模型中的 `<|im_start|>`）。对于具有自定义分词器的模型很有用。
* `--chunk | --from-chunk` 跳过输入数据的前 `n` 个 token 块。对于恢复或跳过初始低质量数据很有用。
* `--chunks` 要处理的最大块数。默认为 -1，表示所有可用块。
* `--no-ppl` 禁用已处理块的困惑度计算。如果您想加快处理速度并且不关心困惑度，这很有用。
* `--show-statistics` 显示 imatrix 文件的统计信息。

为了加快计算速度，请确保通过 `-ngl | --n-gpu-layers` 参数使用 GPU 卸载。

`llama-imatrix` 的最新版本默认以 GGUF 格式存储数据。对于传统格式，请在保存输出文件时使用 `.gguf` 以外的扩展名。更多信息请参阅 <https://github.com/ggml-org/llama.cpp/pull/9400>。

## 示例

### 生成重要性矩阵（使用默认文件名）

```bash
# 使用默认文件名（imatrix.gguf）生成重要性矩阵，将 99 层卸载到 GPU
./llama-imatrix -m ggml-model-f16.gguf -f calibration-data.txt -ngl 99

# 使用 imatrix 执行 Q4_K_M 量化
./llama-quantize --imatrix imatrix.gguf ggml-model-f16.gguf ./ggml-model-q4_k_m.gguf q4_k_m
```

### 使用传统格式生成和保存 imatrix

```bash
./llama-imatrix -m ggml-model-f16.gguf -f calibration-data.txt --output-format dat -o imatrix-legacy-format.dat -ngl 99
```

### 转换传统格式到新格式

```bash
# 将传统（二进制）imatrix 格式转换为新（GGUF）格式
./llama-imatrix --in-file imatrix-legacy-format.dat -o imatrix-new-format.gguf
```

### 转换新格式到传统格式

```bash
# 将新（GGUF）imatrix 格式转换为传统（二进制）格式
./llama-imatrix --in-file imatrix-new-format.gguf --output-format dat -o imatrix-legacy-format.dat
```

### 合并现有的 imatrix

```bash
./llama-imatrix --in-file imatrix-prev-0.gguf --in-file imatrix-prev-1.gguf -o imatrix-combined.gguf
```

### 使用高级选项

```bash
# 跳过前 5 个块，每 20 个块保存中间结果，每 50 个块保存快照，解析特殊 token
./llama-imatrix -m ggml-model-f16.gguf -f calibration-data.txt --chunk 5 --output-frequency 20 --save-frequency 50 --parse-special
```

### 显示 imatrix 统计信息

```bash
# 分析 imatrix 文件并显示摘要统计信息，而不是运行推理
./llama-imatrix --in-file imatrix.gguf --show-statistics
```

`--show-statistics` 将显示以下统计信息：

#### 每个张量

* Σ(Act²)：所有平方激活的总和（重要性得分）
* 最小值和最大值：平方激活的最小值和最大值
* μ 和 σ：平方激活的均值和标准差
* % Active：其平均平方激活超过小阈值（1e-5）的元素比例。有助于确定张量在推理期间是活跃/休眠的程度
* N：平方激活的数量
* 熵：平方激活分布的熵，以位为单位（标准香农熵测量）$S = -\sum_{i=1}^N p_i \log_2 p_i$
* E (norm)：归一化熵。$E(norm)=\frac{-\sum_{i=1}^N p_i \log_2 p_i}{log_2 N}$。这两个度量可用于确定提示"锻炼"模型能力的程度
* ZD Score：[Layer-Wise Quantization](https://arxiv.org/abs/2406.17415) 的 _3.1 Layer Importance Scores_ 中描述的 z-score 分布
* CosSim：与上一层张量的余弦相似度。有助于确定当前层的平方激活与上一层平方激活的相似程度

#### 每一层

还计算 Σ(Act²)、ZD Score 和 CosSim 的加权平均值。

#### 关于计算统计信息的重要说明

使用这些统计信息时，请注意它们是**在平方激活上计算的**，而不是在实际（原始）激活上计算的。
虽然结果仍然有用，但它们比使用原始值不太可靠，并且在余弦相似度的情况下，如果张量包含相反的向量，可能会产生误导。

## 功能说明

重要性矩阵（Importance Matrix）是一种用于优化模型量化质量的技术。它通过分析模型在特定数据集上的激活值，来确定哪些权重对模型输出更重要，从而在量化过程中对重要的权重保留更高的精度。

### 主要功能

1. **生成重要性矩阵**：基于给定文本数据集计算权重的重要性得分
2. **合并多个矩阵**：可以从多个运行或数据集合并重要性矩阵
3. **格式转换**：支持 GGUF 和传统二进制格式之间的转换
4. **统计分析**：提供详细的统计信息以分析张量重要性

### 使用场景

- **量化优化**：使用重要性矩阵进行量化可以显著减少质量损失
- **质量评估**：通过比较量化前后的指标（如困惑度）来评估量化质量
- **数据分析**：了解模型中哪些张量/层对推理最关键

## 最佳实践

1. **选择合适的校准数据**：使用与您的用例相似的文本数据集
2. **GPU 加速**：使用 `-ngl` 参数启用 GPU 卸载以加快计算速度
3. **分批保存**：对于大型数据集，使用 `--save-frequency` 定期保存中间结果
4. **验证结果**：使用困惑度指标验证量化质量

## 相关工具

- `llama-quantize`：使用重要性矩阵进行模型量化的工具
- `perplexity`：评估量化后模型质量的工具