# 推测解码文档摘要 (speculative.md)

## 概述

llama.cpp 支持推测解码，这是一种可以通过预测主模型之前的多个 token 来显著加速 token 生成技术。

推测解码利用了以下事实：批量计算 n 个 token（如提示处理）比顺序计算 n 个（如响应生成）更高效。通过快速生成草稿 token，然后使用目标模型在单个批次中验证它们，当草稿预测经常正确时，这种方法可以实现显著的加速。

## 实现方式

`llama-server` 应用程序支持几种推测解码的实现。使用草稿模型的实现可以与不使用草稿模型的实现混合使用。

### 1. 草稿模型 (Draft Model, `draft`)

- 一个小得多的模型（称为_草稿模型_）生成草稿
- 草稿模型是推测解码中最常用的方法

### 2. n-gram 缓存 (ngram-cache)

- n-gram 是 n 个 token 的序列
- n-gram 缓存实现维护关于短 n-gram 序列的统计信息
- 使用从这些统计信息得出的概率计算草稿
- 也可以从文件加载外部统计信息以提高准确性

参考：#5479, #6828, #6848

### 3. n-gram 映射 (ngram-simple, ngram-map-*)

这些实现在 token 历史中搜索模式，并使用匹配序列作为草稿候选。它们不需要额外的模型，但依赖于已出现在生成文本中的模式。一个使用这种方法的例子是由 LLM 重写源代码。

#### n-gram 映射 (ngram-simple)

- 在历史记录中查找与当前 n-gram 匹配的最后一个 n-gram
- 使用匹配 n-gram 之后的 m 个 token 创建草稿
- 这是最简单的自推测方法，开销最小

```
llama-server [...] --spec-type ngram-simple --spec-draft-n-max 64
```

#### n-gram 映射密钥 (ngram-map-k)

- 在 token 历史中查找大小为 n 的当前 n-gram（称为_密钥_）
- 如果密钥 n-gram 后面多次跟随相同的 m 个 token（称为_mgram_），则使用这些 m 个 token 创建草稿
- 此方法需要最少出现次数（参数 `--spec-ngram-map-k-min-hits`，默认为 1）才能生成草稿
- 为每个使用的 n-gram 存储接受的 token 数量

**示例：**
```
llama-server [...] --spec-type ngram-map-k --spec-draft-n-max 64
```

#### n-gram 映射密钥-4-值 (ngram-map-k4v)

- 实验性实现
- 在 token 历史中查找大小为 n 的当前 n-gram（称为_密钥_）
- 对于每个密钥，跟踪最多四个_值_（大小为 m 的 n-gram，称为_mgrams_）
- 内部统计计算密钥 n-gram 之后每个 mgram 的出现次数
- 如果一个 mgram 明显比其他 mgram 更频繁，则将其用作草稿
- 为每个使用的 n-gram 存储接受的 token 数量

**示例：** 当有很多更长的重复时使用的服务器选项。
```
llama-server [...] --spec-type ngram-map-k4v --spec-ngram-map-k4v-size-n 8 --spec-ngram-map-k4v-size-m 8 --spec-ngram-map-k4v-min-hits 2 --spec-draft-n-max 64
```

### 4. n-gram 模数 (ngram-mod)

为推测解码添加基本 ngram 哈希器：

- 对于每个 ngram，使用 LCG 计算哈希
- 对于每个计算的哈希，存储下一个 token
- 在推测期间，迭代计算最后 n 个 token 的滚动哈希，并从存储中选择下一个 token

一些特性：

- 轻量级（约 16 MB）
- 恒定的内存和复杂性
- 可以生成可变的草稿长度（即 m 不固定）

目前，单个哈希池在所有服务器插槽之间共享，因此不同的请求可以相互受益。

**示例用法：**

```
# 说明：
# - 小 `n` 不推荐
# - MoEs 需要长草稿
# - 密集模型：可以减少 `--spec-ngram-mod-n-min` 和 `--spec-ngram-mod-n-max`

llama-server ... --spec-type ngram-mod --spec-ngram-mod-n-match 24 --spec-ngram-mod-n-min 48 --spec-ngram-mod-n-max 64
```

应用场景：

- 迭代文本/代码块（例如在 llama.vim 中）
- 推理模型（当它们必须在最终答案中重复其思考时）
- 摘要

