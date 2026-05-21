# 多GPU文档摘要 (multi-gpu.md)

## 概述

本文档介绍如何在多个 GPU 上运行 llama.cpp，涵盖了分割模式、控制命令行标志、限制条件以及 llama-cli 和 llama-server 的即用配方。

## 何时需要多GPU

当满足以下任一条件时需要使用多GPU：

1. **模型无法放入单个 GPU 的 VRAM**
   - 通过在两个或多个 GPU 上分布权重，整个模型可以保持在加速器上
   - 否则部分模型需要在相对较慢的系统 RAM 上运行

2. **需要更高的吞吐量**
   - 通过在多个 GPU 上分布计算，每个 GPU 需要完成的工作更少
   - 根据分割模式和互连速度与单个 GPU 速度的对比，可以获得更好的预填充和/或 token 生成性能

## 分割模式

通过 `--split-mode` / `-sm` 设置。

| 模式 | 功能 | 使用场景 |
|------|------|----------|
| `none` | 仅使用单个 GPU。通过 `--main-gpu` 选择使用哪一个。 | 即使可以看到更多 GPU，也明确希望将模型限制在一个 GPU 上。 |
| `layer` (**默认**) | 流水线并行。每个 GPU 持有一层连续的切片。第 l 层的 KV cache 驻留在拥有第 l 层的 GPU 上。 | 默认且最兼容的多 GPU 选择。需要比单个 GPU 提供更多的内存，优先考虑快速预填充。可以容忍 GPU 之间较慢的互连速度。 |
| `row` | **已弃用**。较旧的行分割张量并行路径，性能相对较差。仅在 GPU 之间分割密集权重。已被 `tensor` 取代，后者应该普遍更优。 | 避免在新部署中使用。 |
| `tensor` | **实验性**。通过"元设备"抽象在参与 GPU 之间分割权重和 KV 的张量并行。 | 需要比单个 GPU 提供更多的内存，优先考虑快速 token 生成。对于大型密集模型和快速的 GPU 互连速度，预填充速度接近流水线并行速度。由于代码不如流水线并行成熟，请将其视为实验性。使用 CUDA 后端时性能应该良好，其他后端无法保证。 |

**流水线并行 (`layer`) 与张量并行 (`tensor`) 对比：**
- 流水线并行在不同 GPU 上运行不同层，并通过流水线按顺序处理 token。这最小化了 GPU 之间的数据传输，但需要许多 token 才能良好扩展。
- 张量并行在 GPU 之间分割每一层，并在每层进行多次跨 GPU 约简。这允许并行化任何工作负载，但更受 GPU 互连速度的瓶颈限制。
- 流水线并行最大化批量吞吐量；张量并行最小化延迟。

## 命令行参数参考

| 简写 | 完整参数 | 值 | 默认值 | 说明 |
|------|----------|---|--------|------|
| `-sm` | `--split-mode` | `none` \| `layer` \| `tensor` | `layer` | 见上方模式说明 |
| `-ts` | `--tensor-split` | 逗号分隔的比例，如 `3,1` | 模式相关 | 每个 GPU 分配多少模型。如果省略，`layer`/`row` 使用按内存自动分割，而 `tensor` 平均分割张量段。在两个 GPU 上使用 `3,1` 时，GPU 0 获得 75%，GPU 1 获得 25%。值遵循 `--device` 中的顺序。 |
| `-mg` | `--main-gpu` | 整数设备索引 | `0` | `--split-mode none` 中使用的单个 GPU |
| `-ngl` | `--n-gpu-layers` / `--gpu-layers` | 整数 \| `auto` \| `all` | `auto` | 保持在 VRAM 中的最大层数。使用 `999` 或 `all` 将尽可能多的内容推送到 GPU |
| `-dev` | `--device` | 逗号分隔的设备名称，或 `none` | auto | 限制 llama.cpp 可以使用的设备。使用 `--list-devices` 查看名称 |
| | `--list-devices` | - | - | 打印可用设备及其内存。首先运行此项以了解要传递给 `--device` 的名称 |
| `-fa` | `--flash-attn` | `on` \| `off` \| `auto` | `auto` | 使用 `--split-mode tensor` 和/或量化 V cache 时需要。对于大多数模型和后端组合都支持（因此默认启用） |
| `-ctk` | `--cache-type-k` | `f32` \| `f16` \| `bf16` \| `q8_0` \| `q4_0` \| ... | `f16` | K 的 KV cache 类型 |
| `-ctv` | `--cache-type-v` | 同 `-ctk` | `f16` | V 的 KV cache 类型 |
| `-fit` | `--fit` | `on` \| `off` | `on` | 自动调整未设置的参数以适应设备内存。**不支持 `tensor` 模式。您可能需要手动设置 `--ctx-size` 以使模型适应。** |

