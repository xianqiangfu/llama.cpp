# 聊天模板说明

本目录包含各种 LLM 模型的 Jinja 聊天模板文件。

## 简介

聊天模板（Chat Templates）用于格式化对话输入，使其与模型训练时使用的格式一致。llama.cpp 使用 Jinja 模板引擎来实现这一功能。

## 模板更新

可以使用以下命令从 Hugging Face 模型获取聊天模板：

```bash
# 获取 Command R 工具使用模板
./scripts/get_chat_template.py CohereForAI/c4ai-command-r-plus tool_use \
  > models/templates/CohereForAI-c4ai-command-r-plus-tool_use.jinja

# 获取 Qwen 模板
./scripts/get_chat_template.py Qwen/Qwen2.5-7B-Instruct \
  > models/templates/Qwen-Qwen2.5-7B-Instruct.jinja

# 获取 LLaMA 模板
./scripts/get_chat_template.py meta-llama/Llama-3.1-8B-Instruct \
  > models/templates/meta-llama-Llama-3.1-8B-Instruct.jinja
```

## 支持的模板

本目录包含 50+ 个聊天模板文件，涵盖各种模型系列：

### LLaMA 系列

| 文件 | 模型 |
|------|------|
| meta-llama-Llama-3.1-8B-Instruct.jinja | LLaMA 3.1 8B |
| meta-llama-Llama-3.2-3B-Instruct.jinja | LLaMA 3.2 3B |
| meta-llama-Llama-3.3-70B-Instruct.jinja | LLaMA 3.3 70B |

### Gemma 系列

| 文件 | 模型 |
|------|------|
| google-gemma-2-2b-it.jinja | Gemma 2 2B |
| google-gemma-4-31B-it.jinja | Gemma 4 31B |
| google-gemma-4-31B-it-interleaved.jinja | Gemma 4 多模态 31B |

### Qwen 系列

| 文件 | 模型 |
|------|------|
| Qwen-Qwen2.5-7B-Instruct.jinja | Qwen 2.5 7B |
| Qwen-QwQ-32B.jinja | Qwen QwQ 32B |
| Qwen-Qwen3-0.6B.jinja | Qwen 3 0.6B |
| Qwen3-Coder.jinja | Qwen 3 Coder |
| Qwen3.5-4B.jinja | Qwen 3.5 4B |

### DeepSeek 系列

| 文件 | 模型 |
|------|------|
| deepseek-ai-DeepSeek-R1-Distill-Llama-8B.jinja | DeepSeek R1 8B |
| deepseek-ai-DeepSeek-R1-Distill-Qwen-32B.jinja | DeepSeek R1 32B |
| deepseek-ai-DeepSeek-V3.1.jinja | DeepSeek V3.1 |
| deepseek-ai-DeepSeek-V3.2.jinja | DeepSeek V3.2 |

### Command R 系列

| 文件 | 模型 |
|------|------|
| CohereForAI-c4ai-command-r-plus-tool_use.jinja | Command R Plus 工具使用 |
| CohereForAI-c4ai-command-r7b-12-2024-tool_use.jinja | Command R 7B 工具使用 |

### Phi 系列

| 文件 | 模型 |
|------|------|
| microsoft-Phi-3.5-mini-instruct.jinja | Phi 3.5 Mini |

### Mistral 系列

| 文件 | 模型 |
|------|------|
| mistralai-Mistral-Nemo-Instruct-2407.jinja | Mistral Nemo |
| mistralai-Ministral-3-14B-Reasoning-2512.jinja | Ministral 3 14B Reasoning |

### Hermes 系列

| 文件 | 模型 |
|------|------|
| NousResearch-Hermes-2-Pro-Llama-3-8B-tool_use.jinja | Hermes 2 Pro 8B |
| NousResearch-Hermes-3-Llama-3.1-8B-tool_use.jinja | Hermes 3 8B |

### 其他模型

还有其他各种模型的模板，包括：
- Apertus, Bielik, ByteDance Seed, GLM 系列
- GigaChat, SmolLM, Kimi, MiMo, MiniMax
- NVIDIA Nemotron, Reka, StepFun, Upstage 等

## Jinja 模板语法

### 基本语法

```jinja
# 变量
{{ variable }}

# 条件
{% if condition %}
  content
{% endif %}

# 循环
{% for message in messages %}
  {{ message }}
{% endfor %}
```