示例视频：#19164

### ngram-simple、ngram-map 和 ngram-mod 之间的区别

- `ngram-simple` 查找先前的匹配 n-gram 并插入以下 m-gram
- `ngram-map-k` 查找先前的匹配 n-gram 并插入以下 m-gram，但使用当前上下文窗口中 n-gram 的内部哈希映射
- `ngram-mod` 使用在所有服务器插槽之间共享的哈希池。哈希池是从 n-gram 哈希到下一个 token 的映射（而不是像 ngram-map 中的下一个 m-gram）

## 命令行选项

如果草稿模型与无草稿解码结合使用，则无草稿解码具有更高优先级。

### 通用推测参数

```
--spec-type [none|draft-simple|draft-mtp|ngram-cache|ngram-simple|ngram-map-k|ngram-map-k4v|ngram-mod]
                                        逗号分隔的推测解码类型列表
                                        （默认：none）
                                        （env: LLAMA_ARG_SPEC_TYPE）
--spec-default                          使用默认推测解码配置
                                        （启用 ngram-mod）
```

### 草稿模型参数

```
--spec-draft-model, -md, --model-draft  FNAME
                                        推测解码的草稿模型（默认：未使用）
                                        （env: LLAMA_ARG_SPEC_DRAFT_MODEL）
--spec-draft-hf, -hfd, -hfrd, --hf-repo-draft  <user>/<model>[:quant]
                                        草稿模型的 HuggingFace 仓库
                                        （env: LLAMA_ARG_SPEC_DRAFT_HF_REPO）
--spec-draft-n-max                      N
                                        推测解码要草稿的 token 数量（默认：3）
                                        （env: LLAMA_ARG_SPEC_DRAFT_N_MAX）
--spec-draft-n-min                      N
                                        推测解码要使用的最小草稿 token 数量（默认：0）
                                        （env: LLAMA_ARG_SPEC_DRAFT_N_MIN）
--spec-draft-p-split, --draft-p-split   P
                                        推测解码分割概率（默认：0.10）
                                        （env: LLAMA_ARG_SPEC_DRAFT_P_SPLIT）
--spec-draft-p-min, --draft-p-min       P
                                        最小推测解码概率（贪婪）（默认：0.00）
                                        （env: LLAMA_ARG_SPEC_DRAFT_P_MIN）
--spec-draft-ngl, -ngld, --gpu-layers-draft, --n-gpu-layers-draft  N
                                        在 VRAM 中存储的草稿模型层的最大数量，可以是精确数字、'auto' 或 'all'（默认：auto）
                                        （env: LLAMA_ARG_N_GPU_LAYERS_DRAFT）
--spec-draft-device, -devd, --device-draft  <dev1,dev2,..>
                                        用于卸载草稿模型的设备的逗号分隔列表
                                        （使用 --list-devices 查看可用设备）
```

### 草稿模型 CPU 调度参数

```
--spec-draft-threads, -td, --threads-draft  N
                                        生成期间使用的 CPU 线程数
--spec-draft-threads-batch, -tbd, --threads-batch-draft  N
                                        批次和提示处理期间使用的线程数（默认：与 --threads-draft 相同）
--spec-draft-cpu-mask, -Cd, --cpu-mask-draft  M
                                        草稿模型 CPU 亲和掩码。补充 cpu-range-draft
--spec-draft-cpu-range, -Crd, --cpu-range-draft  lo-hi
                                        亲和力的 CPU 范围。补充 --cpu-mask-draft
--spec-draft-cpu-strict, --cpu-strict-draft  <0|1>
                                        为草稿模型使用严格的 CPU 放置（默认：与 --cpu-strict 相同）
--spec-draft-prio, --prio-draft  N
                                        设置草稿进程/线程优先级：0-正常，1-中等，2-高，3-实时
--spec-draft-poll, --poll-draft  <0|1>
                                        使用轮询等待草稿模型工作（默认：与 --poll 相同）
--spec-draft-cpu-mask-batch, -Cbd, --cpu-mask-batch-draft  M
                                        批次的草稿模型 CPU 亲和掩码。补充 cpu-range-batch-draft
--spec-draft-cpu-range-batch, -Crbd, --cpu-range-batch-draft  lo-hi
                                        批次亲和力的 CPU 范围。补充 --cpu-mask-batch-draft
--spec-draft-cpu-strict-batch, --cpu-strict-batch-draft  <0|1>
                                        为草稿模型批次使用严格的 CPU 放置（默认：--cpu-strict-draft）
--spec-draft-prio-batch, --prio-batch-draft  N
                                        设置批次的草稿进程/线程优先级：0-正常，1-中等，2-高，3-实时
--spec-draft-poll-batch, --poll-batch-draft  <0|1>
                                        使用轮询等待草稿模型批次工作（默认：--poll-draft）
```

