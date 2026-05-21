# llama.cpp/tools/completion

该示例程序允许您轻松高效地使用各种 LLaMA 语言模型。它专门设计用于与 [llama.cpp](https://github.com/ggml-org/llama.cpp) 项目配合使用，该项目提供纯 C/C++ 实现并支持可选的 4 位量化，以实现更快、更低内存的推理，并针对桌面 CPU 进行了优化。该程序可用于使用 LLaMA 模型执行各种推理任务，包括基于用户提供的提示词生成文本以及使用反向提示词进行类似聊天的交互。

## 目录

1. [快速开始](#快速开始)
2. [使用方法](#使用方法)
3. [常用选项](#常用选项)
4. [输入提示词](#输入提示词)
5. [交互](#交互)
6. [上下文管理](#上下文管理)
7. [生成标志](#生成标志)
8. [性能调优和内存选项](#性能调优和内存选项)
9. [其他选项](#其他选项)

## 快速开始

要立即开始，请运行以下命令，确保使用您所拥有的正确模型路径：

首先，我们需要下载一个模型。在这些示例中，我们将使用 Hugging Face 上 ggml-org 仓库中的 Gemma 模型。
[https://huggingface.co/ggml-org/gemma-1.1-7b-it-Q4_K_M-GGUF/resolve/main/gemma-1.1-7b-it.Q4_K_M.gguf?download=true](https://huggingface.co/ggml-org/gemma-1.1-7b-it-Q4_K_M-GGUF/resolve/main/gemma-1.1-7b-it.Q4_K_M.gguf?download=true)

下载后，将您的模型放在 llama.cpp 的 models 文件夹中。

### 基于 Unix 的系统（Linux、macOS 等）：

##### 输入提示词（一次性）

```bash
./llama-completion -m models/gemma-1.1-7b-it.Q4_K_M.gguf -no-cnv --prompt "Once upon a time"
```

##### 对话模式（允许与模型持续交互）

```bash
./llama-completion -m models/gemma-1.1-7b-it.Q4_K_M.gguf --chat-template gemma
```

##### 使用内置 jinja 聊天模板的对话模式

```bash
./llama-completion -m models/gemma-1.1-7b-it.Q4_K_M.gguf --jinja
```

##### 使用 jinja 的单轮查询，自定义系统提示词和初始提示词

```bash
./llama-completion -m models/gemma-1.1-7b-it.Q4_K_M.gguf --jinja --single-turn -sys "You are a helpful assistant" -p "Hello"
```

##### 从初始提示词生成无限文本（您可以使用 `Ctrl-C` 停止）：
```bash
./llama-completion -m models/gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1
```

### Windows：

##### 输入提示词（一次性）
```powershell
./llama-completion.exe -m models\gemma-1.1-7b-it.Q4_K_M.gguf -no-cnv --prompt "Once upon a time"
```

##### 对话模式（允许与模型持续交互）

```powershell
./llama-completion.exe -m models\gemma-1.1-7b-it.Q4_K_M.gguf --chat-template gemma
```

##### 使用内置 jinja 聊天模板的对话模式

```powershell
./llama-completion.exe -m models\gemma-1.1-7b-it.Q4_K_M.gguf --jinja
```

##### 使用 jinja 的单轮查询，自定义系统提示词和初始提示词

```powershell
./llama-completion.exe -m models\gemma-1.1-7b-it.Q4_K_M.gguf --jinja --single-turn -sys "You are a helpful assistant" -p "Hello"
```

#### 从初始提示词生成无限文本（您可以使用 `Ctrl-C` 停止）：

```powershell
llama-completion.exe -m models\gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1
```

## 使用方法

### 常用参数

| 参数 | 说明 |
| -------- | ----------- |
| `-h, --help, --usage` | 打印使用说明并退出 |
| `--version` | 显示版本和构建信息 |
| `--license` | 显示源代码许可证和依赖项 |
| `-cl, --cache-list` | 显示缓存中的模型列表 |
| `--completion-bash` | 打印可被 source 的 llama.cpp bash 自动补全脚本 |
| `-t, --threads N` | 生成期间使用的 CPU 线程数（默认：-1）<br/>(环境变量: LLAMA_ARG_THREADS) |
| `-tb, --threads-batch N` | 批处理和提示词处理期间使用的线程数（默认：与 --threads 相同） |
| `-C, --cpu-mask M` | CPU 亲和性掩码：任意长度的十六进制。补充 cpu-range（默认：""） |
| `-Cr, --cpu-range lo-hi` | 亲和性的 CPU 范围。补充 --cpu-mask |
| `--cpu-strict <0\|1>` | 使用严格的 CPU 放置（默认：0） |
| `--prio N` | 设置进程/线程优先级：low(-1), normal(0), medium(1), high(2), realtime(3)（默认：0） |
| `--poll <0...100>` | 使用轮询级别等待工作（0 - 不轮询，默认：50） |
| `-Cb, --cpu-mask-batch M` | CPU 亲和性掩码：任意长度的十六进制。补充 cpu-range-batch（默认：与 --cpu-mask 相同） |
| `-Crb, --cpu-range-batch lo-hi` | 亲和性的 CPU 范围。补充 --cpu-mask-batch |
| `--cpu-strict-batch <0\|1>` | 使用严格的 CPU 放置（默认：与 --cpu-strict 相同） |
| `--prio-batch N` | 设置进程/线程优先级：0-normal, 1-medium, 2-high, 3-realtime（默认：0） |
| `--poll-batch <0\|1>` | 使用轮询等待工作（默认：与 --poll 相同） |
| `-c, --ctx-size N` | 提示词上下文大小（默认：0，0 = 从模型加载）<br/>(环境变量: LLAMA_ARG_CTX_SIZE) |
| `-n, --predict, --n-predict N` | 预测的令牌数量（默认：-1，-1 = 无限，-2 = 直到上下文填满）<br/>(环境变量: LLAMA_ARG_N_PREDICT) |
| `-b, --batch-size N` | 逻辑最大批处理大小（默认：2048）<br/>(环境变量: LLAMA_ARG_BATCH) |
| `-ub, --ubatch-size N` | 物理最大批处理大小（默认：512）<br/>(环境变量: LLAMA_ARG_UBATCH) |
| `--keep N` | 从初始提示词保留的令牌数（默认：0，-1 = 全部） |
| `--swa-full` | 使用全大小 SWA 缓存（默认：false）<br/>[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/13194#issuecomment-2868343055)<br/>(环境变量: LLAMA_ARG_SWA_FULL) |
| `-fa, --flash-attn [on\|off\|auto]` | 设置 Flash Attention 使用（'on'、'off' 或 'auto'，默认：'auto'）<br/>(环境变量: LLAMA_ARG_FLASH_ATTN) |
| `-p, --prompt PROMPT` | 开始生成时使用的提示词；对于系统消息，使用 -sys |
| `--perf, --no-perf` | 是否启用内部 libllama 性能计时（默认：false）<br/>(环境变量: LLAMA_ARG_PERF) |
| `-f, --file FNAME` | 包含提示词的文件（默认：none） |
| `-bf, --binary-file FNAME` | 包含提示词的二进制文件（默认：none） |
| `-e, --escape, --no-escape` | 是否处理转义序列（\n, \r, \t, \', \", \\）（默认：true） |
| `--rope-scaling {none,linear,yarn}` | RoPE 频率缩放方法，除非模型指定，否则默认为 linear<br/>(环境变量: LLAMA_ARG_ROPE_SCALING_TYPE) |
| `--rope-scale N` | RoPE 上下文缩放因子，将上下文扩展 N 倍<br/>(环境变量: LLAMA_ARG_ROPE_SCALE) |
| `--rope-freq-base N` | RoPE 基础频率，用于 NTK 感知缩放（默认：从模型加载）<br/>(环境变量: LLAMA_ARG_ROPE_FREQ_BASE) |
| `--rope-freq-scale N` | RoPE 频率缩放因子，将上下文扩展 1/N 倍<br/>(环境变量: LLAMA_ARG_ROPE_FREQ_SCALE) |
| `--yarn-orig-ctx N` | YaRN：模型的原始上下文大小（默认：0 = 模型训练上下文大小）<br/>(环境变量: LLAMA_ARG_YARN_ORIG_CTX) |
| `--yarn-ext-factor N` | YaRN：外推混合因子（默认：-1.00，0.0 = 完全插值）<br/>(环境变量: LLAMA_ARG_YARN_EXT_FACTOR) |
| `--yarn-attn-factor N` | YaRN：缩放 sqrt(t) 或注意力幅度（默认：-1.00）<br/>(环境变量: LLAMA_ARG_YARN_ATTN_FACTOR) |
| `--yarn-beta-slow N` | YaRN：高校正维度或 alpha（默认：-1.00）<br/>(环境变量: LLAMA_ARG_YARN_BETA_SLOW) |
| `--yarn-beta-fast N` | YaRN：低保正维度或 beta（默认：-1.00）<br/>(环境变量: LLAMA_ARG_YARN_BETA_FAST) |
| `-kvo, --kv-offload, -nkvo, --no-kv-offload` | 是否启用 KV 缓存卸载（默认：已启用）<br/>(环境变量: LLAMA_ARG_KV_OFFLOAD) |
| `--repack, -nr, --no-repack` | 是否启用权重重新打包（默认：已启用）<br/>(环境变量: LLAMA_ARG_REPACK) |
| `--no-host` | 绕过主机缓冲区，允许使用额外缓冲区<br/>(环境变量: LLAMA_ARG_NO_HOST) |
| `-ctk, --cache-type-k TYPE` | K 的 KV 缓存数据类型<br/>允许的值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1<br/>(默认：f16)<br/>(环境变量: LLAMA_ARG_CACHE_TYPE_K) |
| `-ctv, --cache-type-v TYPE` | V 的 KV 缓存数据类型<br/>允许的值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1<br/>(默认：f16)<br/>(环境变量: LLAMA_ARG_CACHE_TYPE_V) |
| `-dt, --defrag-thold N` | KV 缓存碎片整理阈值（已弃用）<br/>(环境变量: LLAMA_ARG_DEFRAG_THOLD) |
| `-np, --parallel N` | 并行解码的序列数（默认：1）<br/>(环境变量: LLAMA_ARG_N_PARALLEL) |
| `--mlock` | 强制系统将模型保留在 RAM 中，而不是交换或压缩<br/>(环境变量: LLAMA_ARG_MLOCK) |
| `--mmap, --no-mmap` | 是否内存映射模型。（如果禁用 mmap，加载较慢，但在不使用 mlock 时可能会减少页面换出）（默认：已启用）<br/>(环境变量: LLAMA_ARG_MMAP) |
| `-dio, --direct-io, -ndio, --no-direct-io` | 如果可用，使用 DirectIO。（默认：已禁用）<br/>(环境变量: LLAMA_ARG_DIO) |
| `--numa TYPE` | 尝试对某些 NUMA 系统有帮助的优化<br/>- distribute：在所有节点上均匀分布执行<br/>- isolate：仅在执行开始的节点上的 CPU 上生成线程<br/>- numactl：使用 numactl 提供的 CPU 映射<br/>如果之前没有运行过，建议在使用此功能之前删除系统页面缓存<br/>参见 https://github.com/ggml-org/llama.cpp/issues/1437<br/>(环境变量: LLAMA_ARG_NUMA) |
| `-dev, --device <dev1,dev2,..>` | 用于卸载的设备逗号分隔列表（none = 不卸载）<br/>使用 --list-devices 查看可用设备列表<br/>(环境变量: LLAMA_ARG_DEVICE) |
| `--list-devices` | 打印可用设备列表并退出 |
| `-ot, --override-tensor <tensor name pattern>=<buffer type>,...` | 覆盖张量缓冲区类型<br/>(环境变量: LLAMA_ARG_OVERRIDE_TENSOR) |
| `-cmoe, --cpu-moe` | 将所有混合专家（MoE）权重保留在 CPU 中<br/>(环境变量: LLAMA_ARG_CPU_MOE) |
| `-ncmoe, --n-cpu-moe N` | 将前 N 层的混合专家（MoE）权重保留在 CPU 中<br/>(环境变量: LLAMA_ARG_N_CPU_MOE) |
| `-ngl, --gpu-layers, --n-gpu-layers N` | 存储在 VRAM 中的最大层数，可以是确切数字、'auto' 或 'all'（默认：auto）<br/>(环境变量: LLAMA_ARG_N_GPU_LAYERS) |
| `-sm, --split-mode {none,layer,row,tensor}` | 如何在多个 GPU 之间拆分模型，之一：<br/>- none：仅使用一个 GPU<br/>- layer（默认）：跨 GPU 拆分层和 KV（流水线化）<br/>- row：按行跨 GPU 拆分权重（并行化）<br/>- tensor：跨 GPU 拆分权重和 KV（并行化，实验性）<br/>(环境变量: LLAMA_ARG_SPLIT_MODE) |
| `-ts, --tensor-split N0,N1,N2,...` | 卸载到每个 GPU 的模型比例，逗号分隔的比例列表，例如 3,1<br/>(环境变量: LLAMA_ARG_TENSOR_SPLIT) |
| `-mg, --main-gpu INDEX` | 用于模型的 GPU（split-mode = none 时），或用于中间结果和 KV（split-mode = row 时）（默认：0）<br/>(环境变量: LLAMA_ARG_MAIN_GPU) |
| `-fit, --fit [on\|off]` | 是否调整未设置的参数以适应设备内存（'on' 或 'off'，默认：'on'）<br/>(环境变量: LLAMA_ARG_FIT) |
| `-fitt, --fit-target MiB0,MiB1,MiB2,...` | 每个设备的 --fit 目标边距，逗号分隔的值列表，单个值广播到所有设备，默认：1024<br/>(环境变量: LLAMA_ARG_FIT_TARGET) |
| `-fitc, --fit-ctx N` | 可由 --fit 选项设置的最小上下文大小，默认：4096<br/>(环境变量: LLAMA_ARG_FIT_CTX) |
| `--check-tensors` | 检查模型张量数据中的无效值（默认：false） |
| `--override-kv KEY=TYPE:VALUE,...` | 按键覆盖模型元数据的高级选项。要指定多个覆盖，可以使用逗号分隔的值。<br/>类型：int、float、bool、str。例如：--override-kv tokenizer.ggml.add_bos_token=bool:false,tokenizer.ggml.add_eos_token=bool:false |
| `--op-offload, --no-op-offload` | 是否将主机张量操作卸载到设备（默认：true） |
| `--lora FNAME` | LoRA 适配器的路径（使用逗号分隔的值加载多个适配器） |
| `--lora-scaled FNAME:SCALE,...` | 带有用户定义缩放的 LoRA 适配器路径（格式：FNAME:SCALE,...）<br/>注意：使用逗号分隔的值 |
| `--control-vector FNAME` | 添加控制向量<br/>注意：使用逗号分隔的值添加多个控制向量 |
| `--control-vector-scaled FNAME:SCALE,...` | 添加带有用户定义缩放 SCALE 的控制向量<br/>注意：使用逗号分隔的值（格式：FNAME:SCALE,...） |
| `--control-vector-layer-range START END` | 应用控制向量的层范围，开始和结束都包括在内 |
| `-m, --model FNAME` | 要加载的模型路径<br/>(环境变量: LLAMA_ARG_MODEL) |
| `-mu, --model-url MODEL_URL` | 模型下载 url（默认：未使用）<br/>(环境变量: LLAMA_ARG_MODEL_URL) |
| `-dr, --docker-repo [<repo>/]<model>[:quant]` | Docker Hub 模型仓库。repo 是可选的，默认为 ai/。quant 是可选的，默认为 :latest。<br/>例如：gemma3<br/>(默认：未使用）<br/>(环境变量: LLAMA_ARG_DOCKER_REPO) |
| `-hf, -hfr, --hf-repo <user>/<model>[:quant]` | Hugging Face 模型仓库；quant 是可选的，不区分大小写，默认为 Q4_K_M，如果 Q4_K_M 不存在，则回退到仓库中的第一个文件。<br/>mmproj 也会自动下载（如果可用）。要禁用，添加 --no-mmproj<br/>例如：ggml-org/GLM-4.7-Flash-GGUF:Q4_K_M<br/>(默认：未使用）<br/>(环境变量: LLAMA_ARG_HF_REPO) |
| `-hff, --hf-file FILE` | Hugging Face 模型文件。如果指定，它将覆盖 --hf-repo 中的 quant（默认：未使用）<br/>(环境变量: LLAMA_ARG_HF_FILE) |
| `-hfv, -hfrv, --hf-repo-v <user>/<model>[:quant]` | 声码器模型的 Hugging Face 模型仓库（默认：未使用）<br/>(环境变量: LLAMA_ARG_HF_REPO_V) |
| `-hffv, --hf-file-v FILE` | 声码器模型的 Hugging Face 模型文件（默认：未使用）<br/>(环境变量: LLAMA_ARG_HF_FILE_V) |
| `-hft, --hf-token TOKEN` | Hugging Face 访问令牌（默认：来自 HF_TOKEN 环境变量的值）<br/>(环境变量: HF_TOKEN) |
| `--log-disable` | 禁用日志 |
| `--log-file FNAME` | 记录到文件<br/>(环境变量: LLAMA_LOG_FILE) |
| `--log-colors [on\|off\|auto]` | 设置彩色日志（'on'、'off' 或 'auto'，默认：'auto'）<br/>'auto' 在输出到终端时启用颜色<br/>(环境变量: LLAMA_LOG_COLORS) |
| `-v, --verbose, --log-verbose` | 将详细程度级别设置为无限（即记录所有消息，对调试有用） |
| `--offline` | 离线模式：强制使用缓存，阻止网络访问<br/>(环境变量: LLAMA_OFFLINE) |
| `-lv, --verbosity, --log-verbosity N` | 设置详细程度阈值。忽略具有更高详细程度的信息。值：<br/> - 0：通用输出<br/> - 1：错误<br/> - 2：警告<br/> - 3：信息<br/> - 4：调试<br/>(默认：3）<br/><br/>(环境变量: LLAMA_LOG_VERBOSITY) |
| `--log-prefix, --no-log-prefix` | 在日志消息中启用前缀<br/>(环境变量: LLAMA_ARG_LOG_PREFIX) |
| `--log-timestamps, --no-log-timestamps` | 在日志消息中启用时间戳<br/>(环境变量: LLAMA_ARG_LOG_TIMESTAMPS) |
| `--spec-draft-type-k, -ctkd, --cache-type-k-draft TYPE` | 草稿模型的 K 的 KV 缓存数据类型<br/>允许的值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1<br/>(默认：f16)<br/>(环境变量: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_K) |
| `--spec-draft-type-v, -ctvd, --cache-type-v-draft TYPE` | 草稿模型的 V 的 KV 缓存数据类型<br/>允许的值：f32, f16, bf16, q8_0, q4_0, q4_1, iq4_nl, q5_0, q5_1<br/>(默认：f16)<br/>(环境变量: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_V) |

### 采样参数

| 参数 | 说明 |
| -------- | ----------- |
| `--samplers SAMPLERS` | 按顺序用于生成的采样器，用 ';' 分隔<br/>(默认：penalties;dry;top_n_sigma;top_k;typ_p;top_p;min_p;xtc;temperature) |
| `-s, --seed SEED` | RNG 种子（默认：-1，-1 使用随机种子） |
| `--sampler-seq, --sampling-seq SEQUENCE` | 用于采样的简化序列（默认：edskypmxt） |
| `--ignore-eos` | 忽略流结束令牌并继续生成（意味着 --logit-bias EOS-inf） |
| `--temp, --temperature N` | 温度（默认：0.80） |
| `--top-k N` | Top-k 采样（默认：40，0 = 禁用）<br/>(环境变量: LLAMA_ARG_TOP_K) |
| `--top-p N` | Top-p 采样（默认：0.95，1.0 = 禁用） |
| `--min-p N` | Min-p 采样（默认：0.05，0.0 = 禁用） |
| `--top-nsigma, --top-n-sigma N` | Top-n-sigma 采样（默认：-1.00，-1.0 = 禁用） |
| `--xtc-probability N` | xtc 概率（默认：0.00，0.0 = 禁用） |
| `--xtc-threshold N` | xtc 阈值（默认：0.10，1.0 = 禁用） |
| `--typical, --typical-p N` | 局部典型采样，参数 p（默认：1.00，1.0 = 禁用） |
| `--repeat-last-n N` | 考虑惩罚的最后 n 个令牌（默认：64，0 = 禁用，-1 = ctx_size） |
| `--repeat-penalty N` | 惩罚重复令牌序列（默认：1.00，1.0 = 禁用） |
| `--presence-penalty N` | 重复 alpha 存在惩罚（默认：0.00，0.0 = 禁用） |
| `--frequency-penalty N` | 重复 alpha 频率惩罚（默认：0.00，0.0 = 禁用） |
| `--dry-multiplier N` | 设置 DRY 采样乘数（默认：0.00，0.0 = 禁用） |
| `--dry-base N` | 设置 DRY 采样基准值（默认：1.75） |
| `--dry-allowed-length N` | 设置 DRY 采样的允许长度（默认：2） |
| `--dry-penalty-last-n N` | 设置最后 n 个令牌的 DRY 惩罚（默认：-1，0 = 禁用，-1 = 上下文大小） |
| `--dry-sequence-breaker STRING` | 为 DRY 采样添加序列中断器，清除默认中断器（'\n'、':'、'"'、'*'）；使用 "none" 不使用任何序列中断器 |
| `--adaptive-target N` | adaptive-p：选择接近此概率的令牌（有效范围 0.0 到 1.0；负数 = 禁用）（默认：-1.00）<br/>[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/17927) |
| `--adaptive-decay N` | adaptive-p：目标适应随时间的衰减率。较低的值更具响应性，较高的值更稳定。<br/>（有效范围 0.0 到 0.99）（默认：0.90） |
| `--dynatemp-range N` | 动态温度范围（默认：0.00，0.0 = 禁用） |
| `--dynatemp-exp N` | 动态温度指数（默认：1.00） |
| `--mirostat N` | 使用 Mirostat 采样。<br/>如果使用，将忽略 Top K、Nucleus 和局部典型采样器。<br/>（默认：0，0 = 禁用，1 = Mirostat，2 = Mirostat 2.0） |
| `--mirostat-lr N` | Mirostat 学习率，参数 eta（默认：0.10） |
| `--mirostat-ent N` | Mirostat 目标熵，参数 tau（默认：5.00） |
| `-l, --logit-bias TOKEN_ID(+/-)BIAS` | 修改令牌在补全中出现的可能性，<br/>例如 `--logit-bias 15043+1` 以增加令牌 ' Hello' 的可能性，<br/>或 `--logit-bias 15043-1` 以减少令牌 ' Hello' 的可能性 |
| `--grammar GRAMMAR` | BNF 类语法以约束生成（请参阅 grammars/ 目录中的示例） |
| `--grammar-file FNAME` | 从文件读取语法 |
| `-j, --json-schema SCHEMA` | 用于约束生成的 JSON 模式（https://json-schema.org/），例如 `{}` 表示任何 JSON 对象<br/>对于带有外部 $refs 的模式，请改用 --grammar + example/json_schema_to_grammar.py |
| `-jf, --json-schema-file FILE` | 包含用于约束生成的 JSON 模式的文件（https://json-schema.org/），例如 `{}` 表示任何 JSON 对象<br/>对于带有外部 $refs 的模式，请改用 --grammar + example/json_schema_to_grammar.py |
| `-bs, --backend-sampling` | 启用后端采样（实验性）（默认：禁用）<br/>(环境变量: LLAMA_ARG_BACKEND_SAMPLING) |

### 补全特定参数

| 参数 | 说明 |
| -------- | ----------- |
| `--verbose-prompt` | 在生成之前打印详细提示词（默认：false） |
| `--display-prompt, --no-display-prompt` | 是否在生成时打印提示词（默认：true） |
| `-co, --color [on\|off\|auto]` | 颜色化输出以区分提示词和用户输入与生成内容（'on'、'off' 或 'auto'，默认：'auto'）<br/>'auto' 在输出到终端时启用颜色 |
| `--context-shift, --no-context-shift` | 是否在无限文本生成时使用上下文移位（默认：禁用）<br/>(环境变量: LLAMA_ARG_CONTEXT_SHIFT) |
| `-sys, --system-prompt PROMPT` | 与模型一起使用的系统提示词（如果适用，取决于聊天模板） |
| `-sysf, --system-prompt-file FNAME` | 包含系统提示词的文件（默认：none） |
| `-ptc, --print-token-count N` | 每 N 个令牌打印令牌数（默认：-1） |
| `--prompt-cache FNAME` | 用于缓存提示词状态以更快启动的文件（默认：none） |
| `--prompt-cache-all` | 如果指定，也将用户输入和生成内容保存到缓存中 |
| `--prompt-cache-ro` | 如果指定，使用提示词缓存但不更新它 |
| `-r, --reverse-prompt PROMPT` | 在 PROMPT 处暂停生成，在交互模式下返回控制 |
| `-sp, --special` | 启用特殊令牌输出（默认：false） |
| `-cnv, --conversation, -no-cnv, --no-conversation` | 是否在对话模式下运行：<br/>- 不打印特殊令牌和后缀/前缀<br/>- 也启用交互模式<br/>(默认：如果聊天模板可用，则自动启用) |
| `-st, --single-turn` | 仅运行单轮对话，完成后退出<br/>如果第一轮是使用 --prompt 预定义的，则不会交互<br/>(默认：false) |
| `-i, --interactive` | 在交互模式下运行（默认：false） |
| `-if, --interactive-first` | 在交互模式下运行并立即等待输入（默认：false） |
| `-mli, --multiline-input` | 允许您编写或粘贴多行而不以 '\' 结尾 |
| `--in-prefix-bos` | 在用户输入前添加 BOS 前缀，在 `--in-prefix` 字符串之前 |
| `--in-prefix STRING` | 用户输入的前缀字符串（默认：空） |
| `--in-suffix STRING` | 用户输入后的后缀字符串（默认：空） |
| `--warmup, --no-warmup` | 是否通过空运行执行预热（默认：已启用） |
| `-gan, --grp-attn-n N` | 组注意力因子（默认：1）<br/>(环境变量: LLAMA_ARG_GRP_ATTN_N) |
| `-gaw, --grp-attn-w N` | 组注意力宽度（默认：512）<br/>(环境变量: LLAMA_ARG_GRP_ATTN_W) |
| `--jinja, --no-jinja` | 是否在聊天中使用 jinja 模板引擎（默认：禁用）<br/>(环境变量: LLAMA_ARG_JINJA) |
| `--reasoning-format FORMAT` | 控制是否允许和/或从响应中提取思维标记，以及以什么格式返回它们；之一：<br/>- none：将思维保留在 `message.content` 中未解析<br/>- deepseek：将思维放在 `message.reasoning_content` 中<br/>- deepseek-legacy：在 `message.content` 中保留 `<think>` 标记，同时也填充 `message.reasoning_content`<br/>(默认：auto）<br/>(环境变量: LLAMA_ARG_THINK) |
| `-rea, --reasoning [on\|off\|auto]` | 在聊天中使用推理/思考（'on'、'off' 或 'auto'，默认：'auto'（从模板检测））<br/>(环境变量: LLAMA_ARG_REASONING) |
| `--reasoning-budget N` | 思考的令牌预算：-1 表示不受限制，0 表示立即结束，N>0 表示令牌预算（默认：-1）<br/>(环境变量: LLAMA_ARG_THINK_BUDGET) |
| `--reasoning-budget-message MESSAGE` | 当推理预算耗尽时在思维结束标记之前注入的消息（默认：none）<br/>(环境变量: LLAMA_ARG_THINK_BUDGET_MESSAGE) |
| `--chat-template JINJA_TEMPLATE` | 设置自定义 jinja 聊天模板（默认：从模型元数据获取的模板）<br/>如果指定了后缀/前缀，模板将被禁用<br/>仅接受常用模板（除非在此标志之前设置了 --jinja）：<br/>内置模板列表：<br/>bailing, bailing-think, bailing2, chatglm3, chatglm4, chatml, command-r, deepseek, deepseek-ocr, deepseek2, deepseek3, exaone-moe, exaone3, exaone4, falcon3, gemma, gigachat, glmedge, gpt-oss, granite, granite-4.0, grok-2, hunyuan-dense, hunyuan-moe, hunyuan-vl, kimi-k2, llama2, llama2-sys, llama2-sys-bos, llama2-sys-strip, llama3, llama4, megrez, minicpm, mistral-v1, mistral-v3, mistral-v3-tekken, mistral-v7, mistral-v7-tekken, monarch, openchat, orion, pangu-embedded, phi3, phi4, rwkv-world, seed_oss, smolvlm, solar-open, vicuna, vicuna-orca, yandex, zephyr<br/>(环境变量: LLAMA_ARG_CHAT_TEMPLATE) |
| `--chat-template-file JINJA_TEMPLATE_FILE` | 从外部文件设置自定义 jinja 聊天模板（默认：从模型元数据获取的模板）<br/>如果指定了后缀/前缀，模板将被禁用<br/>仅接受常用模板（除非在此标志之前设置了 --jinja）：<br/>内置模板列表：<br/>bailing, bailing-think, bailing2, chatglm3, chatglm4, chatml, command-r, deepseek, deepseek-ocr, deepseek2, deepseek3, exaone-moe, exaone3, exaone4, falcon3, gemma, gigachat, glmedge, gpt-oss, granite, granite-4.0, grok-2, hunyuan-dense, hunyuan-moe, hunyuan-vl, kimi-k2, llama2, llama2-sys, llama2-sys-bos, llama2-sys-strip, llama3, llama4, megrez, minicpm, mistral-v1, mistral-v3, mistral-v3-tekken, mistral-v7, mistral-v7-tekken, monarch, openchat, orion, pangu-embedded, phi3, phi4, rwkv-world, seed_oss, smolvlm, solar-open, vicuna, vicuna-orca, yandex, zephyr<br/>(环境变量: LLAMA_ARG_CHAT_TEMPLATE_FILE) |
| `--skip-chat-parsing, --no-skip-chat-parsing` | 即使指定了 Jinja 模板，也强制使用纯内容解析器；模型将在内容部分输出所有内容，包括任何推理和/或工具调用（默认：禁用）<br/>(环境变量: LLAMA_ARG_SKIP_CHAT_PARSING) |
| `--simple-io` | 使用基本 IO 以在子进程和有限控制台中具有更好的兼容性 |

## 常用选项

在本节中，我们介绍使用 `llama-completion` 程序运行 LLaMA 模型时最常用的选项：

-   `-m FNAME, --model FNAME`: 指定 LLaMA 模型文件的路径（例如，`models/gemma-1.1-7b-it.Q4_K_M.gguf`；如果设置了 `--model-url`，则从中推断）。
-   `-mu MODEL_URL --model-url MODEL_URL`: 指定用于下载文件的远程 http url（例如 [https://huggingface.co/ggml-org/gemma-1.1-7b-it-Q4_K_M-GGUF/resolve/main/gemma-1.1-7b-it.Q4_K_M.gguf?download=true](https://huggingface.co/ggml-org/gemma-1.1-7b-it-Q4_K_M-GGUF/resolve/main/gemma-1.1-7b-it.Q4_K_M.gguf?download=true)）。
-   `-i, --interactive`: 在交互模式下运行程序，允许您直接提供输入并接收实时响应。
-   `-n N, --n-predict N`: 设置生成文本时要预测的令牌数量。调整此值可以影响生成文本的长度。
-   `-c N, --ctx-size N`: 设置提示词上下文的大小。默认为 4096，但如果 LLaMA 模型构建时具有更长的上下文，增加此值将为更长的输入/推理提供更好的结果。
-   `-mli, --multiline-input`: 允许您编写或粘贴多行而不以 '\' 结尾
-   `-t N, --threads N`: 设置生成期间使用的线程数。为了获得最佳性能，建议将此值设置为您系统拥有的物理 CPU 核心数。
-   `-ngl N, --n-gpu-layers N`: 在编译 GPU 支持时，此选项允许将某些层卸载到 GPU 进行计算。通常会提高性能。

## 输入提示词

`llama-completion` 程序提供了几种使用输入提示词与 LLaMA 模型交互的方法：

-   `--prompt PROMPT`: 直接作为命令行选项提供提示词。
-   `--file FNAME`: 提供包含提示词或多个提示词的文件。
-   `--system-prompt PROMPT`: 提供系统提示词（否则将使用聊天模板中的默认提示词（如果提供））。
-   `--system-prompt-file FNAME`: 提供包含系统提示词的文件。
-   `--interactive-first`: 在交互模式下运行程序并立即等待输入。（更多内容见下文。）

## 交互

`llama-completion` 程序提供了一种与 LLaMA 模型交互的无缝方式，允许用户参与实时对话或为特定任务提供指令。可以使用各种选项触发交互模式，包括 `--interactive` 和 `--interactive-first`。

在交互模式下，用户可以通过在生成过程中注入他们的输入来参与文本生成。用户可以随时按 `Ctrl+C` 插入并输入他们的内容，然后按 `Return` 将其提交给 LLaMA 模型。要在不结束输入的情况下提交额外的行，用户可以在当前行的末尾使用反斜杠（`\`）并继续输入。

### 交互选项

-   `-i, --interactive`: 在交互模式下运行程序，允许用户参与实时对话或向模型提供特定指令。
-   `--interactive-first`: 在交互模式下运行程序并立即等待用户输入，然后再开始文本生成。
-   `-cnv,  --conversation`:  在对话模式下运行程序（不打印特殊令牌和后缀/前缀，使用默认或提供的聊天模板）（如果找到聊天模板，默认为 true）
-   `-no-cnv`:  禁用对话模式（默认为 false）
-   `-st, --single-turn`:  仅处理单轮对话（用户输入），然后退出。
-   `--jinja`:  启用 jinja 聊天模板解析器，将使用模型的内置模板或用户提供的模板（默认为 false）
-   `--color`: 启用颜色化输出，以在视觉上区分提示词、用户输入和生成的文本。

通过理解和利用这些交互选项，您可以与 LLaMA 模型创建引人入胜和动态的体验，根据您的特定需求定制文本生成过程。

### 反向提示词

反向提示词是一种通过与 LLaMA 模型创建类似聊天体验的强大方法，它在遇到特定文本字符串时暂停文本生成：

-   `-r PROMPT, --reverse-prompt PROMPT`: 指定一个或多个反向提示词以暂停文本生成并切换到交互模式。例如，`-r "User:"` 可以用于每当轮到用户说话时跳回对话。这有助于创建更具交互性和对话性的体验。但是，当反向提示词以空格结尾时，它不起作用。

为了克服此限制，您可以使用 `--in-prefix` 标志在反向提示词后添加一个空格或任何其他字符。

### 输入前缀

`--in-prefix` 标志用于为您的输入添加前缀，主要用于在反向提示词后插入一个空格。以下是如何结合使用 `--in-prefix` 标志和 `--reverse-prompt` 标志的示例：

```sh
./llama-completion -r "User:" --in-prefix " "
```

### 输入后缀

`--in-suffix` 标志用于在您的输入后添加后缀。这对于在用户输入后添加 "Assistant:" 提示词很有用。它是在自动添加到用户输入末尾的换行符（`\n`）之后添加的。以下是如何结合使用 `--in-suffix` 标志和 `--reverse-prompt` 标志的示例：

```sh
./llama-completion -r "User:" --in-prefix " " --in-suffix "Assistant:"
```
当启用 --in-prefix 或 --in-suffix 选项时，聊天模板（--chat-template）被禁用

### 聊天模板

 `--chat-template JINJA_TEMPLATE`: 此选项设置自定义 jinja 聊天模板。它接受一个字符串，而不是文件名。默认：从模型元数据获取的模板。Llama.cpp 仅支持 [一些预定义的模板](https://github.com/ggml-org/llama.cpp/wiki/Templates-supported-by-llama_chat_apply_template)。这些包括 llama2, llama3, gemma, monarch, chatml, orion, vicuna, vicuna-orca, deepseek, command-r, zephyr。当启用 --in-prefix 或 --in-suffix 选项时，聊天模板（--chat-template）被禁用。

 示例用法：`--chat-template gemma`

`--chat-template-file FNAME`:  从外部文件加载自定义 jinja 聊天模板，如果模型包含过时或不兼容的模板，这很有用，一些示例可以在 models/templates 中找到。最新的聊天模板可以使用 scripts/get_chat_template.py 从 Hugging Face 下载

## 上下文管理

在文本生成期间，LLaMA 模型具有有限的上下文大小，这意味着它们只能从输入和生成的文本中考虑一定数量的令牌。当上下文填满时，模型会在内部重置，可能会丢失对话开始或说明的一些信息。上下文管理选项有助于在这些情况下保持连续性和连贯性。

### 上下文大小

- `-c N, --ctx-size N`: 设置提示词上下文的大小（默认：4096，0 = 从模型加载）。如果 LLaMA 模型构建时具有更长的上下文，增加此值将为更长的输入/推理产生最佳结果。

### 扩展上下文大小

一些微调模型通过缩放 RoPE 扩展了上下文长度。例如，如果原始预训练模型的上下文长度（最大序列长度）为 4096（4k），而微调模型为 32k。这是缩放因子 8，应该可以通过将上面的 `--ctx-size` 设置为 32768（32k）并将 `--rope-scale` 设置为 8 来工作。

-   `--rope-scale N`: 其中 N 是微调模型使用的线性缩放因子。

### 保留提示词

`--keep` 选项允许用户在模型用尽上下文时保留原始提示词，确保维持与初始说明或对话主题的连接。

-   `--keep N`: 指定当模型重置其内部上下文时从初始提示词保留的令牌数。默认情况下，此值设置为 0（意味着不保留任何令牌）。使用 `-1` 保留初始提示词中的所有令牌。

通过利用上下文管理选项，如 `--ctx-size` 和 `--keep`，您可以与 LLaMA 模型保持更连贯和一致的交互，确保生成的文本与原始提示词或对话保持相关。

## 生成标志

以下选项允许您控制文本生成过程，并根据您的需求微调生成文本的多样性、创造性和质量。通过调整这些选项并尝试不同的值组合，您可以找到最适合您特定用例的设置。

### 要预测的令牌数

-   `-n N, --predict N`: 设置生成文本时要预测的令牌数（默认：-1，-1 = 无限，-2 = 直到上下文填满）

`--predict` 选项控制模型作为对输入提示词的响应而生成的令牌数量。通过调整此值，您可以影响生成文本的长度。较高的值将导致更长的文本，而较低的值将产生更短的文本。

值为 -1 将启用无限文本生成，即使我们有一个有限的上下文窗口。当上下文窗口填满时，一些较早的令牌（`--keep` 之后的一半令牌）将被丢弃。然后必须重新评估上下文才能恢复生成。对于大型模型和/或大型上下文窗口，这将导致输出中的显著暂停。

如果暂停是不可取的，值为 -2 将在上下文填满时立即停止生成。

`--no-context-shift` 选项允许您在有限的上下文窗口填满时停止无限文本生成。

重要的是要注意，如果遇到序列结束（EOS）令牌或反向提示词，生成的文本可能会短于指定的令牌数。在交互模式下，文本生成将暂停，控制权将返回给用户。在非交互模式下，程序将结束。在这两种情况下，文本生成可能会在达到指定的 `--predict` 值之前停止。如果您希望模型永远不自己生成序列结束，可以使用 `--ignore-eos` 参数。

### 温度

-   `--temp N`: 调整生成文本的随机性（默认：0.8）。

温度是控制生成文本随机性的超参数。它影响模型输出令牌的概率分布。较高的温度（例如 1.5）使输出更加随机和创造性，而较低的温度（例如 0.5）使输出更加专注、确定性和保守。默认值为 0.8，在随机性和确定性之间提供了平衡。在极端情况下，温度为 0 将始终选择最可能的下一个令牌，导致每次运行产生相同的输出。

示例用法：`--temp 0`

### 重复惩罚

-   `--repeat-penalty N`: 控制生成文本中令牌序列的重复默认：1.0，1.0 = 禁用）。
-   `--repeat-last-n N`: 考虑惩罚重复的最后 n 个令牌（默认：64，0 = 禁用，-1 = ctx-size）。

`repeat-penalty` 选项有助于防止模型生成重复或单调的文本。较高的值（例如 1.5）将更强烈地惩罚重复，而较低的值（例如 0.9）将更加宽松。默认值为 1。

`repeat-last-n` 选项控制历史记录中用于惩罚重复的令牌数。较大的值将在生成的文本中进一步向后查看以防止重复，而较小的值将仅考虑最近的令牌。值为 0 禁用惩罚，值为 -1 将考虑的令牌数设置为等于上下文大小（`ctx-size`）。

### DRY 重复惩罚

DRY（Don't Repeat Yourself）采样是一种有效的技术，可以通过根据其最近的使用模式惩罚令牌来减少生成文本中的重复，即使跨越长上下文（原始 [PR 链接](https://github.com/oobabooga/text-generation-webui/pull/5677)）。

- `--dry-multiplier N`: 设置 DRY 采样乘数（默认：0.0，0.0 = 禁用）。
- `--dry-base N`: 设置 DRY 采样基准值（默认：1.75）。
- `--dry-allowed-length N`: 设置 DRY 采样的允许长度（默认：2）。
- `--dry-penalty-last-n N`: 设置最后 n 个令牌的 DRY 惩罚（默认：-1，0 = 禁用，-1 = 上下文大小）。
- `--dry-sequence-breaker STRING`: 为 DRY 采样添加序列中断器。可以多次使用以添加多个序列中断器。使用此功能将清除默认中断器，其中包括：`['\n', ':', '"', '*']`。如果提供了字符串 `"none"`，则不使用序列中断器。

`dry-multiplier` 选项控制 DRY 采样效果的强度。值为 0.0 禁用 DRY 采样，而较高的值会增加其影响。典型的推荐值为 0.8。

`dry-base` 选项设置 DRY 采样中指数惩罚计算的基准值。较高的值会导致更积极地惩罚重复。

`dry-allowed-length` 选项设置不会受到惩罚的重复序列的最大长度。小于或等于此长度的重复不会受到惩罚，允许自然地重复短短语或常用词。

`dry-penalty-last-n` 选项控制在应用 DRY 惩罚时要考虑的最近令牌的数量。值为 -1 考虑整个上下文。使用正值来限制考虑特定数量的最近令牌。

`dry-sequence-breaker` 选项添加单个序列中断器，可以多次使用以指定多个序列中断器。序列中断器中断序列匹配并将输入分割为可以应用匹配的部分。

DRY 采样提供了对文本生成更细致的控制，特别是对于减少长范围重复并保持全局连贯性。

示例用法：`--dry-multiplier 0.8 --dry-base 1.75 --dry-allowed-length 2 --dry-penalty-last-n -1 --dry-sequence-breaker "—" --dry-sequence-breaker "##"`

### Top-K 采样

-   `--top-k N`: 将下一个令牌选择限制为 K 个最可能的令牌（默认：40）。

Top-k 采样是一种文本生成方法，仅从模型预测的 K 个最可能的令牌中选择下一个令牌。它有助于降低生成低概率或荒谬令牌的风险，但也可能限制输出的多样性。top-k 的较高值（例如 100）将考虑更多令牌并导致更多样化的文本，而较低的值（例如 10）将专注于最可能的令牌并生成更保守的文本。默认值为 40。

示例用法：`--top-k 30`

### Top-P 采样

-   `--top-p N`: 将下一个令牌选择限制为累积概率高于阈值 P 的令牌子集（默认：0.9）。

Top-p 采样，也称为核心采样，是另一种文本生成方法，它从具有至少累积概率 p 的令牌子集中选择下一个令牌。此方法通过同时考虑令牌的概率和从中采样的令牌数量，在多样性和质量之间提供了平衡。top-p 的较高值（例如 0.95）将导致更多样化的文本，而较低的值（例如 0.5）将生成更专注和保守的文本。默认值为 0.9。

示例用法：`--top-p 0.95`

### Min-P 采样

-   `--min-p N`: 为令牌选择设置最小基础概率阈值（默认：0.1）。

Min-P 采样方法被设计为 Top-P 的替代方案，旨在确保质量和多样性的平衡。参数 *p* 表示相对于最可能令牌的概率，令牌被考虑的最小概率。例如，对于 *p*=0.05，最可能令牌的概率为 0.9，小于 0.045 的 logit 将被过滤掉。

示例用法：`--min-p 0.05`

### Adaptive-P 采样

-   `--adaptive-target N`: 选择接近此概率的令牌（有效范围 0.0 到 1.0；负数 = 禁用）
-   `--adaptive-decay N`: 适应的 EMA 衰减；历史 ≈ 1/(1-decay) 令牌（有效范围 0.0 - 0.99）

Adaptive-P：随着时间的推移，选择接近可配置目标概率的令牌。

adaptive-p 采样器转换令牌概率分布，以偏向接近用户可配置概率目标的令牌。在内部，采样器在每个采样步骤维护所选令牌在每个采样步骤的 *原始* 概率的指数移动平均值。它使用此 EMA 计算每个采样步骤的适应目标概率，从而随时间保持所需的目标概率。建议仅在此采样器之前进行轻微截断。建议在 adaptive-p 之前应用 min-p 作为唯一其他活动的采样器。

推荐的起始值：`--adaptive-target 0.55 --adaptive-decay 0.9`

有关更多信息，请参阅：[llama.cpp#17927](https://github.com/ggml-org/llama.cpp/pull/17927)

### 局部典型采样

-   `--typical N`: 使用参数 p 启用局部典型采样（默认：1.0，1.0 = 禁用）。

局部典型采样通过根据周围上下文采样典型或预期的令牌来促进上下文连贯和多样化文本的生成。通过将参数 p 设置在 0 和 1 之间，您可以控制产生局部连贯和多样化文本之间的平衡。更接近 1 的值将促进更上下文连贯的令牌，而更接近 0 的值将促进更多样化的令牌。等于 1 的值禁用局部典型采样。

示例用法：`--typical 0.9`

### Mirostat 采样

-   `--mirostat N`: 启用 Mirostat 采样，在文本生成期间控制困惑度（默认：0，0 = 禁用，1 = Mirostat，2 = Mirostat 2.0）。
-   `--mirostat-lr N`: 设置 Mirostat 学习率，参数 eta（默认：0.1）。
-   `--mirostat-ent N`: 设置 Mirostat 目标熵，参数 tau（默认：5.0）。

Mirostat 是一种算法，在文本生成期间主动将生成文本的质量保持在所需范围内。它旨在在连贯性和多样性之间取得平衡，避免由于过度重复（无聊陷阱）或不连贯（混乱陷阱）导致的低质量输出。

`--mirostat-lr` 选项设置 Mirostat 学习率（eta）。学习率影响算法对生成文本反馈的响应速度。较低的学习率将导致较慢的调整，而较高的学习率将使算法更加响应。默认值为 `0.1`。

`--mirostat-ent` 选项设置 Mirostat 目标熵（tau），代表生成文本的期望困惑度值。调整目标熵允许您控制生成文本中连贯性和多样性之间的平衡。较低的值将导致更加专注和连贯的文本，而较高的值将导致更加多样化和可能不太连贯的文本。默认值为 `5.0`。

示例用法：`--mirostat 2 --mirostat-lr 0.05 --mirostat-ent 3.0`

### XTC 采样

-   `--xtc-probability N`: 设置令牌移除的机会（在采样器启动时检查一次）（默认：0.0）。
-   `--xtc-threshold N`: 设置令牌要移除的最小概率阈值（默认：0.1）。

排除顶级选择（XTC）是一个独特的采样器，旨在从考虑中移除顶级令牌，以避免更加明显和重复的输出。它有 `xtc-probability` 的机会搜索概率为 `xtc-threshold` 及以上的令牌，然后移除所有此类令牌，除了最不可能的一个。

通过移除顶级令牌，XTC 可以提高答案的多样性，打破写作陈词滥调并抑制重复，因为陈词滥调和重复短语通常更有可能出现。通过保持高于阈值的最后一个令牌，XTC 确保答案仍然是连贯的。XTC 旨在用于创意任务，但可以随意尝试不同的设置和不同的模型。

作为实验性和独特的采样器，默认情况下禁用 XTC。推荐的采样器组合是 Min-P 后跟其默认设置下的 XTC：`--sampling-seq mx --min-p 0.02 --xtc-probability 0.5`。

示例用法：`--xtc-probability 0.5 --xtc-threshold 0.1`

### Top-nσ 采样

-   `--top-nsigma N`: 将下一个令牌选择限制为 softmax 前的对数值在最大对数的 n * σ 以内的令牌子集（默认：-1，-1 = 禁用）。

Top-nσ 采样是一种基于 softmax 前的对数中的统计阈值选择令牌的文本生成方法。它仅从对数在最大对数的 n * σ 以内的令牌中采样。此方法有助于无论温度缩放如何都保持稳定的采样空间，使其即使在高温度下也能在推理任务上表现良好。无需复杂的概率操作，它直接在 softmax 前的对数上有效地过滤令牌。top-nsigma 的较高值（例如 5）将考虑更多的噪声令牌，而较低的值（例如 1）将专注于采样空间中更具信息性的区域。

示例用法：`--top-nsigma 1`

### Logit 偏差

-   `-l TOKEN_ID(+/-)BIAS, --logit-bias TOKEN_ID(+/-)BIAS`: 修改特定令牌在生成的文本补全中出现的可能性。

logit 偏差选项允许您手动调整特定令牌在生成的文本中出现的可能性。通过提供令牌 ID 和正或负偏差值，您可以增加或减少该令牌生成的概率。

例如，使用 `--logit-bias 15043+1` 增加令牌 'Hello' 的可能性，或使用 `--logit-bias 15043-1` 减少其可能性。使用负无穷大的值，`--logit-bias 15043-inf` 确保令牌 `Hello` 永远不会生成。

一个更实际的用例可能是通过将 `\` 令牌（29905）设置为负无穷大来阻止生成 `\code{begin}` 和 `\code{end}`：`-l 29905-inf`。（这是由于 LaTeX 代码在 LLaMA 模型推理中经常出现。）

示例用法：`--logit-bias 29905-inf`

### RNG 种子

-   `-s SEED, --seed SEED`: 设置随机数生成器（RNG）种子（默认：-1，-1 = 随机种子）。

RNG 种子用于初始化影响文本生成过程的随机数生成器。通过设置特定的种子值，您可以在多次运行中使用相同的输入和设置获得一致和可重复的结果。这对于测试、调试或比较不同选项对生成文本的影响以查看它们何时分歧非常有帮助。如果种子设置为小于 0 的值，将使用随机种子，这将导致每次运行都产生不同的输出。

## 性能调优和内存选项

这些选项有助于提高 LLaMA 模型的性能和内存使用。通过调整这些设置，您可以微调模型的行为，以更好地适应您系统的功能，并为您特定的用例实现最佳性能。

### 线程数

-   `-t N, --threads N`: 设置生成期间使用的线程数。为了获得最佳性能，建议将此值设置为您系统拥有的物理 CPU 核心数（而不是逻辑核心数）。使用正确的线程数可以大大提高性能。
-   `-tb N, --threads-batch N`: 设置批处理和提示词处理期间使用的线程数。在某些系统上，在批处理期间使用比生成期间更多的线程是有益的。如果未指定，批处理期间使用的线程数将与生成期间使用的线程数相同。

### Mlock

-   `--mlock`: 将模型锁定在内存中，防止在内存映射时被换出。这可以提高性能，但会牺牲一些内存映射的优势，因为它需要更多的 RAM 来运行，并且可能会减慢加载时间，因为模型加载到 RAM 中。

### 禁用内存映射

-   `--no-mmap`: 不内存映射模型。默认情况下，模型被映射到内存中，这允许系统根据需要仅加载模型的必要部分。但是，如果模型大于您的 RAM 总量或您的系统可用内存较低，使用 mmap 可能会增加页面换出的风险，从而对性能产生负面影响。禁用 mmap 会导致加载时间变慢，但如果您不使用 `--mlock`，可能会减少页面换出。请注意，如果模型大于 RAM 总量，关闭 mmap 将阻止模型完全加载。

### NUMA 支持

-   `--numa distribute`: 将相等比例的线程固定到每个 NUMA 节点上的核心。这将在系统上的所有核心上分布负载，利用所有内存通道，但可能需要内存通过节点之间的慢速链接传输。
-   `--numa isolate`: 将所有线程固定到程序启动的 NUMA 节点上。这限制了可以使用的核心数量和内存量，但保证所有内存访问保持对 NUMA 节点本地。
-   `--numa numactl`: 使用 numactl 实用传递给程序的 CPUMAP 将线程固定。这是最灵活的模式，允许任意核心使用模式，例如使用一个 NUMA 节点上的所有核心，以及第二个节点上足够的核心来饱和节点间内存总线。

 这些标志尝试对某些具有非统一内存访问的系统有帮助的优化。这目前包括上述策略之一，并禁用 mmap 的预取和预读。后者导致映射的页面在第一次访问时被错误地加载，而不是一次全部加载，并结合将线程固定到 NUMA 节点，更多的页面最终落在它们使用的 NUMA 节点上。请注意，如果模型已经在系统页面缓存中，例如因为之前没有此选项的运行，除非您首先删除页面缓存，否则这几乎没有效果。这可以通过重新启动系统或在 Linux 上以 root 身份将 '3' 写入 '/proc/sys/vm/drop_caches' 来完成。

### 批处理大小

- `-ub N`，`--ubatch-size N`: 物理批处理大小。这是一次可以处理的最大令牌数。增加此值可能会提高提示词处理期间的性能，但会增加内存使用。默认：`512`。

- `-b N`，`--batch-size N`: 逻辑批处理大小。在使用具有流水线并行的多个 GPU 时，将此值增加到物理批处理大小值以上可能会提高提示词处理性能。默认：`2048`。

### 提示词缓存

-   `--prompt-cache FNAME`: 指定一个文件以缓存初始提示词后的模型状态。当您使用较长的提示词时，这可以显着加快启动时间。该文件在第一次运行期间创建，并在后续运行中重复使用和更新。**注意**：恢复缓存的提示词并不意味着将会话恢复到保存时的确切状态。因此，即使指定了特定的种子，您也不能保证获得与原始生成相同的令牌序列。

### 语法和 JSON 模式

-   `--grammar GRAMMAR`，`--grammar-file FILE`: 指定语法（内联定义或在文件中）以将模型输出约束为特定格式。例如，您可以强制模型输出 JSON 或仅使用表情符号说话。有关语法的详细信息，请参阅 [GBNF 指南](../../grammars/README.md)。

-   `--json-schema SCHEMA`: 指定 [JSON 模式](https://json-schema.org/)以将模型输出约束为（例如 `{}` 表示任何 JSON 对象，或 `{"items": {"type": "string", "minLength": 10, "maxLength": 100}, "minItems": 10}` 表示具有大小约束的字符串的 JSON 数组）。如果模式使用外部 `$ref`s，您应该使用 `--grammar "$( python examples/json_schema_to_grammar.py myschema.json )"` 代替。

### 量化

有关 4 位量化的信息，它可以显着提高性能并减少内存使用，请参阅 llama.cpp 的主要 [README](../../README.md#prepare-and-quantize)。

## LoRA（低秩适应）适配器

-   `--lora FNAME`: LoRA 适配器的可选路径，缩放为 1.0。可以与 `--lora-scaled` 混合，并可以重复使用多个适配器。
-   `--lora-scaled FNAME`: LoRA 适配器的可选路径，具有用户定义的缩放。可以与 `--lora` 混合并重复使用多个适配器。

您可以使用 `--lora` 或 `--lora-scaled` 添加 LoRA 适配器。例如：`--lora my_adapter_1.gguf --lora my_adapter_2.gguf ...` 或 `--lora-scaled lora_task_A.gguf 0.5 --lora-scaled lora_task_B.gguf 0.5`。

LoRA 适配器应采用 GGUF 格式。要从 Hugging Face 格式转换，请使用 `convert-lora-to-gguf.py` 脚本。LoRA 适配器被单独加载并在推理期间应用 - 它们不与主模型合并。这意味着使用 LoRA 适配器时完全支持 mmap 模型加载。旧的 `--lora-base` 标志已被删除，因为不再执行合并。

## 其他选项

这些选项在运行 LLaMA 模型时提供额外的功能和自定义：

-   `-h, --help`: 显示帮助消息，显示所有可用选项及其默认值。这对于检查最新的选项和默认值特别有用，因为它们可能会频繁更改，并且本文档中的信息可能会过时。
-   `--verbose-prompt`: 在生成文本之前打印提示词。
-   `--no-display-prompt`: 在生成时不打印提示词。
-   `-mg i, --main-gpu i`: 使用多个 GPU 时，此选项控制哪个 GPU 用于小张量，对于这些张量，跨所有 GPU 分割计算的开销不值得。该 GPU 将使用稍多的 VRAM 来存储临时结果的缓冲区。默认情况下使用 GPU 0。
-   `-ts SPLIT, --tensor-split SPLIT`: 使用多个设备时，此选项控制张量如何在设备之间拆分。`SPLIT` 是逗号分隔的非负值列表，按顺序分配每个设备应获得的数据比例。例如，"3,2" 将 60% 的数据分配给设备 0，40% 分配给设备 1。默认情况下，数据按 VRAM 比例拆分，但这可能不是最佳性能。正在使用的设备列表在启动时打印，并且可能与 `--list-devices` 提供的设备列表或例如 `nvidia-smi` 不同。
-   `-hfr URL --hf-repo URL`: Hugging Face 模型仓库的 url。与 `--hf-file` 或 `-hff` 结合使用。模型被下载并存储在 `-m` 或 `--model` 提供的文件中。如果未提供 `-m`，则模型自动存储在 `LLAMA_CACHE` 环境变量指定的路径或操作系统特定的本地缓存中。