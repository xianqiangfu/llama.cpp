# 模型转换脚本说明

本目录包含将各种 LLM 模型从 Hugging Face 格式转换为 GGUF 格式的 Python 脚本。

## 简介

llama.cpp 使用 GGUF (GGML Universal File) 格式作为其模型文件格式。本目录中的脚本用于将各种开源模型的原始格式（主要是 Hugging Face 格式）转换为 GGUF 格式。

## 目录结构

```
conversion/
├── __init__.py          # 模块初始化
├── base.py              # 基础转换类和工具函数
├── llama.py             # LLaMA 系列模型转换
├── mistral.py           # Mistral 系列模型转换
├── gemma.py             # Gemma 系列模型转换
├── qwen.py              # Qwen 系列模型转换
├── phi.py               # Phi 系列模型转换
├── deepseek.py          # DeepSeek 系列模型转换
├── llava.py             # LLaVA 多模态模型转换
└── [其他模型].py        # 其他各种模型转换脚本
```

## 支持的模型

### 主流 LLM 系列

| 脚本 | 模型系列 |
|------|----------|
| llama.py | LLaMA 1, 2, 3, 3.1, 3.2, 3.3, 4 |
| mistral.py | Mistral 7B, Mixtral 8x7B, 8x22B, Mistral 3 |
| gemma.py | Gemma 1, 2, 3 |
| qwen.py | Qwen 1, 2, 2.5, 3, 3.5 |
| phi.py | Phi 1, 1.5, 2, 3, 3.5 |
| deepseek.py | DeepSeek V2, V3, R1 |

### 其他模型

| 脚本 | 模型系列 |
|------|----------|
| baichuan.py | 百川系列 |
| chatglm.py | ChatGLM 2, 3, 4 |
| internlm.py | 书生系列 |
| yi.py | Yi 系列 |
| grok.py | Grok 1, 2 |
| falcon.py | Falcon 7B, 40B, 180B |
| mpt.py | MPT 系列 |
| refact.py | Refact 系列 |
| olmo.py | OLMo 系列 |
| granite.py | IBM Granite 系列 |
| nemotron.py | NVIDIA Nemotron 系列 |
| hunyuan.py | 腾讯混元系列 |

### 多模态模型

| 脚本 | 模型系列 |
|------|----------|
| llava.py | LLaVA 系列 |
| minicpm.py | MiniCPM-V 系列 |
| internvl.py | InternVL 系列 |
| kimivl.py | Kimi VL 系列 |
| qwen3vl.py | Qwen3-VL 系列 |
| qwenvl.py | Qwen-VL 系列 |
| pixtral.py | Pixtral |

### 架构变体

| 脚本 | 架构类型 |
|------|----------|
| mamba.py | Mamba 架构 |
| rwkv.py | RWKV 架构 |
| jamba.py | Jamba 架构 |
| t5.py | T5 系列 |
| bert.py | BERT 系列 |

### 特殊模型

| 脚本 | 模型类型 |
|------|----------|
| lfm2.py | LFM2 |
| llada.py | LLADA |
| falcon_h1.py | Falcon H1 |
| afmoe.py | AF MoE |
| grovemoe.py | Grove MoE |
| bailingmoe.py | BaiLing MoE |

## 使用方法

### 主转换脚本

llama.cpp 主目录中的 `convert_hf_to_gguf.py` 是主要的转换入口：

```bash
python convert_hf_to_gguf.py \
  --model <model-type> \
  --outfile <output-file>.gguf \
  --outtype <output-type> \
  <model-path>
```

### 参数说明

| 参数 | 说明 | 示例 |
|------|------|------|
| `--model` | 模型类型 | `--model llama` |
| `--outfile` | 输出文件名 | `--outfile llama-7b.gguf` |
| `--outtype` | 输出类型 | `--outtype f16`, `--outtype q8_0` |
| `model-path` | 模型目录路径 | `/path/to/model` |

### 输出类型

| 类型 | 说明 | 大小 | 质量 |
|------|------|------|------|
| f32 | 32位浮点 | 最大 | 最高 |
| f16 | 16位浮点 | 中等 | 高 |
| q8_0 | 8位量化 | 小 | 好 |
| q6_k | 6位量化 | 更小 | 较好 |
| q5_k_m | 5位量化 | 很小 | 一般 |
| q4_k_m | 4位量化 | 最小 | 可接受 |

## 示例

### LLaMA 模型

```bash
python convert_hf_to_gguf.py \
  --model llama \
  --outfile llama-3-8b.gguf \
  --outtype q4_k_m \
  meta-llama/Meta-Llama-3-8B
```