### 示例模板

```jinja
{%- set ns = namespace(found=false) -%}
{%- for message in messages -%}
  {%- if message['role'] == 'system' -%}
    {{ message['content'] }}
    {%- set ns.found = true -%}
  {%- endif -%}
{%- endfor -%}

{%- if not ns.found -%}
  You are a helpful assistant.
{%- endif -%}

{% for message in messages %}
  {{ message['role'] }}: {{ message['content'] }}
{% endfor %}

assistant:
```

## 使用模板

### 在 CLI 中使用

```bash
# 使用 Jinja 模板
./llama-cli \
  -m model.gguf \
  --chat-template models/templates/meta-llama-Llama-3.1-8B-Instruct.jinja \
  -p "Hello, how are you?"

# 指定对话
./llama-cli \
  -m model.gguf \
  --chat-template models/templates/Qwen-Qwen2.5-7B-Instruct.jinja \
  -c "user: Hello\nassistant: Hi!"
```

### 在服务器中使用

```bash
# 服务器自动使用模型的聊天模板
./llama-server \
  -m model.gguf \
  --host 0.0.0.0 \
  --port 8080
```

### 自定义模板

```bash
# 使用自定义模板
./llama-cli \
  -m model.gguf \
  --chat-template my_template.jinja \
  -p "Custom prompt"
```

## 模板变量

### 消息格式

每条消息包含：

| 字段 | 说明 | 示例 |
|------|------|------|
| role | 消息角色 | "user", "assistant", "system" |
| content | 消息内容 | "Hello, world!" |

### 特殊角色

| 角色 | 说明 |
|------|------|
| system | 系统提示词 |
| user | 用户消息 |
| assistant | 助手回复 |
| tool | 工具调用结果 |

## 工具调用

支持工具调用的模板：

```jinja
{% if message['role'] == 'assistant' and 'tool_calls' in message %}
  {{ message['tool_calls'] | tojson }}
{% endif %}
```

相关模板：
- `*-tool_use.jinja` - 工具使用模板
- `*-rag.jinja` - 检索增强生成模板

## 常见问题

### Q: 如何选择正确的模板？

A: 根据你的模型选择对应的模板，例如 LLaMA 3 使用 `meta-llama-Llama-3.1-8B-Instruct.jinja`。

### Q: 模板不匹配会怎样？

A: 可能导致模型输出质量下降或格式错误。

### Q: 如何创建自定义模板？

A: 参考现有模板，使用 Jinja 语法创建。

### Q: 模板支持哪些功能？

A: 支持 Jinja 的所有功能，包括变量、条件、循环、过滤器等。

### Q: 如何更新模板？

A: 使用 `get_chat_template.py` 脚本从 Hugging Face 获取最新模板。

## 模板最佳实践

### 1. 保持简洁

```jinja
{# 好的模板 - 简洁清晰 #}
{% for message in messages %}
  {{ message.role }}: {{ message.content }}
{% endfor %}
```

### 2. 正确处理换行

```jinja
{# 使用 - 去除空白 #}
{%- for message in messages -%}
  {{ message.role }}: {{ message.content }}
{%- endfor -%}
```

### 3. 添加注释

```jinja
{# 系统提示词处理 #}
{% set system_message = messages | selectattr('role', 'equalto', 'system') | list | first %}
```

## 调试模板

### 验证模板语法

```bash
python -c "from jinja2 import Template; Template(open('template.jinja').read())"
```

### 测试模板

```bash
./llama-cli \
  -m model.gguf \
  --chat-template template.jinja \
  --verbose \
  -p "Test"
```

## 相关文档

- [模型文件说明](../README_ZH.md)
- [Jinja 模板文档](https://jinja.palletsprojects.com/)
- [get_chat_template.py](../../scripts/get_chat_template.py)

## 贡献

欢迎贡献新的聊天模板：

1. 使用 `get_chat_template.py` 获取模板
2. 测试模板兼容性
3. 更新模板列表
4. 提交 Pull Request

## 参考资料

- [Jinja 文档](https://jinja.palletsprojects.com/)
- [Hugging Face Chat Templates](https://huggingface.co/docs/transformers/chat_templating)
- [llama.cpp 文档](https://github.com/ggml-org/llama.cpp/wiki)
