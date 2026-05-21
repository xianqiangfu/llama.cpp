# 模型实现目录说明

本目录包含 128 种不同大语言模型架构的具体实现。

## 目录概览

每个模型架构对应一个 `.cpp` 文件，负责实现该模型的特定架构逻辑和参数加载。

## 模型架构分类

### LLaMA 系列
| 文件 | 说明 |
|------|------|
| `llama.cpp` | 标准 LLaMA/Llama2 架构 |
| `llama4.cpp` | LLaMA 4 架构 |
| `llama-embed.cpp` | LLaMA 嵌入模型 |

### Gemma 系列
| 文件 | 说明 |
|------|------|
| `gemma.cpp` | Gemma 1 架构 |
| `gemma2.cpp` | Gemma 2 架构 |
| `gemma3.cpp` | Gemma 3 架构 |
| `gemma-embedding.cpp` | Gemma 嵌入模型 |

### Qwen 系列
| 文件 | 说明 |
|------|------|
| `qwen.cpp` | Qwen 1 架构 |
| `qwen2.cpp` | Qwen 2 架构 |
| `qwen2moe.cpp` | Qwen 2 MOE 架构 |
| `qwen2vl.cpp` | Qwen 2 视觉语言模型 |
| `qwen3.cpp` | Qwen 3 架构 |
| `qwen3moe.cpp` | Qwen 3 MOE 架构 |
| `qwen3next.cpp` | Qwen 3 Next 架构 |
| `qwen3vl.cpp` | Qwen 3 视觉语言模型 |
| `qwen3vlmoe.cpp` | Qwen 3 VL MOE 架构 |
| `qwen35.cpp` | Qwen 3.5 架构 |
| `qwen35moe.cpp` | Qwen 3.5 MOE 架构 |

### Mistral/DeepSeek 系列
| 文件 | 说明 |
|------|------|
| `deepseek.cpp` | DeepSeek Coder 架构 |
| `deepseek2.cpp` | DeepSeek V2 架构 |
| `deepseek2ocr.cpp` | DeepSeek V2 OCR 模型 |

### BERT 系列
| 文件 | 说明 |
|------|------|
| `bert.cpp` | 标准 BERT 架构 |
| `eurobert.cpp` | EuroBERT 架构 |

### RWKV/Mamba 系列（非Transformer架构）
| 文件 | 说明 |
|------|------|
| `mamba.cpp` | Mamba SSM 架构 |
| `mamba2.cpp` | Mamba 2 架构 |
| `rwkv.cpp` | RWKV 4/5 架构 |
| `rwkv6.cpp` | RWKV 6 架构 |
| `rwkv6qwen2.cpp` | RWKV6-Qwen2 混合架构 |
| `rwkv7.cpp` | RWKV 7 架构 |
| `arwkv7.cpp` | A-RWKV 7 架构 |

### 混合 MOE 架构
| 文件 | 说明 |
|------|------|
| `afmoe.cpp` | AFMOE 架构 |
| `bailingmoe.cpp` | BailingMOE 架构 |
| `bailingmoe2.cpp` | BailingMOE 2 架构 |
| `granite.cpp` | IBM Granite 架构 |
| `granite-moe.cpp` | Granite MOE 架构 |
| `granite-hybrid.cpp` | Granite 混合架构 |
| `ernie4-5.cpp` | ERNIE 4.5 架构 |
| `ernie4-5-moe.cpp` | ERNIE 4.5 MOE 架构 |
| `exaone-moe.cpp` | EXAONE MOE 架构 |
| `openai-moe.cpp` | OpenAI MOE 架构 |
| `lfm2.cpp` | LFM2 架构 |
| `lfm2moe.cpp` | LFM2 MOE 架构 |

