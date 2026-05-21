# quantize

此工具接受 GGUF 输入模型文件，通常是 F32 或 BF16 等高精度格式，并将其转换为量化格式。
量化减少了模型权重的精度（例如，从 32 位浮点数到 4 位整数），从而缩小模型的大小并可以加快推理速度。
然而，此过程可能会引入一些准确性损失，通常以 [困惑度](https://huggingface.co/docs/transformers/en/perplexity) (ppl) 和/或 [Kullback–Leibler 散度](https://en.wikipedia.org/wiki/Kullback%E2%80%93Leibler_divergence) (kld) 来衡量。
这可以通过使用合适的 imatrix 文件来最小化。

您还可以使用 Hugging Face 上的 [GGUF-my-repo](https://huggingface.co/spaces/ggml-org/gguf-my-repo) 空间来构建自己的量化模型，无需任何设置。

注意：它每 6 小时从 llama.cpp `main` 同步一次。

示例用法：

```./llama-quantize [options] input-model-f32.gguf [output-model-quant.gguf] type [threads]```

```bash
# 从 Hugginface 获取官方 meta-llama/Llama-3.1-8B 模型权重并将其放置在 ./models 中
ls ./models
config.json             model-00001-of-00004.safetensors  model-00004-of-00004.safetensors  README.md                tokenizer.json
generation_config.json  model-00002-of-00004.safetensors  model.safetensors.index.json      special_tokens_map.json  USE_POLICY.md
LICENSE                 model-00003-of-00004.safetensors  original                          tokenizer_config.json

# [可选] 对于 PyTorch .bin 模型（如 Mistral-7B）
ls ./models
<folder containing weights and tokenizer json>

# 安装 Python 依赖项
python3 -m pip install -r requirements.txt

# 将模型转换为 ggml FP16 格式
python3 convert_hf_to_gguf.py ./models/mymodel/

# 将模型量化为 4 位（使用 Q4_K_M 方法）
./llama-quantize ./models/mymodel/ggml-model-f16.gguf ./models/mymodel/ggml-model-Q4_K_M.gguf Q4_K_M

# 如果旧版本现在不受支持，则更新 gguf 文件类型到当前版本
./llama-quantize ./models/mymodel/ggml-model-Q4_K_M.gguf ./models/mymodel/ggml-model-Q4_K_M-v2.gguf COPY
```

运行量化模型：

```bash
# 在 gguf 模型上开始推理
./llama-cli -m ./models/mymodel/ggml-model-Q4_K_M.gguf -cnv -p "You are a helpful assistant"
```

选项：
* `--allow-requantize` 允许重新量化已经量化的张量。警告：与从 16 位或 32 位量化相比，这会严重降低质量
* `--leave-output-tensor` 将 output.weight 保持未（重新）量化。增加模型大小，但也可能增加质量，特别是在重新量化时
* `--pure` 禁用 k-quant 混合并将所有张量量化为相同类型
* `--imatrix` 使用 `llama-imatrix` 生成的文件中的数据作为量化优化的重要性矩阵（强烈推荐）
* `--include-weights` 对列表中的张量使用重要性矩阵。不能与 `--exclude-weights` 一起使用
* `--exclude-weights` 对列表中的张量使用重要性矩阵。不能与 `--include-weights` 一起使用
* `--output-tensor-type` 对 output.weight 张量使用特定的量化类型
* `--token-embedding-type` 对 token 嵌入张量使用特定的量化类型
* `--keep-split` 将在与输入文件相同的分片中生成量化模型，否则它将生成单个量化文件

高级选项：
* `--tensor-type` 将特定张量量化为特定量化类型。支持正则表达式语法。可以指定多次。
* `--prune-layers` 修剪（删除）列表中的层
* `--override-kv` 选项，通过键在量化模型中覆盖模型元数据。可以指定多次

示例：

```bash
# 使用默认设置和 8 个 CPU 线程的简单 Q4_K_M 量化。输出将为 "ggml-model-Q4_K_M.gguf"
./llama-quantize input-model-f32.gguf q4_k_m 8
```

```bash
# 量化模型，启用重新量化，保持输出张量未量化，所有其他张量在同一级别（Q4_K）量化
./llama-quantize --allow-requantize --leave-output-tensor --pure input-model-f32.gguf q4_k_m 8
```

```bash
# 使用重要性矩阵仅对指定张量（attn_v 和 ffn_down）进行模型量化
./llama-quantize --imatrix imatrix.gguf --include-weights attn_v --include-weights ffn_down input-model-f32.gguf q4_k_m 8
```

```bash
# 量化模型，将输出张量设置为 Q5_K_M，token 嵌入设置为 Q3_K_M，并保持输入文件的分片
./llama-quantize --imatrix imatrix.gguf --output-tensor-type q5_k --token-embedding-type q3_k --keep-split input-model-f32.gguf q4_k_m 8
```

```bash
# 使用正则表达式将奇数层的 attn_k 张量量化为 Q5_K_M，将偶数层的 attn_q 张量量化为 Q3_K_M
./llama-quantize --imatrix imatrix.gguf --tensor-type "\.(\d*[13579])\.attn_k=q5_k" --tensor-type "\.(\d*[02468])\.attn_q=q3_k" input-model-f32.gguf q4_k_m 8
```

```bash
# 量化模型，将张量 attn_v 和 ffn_down 设置为 Q5_K_M，并修剪第 20、21 和 22 层
./llama-quantize --imatrix imatrix.gguf --tensor-type attn_v=q5_k --tensor-type ffn_down=q5_k --prune-layers 20,21,22 input-model-f32.gguf q4_k_m 8
```

```bash
# 覆盖专家使用计数元数据为 16，修剪第 20、21 和 22 层而不量化模型（复制张量）并使用指定名称作为输出文件
./llama-quantize --imatrix imatrix.gguf --override-kv qwen3moe.expert_used_count=int:16 --prune-layers 20,21,22 input-model-f32.gguf pruned-model-f32.gguf copy 8
```

## 内存/磁盘要求

运行更大的模型时，请确保您有足够的磁盘空间来存储所有中间文件。
由于模型目前完全加载到内存中，您将需要足够的磁盘空间来保存它们，以及足够的 RAM 来加载它们。目前，内存和磁盘要求是相同的。例如（Llama 3.1）：

| 模型 | 原始大小 | 量化大小 (Q4_K_M) |
| ----: | ------------: | ----------------------: |
|    8B |       32.1 GB |                  4.9 GB |
|   70B |      280.9 GB |                 43.1 GB |
|  405B |    1,625.1 GB |                249.1 GB |


## 量化

支持多种量化方法。它们在结果模型的磁盘大小和推理速度方面有所不同。例如，

### [meta-llama/Llama-3.1-8B](https://huggingface.co/meta-llama/Llama-3.1-8B)

| 指标                     | IQ1_S        | IQ1_M        | IQ2_XXS      | IQ2_XS        | IQ2_S         | IQ2_M        |
| --------------------------- | ------------ | ------------ | ------------ | ------------- | ------------- | ------------ |
| bits/weight                 |       2.0042 |       2.1460 |       2.3824 |        2.5882 |        2.7403 |       2.9294 |
| size (GiB)                  |       1.87   |       2.01   |       2.23   |        2.42   |        2.56   |       2.74   |
| prompt processing t/s @ 512 | 858.88 ±1.22 | 847.99 ±0.47 | 852.39 ±0.85 | 826.99 ±12.51 | 783.55 ±13.73 | 787.68 ±7.00 |
| text generation t/s @ 128   |  79.73 ±0.79 |  72.92 ±0.14 |  79.86 ±0.22 |  78.04 ±0.46  |  77.30 ±2.47  |  74.44 ±0.15 |

| 指标                     | IQ3_XXS      | IQ3_XS       | IQ3_S        | IQ3_M         | IQ4_XS        | IQ4_NL       |
| --------------------------- | ------------ | ------------ | ------------ | ------------- | ------------- | ------------ |
| bits/weight                 |       3.2548 |       3.4977 |       3.6606 |        3.7628 |        4.4597 |       4.6818 |
| size (GiB)                  |       3.04   |       3.27   |       3.42   |        3.52   |        4.17   |       4.38   |
| prompt processing t/s @ 512 | 813.88 ±6.53 | 708.71 ±1.26 | 798.78 ±8.81 | 768.70 ±13.73 | 771.80 ±11.38 | 806.03 ±7.07 |
| text generation t/s @ 128   |  73.95 ±0.20 |  71.67 ±0.54 |  69.31 ±0.63 |  70.15 ±0.33  |  77.51 ±0.20  |  76.63 ±0.28 |


| 指标                     | Q2_K_S       | Q2_K         | Q3_K_S       | Q3_K_M       | Q3_K_L       | Q4_K_S       |
| --------------------------- | ------------ | ------------ | ------------ | ------------ | ------------ | ------------ |
| bits/weight                 |       2.9697 |       3.1593 |       3.6429 |       3.9960 |       4.2979 |       4.6672 |
| size (GiB)                  |       2.78   |       2.95   |       3.41   |       3.74   |       4.02   |       4.36   |
| prompt processing t/s @ 512 | 798.91 ±6.40 | 784.45 ±7.85 | 752.17 ±7.94 | 783.44 ±9.92 | 761.17 ±7.55 | 818.55 ±9.58 |
| text generation t/s @ 128   |  90.01 ±0.12 |  79.85 ±0.20 |  69.84 ±0.18 |  71.68 ±0.22 |  69.38 ±0.49 |  76.71 ±0.20 |

| 指标                     | Q4_K_S       | Q4_K_M        | Q5_K_S       | Q5_K_M       | Q6_K          | Q8_0         |
| --------------------------- | ------------ | ------------- | ------------ | ------------ | ------------- | ------------ |
| bits/weight                 |       4.6672 |        4.8944 |       5.5704 |       5.7036 |        6.5633 |       8.5008 |
| size (GiB)                  |       4.36   |        4.58   |       5.21   |       5.33   |        6.14   |       7.95   |
| prompt processing t/s @ 512 | 818.55 ±9.58 | 821.81 ±21.44 | 752.52 ±0.99 | 758.69 ±7.43 | 812.01 ±10.82 | 865.09 ±8.30 |
| text generation t/s @ 128   |  76.71 ±0.20 |  71.93 ±1.52  |  69.53 ±0.18 |  67.23 ±1.08 |  58.67 ±3.13  |  50.93 ±0.08 |

| 指标                     | F16          |
| --------------------------- | ------------ |
| bits/weight                 |      16.0005 |
| size (GiB)                  |      14.96   |
| prompt processing t/s @ 512 | 923.49 ±0.53 |
| text generation t/s @ 128   |  29.17 ±0.04 |

## llama-quantize 背景信息

- [k-quants](https://github.com/ggml-org/llama.cpp/pull/1684)
- k-quants 改进和 i-quants
  - [#2707](https://github.com/ggml-org/llama.cpp/pull/2707)
  - [#2807](https://github.com/ggml-org/llama.cpp/pull/2807)
  - [#4773 - 2-bit i-quants (inference)](https://github.com/ggml-org/llama.cpp/pull/4773)
  - [#4856 - 2-bit i-quants (inference)](https://github.com/ggml-org/llama.cpp/pull/4856)
  - [#4861 - importance matrix](https://github.com/ggml-org/llama.cpp/pull/4861)
  - [#4872 - MoE models](https://github.com/ggml-org/llama.cpp/pull/4872)
  - [#4897 - 2-bit quantization](https://github.com/ggml-org/llama.cpp/pull/4897)
  - [#4930 - imatrix for all k-quants](https://github.com/ggml-org/llama.cpp/pull/4930)
  - [#4951 - imatrix on the GPU](https://github.com/ggml-org/llama.cpp/pull/4957)
  - [#4969 - imatrix for legacy quants](https://github.com/ggml-org/llama.cpp/pull/4969)
  - [#4996 - k-quants tuning](https://github.com/ggml-org/llama.cpp/pull/4996)
  - [#5060 - Q3_K_XS](https://github.com/ggml-org/llama.cpp/pull/5060)
  - [#5196 - 3-bit i-quants](https://github.com/ggml-org/llama.cpp/pull/5196)
  - [quantization tuning](https://github.com/ggml-org/llama.cpp/pull/5320), [another one](https://github.com/ggml-org/llama.cpp/pull/5334), and [another one](https://github.com/ggml-org/llama.cpp/pull/5361)