对于任何 CUDA 程序，环境变量 `CUDA_VISIBLE_DEVICES` 可用于控制 CUDA 后端使用哪些 GPU：如果设置了它，llama.cpp 只能看到指定的 GPU。使用 `--device` 从 llama.cpp 可见的 GPU 中选择 GPU，这适用于任何后端。

## 使用配方

### 1. 默认 - 在所有可见 GPU 上进行流水线并行

```bash
llama-cli -m model.gguf
llama-server -m model.gguf
```

最简单的配置。KV cache 与层一起分布在 GPU 上。`--fit`（默认开启）自动调整大小。

### 2. 使用自定义分割比例的流水线并行

```bash
llama-cli -m model.gguf -ts 3,1
```

当 GPU 有不同内存时很有用：GPU 0（3 部分）和 GPU 1（1 部分）。比例会被归一化，因此 `-ts 3,1` 与例如 `-ts 75,25` 相同。

### 3. 单 GPU 模式，选择特定 GPU

```bash
llama-cli --list-devices
llama-cli -m model.gguf -dev CUDA1
```

使用调用 `--list-devices` 时列为 `CUDA1` 的设备。

### 4. 张量并行（实验性）

```bash
llama-cli -m model.gguf -sm tensor -ctk f16 -ctv f16
```

- `--flash-attn off` 或（`--flash-attn auto` 在不支持时解析为 `off`）是一个硬错误
- KV cache 类型必须是非量化的：`f32`、`f16` 或 `bf16`。不支持量化 KV cache，尝试使用它会导致错误
- 在工具中标记此配置为实验性：部署前验证输出质量
- `--split-mode tensor` 并未为所有架构实现。以下将失败，并提示 *"LLAMA_SPLIT_MODE_TENSOR not implemented for architecture '...'"*：

  - **MoE / 混合模型：** Grok、MPT、OLMoE、DeepSeek2、GLM-DSA、Nemotron-H、Nemotron-H-MoE、Granite-Hybrid、LFM2-MoE、Minimax-M2、Mistral4、Kimi-Linear、Jamba、Falcon-H1
  - **状态空间 / RWKV 风格：** Mamba、Mamba2（以及上面的混合 Mamba-attention 模型）
  - **其他：** PLAMO2、MiniCPM3、Gemma-3n、OLMo2、BitNet、T5

### 5. 使用 NCCL

没有 NCCL 的运行时标志 - 它是在构建时选择的（`-DGGML_CUDA_NCCL=ON`，这是默认值）。请注意，NCCL **不会**随 CUDA 自动分发，您可能需要手动安装它 - 如有疑问，请检查 CMake 日志以查看是否可以找到该包。当 llama.cpp 使用 NCCL 支持编译时，它会在 `tensor` 模式下自动使用它进行跨 GPU 约简。当多 GPU 构建中缺少 NCCL 时，您将看到此一次性警告，性能会较低：

```
NVIDIA Collective Communications Library (NCCL) is unavailable, multi GPU performance will be suboptimal
```

使用"ROCm"后端（这是通过 HIP 为 AMD 翻译的 ggml CUDA 代码）时，可以通过使用 `-DGGML_HIP_RCCL=ON` 编译来使用 AMD 等效的 RCCL。请注意，RCCL 默认*禁用*，因为（与 NCCL 不同）它在测试期间并不普遍有益。

