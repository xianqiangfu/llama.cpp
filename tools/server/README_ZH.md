# LLaMA.cpp HTTP 服务器

基于 [httplib](https://github.com/yhirose/cpp-httplib)、[nlohmann::json](https://github.com/nlohmann/json) 和 **llama.cpp** 的快速、轻量级纯 C/C++ HTTP 服务器。

一组 LLM REST API 和 Web UI，用于与 llama.cpp 交互。

**功能特性：**
 * 在 GPU 和 CPU 上进行 F16 和量化模型的 LLM 推理
 * [OpenAI API](https://github.com/openai/openai-openapi) 兼容的聊天完成、响应和嵌入路由
 * [Anthropic Messages API](https://docs.anthropic.com/en/api/messages) 兼容的聊天完成
 * 重排序端点（https://github.com/ggml-org/llama.cpp/pull/9510）
 * 支持多用户并行解码
 * 连续批处理
 * 多模态（[文档](../../docs/multimodal.md)）/ 支持 OpenAI 兼容 API
 * 监控端点
 * 模式约束的 JSON 响应格式
 * 类似于 Claude API 的预填充助手消息
 * [函数调用](../../docs/function-calling.md) / 工具使用，适用于几乎所有模型
 * 投机解码
 * 易于使用的 Web UI

有关完整的功能列表，请参阅[服务器的更新日志](https://github.com/ggml-org/llama.cpp/issues/9291)

## 使用方法

<!-- HELP_START -->

<!-- 重要提示：下面的列表由 llama-gen-docs 自动生成；请勿手动修改 -->

### 通用参数

| 参数 | 说明 |
| -------- | ----------- |
| `-h, --help, --usage` | 打印使用信息并退出 |
| `--version` | 显示版本和构建信息 |
| `--license` | 显示源代码许可证和依赖项 |
| `-cl, --cache-list` | 显示缓存中的模型列表 |
| `--completion-bash` | 打印可用于 llama.cpp 的 bash 完成脚本 |
| `-t, --threads N` | 生成过程中使用的 CPU 线程数（默认：-1）<br/>(env: LLAMA_ARG_THREADS) |
| `-tb, --threads-batch N` | 批处理和提示处理期间使用的线程数（默认：与 --threads 相同） |
| `-C, --cpu-mask M` | CPU 亲和性掩码：任意长的十六进制。补充 cpu-range（默认：""） |
| `-Cr, --cpu-range lo-hi` | CPU 亲和性的范围。补充 --cpu-mask |
| `--cpu-strict <0\|1>` | 使用严格的 CPU 放置（默认：0） |
| `--prio N` | 设置进程/线程优先级：low(-1)、normal(0)、medium(1)、high(2)、realtime(3)（默认：0） |
| `--poll <0...100>` | 使用轮询级别等待工作（0 - 无轮询，默认：50） |
| `-Cb, --cpu-mask-batch M` | CPU 亲和性掩码：任意长的十六进制。补充 cpu-range-batch（默认：与 --cpu-mask 相同） |
| `-Crb, --cpu-range-batch lo-hi` | CPU 亲和性的范围。补充 --cpu-mask-batch |
| `--cpu-strict-batch <0\|1>` | 使用严格的 CPU 放置（默认：与 --cpu-strict 相同） |
| `--prio-batch N` | 设置进程/线程优先级：0-normal、1-medium、2-high、3-realtime（默认：0） |
| `--poll-batch <0\|1>` | 使用轮询等待工作（默认：与 --poll 相同） |
| `-c, --ctx-size N` | 提示上下文的大小（默认：0，0 = 从模型加载）<br/>(env: LLAMA_ARG_CTX_SIZE) |
| `-n, --predict, --n-predict N` | 要预测的 token 数量（默认：-1，-1 = 无限）<br/>(env: LLAMA_ARG_N_PREDICT) |
| `-b, --batch-size N` | 逻辑最大批处理大小（默认：2048）<br/>(env: LLAMA_ARG_BATCH) |
| `-ub, --ubatch-size N` | 物理最大批处理大小（默认：512）<br/>(env: LLAMA_ARG_UBATCH) |
| `--keep N` | 从初始提示中保留的 token 数量（默认：0，-1 = 全部） |
| `--swa-full` | 使用全大小的 SWA 缓存（默认：false）<br/>[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/13194#issuecomment-2868343055)<br/>(env: LLAMA_ARG_SWA_FULL) |
| `-fa, --flash-attn [on\|off\|auto]` | 设置 Flash Attention 使用（'on'、'off' 或 'auto'，默认：'auto'）<br/>(env: LLAMA_ARG_FLASH_ATTN) |
| `--perf, --no-perf` | 是否启用内部 libllama 性能计时（默认：false）<br/>(env: LLAMA_ARG_PERF) |
| `-e, --escape, --no-escape` | 是否处理转义序列（\n、\r、\t、\'、\"、\\）（默认：true） |
| `--rope-scaling {none,linear,yarn}` | RoPE 频率缩放方法，除非模型指定，否则默认为 linear<br/>(env: LLAMA_ARG_ROPE_SCALING_TYPE) |
| `--rope-scale N` | RoPE 上下文缩放因子，将上下文扩展 N 倍<br/>(env: LLAMA_ARG_ROPE_SCALE) |
| `--rope-freq-base N` | RoPE 基频，由 NTK 感知缩放使用（默认：从模型加载）<br/>(env: LLAMA_ARG_ROPE_FREQ_BASE) |
| `--rope-freq-scale N` | RoPE 频率缩放因子，将上下文扩展 1/N 倍<br/>(env: LLAMA_ARG_ROPE_FREQ_SCALE) |
| `--yarn-orig-ctx N` | YaRN：模型的原始上下文大小（默认：0 = 模型训练上下文大小）<br/>(env: LLAMA_ARG_YARN_ORIG_CTX) |
| `--yarn-ext-factor N` | YaRN：外推混合因子（默认：-1.00，0.0 = 完全插值）<br/>(env: LLAMA_ARG_YARN_EXT_FACTOR) |
| `--yarn-attn-factor N` | YaRN：缩放 sqrt(t) 或注意力幅度（默认：-1.00）<br/>(env: LLAMA_ARG_YARN_ATTN_FACTOR) |
| `--yarn-beta-slow N` | YaRN：高校正维度或 alpha（默认：-1.00）<br/>(env: LLAMA_ARG_YARN_BETA_SLOW) |
| `--yarn-beta-fast N` | YaRN：低校正维度或 beta（默认：-1.00）<br/>(env: LLAMA_ARG_YARN_BETA_FAST) |
| `-kvo, --kv-offload, -nkvo, --no-kv-offload` | 是否启用 KV 缓存卸载（默认：已启用）<br/>(env: LLAMA_ARG_KV_OFFLOAD) |
| `--repack, -nr, --no-repack` | 是否启用权重重新打包（默认：已启用）<br/>(env: LLAMA_ARG_REPACK) |
| `--no-host` | 绕过主机缓冲区，允许使用额外的缓冲区<br/>(env: LLAMA_ARG_NO_HOST) |
| `-ctk, --cache-type-k TYPE` | K 的 KV 缓存数据类型<br/>允许值：f32、f16、bf16、q8_0、q4_0、q4_1、iq4_nl、q5_0、q5_1<br/>(默认：f16)<br/>(env: LLAMA_ARG_CACHE_TYPE_K) |
| `-ctv, --cache-type-v TYPE` | V 的 KV 缓存数据类型<br/>允许值：f32、f16、bf16、q8_0、q4_0、q4_1、iq4_nl、q5_0、q5_1<br/>(默认：f16)<br/>(env: LLAMA_ARG_CACHE_TYPE_V) |
| `-dt, --defrag-thold N` | KV 缓存碎片整理阈值（已弃用）<br/>(env: LLAMA_ARG_DEFRAG_THOLD) |
| `--mlock` | 强制系统将模型保留在 RAM 中，而不是交换或压缩<br/>(env: LLAMA_ARG_MLOCK) |
| `--mmap, --no-mmap` | 是否对模型进行内存映射。（如果禁用 mmap，加载较慢，但如果不使用 mlock，可能会减少页面交换）（默认：已启用）<br/>(env: LLAMA_ARG_MMAP) |
| `-dio, --direct-io, -ndio, --no-direct-io` | 如果可用，使用 DirectIO。（默认：已禁用）<br/>(env: LLAMA_ARG_DIO) |
| `--numa TYPE` | 尝试对某些 NUMA 系统有帮助的优化<br/>- distribute：均匀分布在所有节点上<br/>- isolate：仅在执行开始的节点上的 CPU 上生成线程<br/>- numactl：使用 numactl 提供的 CPU 映射<br/>如果之前没有运行，建议在使用前清除系统页面缓存<br/>参见 https://github.com/ggml-org/llama.cpp/issues/1437<br/>(env: LLAMA_ARG_NUMA) |
| `-dev, --device <dev1,dev2,..>` | 用于卸载的设备的逗号分隔列表（none = 不卸载）<br/>使用 --list-devices 查看可用设备列表<br/>(env: LLAMA_ARG_DEVICE) |
| `--list-devices` | 打印可用设备列表并退出 |
| `-ot, --override-tensor <tensor name pattern>=<buffer type>,...` | 覆盖张量缓冲区类型<br/>(env: LLAMA_ARG_OVERRIDE_TENSOR) |
| `-cmoe, --cpu-moe` | 将所有混合专家（MoE）权重保留在 CPU 中<br/>(env: LLAMA_ARG_CPU_MOE) |
| `-ncmoe, --n-cpu-moe N` | 将前 N 层的混合专家（MoE）权重保留在 CPU 中<br/>(env: LLAMA_ARG_N_CPU_MOE) |
| `-ngl, --gpu-layers, --n-gpu-layers N` | 存储在 VRAM 中的最大层数，可以是精确数字、'auto' 或 'all'（默认：auto）<br/>(env: LLAMA_ARG_N_GPU_LAYERS) |
| `-sm, --split-mode {none,layer,row,tensor}` | 如何在多个 GPU 之间拆分模型，以下之一：<br/>- none：仅使用一个 GPU<br/>- layer（默认）：在 GPU 之间拆分层和 KV（流水线）<br/>- row：按行在 GPU 之间拆分权重（并行化）<br/>- tensor：在 GPU 之间拆分权重和 KV（并行化，实验性）<br/>(env: LLAMA_ARG_SPLIT_MODE) |
| `-ts, --tensor-split N0,N1,N2,...` | 卸载到每个 GPU 的模型分数，逗号分隔的比例列表，例如 3,1<br/>(env: LLAMA_ARG_TENSOR_SPLIT) |
| `-mg, --main-gpu INDEX` | 用于模型的 GPU（split-mode = none 时），或用于中间结果和 KV（split-mode = row 时）（默认：0）<br/>(env: LLAMA_ARG_MAIN_GPU) |
| `-fit, --fit [on\|off]` | 是否调整未设置的参数以适合设备内存（'on' 或 'off'，默认：'on'）<br/>(env: LLAMA_ARG_FIT) |
| `-fitt, --fit-target MiB0,MiB1,MiB2,...` | 每个设备的 --fit 目标边距，逗号分隔的值列表，单个值广播到所有设备，默认：1024<br/>(env: LLAMA_ARG_FIT_TARGET) |
| `-fitc, --fit-ctx N` | --fit 可以设置的最小 ctx 大小，默认：4096<br/>(env: LLAMA_ARG_FIT_CTX) |
| `--check-tensors` | 检查模型张量数据中的无效值（默认：false） |
| `--override-kv KEY=TYPE:VALUE,...` | 按键覆盖模型元数据的高级选项。要指定多个覆盖，请使用逗号分隔的值。<br/>类型：int、float、bool、str。示例：--override-kv tokenizer.ggml.add_bos_token=bool:false,tokenizer.ggml.add_eos_token=bool:false |
| `--op-offload, --no-op-offload` | 是否将主机张量操作卸载到设备（默认：true） |
| `--lora FNAME` | LoRA 适配器路径（使用逗号分隔的值加载多个适配器） |
| `--lora-scaled FNAME:SCALE,...` | 带有用户定义缩放的 LoRA 适配器路径（格式：FNAME:SCALE,...）<br/>注意：使用逗号分隔的值 |
| `--control-vector FNAME` | 添加控制向量<br/>注意：使用逗号分隔的值添加多个控制向量 |
| `--control-vector-scaled FNAME:SCALE,...` | 添加带有用户定义缩放 SCALE 的控制向量<br/>注意：使用逗号分隔的值（格式：FNAME:SCALE,...） |
| `--control-vector-layer-range START END` | 将控制向量应用到的层范围，开始和结束包括在内 |
| `-m, --model FNAME` | 要加载的模型路径<br/>(env: LLAMA_ARG_MODEL) |
| `-mu, --model-url MODEL_URL` | 模型下载 URL（默认：未使用）<br/>(env: LLAMA_ARG_MODEL_URL) |
| `-dr, --docker-repo [<repo>/]<model>[:quant]` | Docker Hub 模型仓库。repo 是可选的，默认为 ai/。quant 是可选的，默认为 :latest。<br/>示例：gemma3<br/>(默认：未使用）<br/>(env: LLAMA_ARG_DOCKER_REPO) |
| `-hf, -hfr, --hf-repo <user>/<model>[:quant]` | Hugging Face 模型仓库；quant 是可选的，不区分大小写，默认为 Q4_K_M，如果 Q4_K_M 不存在，则回退到仓库中的第一个文件。<br/>mmproj 也会自动下载（如果可用）。要禁用，请添加 --no-mmproj<br/>示例：ggml-org/GLM-4.7-Flash-GGUF:Q4_K_M<br/>(默认：未使用）<br/>(env: LLAMA_ARG_HF_REPO) |
| `-hff, --hf-file FILE` | Hugging Face 模型文件。如果指定，它将覆盖 --hf-repo 中的 quant（默认：未使用）<br/>(env: LLAMA_ARG_HF_FILE) |
| `-hfv, -hfrv, --hf-repo-v <user>/<model>[:quant]` | 声码器模型的 Hugging Face 模型仓库（默认：未使用）<br/>(env: LLAMA_ARG_HF_REPO_V) |
| `-hffv, --hf-file-v FILE` | 声码器模型的 Hugging Face 模型文件（默认：未使用）<br/>(env: LLAMA_ARG_HF_FILE_V) |
| `-hft, --hf-token TOKEN` | Hugging Face 访问令牌（默认：HF_TOKEN 环境变量的值）<br/>(env: HF_TOKEN) |
| `--log-disable` | 禁用日志 |
| `--log-file FNAME` | 记录到文件<br/>(env: LLAMA_LOG_FILE) |
| `--log-colors [on\|off\|auto]` | 设置彩色日志记录（'on'、'off' 或 'auto'，默认：'auto'）<br/>'auto' 在输出到终端时启用颜色<br/>(env: LLAMA_LOG_COLORS) |
| `-v, --verbose, --log-verbose` | 将详细级别设置为无穷大（即记录所有消息，对调试有用） |
| `--offline` | 离线模式：强制使用缓存，防止网络访问<br/>(env: LLAMA_OFFLINE) |
| `-lv, --verbosity, --log-verbosity N` | 设置详细阈值。具有更高详细性的消息将被忽略。值：<br/> - 0：通用输出<br/> - 1：错误<br/> - 2：警告<br/> - 3：信息<br/> - 4：调试<br/>(默认：3)<br/><br/>(env: LLAMA_LOG_VERBOSITY) |
| `--log-prefix, --no-log-prefix` | 在日志消息中启用前缀<br/>(env: LLAMA_ARG_LOG_PREFIX) |
| `--log-timestamps, --no-log-timestamps` | 在日志消息中启用时间戳<br/>(env: LLAMA_ARG_LOG_TIMESTAMPS) |
| `--spec-draft-type-k, -ctkd, --cache-type-k-draft TYPE` | K 的 KV 缓存数据类型（用于草稿模型）<br/>允许值：f32、f16、bf16、q8_0、q4_0、q4_1、iq4_nl、q5_0、q5_1<br/>(默认：f16)<br/>(env: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_K) |
| `--spec-draft-type-v, -ctvd, --cache-type-v-draft TYPE` | V 的 KV 缓存数据类型（用于草稿模型）<br/>允许值：f32、f16、bf16、q8_0、q4_0、q4_1、iq4_nl、q5_0、q5_1<br/>(默认：f16)<br/>(env: LLAMA_ARG_SPEC_DRAFT_CACHE_TYPE_V) |


### 采样参数

| 参数 | 说明 |
| -------- | ----------- |
| `--samplers SAMPLERS` | 将按顺序用于生成的采样器，用 ';' 分隔<br/>(默认：penalties;dry;top_n_sigma;top_k;typ_p;top_p;min_p;xtc;temperature) |
| `-s, --seed SEED` | RNG 种子（默认：-1，-1 使用随机种子） |
| `--sampler-seq, --sampling-seq SEQUENCE` | 将使用的采样器的简化序列（默认：edskypmxt） |
| `--ignore-eos` | 忽略流结束 token 并继续生成（意味着 --logit-bias EOS-inf） |
| `--temp, --temperature N` | 温度（默认：0.80） |
| `--top-k N` | top-k 采样（默认：40，0 = 禁用）<br/>(env: LLAMA_ARG_TOP_K) |
| `--top-p N` | top-p 采样（默认：0.95，1.0 = 禁用） |
| `--min-p N` | min-p 采样（默认：0.05，0.0 = 禁用） |
| `--top-nsigma, --top-n-sigma N` | top-n-sigma 采样（默认：-1.00，-1.0 = 禁用） |
| `--xtc-probability N` | xtc 概率（默认：0.00，0.0 = 禁用） |
| `--xtc-threshold N` | xtc 阈值（默认：0.10，1.0 = 禁用） |
| `--typical, --typical-p N` | 局部典型采样，参数 p（默认：1.00，1.0 = 禁用） |
| `--repeat-last-n N` | 考虑惩罚的最后 n 个 token（默认：64，0 = 禁用，-1 = ctx_size） |
| `--repeat-penalty N` | 惩罚重复的 token 序列（默认：1.00，1.0 = 禁用） |
| `--presence-penalty N` | 重复 alpha 存在惩罚（默认：0.00，0.0 = 禁用） |
| `--frequency-penalty N` | 重复 alpha 频率惩罚（默认：0.00，0.0 = 禁用） |
| `--dry-multiplier N` | 设置 DRY 采样乘数（默认：0.00，0.0 = 禁用） |
| `--dry-base N` | 设置 DRY 采样基准值（默认：1.75） |
| `--dry-allowed-length N` | 设置 DRY 采样的允许长度（默认：2） |
| `--dry-penalty-last-n N` | 设置最后 n 个 token 的 DRY 惩罚（默认：-1，0 = 禁用，-1 = 上下文大小） |
| `--dry-sequence-breaker STRING` | 为 DRY 采样添加序列中断符，在此过程中清除默认中断符（'\n'、':'、'"'、'*'）；使用 "none" 不使用任何序列中断符 |
| `--adaptive-target N` | adaptive-p：选择接近此概率的 token（有效范围 0.0 到 1.0；负数 = 禁用）（默认：-1.00）<br/>[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/17927) |
| `--adaptive-decay N` | adaptive-p：目标适应的衰减速率。较低的值更敏感，较高的值更稳定。<br/>（有效范围 0.0 到 0.99）（默认：0.90） |
| `--dynatemp-range N` | 动态温度范围（默认：0.00，0.0 = 禁用） |
| `--dynatemp-exp N` | 动态温度指数（默认：1.00） |
| `--mirostat N` | 使用 Mirostat 采样。<br/>如果使用，将忽略 Top K、Nucleus 和 Locally Typical 采样器。<br/>(默认：0，0 = 禁用，1 = Mirostat，2 = Mirostat 2.0) |
| `--mirostat-lr N` | Mirostat 学习率，参数 eta（默认：0.10） |
| `--mirostat-ent N` | Mirostat 目标熵，参数 tau（默认：5.00） |
| `-l, --logit-bias TOKEN_ID(+/-)BIAS` | 修改 token 在完成中出现的可能性，<br/>即 `--logit-bias 15043+1` 增加 token 'Hello' 的可能性，<br/>或 `--logit-bias 15043-1` 减少 token 'Hello' 的可能性 |
| `--grammar GRAMMAR` | BNF 类语法以约束生成（参见 grammars/ 目录中的示例） |
| `--grammar-file FNAME` | 从文件读取语法 |
| `-j, --json-schema SCHEMA` | JSON 模式以约束生成（https://json-schema.org/），例如 `{}` 表示任何 JSON 对象<br/>对于带有外部 $refs 的模式，请使用 --grammar + example/json_schema_to_grammar.py |
| `-jf, --json-schema-file FILE` | 包含 JSON 模式的文件以约束生成（https://json-schema.org/），例如 `{}` 表示任何 JSON 对象<br/>对于带有外部 $refs 的模式，请使用 --grammar + example/json_schema_to_grammar.py |
| `-bs, --backend-sampling` | 启用后端采样（实验性）（默认：禁用）<br/>(env: LLAMA_ARG_BACKEND_SAMPLING) |


### 服务器专用参数

| 参数 | 说明 |
| -------- | ----------- |
| `-lcs, --lookup-cache-static FNAME` | 用于查找解码的静态查找缓存路径（不通过生成更新） |
| `-lcd, --lookup-cache-dynamic FNAME` | 用于查找解码的动态查找缓存路径（通过生成更新） |
| `-ctxcp, --ctx-checkpoints, --swa-checkpoints N` | 每个插槽创建的最大上下文检查点数（默认：32）[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/15293)<br/>(env: LLAMA_ARG_CTX_CHECKPOINTS) |
| `-cpent, --checkpoint-every-n-tokens N` | 在预填充（处理）期间每 n 个 token 创建一个检查点，-1 禁用（默认：8192）<br/>(env: LLAMA_ARG_CHECKPOINT_EVERY_NT) |
| `-cram, --cache-ram N` | 以 MiB 为单位设置最大缓存大小（默认：8192，-1 - 无限制，0 - 禁用）[(更多信息)](https://github.com/ggml-org/llama.cpp/pull/16391)<br/>(env: LLAMA_ARG_CACHE_RAM) |
| `-kvu, --kv-unified, -no-kvu, --no-kv-unified` | 在所有序列之间使用单个统一的 KV 缓冲区（默认：如果插槽数量为 auto 则启用）<br/>(env: LLAMA_ARG_KV_UNIFIED) |
| `--cache-idle-slots, --no-cache-idle-slots` | 在新任务上保存并清除空闲插槽（默认：已启用，需要统一的 KV 和 cache-ram）<br/>(env: LLAMA_ARG_CACHE_IDLE_SLOTS) |
| `--context-shift, --no-context-shift` | 是否在无限文本生成中使用上下文移动（默认：禁用）<br/>(env: LLAMA_ARG_CONTEXT_SHIFT) |
| `-r, --reverse-prompt PROMPT` | 在 PROMPT 处停止生成，在交互模式下返回控制权 |
| `-sp, --special` | 特殊 token 输出已启用（默认：false） |
| `--warmup, --no-warmup` | 是否通过空运行执行预热（默认：已启用） |
| `--spm-infill` | 使用 Suffix/Prefix/Middle 模式进行填充（而不是 Prefix/Suffix/Middle），因为某些模型更喜欢这种方式。（默认：禁用） |
| `--pooling {none,mean,cls,last,rank}` | 嵌入的池化类型，如果未指定，使用模型默认值<br/>(env: LLAMA_ARG_POOLING) |
| `-np, --parallel N` | 服务器插槽数量（默认：-1，-1 = auto）<br/>(env: LLAMA_ARG_N_PARALLEL) |
| `-cb, --cont-batching, -nocb, --no-cont-batching` | 是否启用连续批处理（又称动态批处理）（默认：已启用）<br/>(env: LLAMA_ARG_CONT_BATCHING) |
| `-mm, --mmproj FILE` | 多模态投影器文件路径。参见 tools/mtmd/README.md<br/>注意：如果使用 -hf，则可以省略此参数<br/>(env: LLAMA_ARG_MMPROJ) |
| `-mmu, --mmproj-url URL` | 多模态投影器文件 URL。参见 tools/mtmd/README.md<br/>(env: LLAMA_ARG_MMPROJ_URL) |
| `--mmproj-auto, --no-mmproj, --no-mmproj-auto` | 是否使用多模态投影器文件（如果可用），在使用 -hf 时很有用（默认：已启用）<br/>(env: LLAMA_ARG_MMPROJ_AUTO) |
| `--mmproj-offload, --no-mmproj-offload` | 是否为多模态投影器启用 GPU 卸载（默认：已启用）<br/>(env: LLAMA_ARG_MMPROJ_OFFLOAD) |
| `--image-min-tokens N` | 每个图像可以采用的最小 token 数，仅由具有动态分辨率的视觉模型使用（默认：从模型读取）<br/>(env: LLAMA_ARG_IMAGE_MIN_TOKENS) |
| `--image-max-tokens N` | 每个图像可以采用的最大 token 数，仅由具有动态分辨率的视觉模型使用（默认：从模型读取）<br/>(env: LLAMA_ARG_IMAGE_MAX_TOKENS) |
| `-a, --alias STRING` | 设置模型名称别名，逗号分隔（由 API 使用）<br/>(env: LLAMA_ARG_ALIAS) |
| `--tags STRING` | 设置模型标签，逗号分隔（信息性，不用于路由）<br/>(env: LLAMA_ARG_TAGS) |
| `--embd-normalize N` | 嵌入的归一化（默认：2）（-1=none、0=max absolute int16、1=taxicab、2=euclidean、>2=p-norm） |
| `--host HOST` | 要侦听的 IP 地址，或绑定到 UNIX 套接字（如果地址以 .sock 结尾）（默认：127.0.0.1）<br/>(env: LLAMA_ARG_HOST) |
| `--port PORT` | 要侦听的端口（默认：8080）<br/>(env: LLAMA_ARG_PORT) |
| `--reuse-port` | 允许多个套接字绑定到同一端口（默认：禁用）<br/>(env: LLAMA_ARG_REUSE_PORT) |
| `--path PATH` | 从中提供静态文件的路径（默认：）<br/>(env: LLAMA_ARG_STATIC_PATH) |
| `--api-prefix PREFIX` | 服务器提供服务的前缀路径，不带尾部斜杠（默认：）<br/>(env: LLAMA_ARG_API_PREFIX) |
| `--webui-config JSON` | [已弃用：使用 --ui-config] JSON，提供默认的 WebUI 设置（覆盖 WebUI 默认值）<br/>(env: LLAMA_ARG_WEBUI_CONFIG) |
| `--ui-config JSON` | JSON，提供默认的 UI 设置（覆盖 UI 默认值）<br/>(env: LLAMA_ARG_UI_CONFIG) |
| `--webui-config-file PATH` | [已弃用：使用 --ui-config-file] JSON 文件，提供默认的 WebUI 设置（覆盖 WebUI 默认值）<br/>(env: LLAMA_ARG_WEBUI_CONFIG_FILE) |
| `--ui-config-file PATH` | JSON 文件，提供默认的 UI 设置（覆盖 UI 默认值）<br/>(env: LLAMA_ARG_UI_CONFIG_FILE) |
| `--webui-mcp-proxy, --no-webui-mcp-proxy` | [已弃用：使用 --ui-mcp-proxy/--no-ui-mcp-proxy] 实验性：是否启用 MCP CORS 代理<br/>(env: LLAMA_ARG_WEBUI_MCP_PROXY) |
| `--ui-mcp-proxy, --no-ui-mcp-proxy` | 实验性：是否启用 MCP CORS 代理 - 不要在不受信任的环境中启用（默认：禁用）<br/>(env: LLAMA_ARG_UI_MCP_PROXY) |
| `--tools TOOL1,TOOL2,...` | 实验性：是否为 AI 代理启用内置工具 - 不要在不受信任的环境中启用（默认：无工具）<br/>指定 "all" 以启用所有工具<br/>可用工具：read_file、file_glob_search、grep_search、exec_shell_command、write_file、edit_file、apply_diff、get_datetime<br/>(env: LLAMA_ARG_TOOLS) |
| `--webui, --no-webui` | [已弃用：使用 --ui/--no-ui] 是否启用 Web UI<br/>(env: LLAMA_ARG_WEBUI) |
| `--ui, --no-ui` | 是否启用 Web UI（默认：已启用）<br/>(env: LLAMA_ARG_UI) |
| `--embedding, --embeddings` | 仅限制支持嵌入用例；仅与专用嵌入模型一起使用（默认：禁用）<br/>(env: LLAMA_ARG_EMBEDDINGS) |
| `--rerank, --reranking` | 在服务器上启用重排序端点（默认：禁用）<br/>(env: LLAMA_ARG_RERANKING) |
| `--api-key KEY` | 用于身份验证的 API 密钥，可以提供多个密钥作为逗号分隔的列表（默认：无）<br/>(env: LLAMA_API_KEY) |
| `--api-key-file FNAME` | 包含 API 密钥的文件路径（默认：无） |
| `--ssl-key-file FNAME` | PEM 编码的 SSL 私钥文件路径<br/>(env: LLAMA_ARG_SSL_KEY_FILE) |
| `--ssl-cert-file FNAME` | PEM 编码的 SSL 证书文件路径<br/>(env: LLAMA_ARG_SSL_CERT_FILE) |
| `--chat-template-kwargs STRING` | 为 json 模板解析器设置附加参数，必须是有效的 json 对象字符串，例如 '{"key1":"value1","key2":"value2"}'<br/>(env: LLAMA_CHAT_TEMPLATE_KWARGS) |
| `-to, --timeout N` | 服务器读/写超时（秒）（默认：600）<br/>(env: LLAMA_ARG_TIMEOUT) |
| `--threads-http N` | 用于处理 HTTP 请求的线程数（默认：-1）<br/>(env: LLAMA_ARG_THREADS_HTTP) |
| `--cache-prompt, --no-cache-prompt` | 是否启用提示缓存（默认：已启用）<br/>(env: LLAMA_ARG_CACHE_PROMPT) |
| `--cache-reuse N` | 通过 KV 移位尝试从缓存重用的最小块大小，需要启用提示缓存（默认：0）<br/>[(卡片)](https://ggml.ai/f0.png)<br/>(env: LLAMA_ARG_CACHE_REUSE) |
| `--metrics` | 启用 prometheus 兼容的指标端点（默认：禁用）<br/>(env: LLAMA_ARG_ENDPOINT_METRICS) |
| `--props` | 启用通过 POST /props 更改全局属性（默认：禁用）<br/>(env: LLAMA_ARG_ENDPOINT_PROPS) |
| `--slots, --no-slots` | 暴露插槽监控端点（默认：已启用）<br/>(env: LLAMA_ARG_ENDPOINT_SLOTS) |
| `--slot-save-path PATH` | 保存插槽 KV 缓存的路径（默认：禁用） |
| `--media-path PATH` | 用于加载本地媒体文件的目录；文件可以通过带有相对路径的 file:// URL 访问（默认：禁用） |
| `--models-dir PATH` | 包含路由器服务器模型的目录（默认：禁用）<br/>(env: LLAMA_ARG_MODELS_DIR) |
| `--models-preset PATH` | INI 文件路径，包含路由器服务器的模型预设（默认：禁用）<br/>(env: LLAMA_ARG_MODELS_PRESET) |
| `--models-max N` | 对于路由器服务器，同时加载的最大模型数（默认：4，0 = 无限制）<br/>(env: LLAMA_ARG_MODELS_MAX) |
| `--models-autoload, --no-models-autoload` | 对于路由器服务器，是否自动加载模型（默认：已启用）<br/>(env: LLAMA_ARG_MODELS_AUTOLOAD) |
| `--jinja, --no-jinja` | 是否在聊天中使用 jinja 模板引擎（默认：已启用）<br/>(env: LLAMA_ARG_JINJA) |
| `--reasoning-format FORMAT` | 控制是否允许和/或从响应中提取思维标签，以及以哪种格式返回；以下之一：<br/>- none：将思维保留在 `message.content` 中未解析<br/>- deepseek：将思维放在 `message.reasoning_content` 中<br/>- deepseek-legacy：在 `message.content` 中保留 `

` 标签，同时填充 `message.reasoning_content`<br/>（默认：auto）<br/>(env: LLAMA_ARG_THINK) |
| `-rea, --reasoning [on\|off\|auto]` | 在聊天中使用推理/思考（'on'、'off' 或 'auto'，默认：'auto'（从模板检测））<br/>(env: LLAMA_ARG_REASONING) |
| `--reasoning-budget N` | 思考的 token 预算：-1 表示无限制，0 表示立即结束，N>0 表示 token 预算（默认：-1）<br/>(env: LLAMA_ARG_THINK_BUDGET) |
| `--reasoning-budget-message MESSAGE` | 当推理预算耗尽时在思维结束标签之前注入的消息（默认：无）<br/>(env: LLAMA_ARG_THINK_BUDGET_MESSAGE) |
| `--chat-template JINJA_TEMPLATE` | 设置自定义 jinja 聊天模板（默认：从模型的元数据中获取的模板）<br/>如果指定了后缀/前缀，模板将被禁用<br/>仅接受常用模板（除非在此标志之前设置了 --jinja）：<br/>内置模板列表：<br/>bailing、bailing-think、bailing2、chatglm3、chatglm4、chatml、command-r、deepseek、deepseek-ocr、deepseek2、deepseek3、exaone-moe、exaone3、exaone4、falcon3、gemma、gigachat、glmedge、gpt-oss、granite、granite-4.0、grok-2、hunyuan-dense、hunyuan-moe、hunyuan-vl、kimi-k2、llama2、llama2-sys、llama2-sys-bos、llama2-sys-strip、llama3、llama4、megrez、minicpm、mistral-v1、mistral-v3、mistral-v3-tekken、mistral-v7、mistral-v7-tekken、monarch、openchat、orion、pangu-embedded、phi3、phi4、rwkv-world、seed_oss、smolvlm、solar-open、vicuna、vicuna-orca、yandex、zephyr<br/>(env: LLAMA_ARG_CHAT_TEMPLATE) |
| `--chat-template-file JINJA_TEMPLATE_FILE` | 设置自定义 jinja 聊天模板文件（默认：从模型的元数据中获取的模板）<br/>如果指定了后缀/前缀，模板将被禁用<br/>仅接受常用模板（除非在此标志之前设置了 --jinja）：<br/>内置模板列表：<br/>bailing、bailing-think、bailing2、chatglm3、chatglm4、chatml、command-r、deepseek、deepseek-ocr、deepseek2、deepseek3、exaone-moe、exaone3、exaone4、falcon3、gemma、gigachat、glmedge、gpt-oss、granite、granite-4.0、grok-2、hunyuan-dense、hunyuan-moe、hunyuan-vl、kimi-k2、llama2、llama2-sys、llama2-sys-bos、llama2-sys-strip、llama3、llama4、megrez、minicpm、mistral-v1、mistral-v3、mistral-v3-tekken、mistral-v7、mistral-v7-tekken、monarch、openchat、orion、pangu-embedded、phi3、phi4、rwkv-world、seed_oss、smolvlm、solar-open、vicuna、vicuna-orca、yandex、zephyr<br/>(env: LLAMA_ARG_CHAT_TEMPLATE_FILE) |
| `--skip-chat-parsing, --no-skip-chat-parsing` | 即使指定了 Jinja 模板，也强制使用纯内容解析器；模型将在内容部分输出所有内容，包括任何推理和/或工具调用（默认：禁用）<br/>(env: LLAMA_ARG_SKIP_CHAT_PARSING) |
| `--prefill-assistant, --no-prefill-assistant` | 如果最后一条消息是助手消息，是否预填充助手的响应（默认：启用预填充）<br/>设置此标志后，如果最后一条消息是助手消息，则将其视为完整消息而不是预填充<br/><br/>(env: LLAMA_ARG_PREFILL_ASSISTANT) |
| `-sps, --slot-prompt-similarity SIMILARITY` | 请求的提示必须与插槽的提示匹配多少才能使用该插槽（默认：0.10，0.0 = 禁用） |
| `--lora-init-without-apply` | 加载 LoRA 适配器而不应用它们（稍后通过 POST /lora-adapters 应用）（默认：禁用） |
| `--sleep-idle-seconds SECONDS` | 服务器空闲后将休眠的秒数（默认：-1；-1 = 禁用） |
| `--spec-draft-hf, -hfd, -hfrd, --hf-repo-draft <user>/<model>[:quant]` | 与 --hf-repo 相同，但用于草稿模型（默认：未使用）<br/>(env: LLAMA_ARG_SPEC_DRAFT_HF_REPO) |
| `--spec-draft-threads, -td, --threads-draft N` | 生成过程中使用的线程数（默认：与 --threads 相同） |
| `--spec-draft-threads-batch, -tbd, --threads-batch-draft N` | 批处理和提示处理期间使用的线程数（默认：与 --threads-draft 相同） |
| `--spec-draft-cpu-mask, -Cd, --cpu-mask-draft M` | 草稿模型 CPU 亲和性掩码。补充 cpu-range-draft（默认：与 --cpu-mask 相同） |
| `--spec-draft-cpu-range, -Crd, --cpu-range-draft lo-hi` | CPU 亲和性的范围。补充 --cpu-mask-draft |
| `--spec-draft-cpu-strict, --cpu-strict-draft <0\|1>` | 对草稿模型使用严格的 CPU 放置（默认：与 --cpu-strict 相同） |
| `--spec-draft-prio, --prio-draft N` | 设置草稿进程/线程优先级：0-normal、1-medium、2-high、3-realtime（默认：0） |
| `--spec-draft-poll, --poll-draft <0\|1>` | 使用轮询等待草稿模型工作（默认：与 --poll 相同） |
| `--spec-draft-cpu-mask-batch, -Cbd, --cpu-mask-batch-draft M` | 草稿模型 CPU 亲和性掩码。补充 cpu-range-draft（默认：与 --cpu-mask 相同） |
| `--spec-draft-cpu-strict-batch, --cpu-strict-batch-draft <0\|1>` | 对草稿模型使用严格的 CPU 放置（默认：--cpu-strict-draft） |
| `--spec-draft-prio-batch, --prio-batch-draft N` | 设置草稿进程/线程优先级：0-normal、1-medium、2-high、3-realtime（默认：0） |
| `--spec-draft-poll-batch, --poll-batch-draft <0\|1>` | 使用轮询等待草稿模型工作（默认：--poll-draft） |
| `--spec-draft-override-tensor, -otd, --override-tensor-draft <tensor name pattern>=<buffer type>,...` | 为草稿模型覆盖张量缓冲区类型 |
| `--spec-draft-cpu-moe, -cmoed, --cpu-moe-draft` | 将草稿模型的所有混合专家（MoE）权重保留在 CPU 中<br/>(env: LLAMA_ARG_SPEC_DRAFT_CPU_MOE) |
| `--spec-draft-n-cpu-moe, --spec-draft-ncmoe, -ncmoed, --n-cpu-moe-draft N` | 将草稿模型前 N 层的混合专家（MoE）权重保留在 CPU 中<br/>(env: LLAMA_ARG_SPEC_DRAFT_N_CPU_MOE) |
| `--spec-draft-n-max N` | 用于投机解码的草稿 token 数量（默认：3）<br/>(env: LLAMA_ARG_N_GPU_LAYERS_DRAFT) |
| `--spec-draft-n-min N` | 用于投机解码的最小草稿 token 数量（默认：0）<br/>(env: LLAMA_ARG_N_GPU_LAYERS_DRAFT) |
| `--spec-draft-p-split, --draft-p-split P` | 投机解码拆分概率（默认：0.10）<br/>(env: LLAMA_ARG_DRAFT_MAX) |
| `--spec-draft-p-min, --draft-p-min P` | 最小投机解码概率（贪婪）（默认：0.00）<br/>(env: LLAMA_ARG_DRAFT_MIN) |
| `--spec-draft-device, -devd, --device-draft <dev1,dev2,..>` | 用于卸载草稿模型的设备的逗号分隔列表（none = 不卸载）<br/>使用 --list-devices 查看可用设备列表 |
| `--spec-draft-ngl, -ngld, --gpu-layers-draft, --n-gpu-layers-draft N` | 存储在 VRAM 中的草稿模型最大层数，可以是精确数字、'auto' 或 'all'（默认：auto）<br/>(env: LLAMA_ARG_N_GPU_LAYERS_DRAFT) |
| `--spec-draft-model, -md, --model-draft FNAME` | 用于投机解码的草稿模型（默认：未使用）<br/>(env: LLAMA_ARG_SPEC_DRAFT_MODEL) |
| `--spec-type none,draft-simple,draft-eagle3,draft-mtp,ngram-simple,ngram-map-k,ngram-map-k4v,ngram-mod,ngram-cache` | 逗号分隔的要使用的投机解码类型列表（默认：none）<br/><br/>(env: LLAMA_ARG_SPEC_TYPE) |
| `--spec-ngram-mod-n-min N` | 用于基于 ngram 的投机解码的最小 ngram token 数量（默认：48） |
| `--spec-ngram-mod-n-max N` | 用于基于 ngram 的投机解码的最大 ngram token 数量（默认：64） |
| `--spec-ngram-mod-n-match N` | ngram-mod 查找长度（默认：24） |
| `--spec-ngram-simple-size-n N` | ngram-simple 投机解码的 ngram 大小 N，查找 n-gram 的长度（默认：12） |
| `--spec-ngram-simple-size-m N` | ngram-simple 投机解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48） |
| `--spec-ngram-simple-min-hits N` | ngram-simple 投机解码的最小命中数（默认：1） |
| `--spec-ngram-map-k-size-n N` | ngram-map-k 投机解码的 ngram 大小 N，查找 n-gram 的长度（默认：12） |
| `--spec-ngram-map-k-size-m N` | ngram-map-k 投机解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48） |
| `--spec-ngram-map-k-min-hits N` | ngram-map-k 投机解码的最小命中数（默认：1） |
| `--spec-ngram-map-k4v-size-n N` | ngram-map-k4v 投机解码的 ngram 大小 N，查找 n-gram 的长度（默认：12） |
| `--spec-ngram-map-k4v-size-m N` | ngram-map-k4v 投机解码的 ngram 大小 M，草稿 m-gram 的长度（默认：48） |
| `--spec-ngram-map-k4v-min-hits N` | ngram-map-k4v 投机解码的最小命中数（默认：1） |
| `--draft, --draft-n, --draft-max N` | 该参数已被删除。使用 --spec-draft-n-max 或 --spec-ngram-mod-n-max<br/>(env: LLAMA_ARG_DRAFT_MAX) |
| `--draft-min, --draft-n-min N` | 该参数已被删除。使用 --spec-draft-n-min 或 --spec-ngram-mod-n-min<br/>(env: LLAMA_ARG_DRAFT_MIN) |
| `--spec-ngram-size-n N` | 该参数已被删除。使用相应的 --spec-ngram-*-size-n 或 --spec-ngram-mod-n-match |
| `--spec-ngram-size-m N` | 该参数已被删除。使用相应的 --spec-ngram-*-size-m |
| `--spec-ngram-min-hits N` | 该参数已被删除。使用相应的 --spec-ngram-*-min-hits |
| `-mv, --model-vocoder FNAME` | 用于音频生成的声码器模型（默认：未使用） |
| `--tts-use-guide-tokens` | 使用引导 token 来改善 TTS 单词召回率 |
| `--embd-gemma-default` | 使用默认的 EmbeddingGemma 模型（注意：可以从互联网下载权重） |
| `--fim-qwen-1.5b-default` | 使用默认的 Qwen 2.5 Coder 1.5B（注意：可以从互联网下载权重） |
| `--fim-qwen-3b-default` | 使用默认的 Qwen 2.5 Coder 3B（注意：可以从互联网下载权重） |
| `--fim-qwen-7b-default` | 使用默认的 Qwen 2.5 Coder 7B（注意：可以从互联网下载权重） |
| `--fim-qwen-7b-spec` | 使用 Qwen 2.5 Coder 7B + 0.5B 草稿进行投机解码（注意：可以从互联网下载权重） |
| `--fim-qwen-14b-spec` | 使用 Qwen 2.5 Coder 14B + 0.5B 草稿进行投机解码（注意：可以从互联网下载权重） |
| `--fim-qwen-30b-default` | 使用默认的 Qwen 3 Coder 30B A3B Instruct（注意：可以从互联网下载权重） |
| `--gpt-oss-20b-default` | 使用 gpt-oss-20b（注意：可以从互联网下载权重） |
| `--gpt-oss-120b-default` | 使用 gpt-oss-120b（注意：可以从互联网下载权重） |
| `--vision-gemma-4b-default` | 使用 Gemma 3 4B QAT（注意：可以从互联网下载权重） |
| `--vision-gemma-12b-default` | 使用 Gemma 3 12B QAT（注意：可以从互联网下载权重） |
| `--spec-default` | 启用默认投机解码配置 |

<!-- HELP_END -->

注意：如果同时为同一参数设置了命令行参数和环境变量，命令行参数将优先于环境变量。

对于 `--mmap` 或 `--kv-offload` 等布尔选项，环境变量的处理方式如下例所示：
- `LLAMA_ARG_MMAP=true` 表示已启用，其他接受的值有：`1`、`on`、`enabled`
- `LLAMA_ARG_MMAP=false` 表示已禁用，其他接受的值有：`0`、`off`、`disabled`
- 如果存在 `LLAMA_ARG_NO_MMAP`（无论值如何），则意味着禁用 mmap

使用环境变量的 docker compose 示例用法：

```yml
services:
  llamacpp-server:
    image: ghcr.io/ggml-org/llama.cpp:server
    ports:
      - 8080:8080
    volumes:
      - ./models:/models
    environment:
      # 或者，您可以使用 "LLAMA_ARG_MODEL_URL" 来下载模型
      LLAMA_ARG_MODEL: /models/my_model.gguf
      LLAMA_ARG_CTX_SIZE: 4096
      LLAMA_ARG_N_PARALLEL: 2
      LLAMA_ARG_ENDPOINT_METRICS: 1
      LLAMA_ARG_PORT: 8080
```

### 多模态支持

多模态支持已在 [#12898](https://github.com/ggml-org/llama.cpp/pull/12898) 中添加，目前是实验性功能。
它目前在以下端点中可用：
- OAI 兼容的聊天端点。
- 非 OAI 兼容的完成端点。
- 非 OAI 兼容的嵌入端点。

有关更多详细信息，请参阅[多模态文档](../../docs/multimodal.md)

### 内置工具支持

服务器包含一组内置工具，使 LLM 可以直接从 Web UI 访问本地文件系统。

要使用此功能，请使用 `--tools all` 启动服务器。您还可以通过传递逗号分隔列表仅启用特定工具：`--tools name1,name2,...`。运行 `--help` 查看可用工具名称的完整列表。

## 构建

`llama-server` 与所有其他内容一起从项目根目录构建

- 使用 `CMake`：

  ```bash
  cmake -B build
  cmake --build build --config Release -t llama-server
  ```

  二进制文件位于 `./build/bin/llama-server`

## 使用 SSL 构建

`llama-server` 也可以使用 OpenSSL 3 构建 SSL 支持

- 使用 `CMake`：

  ```bash
  cmake -B build -DLLAMA_OPENSSL=ON
  cmake --build build --config Release -t llama-server
  ```

## 快速开始

要立即开始，请运行以下命令，确保为您拥有的模型使用正确的路径：

### 基于 Unix 的系统（Linux、macOS 等）

```bash
./llama-server -m models/7B/ggml-model.gguf -c 2048
```

### Windows

```powershell
llama-server.exe -m models\7B\ggml-model.gguf -c 2048
```

上述命令将启动一个默认侦听 `127.0.0.1:8080` 的服务器。
您可以使用 Postman 或带有 axios 库的 NodeJS 使用端点。您可以在同一 url 访问 Web 前端。

### Docker

```bash
docker run -p 8080:8080 -v /path/to/models:/models ghcr.io/ggml-org/llama.cpp:server -m models/7B/ggml-model.gguf -c 512 --host 0.0.0.0 --port 8080

# 或者，使用 CUDA：
docker run -p 8080:8080 -v /path/to/models:/models --gpus all ghcr.io/ggml-org/llama.cpp:server-cuda -m models/7B/ggml-model.gguf -c 512 --host 0.0.0.0 --port 8080 --n-gpu-layers 99
```

## 使用 CURL

使用 [curl](https://curl.se/)。在 Windows 上，`curl.exe` 应该在基本操作系统中可用。

```sh
curl --request POST \
    --url http://localhost:8080/completion \
    --header "Content-Type: application/json" \
    --data '{"prompt": "Building a website can be done in 10 simple steps:","n_predict": 128}'
```

## API 端点

### GET `/health`：返回健康检查结果

此端点是公开的（无 API 密钥检查）。`/v1/health` 也有效。

**响应格式**

- HTTP 状态代码 503
  - 主体：`{"error": {"code": 503, "message": "Loading model", "type": "unavailable_error"}}`
  - 说明：模型仍在加载中。
- HTTP 状态代码 200
  - 主体：`{"status": "ok" }`
  - 说明：模型已成功加载，服务器已准备就绪。

### POST `/completion`：给定 `prompt`，返回预测的完成内容。

> [!IMPORTANT]
>
> 此端点**不**是 OAI 兼容的。对于 OAI 兼容客户端，请改用 `/v1/completions`。

*选项：*

`prompt`：为此完成提供提示，作为字符串或表示 token 的字符串或数字数组。在内部，如果 `cache_prompt` 为 `true`，则将提示与之前的完成进行比较，仅评估"未见"后缀。如果以下所有条件都为真，则在开始处插入 `BOS` token：

  - 提示是字符串或第一个元素作为字符串提供的数组
  - 模型的 `tokenizer.ggml.add_bos_token` 元数据为 `true`

为 `prompt` 允许这些输入形状和数据类型：

  - 单个字符串：`"string"`
  - 单个 token 序列：`[12, 34, 56]`
  - 混合 token 和字符串：`[12, 34, "string", 56, 78]`
  - 可选包含多模态数据的 JSON 对象：`{ "prompt_string": "string", "multimodal_data": ["base64"] }`

还支持多个提示。在这种情况下，完成结果将是一个数组。

  - 仅字符串：`["string1", "string2"]`
  - 字符串、JSON 对象和 token 序列：`["string1", [12, 34, 56], { "prompt_string": "string", "multimodal_data": ["base64"]}]`
  - 混合类型：`[[12, 34, "string", 56, 78], [12, 34, 56], "string", { "prompt_string": "string" }]`

注意 JSON 对象提示中的 `multimodal_data`。这应该是一个字符串数组，包含 base64 编码的多模态数据（如图像和音频）。字符串提示元素中必须有相同数量的 MTMD 媒体标记，它们充当提供到此参数的数据的占位符。多模态数据文件将按顺序替换。标记字符串（例如 `<__media__>`）可以通过调用 [MTMD C API](https://github.com/ggml-org/llama.cpp/blob/5fd160bbd9d70b94b5b11b0001fd7f477005e4a0/tools/mtmd/mtmd.h#L87) 中定义的 `mtmd_default_marker()` 找到。客户端*必须*指定此字段，除非服务器具有多模态能力。客户端应检查 `/models` 或 `/v1/models` 中的 `multimodal` 能力，然后再发出多模态请求。

`temperature`：调整生成文本的随机性。默认：`0.8`

`dynatemp_range`：动态温度范围。最终温度将在 `[temperature - dynatemp_range; temperature + dynatemp_range]` 范围内。默认：`0.0`，这表示禁用。

`dynatemp_exponent`：动态温度指数。默认：`1.0`

`top_k`：将下一个 token 选择限制为 K 个最可能的 token。默认：`40`

`top_p`：将下一个 token 选择限制为累积概率高于阈值 P 的 token 子集。默认：`0.95`

`min_p`：要考虑的 token 的最小概率，相对于最可能 token 的概率。默认：`0.05`

`n_predict`：设置生成文本时要预测的最大 token 数量。**注意：**如果最后一个 token 是部分多字节字符，可能会稍微超过设置的限制。当为 0 时，不会生成任何 token，但会将提示评估到缓存中。默认：`-1`，其中 `-1` 是无穷大。

`n_indent`：指定生成文本的最小行缩进，以空白字符数为单位。对于代码完成任务很有用。默认：`0`

`n_keep`：指定当上下文大小超出且需要丢弃 token 时，从提示中保留的 token 数量。该数字不包括 BOS token。
默认情况下，此值设置为 `0`，表示不保留任何 token。使用 `-1` 保留提示中的所有 token。

`n_cmpl`：从当前提示生成的完成数量。如果输入有多个提示，输出将有 N 个提示乘以 `n_cmpl` 个条目。

`n_cache_reuse`：通过 KV 移位尝试从缓存重用的最小块大小。有关更多信息，请参阅 `--cache-reuse` 参数。默认：`0`，这表示禁用。

`stream`：允许实时接收每个预测的 token，而不是等待完成完成（使用不同的响应格式）。要启用此功能，请设置为 `true`。

`stop`：指定停止字符串的 JSON 数组。
这些单词将不包含在完成中，因此请确保将它们添加到下一次迭代的提示中。默认：`[]`

`typical_p`：启用参数 p 的局部典型采样。默认：`1.0`，这表示禁用。

`repeat_penalty`：控制生成文本中 token 序列的重复。默认：`1.1`

`repeat_last_n`：考虑惩罚的最后 n 个 token。默认：`64`，其中 `0` 表示禁用，`-1` 表示 ctx-size。

`presence_penalty`：重复 alpha 存在惩罚。默认：`0.0`，这表示禁用。

`frequency_penalty`：重复 alpha 频率惩罚。默认：`0.0`，这表示禁用。

`dry_multiplier`：设置 DRY（不要重复自己）重复惩罚乘数。默认：`0.0`，这表示禁用。

`dry_base`：设置 DRY 重复惩罚基准值。默认：`1.75`

`dry_allowed_length`：将重复扩展到此之外的 token 接收指数级增加的惩罚：multiplier * base ^ (repeat sequence length before token - allowed length)。默认：`2`

`dry_penalty_last_n`：扫描重复的 token 数量。默认：`-1`，其中 `0` 表示禁用，`-1` 表示上下文大小。

`dry_sequence_breakers`：为 DRY 采样指定序列中断符的数组。仅接受 JSON 字符串数组。默认：`['\n', ':', '"', '*']`

`xtc_probability`：设置通过 XTC 采样器移除 token 的机会。默认：`0.0`，这表示禁用。

`xtc_threshold`：设置通过 XTC 采样器移除 token 的最小概率阈值。默认：`0.1`（> `0.5` 禁用 XTC）

`mirostat`：启用 Mirostat 采样，控制文本生成期间的困惑度。默认：`0`，其中 `0` 表示禁用，`1` 表示 Mirostat，`2` 表示 Mirostat 2.0。

`mirostat_tau`：设置 Mirostat 目标熵，参数 tau。默认：`5.0`

`mirostat_eta`：设置 Mirostat 学习率，参数 eta。默认：`0.1`

`grammar`：设置基于语法的采样的语法。默认：无语法

`json_schema`：设置基于语法采样的 JSON 模式（例如 `{"items": {"type": "string"}, "minItems": 10, "maxItems": 100}` 的字符串列表，或 `{}` 表示任何 JSON）。有关支持的功能，请参阅[测试](../../tests/test-json-schema-to-grammar.cpp)。默认：无 JSON 模式。

`seed`：设置随机数生成器（RNG）种子。默认：`-1`，这是随机种子。

`ignore_eos`：忽略流结束 token 并继续生成。默认：`false`

`logit_bias`：修改 token 在生成的文本完成中出现的可能性。例如，使用 `"logit_bias": [[15043,1.0]]` 增加 token 'Hello' 的可能性，或使用 `"logit_bias": [[15043,-1.0]]` 降低其可能性。将值设置为 false，`"logit_bias": [[15043,false]]` 确保 token `Hello` 从未生成。token 也可以表示为字符串，例如 `[["Hello, World!",-0.5]]` 将降低表示字符串 `Hello, World!` 的所有单个 token 的可能性，就像 `presence_penalty` 一样。为了与 OpenAI API 兼容，也可以传递 JSON 对象 {"<string or token id>": bias, ...}。默认：`[]`

`n_probs`：如果大于 0，响应还包含根据采样设置为每个生成的 token 的前 N 个 token 的概率。请注意，对于 temperature < 0，token 是贪婪采样的，但 token 概率仍然通过不带任何其他采样器设置的 logits 的简单 softmax 计算。默认：`0`

`min_keep`：如果大于 0，强制采样器返回至少 N 个可能的 token。默认：`0`

`t_max_predict_ms`：设置预测（又称文本生成）阶段的时间限制（毫秒）。如果生成时间超过指定时间（自从生成第一个 token 以来测量）并且已经生成了换行符，则超时将被触发。对于 FIM 应用程序很有用。默认：`0`，这表示禁用。

`id_slot`：将完成任务分配给特定插槽。如果为 -1，则任务将分配给空闲插槽。默认：`-1`

`cache_prompt`：如果可能，从上一个请求重用 KV 缓存。这样，公共前缀不必重新处理，只有请求之间不同的后缀需要重新处理。因为（取决于后端）logit **不**保证对于不同批处理大小（提示处理与 token 生成）是逐位相同的，启用此选项可能导致非确定性结果。默认：`true`

`return_tokens`：在 `tokens` 字段中返回原始生成的 token ID。否则 `tokens` 保持为空。默认：`false`

`samplers`：应按顺序应用的采样器顺序。表示采样器类型名称的字符串数组。如果未设置采样器，则不会使用它。如果多次指定采样器，则将多次应用它。默认：`["dry", "top_k", "typ_p", "top_p", "min_p", "xtc", "temperature"]` - 这些都是可用的值。

`timings_per_token`：在每个响应中包含提示处理和文本生成速度信息。默认：`false`

`return_progress`：在 `stream` 模式中包含提示处理进度。进度将包含在 `prompt_progress` 内，具有 4 个值：`total`、`cache`、`processed` 和 `time_ms`。总体进度为 `processed/total`，而实际计时的进度为 `(processed-cache)/(total-cache)`。`time_ms` 字段包含自提示处理开始以来经过的毫秒数。默认：`false`

`post_sampling_probs`：返回前 `n_probs` 个 token 在应用采样链后的概率。

`response_fields`：响应字段列表，例如：`"response_fields": ["content", "generation_settings/n_predict"]`。如果指定的字段缺失，则只需从响应中省略它，而不会触发错误。请注意，带有斜杠的字段将取消嵌套；例如，`generation_settings/n_predict` 将字段 `n_predict` 从 `generation_settings` 对象移动到响应的根部，并给它一个新名称。

`lora`：要应用于此特定请求的 LoRA 适配器列表。列表中的每个对象必须包含 `id` 和 `scale` 字段。例如：`[{"id": 0, "scale": 0.5}, {"id": 1, "scale": 1.1}]`。如果 LoRA 适配器未在列表中指定，则其比例将默认为 `0.0`。请注意，具有不同 LoRA 配置的请求将不会一起批处理，这可能会导致性能下降。

**响应格式**

- 注意：在流模式（`stream`）中，仅返回 `content`、`tokens` 和 `stop`，直到完成完成。响应使用 [Server-sent events](https://html.spec.whatwg.org/multipage/server-sent-events.html) 标准发送。注意：由于缺乏 `POST` 请求支持，无法使用浏览器的 `EventSource` 接口。

- `completion_probabilities`：每个完成的 token 概率数组。数组长度为 `n_predict`。数组中的每个项目都有一个嵌套数组 `top_logprobs`。它最多包含 **`n_probs`** 个元素：
  ```
  {
    "content": "<the generated completion text>",
    "tokens": [ generated token ids if requested ],
    ...
    "probs": [
      {
        "id": <token id>,
        "logprob": float,
        "token": "<most likely token>",
        "bytes": [int, int, ...],
        "top_logprobs": [
          {
            "id": <token id>,
            "logprob": float,
            "token": "<token text>",
            "bytes": [int, int, ...],
          },
          {
            "id": <token id>,
            "logprob": float,
            "token": "<token text>",
            "bytes": [int, int, ...],
          },
          ...
        ]
      },
      {
        "id": <token id>,
        "logprob": float,
        "token": "<most likely token>",
        "bytes": [int, int, ...],
        "top_logprobs": [
          ...
        ]
      },
      ...
    ]
  },
  ```
  请注意，如果 `post_sampling_probs` 设置为 `true`：
    - `logprob` 将替换为 `prob`，值在 0.0 到 1.0 之间
    - `top_logprobs` 将替换为 `top_probs`。每个元素包含：
      - `id`：token ID
      - `token`：字符串形式的 token
      - `bytes`：字节形式的 token
      - `prob`：token 概率，值在 0.0 到 1.0 之间
    - `top_probs` 中的元素数量可能少于 `n_probs`

- `content`：完成结果作为字符串（如果有的话排除 `stopping_word`）。在流模式的情况下，将包含下一个 token 作为字符串。
- `tokens`：与 `content` 相同，但表示为原始 token ID。仅当请求了 `"return_tokens": true` 或 `"stream": true` 时才填充。
- `stop`：布尔值，用于在流模式下检查生成是否已停止（注意：这与输入选项中的停止单词数组 `stop` 无关）
- `generation_settings`：提供的上述选项，不包括 `prompt`，但包括 `n_ctx`、`model`。这些选项可能与原始选项不同（例如，过滤掉错误的值，字符串转换为 token 等）。
- `model`：模型别名（对于模型路径，请使用 `/props` 端点）
- `prompt`：处理的 `prompt`（可能添加了特殊 token）
- `stop_type`：指示完成是否已停止。可能的值：
  - `none`：生成中（未停止）
  - `eos`：由于遇到 EOS token 而停止
  - `limit`：在遇到停止词或 EOS 之前生成了 `n_predict` 个 token 而停止
  - `word`：由于遇到来自 `stop` JSON 数组提供的停止词而停止
- `stopping_word`：导致生成停止的停止词（如果未因停止词停止，则为 ""）
- `timings`：有关完成的计时信息的哈希，例如 `predicted_per_second` token 数量
- `tokens_cached`：可以从上一个完成重用的提示 token 数量
- `tokens_evaluated`：从提示中评估的总 token 数量
- `truncated`：布尔值，指示在生成过程中是否超出上下文大小，即提供的提示中的 token 数量（`tokens_evaluated`）加上生成的 token（`tokens predicted`）超过了上下文大小（`n_ctx`）


### POST `/tokenize`：对给定文本进行分词

*选项：*

`content`：（必需）要分词的文本。

`add_special`：（可选）布尔值，指示是否应插入特殊 token，即 `BOS`。默认：`false`

`parse_special`：（可选）布尔值，指示是否应分词特殊 token。当 `false` 时，特殊 token 被视为纯文本。默认：`true`

`with_pieces`：（可选）布尔值，指示是否随 ID 一起返回 token 片段。默认：`false`

**响应：**

返回一个 JSON 对象，其中包含 `tokens` 字段，其中包含分词结果。`tokens` 数组仅包含 token ID 或根据 `with_pieces` 参数包含 `id` 和 `piece` 字段的对象。如果片段是有效的 unicode，则片段字段为字符串，否则为字节列表。


如果 `with_pieces` 为 `false`：
```json
{
  "tokens": [123, 456, 789]
}
```

如果 `with_pieces` 为 `true`：
```json
{
  "tokens": [
    {"id": 123, "piece": "Hello"},
    {"id": 456, "piece": " world"},
    {"id": 789, "piece": "!"}
  ]
}
```

在 tinyllama/stories260k 上输入 'á'（utf8 十六进制：C3 A1）
```
{
  "tokens": [
    {"id": 198, "piece": [195]}, // 十六进制 C3
    {"id": 164, "piece": [161]} // 十六进制 A1
  ]
}
```

### POST `/detokenize`：将 token 转换为文本

*选项：*

`tokens`：设置要分词的 token。

### POST `/apply-template`：将聊天模板应用于对话

使用服务器的提示模板格式化功能将聊天消息转换为聊天模型期望的单个字符串，但不执行推理。相反，提示字符串将在 JSON 响应的 `prompt` 字段中返回。然后可以根据需要修改提示（例如，在开始插入 "Sure!"），然后发送到 `/completion` 以生成聊天响应。

*选项：*

`messages`：（必需）与 `/v1/chat/completions` 格式相同的聊天轮次。

**响应格式**

返回一个 JSON 对象，其中包含字段 `prompt`，其中包含根据模型的聊天模板格式格式化的输入消息字符串。

### POST `/embedding`：生成给定文本的嵌入

> [!IMPORTANT]
>
> 此端点**不**是 OAI 兼容的。对于 OAI 兼容客户端，请改用 `/v1/embeddings`。

与[嵌入示例](../embedding)所做的相同。

此端点还支持多模态嵌入。有关如何发送多模态提示的详细信息，请参阅 `/completions` 端点的文档。

*选项：*

`content`：设置要处理的文本。

`embd_normalize`：池化嵌入的归一化。可以是以下值之一：
```
  -1: 无归一化
   0: 最大绝对值
   1: 曼哈顿距离
   2: 欧几里得/L2
  >2: P-范数
```

### POST `/reranking`：根据给定的查询重排序文档

类似于 https://jina.ai/reranker/，但将来可能会更改。
需要重排序器模型（例如 [bge-reranker-v2-m3](https://huggingface.co/BAAI/bge-reranker-v2-m3)）和 `--embedding --pooling rank` 选项。

*选项：*

`query`：文档将根据其进行排序的查询。

`documents`：表示要排序的文档的字符串数组。

*别名：*
  - `/rerank`
  - `/v1/rerank`
  - `/v1/reranking`

*示例：*

```shell
curl http://127.0.0.1:8012/v1/rerank \
    -H "Content-Type: application/json" \
    -d '{
        "model": "some-model",
            "query": "What is panda?",
            "top_n": 3,
            "documents": [
                "hi",
            "it is a bear",
            "The giant panda (Ailuropoda melanoleuca), sometimes called a panda bear or simply panda, is a bear species endemic to China."
            ]
    }' | jq
```

### POST `/infill`：用于代码填充。

接受前缀和后缀，并将预测的完成作为流返回。

*选项：*

- `input_prefix`：设置要填充的代码前缀。
- `input_suffix`：设置要填充的代码后缀。
- `input_extra`：在 FIM 前缀之前插入的附加上下文。
- `prompt`：在 `FIM_MID` token 之后添加

`input_extra` 是 `{"filename": string, "text": string}` 对象的数组。

端点还接受 `/completion` 的所有选项。

如果模型具有 `FIM_REPO` 和 `FIM_FILE_SEP` token，则使用[仓库级模式](https://arxiv.org/pdf/2409.12186)：

```txt
<FIM_REP>myproject
<FIM_SEP>{chunk 0 filename}
{chunk 0 text}
<FIM_SEP>{chunk 1 filename}
{chunk 1 text}
...
<FIM_SEP>filename
<FIM_PRE>[input_prefix]<FIM_SUF>[input_suffix]<FIM_MID>[prompt]
```

如果缺少 token，则附加上下文只是简单地附加在开头：

```txt
[input_extra]<FIM_PRE>[input_prefix]<FIM_SUF>[input_suffix]<FIM_MID>[prompt]
```

### **GET** `/props`：获取服务器全局属性。

默认情况下，它是只读的。要使用 POST 请求更改全局属性，您需要使用 `--props` 启动服务器

**响应格式**

```json
{
  "default_generation_settings": {
    "id": 0,
    "id_task": -1,
    "n_ctx": 1024,
    "speculative": false,
    "is_processing": false,
    "params": {
      "n_predict": -1,
      "seed": 4294967295,
      "temperature": 0.800000011920929,
      "dynatemp_range": 0.0,
      "dynatemp_exponent": 1.0,
      "top_k": 40,
      "top_p": 0.949999988079071,
      "min_p": 0.05000000074505806,
      "xtc_probability": 0.0,
      "xtc_threshold": 0.10000000149011612,
      "typical_p": 1.0,
      "repeat_last_n": 64,
      "repeat_penalty": 1.0,
      "presence_penalty": 0.0,
      "frequency_penalty": 0.0,
      "dry_multiplier": 0.0,
      "dry_base": 1.75,
      "dry_allowed_length": 2,
      "dry_penalty_last_n": -1,
      "dry_sequence_breakers": [
        "\n",
        ":",
        "\"",
        "*"
      ],
      "mirostat": 0,
      "mirostat_tau": 5.0,
      "mirostat_eta": 0.10000000149011612,
      "stop": [],
      "max_tokens": -1,
      "n_keep": 0,
      "n_discard": 0,
      "ignore_eos": false,
      "stream": true,
      "n_probs": 0,
      "min_keep": 0,
      "grammar": "",
      "samplers": [
        "dry",
        "top_k",
        "typ_p",
        "top_p",
        "min_p",
        "xtc",
        "temperature"
      ],
      "speculative.n_max": 16,
      "speculative.n_min": 5,
      "speculative.p_min": 0.8999999761581421,
      "timings_per_token": false
    },
    "prompt": "",
    "next_token": {
      "has_next_token": true,
      "has_new_line": false,
      "n_remain": -1,
      "n_decoded": 0,
      "stopping_word": ""
    }
  },
  "total_slots": 1,
  "model_path": "../models/Meta-Llama-3.1-8B-Instruct-Q4_K_M.gguf",
  "chat_template": "...",
  "chat_template_caps": {},
  "modalities": {
    "vision": false
  },
  "media_marker": "<__media_YoNhud46VdDqbuFmKYEO9PY7A4ARzRfg__>",
  "build_info": "b(build number)-(build commit hash)",
  "is_sleeping": false
}
```

- `default_generation_settings` - `/completion` 端点的默认生成设置，与 `/completion` 端点的 `generation_settings` 响应对象具有相同的字段。
- `total_slots` - 处理请求的插槽数量（由 `--parallel` 选项定义）
- `model_path` - 模型文件路径（与 `-m` 参数相同）
- `chat_template` - 模型的原始 Jinja2 提示模板
- `chat_template_caps` - 聊天模板的能力（有关更多信息，请参阅 `common/jinja/caps.h`）
- `modalities` - 支持的模态列表
- `is_sleeping` - 休眠状态，请参阅[空闲时休眠](#sleeping-on-idle)

### POST `/props`：更改服务器全局属性。

要使用 POST 方法使用此端点，您需要使用 `--props` 启动服务器

*选项：*

- 尚无

### POST `/embeddings`：非 OpenAI 兼容的嵌入 API

此端点支持所有池化，包括 `--pooling none`。当池化为 `none` 时，响应将包含所有输入 token 的*未归一化*嵌入。对于所有其他池化类型，仅返回池化的嵌入，并使用欧几里得范数进行归一化。

请注意，此端点的响应格式与 `/v1/embeddings` 不同。

*选项：*

与 `/v1/embeddings` 端点相同。

*示例：*

与 `/v1/embeddings` 端点相同。

**响应格式**

```
[
  {
    "index": 0,
    "embedding": [
      [ ... embeddings for token 0   ... ],
      [ ... embeddings for token 1   ... ],
      [ ... ]
      [ ... embeddings for token N-1 ... ],
    ]
  },
  ...
  {
    "index": P,
    "embedding": [
      [ ... embeddings for token 0   ... ],
      [ ... embeddings for token 1   ... ],
      [ ... ]
      [ ... embeddings for token N-1 ... ],
    ]
  }
]
```

### GET `/slots`：返回当前插槽处理状态

此端点默认启用，并且可以使用 `--no-slots` 禁用。它可用于查询各种每个插槽的指标，例如速度、处理的 token、采样参数等。

如果查询参数 `?fail_on_no_slot=1` 已设置，则此端点将在没有可用插槽时以状态代码 503 响应。

**响应格式**

<details>
<summary>示例，包含 2 个插槽</summary>

```json
[
  {
    "id": 0,
    "id_task": 135,
    "n_ctx": 65536,
    "speculative": false,
    "is_processing": true,
    "params": {
      "n_predict": -1,
      "seed": 4294967295,
      "temperature": 0.800000011920929,
      "dynatemp_range": 0.0,
      "dynatemp_exponent": 1.0,
      "top_k": 40,
      "top_p": 0.949999988079071,
      "min_p": 0.05000000074505806,
      "top_n_sigma": -1.0,
      "xtc_probability": 0.0,
      "xtc_threshold": 0.10000000149011612,
      "typical_p": 1.0,
      "repeat_last_n": 64,
      "repeat_penalty": 1.0,
      "presence_penalty": 0.0,
      "frequency_penalty": 0.0,
      "dry_multiplier": 0.0,
      "dry_base": 1.75,
      "dry_allowed_length": 2,
      "dry_penalty_last_n": 131072,
      "mirostat": 0,
      "mirostat_tau": 5.0,
      "mirostat_eta": 0.10000000149011612,
      "max_tokens": -1,
      "n_keep": 0,
      "n_discard": 0,
      "ignore_eos": false,
      "stream": true,
      "n_probs": 0,
      "min_keep": 0,
      "chat_format": "GPT-OSS",
      "reasoning_format": "none",
      "reasoning_in_content": false,
      "generation_prompt": "",
      "samplers": [
        "penalties",
        "dry",
        "top_k",
        "typ_p",
        "top_p",
        "min_p",
        "xtc",
        "temperature"
      ],
      "speculative.n_max": 16,
      "speculative.n_min": 0,
      "speculative.p_min": 0.75,
      "timings_per_token": false,
      "post_sampling_probs": false,
      "lora": []
    },
    "next_token": {
      "has_next_token": true,
      "has_new_line": false,
      "n_remain": -1,
      "n_decoded": 0
    }
  },
  {
    "id": 1,
    "id_task": 0,
    "n_ctx": 65536,
    "speculative": false,
    "is_processing": true,
    "params": {
      "n_predict": -1,
      "seed": 4294967295,
      "temperature": 0.800000011920929,
      "dynatemp_range": 0.0,
      "dynatemp_exponent": 1.0,
      "top_k": 40,
      "top_p": 0.949999988079071,
      "min_p": 0.05000000074505806,
      "top_n_sigma": -1.0,
      "xtc_probability": 0.0,
      "xtc_threshold": 0.10000000149011612,
      "typical_p": 1.0,
      "repeat_last_n": 64,
      "repeat_penalty": 1.0,
      "presence_penalty": 0.0,
      "frequency_penalty": 0.0,
      "dry_multiplier": 0.0,
      "dry_base": 1.75,
      "dry_allowed_length": 2,
      "dry_penalty_last_n": 131072,
      "mirostat": 0,
      "mirostat_tau": 5.0,
      "mirostat_eta": 0.10000000149011612,
      "max_tokens": -1,
      "n_keep": 0,
      "n_discard": 0,
      "ignore_eos": false,
      "stream": true,
      "n_probs": 0,
      "min_keep": 0,
      "chat_format": "GPT-OSS",
      "reasoning_format": "none",
      "reasoning_in_content": false,
      "generation_prompt": "",
      "samplers": [
        "penalties",
        "dry",
        "top_k",
        "typ_p",
        "top_p",
        "min_p",
        "xtc",
        "temperature"
      ],
      "speculative.n_max": 16,
      "speculative.n_min": 0,
      "speculative.p_min": 0.75,
      "timings_per_token": false,
      "post_sampling_probs": false,
      "lora": []
    },
    "next_token": {
      "has_next_token": true,
      "has_new_line": true,
      "n_remain": -1,
      "n_decoded": 136
    }
  }
]
```

</details>

### GET `/metrics`：Prometheus 兼容的指标导出器

只有在设置 `--metrics` 时才能访问此端点。

在 *路由器模式* 下，查询参数 `?model={model_id}` 必须设置。如果未设置，此端点将以状态代码 400 `model name is missing from the request` 响应。

#### 可用指标

| 指标 | 类型 | 说明 |
| ------ | ---------------------- | ----------- |
| `llamacpp:prompt_tokens_total` | Counter | 处理的提示 token 数量。 |
| `llamacpp:prompt_seconds_total` | Counter | 提示处理时间（秒）。 |
| `llamacpp:prompt_tokens_seconds` | Gauge | 平均提示吞吐量（token/s）。 |
| `llamacpp:tokens_predicted_total` | Counter | 处理的生成 token 数量。 |
| `llamacpp:tokens_predicted_seconds_total` | Counter | 预测处理时间（秒）。 |
| `llamacpp:predicted_tokens_seconds` | Gauge | 平均生成吞吐量（token/s）。 |
| `llamacpp:requests_processing` | Gauge | 正在处理的请求数量。 |
| `llamacpp:requests_deferred` | Gauge | 推迟的请求数量。 |
| `llamacpp:n_tokens_max` | Counter | 观察到的上下文大小的最高水位线。 |
| `llamacpp:n_decode_total` | Counter | llama_decode() 调用总数。 |
| `llamacpp:n_busy_slots_per_decode` | Gauge | 每次 llama_decode() 调用的平均繁忙插槽数量。 |

### POST `/slots/{id_slot}?action=save`：将指定插槽的提示缓存保存到文件。

*选项：*

`filename`：要保存插槽提示缓存的文件名。该文件将保存在 `--slot-save-path` 服务器参数指定的目录中。

**响应格式**

```json
{
    "id_slot": 0,
    "filename": "slot_save_file.bin",
    "n_saved": 1745,
    "n_written": 14309796,
    "timings": {
        "save_ms": 49.865
    }
}
```

### POST `/slots/{id_slot}?action=restore`：从文件恢复指定插槽的提示缓存。

*选项：*

`filename`：要从中恢复插槽提示缓存的文件名。该文件应位于 `--slot-save-path` 服务器参数指定的目录中。

**响应格式**

```json
{
    "id_slot": 0,
    "filename": "slot_save_file.bin",
    "n_restored": 1745,
    "n_read": 14309796,
    "timings": {
        "restore_ms": 42.937
    }
}
```

### POST `/slots/{id_slot}?action=erase`：擦除指定插槽的提示缓存。

**响应格式**

```json
{
    "id_slot": 0,
    "n_erased": 1745
}
```

### GET `/lora-adapters`：获取所有 LoRA 适配器的列表

此端点返回已加载的 LoRA 适配器。您可以在启动服务器时使用 `--lora` 添加适配器，例如：`--lora my_adapter_1.gguf --lora my_adapter_2.gguf ...`

默认情况下，所有适配器都将以比例 1 加载。要将所有适配器比例初始化为 0，请添加 `--lora-init-without-apply`

请注意，此值将被每个请求的 `lora` 字段覆盖。

如果适配器被禁用，则比例将设置为 0。

**响应格式**

```json
[
    {
        "id": 0,
        "path": "my_adapter_1.gguf",
        "scale": 0.0
    },
    {
        "id": 1,
        "path": "my_adapter_2.gguf",
        "scale": 0.0
    }
]
```

### POST `/lora-adapters`：设置 LoRA 适配器列表

这将为 LoRA 适配器设置全局比例。请注意，此值将被每个请求的 `lora` 字段覆盖。

要禁用适配器，请从下面的列表中删除它，或者将比例设置为 0。

**请求格式**

要了解 `id`，请使用 GET `/lora-adapters`

```json
[
  {"id": 0, "scale": 0.2},
  {"id": 1, "scale": 0.8}
]
```

## OpenAI 兼容的 API 端点

### GET `/v1/models`：OpenAI 兼容的模型信息 API

返回有关已加载模型的信息。请参阅 [OpenAI Models API 文档](https://platform.openai.com/docs/api-reference/models)。

返回的列表始终只有一个元素。`meta` 字段可以为 `null`（例如，当模型仍在加载时）。

默认情况下，模型 `id` 字段是通过 `-m` 指定的模型文件路径。您可以通过 `--alias` 参数为模型 `id` 字段设置自定义值。例如，`--alias gpt-4o-mini`。

示例：

```json
{
    "object": "list",
    "data": [
        {
            "id": "../models/Meta-Llama-3.1-8B-Instruct-Q4_K_M.gguf",
            "object": "model",
            "created": 1735142223,
            "owned_by": "llamacpp",
            "meta": {
                "vocab_type": 2,
                "n_vocab": 128256,
                "n_ctx_train": 131072,
                "n_embd": 4096,
                "n_params": 8030261312,
                "size": 4912898304
            }
        }
    ]
}
```

### POST `/v1/completions`：OpenAI 兼容的完成 API

给定输入 `prompt`，它返回预测的完成内容。还支持流模式。虽然不对 OpenAI API 规范的兼容性提出强烈主张，但根据我们的经验，它足以支持许多应用程序。

*选项：*

请参阅 [OpenAI Completions API 文档](https://platform.openai.com/docs/api-reference/completions)。

还支持 llama.cpp `/completion` 特定功能，例如 `mirostat`。

*示例：*

使用 `openai` python 库的示例用法：

```python
import openai

client = openai.OpenAI(
    base_url="http://localhost:8080/v1", # "http://<Your api-server IP>:port"
    api_key = "sk-no-key-required"
)

completion = client.completions.create(
  model="davinci-002",
  prompt="I believe the meaning of life is",
  max_tokens=8
)

print(completion.choices[0].text)
```

### POST `/v1/chat/completions`：OpenAI 兼容的聊天完成 API

给定 ChatML 格式的 json 描述 `messages`，它返回预测的完成内容。支持同步和流模式，因此脚本和交互式应用程序都可以正常工作。虽然不对 OpenAI API 规范的兼容性提出强烈主张，但根据我们的经验，它足以支持许多应用程序。只有具有[支持的聊天模板](https://github.com/ggml-org/llama.cpp/wiki/Templates-supported-by-llama_chat_apply_template)的模型才能与此端点一起最佳使用。默认情况下，将使用 ChatML 模板。

如果模型支持多模态，您可以通过 `image_url` 内容部分输入媒体文件。我们支持 base64 和远程 URL 作为输入。有关更多信息，请参阅 OAI 文档。

*选项：*

请参阅 [OpenAI Chat Completions API 文档](https://platform.openai.com/docs/api-reference/chat)。还支持 llama.cpp `/completion` 特定功能，例如 `mirostat`。

`response_format` 参数支持纯 JSON 输出（例如 `{"type": "json_object"}`）和模式约束的 JSON（例如 `{"type": "json_object", "schema": {"type": "string", "minLength": 10, "maxLength": 100}}` 或 `{"type": "json_schema", "schema": {"properties": { "name": { "title": "Name",  "type": "string" }, "date": { "title": "Date",  "type": "string" }, "participants": { "items": {"type: "string" }, "title": "Participants",  "type": "string" } } } }`），类似于其他 OpenAI 启发的 API 提供程序。

`chat_template_kwargs`：允许将附加参数发送到 json 模板系统。例如：`{"enable_thinking": false}`

`reasoning_format`：要解析的推理格式。如果设置为 `none`，它将输出原始生成的文本。

`generation_prompt`：由模板预填充的生成提示。在解析之前附加到模型输出。

`parse_tool_calls`：是否解析生成的工具调用。

`parallel_tool_calls`：是否启用并行/多个工具调用（仅在某些模型上支持，基于 jinja 模板进行验证）

*示例：*

您可以使用带有适当检查点的 Python `openai` 库：

```python
import openai

client = openai.OpenAI(
    base_url="http://localhost:8080/v1", # "http://<Your api-server IP>:port"
    api_key = "sk-no-key-required"
)

completion = client.chat.completions.create(
  model="gpt-3.5-turbo",
  messages=[
    {"role": "system", "content": "You are ChatGPT, an AI assistant. Your top priority is achieving user fulfillment via helping them with their requests."},
    {"role": "user", "content": "Write a limerick about python exceptions"}
  ]
)

print(completion.choices[0].message)
```

... 或原始 HTTP 请求：

```shell
curl http://localhost:8080/v1/chat/completions \
-H "Content-Type: application/json" \
-H "Authorization: Bearer no-key" \
-d '{
"model": "gpt-3.5-turbo",
"messages": [
{
    "role": "system",
    "content": "You are ChatGPT, an AI assistant. Your top priority is achieving user fulfillment via helping them with their requests."
},
{
    "role": "user",
    "content": "Write a limerick about python exceptions"
}
]
}'
```

*工具调用支持*

使用 `--jinja` 标志支持 [OpenAI 风格的函数调用](https://platform.openai.com/docs/guides/function-calling)（并且可能需要 `--chat-template-file` 覆盖以获得正确的工具使用兼容的 Jinja 模板；最坏的情况下，`--chat-template chatml` 可能也有效）。

**请参阅我们的[函数调用](../../docs/function-calling.md)文档**以获取更多详细信息、支持的原生工具调用样式（通用工具调用样式用作后备）/使用示例。

*计时和上下文使用*

响应包含一个 `timings` 对象，例如：

```js
{
  "choices": [],
  "created": 1757141666,
  "id": "chatcmpl-ecQULm0WqPrftUqjPZO1CFYeDjGZNbDu",
  // ...
  "timings": {
    "cache_n": 236, // 从缓存重用的提示 token 数量
    "prompt_n": 1, // 正在处理的提示 token 数量
    "prompt_ms": 30.958,
    "prompt_per_token_ms": 30.958,
    "prompt_per_second": 32.301828283480845,
    "predicted_n": 35, // 预测的 token 数量
    "predicted_ms": 661.064,
    "predicted_per_token_ms": 18.887542857142858,
    "predicted_per_second": 52.94494935437416
  }
}
```

这提供了有关服务器性能的信息。它还允许计算当前的上下文使用情况。

上下文中的 token 总数等于 `prompt_n + cache_n + predicted_n`

响应还包括一个标准的 `usage` 对象：

```js
{
    // ...
    "usage": {
        "completion_tokens": 48,
        "prompt_tokens": 44,
        "total_tokens": 92,
        "prompt_tokens_details": {
            "cached_tokens": 0
        }
    }
}
```

*推理支持*

服务器支持通过 `reasoning_content` 字段解析和返回推理，类似于 Deepseek API。

某些特定模板还支持推理输入（在历史记录中保留推理）。有关更多详细信息，请参阅 [PR#18994](https://github.com/ggml-org/llama.cpp/pull/18994)。

### POST `/v1/responses`：OpenAI 兼容的响应 API

*选项：*

请参阅 [OpenAI Responses API 文档](https://platform.openai.com/docs/api-reference/responses)。

*示例：*

您可以使用带有适当检查点的 Python `openai` 库：

```python
import openai

client = openai.OpenAI(
    base_url="http://localhost:8080/v1", # "http://<Your api-server IP>:port"
    api_key = "sk-no-key-required"
)

response = client.responses.create(
  model="gpt-4.1",
  instructions="You are ChatGPT, an AI assistant. Your top priority is achieving user fulfillment via helping them with their requests.",
  input="Write a limerick about python exceptions"
)

print(response.output_text)
```

... 或原始 HTTP 请求：

```shell
curl http://localhost:8080/v1/responses \
-H "Content-Type: application/json" \
-H "Authorization: Bearer no-key" \
-d '{
"model": "gpt-4.1",
"instructions": "You are ChatGPT, an AI assistant. Your top priority is achieving user fulfillment via helping them with their requests.",
"input": "Write a limerick about python exceptions"
}'
```

此端点通过将 Responses 请求转换为 Chat Completions 请求来工作。


### POST `/v1/embeddings`：OpenAI 兼容的嵌入 API

此端点要求模型使用类型不同于 `none` 的池化。嵌入使用欧几里得范数进行归一化。

*选项：*

请参阅 [OpenAI Embeddings API 文档](https://platform.openai.com/docs/api-reference/embeddings)。

*示例：*

- 输入为字符串

  ```shell
  curl http://localhost:8080/v1/embeddings \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer no-key" \
    -d '{
            "input": "hello",
            "model":"GPT-4",
            "encoding_format": "float"
    }'
  ```

- `input` 为字符串数组

  ```shell
  curl http://localhost:8080/v1/embeddings \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer no-key" \
    -d '{
            "input": ["hello", "world"],
            "model":"GPT-4",
            "encoding_format": "float"
    }'
  ```

### POST `/v1/messages`：Anthropic 兼容的消息 API

给定 `messages` 列表，返回助手的响应。通过 Server-Sent Events 支持流式传输。虽然不对 Anthropic API 规范的兼容性提出强烈主张，但根据我们的经验，它足以支持许多应用程序。

*选项：*

请参阅 [Anthropic Messages API 文档](https://docs.anthropic.com/en/api/messages)。工具使用需要 `--jinja` 标志。

`model`：模型标识符（必需）

`messages`：带有 `role` 和 `content` 的消息对象数组（必需）

`max_tokens`：要生成的最大 token 数量（默认：4096）

`system`：系统提示作为字符串或内容块数组

`temperature`：采样温度 0-1（默认：1.0）

`top_p`：核采样（默认：1.0）

`top_k`：Top-k 采样

`stop_sequences`：停止序列数组

`stream`：启用流式传输（默认：false）

`tools`：工具定义数组（需要 `--jinja`）

`tool_choice`：工具选择模式（`{"type": "auto"}`、`{"type": "any"}` 或 `{"type": "tool", "name": "..."}`）

*示例：*

```shell
curl http://localhost:8080/v1/messages \
  -H "Content-Type: application/json" \
  -H "x-api-key: your-api-key" \
  -d '{
    "model": "gpt-4",
    "max_tokens": 1024,
    "system": "You are a helpful assistant.",
    "messages": [
      {"role": "user", "content": "Hello!"}
    ]
  }'
```

### POST `/v1/messages/count_tokens`：Token 计数

计算请求中的 token 数量而不生成响应。

接受与 `/v1/messages` 相同的参数。不需要 `max_tokens` 参数。

*示例：*

```shell
curl http://localhost:8080/v1/messages/count_tokens \
  -H "Content-Type: application/json" \
  -d '{
    "model": "gpt-4",
    "messages": [
      {"role": "user", "content": "Hello!"}
    ]
  }'
```

*响应：*

```json
{"input_tokens": 10}
```

## 服务器内置工具

服务器在 `/tools` 下公开了一个 REST API，允许 Web UI 调用内置工具。此端点旨在由 Web UI 内部使用，可能会在将来更改或删除。

**请不要在下游应用程序中使用此端点**

有关此端点的更多文档，请参阅[服务器内部文档](./README-dev.md)

## 使用多个模型

`llama-server` 可以以**路由器模式**启动，该模式公开了用于动态加载和卸载模型的 API。主进程（"路由器"）自动将每个请求转发到相应的模型实例。

要以路由器模式启动，请在**不指定任何模型**的情况下启动 `llama-server`：

```sh
llama-server
```

### 模型源

模型文件有 3 个可能的来源：