### 其他主流模型
| 文件 | 说明 |
|------|------|
| `gpt2.cpp` | GPT-2 架构 |
| `gptj.cpp` | GPT-J 架构 |
| `gptneox.cpp` | GPT-NeoX 架构 |
| `falcon.cpp` | Falcon 架构 |
| `falcon-h1.cpp` | Falcon H1 架构 |
| `baichuan.cpp` | 百川模型架构 |
| `chatglm.cpp` | ChatGLM 架构 |
| `glm4.cpp` | GLM-4 架构 |
| `glm4-moe.cpp` | GLM-4 MOE 架构 |
| `glm-dsa.cpp` | GLM DSA 架构 |
| `internlm2.cpp` | InternLM 2 架构 |
| `olmo.cpp` | OLMo 架构 |
| `olmo2.cpp` | OLMo 2 架构 |
| `olmoe.cpp` | OLMoE 架构 |
| `phi2.cpp` | Phi-2 架构 |
| `phi3.cpp` | Phi-3 架构 |
| `phimoe.cpp` | Phi-MoE 架构 |
| `stablelm.cpp` | StableLM 架构 |
| `starcoder.cpp` | StarCoder 架构 |
| `starcoder2.cpp` | StarCoder 2 架构 |
| `bloom.cpp` | BLOOM 架构 |

### 视觉语言模型
| 文件 | 说明 |
|------|------|
| `clip.cpp` | CLIP 视觉编码器 |
| `chameleon.cpp` | Chameleon 视觉语言模型 |
| `cogvlm.cpp` | CogVLM 视觉语言模型 |
| `qwen2vl.cpp` | Qwen2 视觉语言模型 |

### 专用模型
| 文件 | 说明 |
|------|------|
| `t5.cpp` | T5 编码器-解码器 |
| `bitnet.cpp` | BitNet 1.58b 架构 |
| `bloom.cpp` | BLOOM 架构 |
| `command-r.cpp` | Command R 模型 |
| `cohere2.cpp` | Cohere 模型 |
| `dbrx.cpp` | DBRX 模型 |
| `minicpm.cpp` | MiniCPM 模型 |
| `minicpm3.cpp` | MiniCPM 3 模型 |
| `minimax-m2.cpp` | MiniMax M2 模型 |
| `codeshell.cpp` | CodeShell 代码模型 |
| `decimator.cpp` | Decimator 模型 |
| `dream.cpp` | Dream 扩散模型 |

### 其他特殊模型
| 文件 | 说明 |
|------|------|
| `arcee.cpp` | Arcee 模型 |
| `apertus.cpp` | Apertus 模型 |
| `arctic.cpp` | Arctic 模型 |
| `dots1.cpp` | DOTS-1 模型 |
| `dbrx.cpp` | DBRX 模型 |
| `dream.cpp` | Dream 扩散模型 |
| `hunyuan-moe.cpp` | 腾讯混元 MOE 架构 |
| `hunyuan-dense.cpp` | 腾讯混元密集架构 |
| `hunyuan-vl.cpp` | 腾讯混元视觉语言模型 |
| `llada.cpp` | LLADA 扩散模型 |
| `llada-moe.cpp` | LLADA MOE 模型 |
| `smallthinker.cpp` | SmallThinker 模型 |
| `seed-oss.cpp` | Seed OSS 模型 |
| `grovemoe.cpp` | GroveMOE 模型 |
| `arctic.cpp` | Arctic 模型 |
| `smollm3.cpp` | SmolLM3 模型 |
| `maincoder.cpp` | MainCoder 模型 |
| `kimi-linear.cpp` | Kimi Linear KDA 模型 |
| `mistral3.cpp` | Mistral 3 架构 |
| `mistral4.cpp` | Mistral 4 架构 |
| `mimo2.cpp` | MIMO-2 模型 |
| `step35.cpp` | Step-35 模型 |

## 模型实现模式

每个模型文件通常包含以下组件：

1. **架构注册** - 在 `llama-arch.h` 中注册对应的 `llm_arch` 枚举值
2. **模型加载** - 从 GGUF 文件读取模型参数
3. **计算图构建** - 定义前向传播的计算流程
4. **特定层实现** - 实现模型特有的层或操作

## 相关文档

- [../README_ZH.md](../README_ZH.md) - 核心源代码总览
- [../../include/llama.h](../../include/llama.h) - C API 接口
- [../../AGENTS.md](../../AGENTS.md) - Agent 工作流程