### 草稿模型 KV Cache 和张量覆盖参数

```
--spec-draft-type-k, -ctkd, --cache-type-k-draft  TYPE
                                        草稿模型的 K 的 KV cache 数据类型
                                        允许值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1
                                        （env: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_K）
--spec-draft-type-v, -ctvd, --cache-type-v-draft  TYPE
                                        草稿模型的 V 的 KV cache 数据类型
                                        允许值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1
                                        （env: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_V）
--spec-draft-override-tensor, -otd, --override-tensor-draft  <tensor name pattern>=<buffer type>,...
                                        覆盖草稿模型的张量缓冲区类型
--spec-draft-cpu-moe, -cmoed, --cpu-moe-draft
                                        将草稿模型的所有混合专家 (MoE) 权重保持在 CPU 中
                                        （env: LLAMA_ARG_SPEC_DRAFT_CPU_MOE）
--spec-draft-n-cpu-moe, --spec-draft-ncmoe, -ncmoed, --n-cpu-moe-draft  N
                                        将草稿模型的前 N 层的 MoE 权重保持在 CPU 中
                                        （env: LLAMA_ARG_SPEC_DRAFT_N_CPU_MOE）
```

### n-gram Mod 参数

```
--spec-ngram-mod-n-match                N
                                        ngram-mod 查找长度（默认：24）
--spec-ngram-mod-n-min                  N
                                        基于 ngram 的推测解码要使用的最小 ngram token 数量（默认：48）
--spec-ngram-mod-n-max                  N
                                        基于 ngram 的推测解码要使用的最大 ngram token 数量（默认：64）
```

### n-gram Simple 参数

```
--spec-ngram-simple-size-n              N
                                        ngram-simple 推测解码的 ngram 大小 N，查找 n-gram 的长度（默认：12）
--spec-ngram-simple-size-m              N
                                        ngram-simple 推测解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48）
--spec-ngram-simple-min-hits            N
                                        ngram-simple 推测解码的最小命中数（默认：1）
```

### n-gram Map Key 参数

```
--spec-ngram-map-k-size-n               N
                                        ngram-map-k 推测解码的 ngram 大小 N，查找 n-gram 的长度（默认：12）
--spec-ngram-map-k-size-m               N
                                        ngram-map-k 推测解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48）
--spec-ngram-map-k-min-hits             N
                                        ngram-map-k 推测解码的最小命中数（默认：1）
```

### n-gram Map Key-4-Values 参数

```
--spec-ngram-map-k4v-size-n             N
                                        ngram-map-k4v 推测解码的 ngram 大小 N，查找 n-gram 的长度（默认：12）
--spec-ngram-map-k4v-size-m             N
                                        ngram-map-k4v 推测解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48）
--spec-ngram-map-k4v-min-hits           N
                                        ngram-map-k4v 推测解码的最小命中数（默认：1）
```

### `--spec-type TYPE`

指定要使用的逗号分隔的推测解码类型列表。

| 类型 | 描述 |
|------|------|
| `none` | 无推测解码（默认） |
| `draft-simple` | 使用简单草稿模型进行推测 |
| `draft-mtp` | 使用主模型的多 token 预测 (MTP) 头 |
| `ngram-cache` | 使用 n-gram 缓存查找 |
| `ngram-simple` | 使用简单 n-gram 模式匹配 |
| `ngram-map-k` | 使用带 n-gram 密钥的 n-gram 模式匹配 |
| `ngram-map-k4v` | 使用带 n-gram 密钥和最多四个 m-gram 值的 n-gram 模式匹配（实验性） |
| `ngram-mod` | 使用带共享池的基本 ngram 哈希器进行推测解码 |

**示例：** 用于重构源代码的服务器实例。
```bash
./llama-server [...] --spec-type ngram-simple
```