### Mistral 模型

```bash
python convert_hf_to_gguf.py \
  --model mistral \
  --outfile mistral-7b.gguf \
  --outtype q4_k_m \
  mistralai/Mistral-7B-Instruct-v0.3
```

### Qwen 模型

```bash
python convert_hf_to_gguf.py \
  --model qwen \
  --outfile qwen2-7b.gguf \
  --outtype q4_k_m \
  Qwen/Qwen2-7B-Instruct
```

### 多模态模型 (LLaVA)

```bash
# 主模型
python convert_hf_to_gguf.py \
  --model llava \
  --outfile llava-v1.5-7b.gguf \
  --outtype q4_k_m \
  liuhaotian/llava-v1.5-7b

# 视觉投影器
python convert_hf_to_gguf.py \
  --model llava \
  --outfile llava-v1.5-7b-mmproj.gguf \
  --outtype f16 \
  --llava-projector \
  liuhaotian/llava-v1.5-7b
```

## 高级用法

### 分步转换

1. **准备模型**
   ```bash
   # 使用 Hugging Face 下载
   git lfs install
   git clone https://huggingface.co/<model-path>
   ```

2. **转换为 GGUF**
   ```bash
   python convert_hf_to_gguf.py --model <type> --outfile model.gguf <model-path>
   ```

3. **量化 (可选)**
   ```bash
   ./llama-quantize model.gguf model-q4.gguf q4_k_m
   ```

### 自定义参数

某些模型支持特殊参数：

```bash
python convert_hf_to_gguf.py \
  --model mistral \
  --vocab-type bpe \
  --outfile mistral.gguf \
  mistralai/Mistral-7B
```

### 批量转换

```bash
# 转换多个模型
for model in model1 model2 model3; do
  python convert_hf_to_gguf.py \
    --model llama \
    --outfile ${model}.gguf \
    --outtype q4_k_m \
    ${model}
done
```

## 添加新模型支持

### 1. 创建新脚本

在 `conversion/` 目录中创建新的 Python 文件：

```python
# conversion/newmodel.py
from .base import *

class NewModelConverter(BaseConverter):
    def __init__(self, model_path, params):
        super().__init__(model_path, params)
        # 初始化代码

    def convert(self):
        # 转换逻辑
        pass
```

### 2. 注册模型

在 `convert_hf_to_gguf.py` 中注册新模型：

```python
from conversion import newmodel

# 在模型注册表中添加
MODEL_CLASSES = {
    # ...
    "newmodel": newmodel.NewModelConverter,
}
```

### 3. 添加测试

为新的转换器添加测试用例。

## 常见问题

### Q: 转换需要多长时间？

A: 取决于模型大小和硬件性能，7B 模型通常需要几分钟。

### Q: 需要多少内存？

A: 7B 模型需要约 16GB 内存，70B 模型需要约 64GB 内存。

### Q: 可以在 CPU 上转换吗？

A: 可以，但会非常慢。建议使用 GPU。

### Q: 转换后模型能用吗？

A: 需要验证转换结果，建议先测试小模型。

### Q: 如何验证转换正确性？

A:
1. 检查输出文件大小
2. 使用 `./llama-cli` 测试加载
3. 对比推理结果

## 故障排查

### 内存不足

- 减小批处理大小
- 使用分块处理
- 增加交换空间

### 转换失败

- 检查模型文件完整性
- 确认模型类型正确
- 查看错误日志

### 性能差

- 使用 GPU 加速
- 优化分块大小
- 使用更快的存储

## 相关工具

### 量化工具

转换完成后，可以使用量化工具：

```bash
./llama-quantize model.gguf model-q4.gguf q4_k_m
```

### GGUF 工具

使用 `gguf-py` 工具操作 GGUF 文件：

```bash
# 查看 GGUF 信息
python -m gguf.scripts.gguf_dump model.gguf

# 修改元数据
python -m gguf.scripts.gguf_set_metadata model.gguf
```

## 相关文档

- [gguf-py 文档](../gguf-py/README_ZH.md)
- [模型文件说明](../models/README_ZH.md)
- [GGUF 格式](https://github.com/ggml-org/ggml/pull/302)
- [构建指南](../docs/build.md)

## 贡献

欢迎添加新模型的转换支持：

1. 实现 `BaseConverter` 类
2. 添加完整的测试
3. 更新文档
4. 提交 Pull Request

## 参考资源

- [Hugging Face](https://huggingface.co/)
- [GGUF 格式说明](https://github.com/ggml-org/ggml/pull/302)
- [llama.cpp Wiki](https://github.com/ggml-org/llama.cpp/wiki)