### 6. 使用 CUDA 点对点访问 (`GGML_CUDA_P2P`)

CUDA 点对点 (P2P) 允许 GPU 直接在彼此之间传输数据，而不是通过系统内存，这通常会提高多 GPU 性能。它在运行时是**可选的** - 将环境变量 `GGML_CUDA_P2P` 设置为任何值以启用它：

```bash
GGML_CUDA_P2P=1 llama-cli -m model.gguf -sm tensor
```

P2P 需要驱动程序支持（通常限于工作站/数据中心 GPU），并且**在某些主板或 BIOS 配置上可能导致崩溃或输出损坏**（例如，当启用 IOMMU 时）。如果在启用后看到不稳定，请取消设置该变量。

## 故障排除

| 症状 | 解决方法 |
|------|----------|
| 启动错误 *"SPLIT_MODE_TENSOR requires flash_attn to be enabled"* | 添加 `-fa on` 或移除 `-fa off` |
| 启动错误 *"simultaneous use of SPLIT_MODE_TENSOR and KV cache quantization not implemented"* | 在 `--split-mode tensor` 下使用 `-ctk f16 -ctv f16`（或 `bf16`/`f32`） |
| 启动错误 *"LLAMA_SPLIT_MODE_TENSOR not implemented for architecture 'X'"* | 架构不在 TENSOR 允许列表中。使用 `--split-mode layer` |
| 警告 *"NCCL is unavailable, multi GPU performance will be suboptimal"* | llama.cpp 未使用 NCCL 构建。要么接受较低的性能，要么安装 NCCL 并重新构建 |
| 在 `--split-mode tensor` 下启动时或预填充期间 CUDA OOM | 此模式下禁用了自动适配，因此自己降低内存压力。从干扰最小到最大：降低 `--ctx-size` (`-c`)（KV cache 大致与 `n_ctx` 成正比）；对于 `llama-server`，降低 `--parallel` (`-np`)（每个并发序列分配一个插槽 KV cache）；作为最后手段，减少 `--n-gpu-layers` (`-ngl`)（其余层在 CPU 上运行，推理会慢得多） |
| 多 GPU 性能比单 GPU 差 | 性能受 GPU 互连速度瓶颈限制。对于 `--split-mode tensor`，验证是否在使用 NCCL。尝试 `--split-mode layer`（比 `tensor` 通信更少）。通过更多 PCIe 通道或例如 NVLink（如果可用）提高 GPU 互连速度 |
| GPU 完全未使用 | `--n-gpu-layers` 为 `0` 或太低 - 尝试显式设置 `-ngl all`。或者您通过环境变量如 `CUDA_VISIBLE_DEVICES=-1` 意外隐藏了 GPU。或者您的构建不包括对相关后端的支持 |
| 设置 `GGML_CUDA_P2P=1` 后崩溃或输出损坏 | 某些主板和 BIOS 设置（例如启用了 IOMMU）无法可靠支持 CUDA 点对点。取消设置 `GGML_CUDA_P2P` |

## 多GPU调度策略

1. **流水线并行 (layer 模式)**：
   - 每个 GPU 处理不同的层
   - 通过流水线顺序处理 token
   - 最小化 GPU 间数据传输
   - 最大化批量吞吐量

2. **张量并行 (tensor 模式)**：
   - 在 GPU 间分割每一层
   - 每层进行多次跨 GPU 约简
   - 可以并行化任何工作负载
   - 最小化延迟，但受 GPU 互连速度限制

## 层并行和流水线并行的实现

**层并行**：
- 每个 GPU 持有一层连续的切片
- KV cache 驻留在拥有对应层的 GPU 上
- 适合需要更多内存的场景
- 优先考虑快速预填充

**流水线并行**：
- 在不同 GPU 上运行不同层
- Token 按顺序通过流水线处理
- 需要足够的 token 才能良好扩展
- 可以容忍 GPU 间较慢的互连速度