**示例：** 多个推测实现。
```bash
./llama-server [...] --spec-type ngram-mod,ngram-map-k4v
```

### `--spec-ngram-*-size-n N`

设置基于 n-gram 映射的推测解码的查找 n-gram 的大小 N。
n-gram 大小 N 确定在搜索匹配模式时要向后查看多少个连续 token。

每个 n-gram 实现都有自己的参数：

- `--spec-ngram-simple-size-n` 用于 `ngram-simple`
- `--spec-ngram-map-k-size-n` 用于 `ngram-map-k`
- `--spec-ngram-map-k4v-size-n` 用于 `ngram-map-k4v`
- `--spec-ngram-mod-n-match` 用于 `ngram-mod`

### `--spec-ngram-*-size-m M`

设置基于 n-gram 映射的推测解码的草稿 m-gram 的大小 M。
m-gram 大小确定找到匹配时要草稿多少个 token。
较大的值可以提供更多加速，但可能会降低接受率。

每个 n-gram 实现都有自己的参数：

- `--spec-ngram-simple-size-m` 用于 `ngram-simple`
- `--spec-ngram-map-k-size-m` 用于 `ngram-map-k`
- `--spec-ngram-map-k4v-size-m` 用于 `ngram-map-k4v`

### `--spec-ngram-*-min-hits H`

此选项定义密钥必须在 token 历史中出现多少次才能用作草稿（默认为 1）。

每个 n-gram 实现都有自己的参数：

- `--spec-ngram-simple-min-hits` 用于 `ngram-simple`
- `--spec-ngram-map-k-min-hits` 用于 `ngram-map-k`
- `--spec-ngram-map-k4v-min-hits` 用于 `ngram-map-k4v`

## 统计信息

每个推测解码实现都打印统计信息。

```
draft acceptance rate = 0.57576 (  171 accepted /   297 generated)
statistics ngram_simple: #calls = 15, #gen drafts = 5, #acc drafts = 5, #gen tokens = 187, #acc tokens = 73
statistics draft: #calls = 10, #gen drafts = 10, #acc drafts = 10, #gen tokens = 110, #acc tokens = 98
```

```
draft acceptance rate = 0.70312 (   90 accepted /   128 generated)
statistics ngram_mod: #calls = 810, #gen drafts = 15, #acc drafts = 15, #gen tokens = 960, #acc tokens = 730, dur(b,g,a) = 0.149, 0.347, 0.005 ms
```

```
statistics ngram_map_k: #calls(b,g,a) = 6 1690 26, #gen drafts = 26, #acc drafts = 26, #gen tokens = 1248, #acc tokens = 968, dur(b,g,a) = 2.234, 1.427, 0.016 ms
```

- `#calls(b,g,a)`：此实现的开始（新提示）、生成和累积调用次数
- `#gen drafts`：此实现生成的草稿数量
- `#acc drafts`：主模型（部分）接受的草稿数量
- `#gen tokens`：此实现生成的 token 数量（包括被拒绝的 token）
- `#acc tokens`：主模型接受的 token 数量
- `dur(b,g,a)`：开始（新提示）、生成和累积（处理接受）的持续时间

## 推测解码的原理

推测解码利用了批量处理比顺序处理更高效的特性：

1. **草稿生成**：使用小模型或 n-gram 技术快速生成候选 token
2. **验证**：使用主模型在单批次中验证这些候选 token
3. **加速**：当预测正确时，可以显著减少主模型的推理次数

## 推测解码的实现方法

1. **草稿模型方法**：使用小模型生成草稿
2. **n-gram 方法**：
   - n-gram 缓存：维护统计信息
   - n-gram 映射：搜索模式匹配
   - n-gram 模数：使用哈希池
3. **混合方法**：可以组合多种实现

## 推测解码的配置选项

### 主要配置参数

- `--spec-type`：选择推测解码类型
- `--spec-draft-n-max`：最大草稿 token 数量
- `--spec-draft-n-min`：最小草稿 token 数量
- `--spec-draft-p-split`：分割概率
- `--spec-draft-p-min`：最小概率

### n-gram 参数

- 查找长度 (size-n)
- 草稿长度 (size-m)
- 最小命中数 (min-hits)

### 性能调优

- 根据模型类型选择适当的实现
- 调整 n-gram 参数以优化接受率
- 考虑 GPU/CPU 资源分配