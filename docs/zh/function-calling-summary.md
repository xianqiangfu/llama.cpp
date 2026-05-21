# 函数调用文档摘要 (function-calling.md)

## 概述

[chat.h](../common/chat.h) (https://github.com/ggml-org/llama.cpp/pull/9639) 添加了对 [OpenAI 风格的函数调用](https://platform.openai.com/docs/guides/function-calling) 的支持，并用于：

- `llama-server`（使用 `--jinja` 标志启动时）

## 通用支持（原生和通用处理程序）

所有模型都支持函数调用（参见 https://github.com/ggml-org/llama.cpp/pull/9639）：

### 支持的原生工具调用格式

- **Llama 3.1 / 3.3**（包括内置工具支持 - `wolfram_alpha`、`web_search` / `brave_search`、`code_interpreter` 的工具名称）、Llama 3.2
- **Functionary v3.1 / v3.2**
- **Hermes 2/3**、**Qwen 2.5**
- **Qwen 2.5 Coder**
- **Mistral Nemo**
- **Firefunction v2**
- **Command R7B**
- **DeepSeek R1**（正在进行中 / 似乎不愿意调用任何工具？）

### 通用工具调用支持

当模板不被原生格式处理程序识别时，支持通用工具调用（您将在日志中看到 `Chat format: Generic`）：

- 在适当的时候使用 `--chat-template-file` 覆盖模板（见下方示例）
- 通用支持可能消耗更多 token，并且比模型的原生格式效率更低

- 一些模型支持多重/并行工具调用，但默认情况下是禁用的。通过在完成端点负载中传递 `"parallel_tool_calls": true` 来启用它。

### 常见模板格式处理程序

文档中列出了超过 270 个模型的模板和使用的格式处理程序，包括：

- **Llama 3.x 格式**：Llama 3.1/3.2/3.3 系列模型
- **Hermes 2 Pro 格式**：Hermes 系列、Qwen 2.5 系列、多个其他模型
- **Mistral Nemo 格式**：Mistral Nemo 系列模型
- **FireFunction v2 格式**：Firefunction v2
- **Command R7B 格式**：Command R7B 系列
- **DeepSeek R1 格式**：DeepSeek R1 系列
- **Functionary 格式**：Functionary v3.1/v3.2
- **Generic 格式**：其他所有不匹配原生格式的模型

## 使用方法 - 需要支持工具的 Jinja 模板

首先，使用任何模型启动服务器，但确保它具有启用工具的模板：您可以通过检查 `http://localhost:8080/props` 中的 `chat_template` 或 `chat_template_tool_use` 属性来验证这一点。

以下是已知可以工作的模型（在需要时使用聊天模板覆盖）：

### 原生支持示例

```shell
# 原生支持：
llama-server --jinja -fa -hf bartowski/Qwen2.5-7B-Instruct-GGUF:Q4_K_M
llama-server --jinja -fa -hf bartowski/Mistral-Nemo-Instruct-2407-GGUF:Q6_K_L
llama-server --jinja -fa -hf bartowski/Llama-3.3-70B-Instruct-GGUF:Q4_K_M
```

### DeepSeek R1 原生支持（使用模板覆盖）

```shell
# DeepSeek R1 的原生支持使用我们的模板覆盖效果最好（官方模板有错误，尽管我们确实解决了它）

llama-server --jinja -fa -hf bartowski/DeepSeek-R1-Distill-Qwen-7B-GGUF:Q6_K_L \
    --chat-template-file models/templates/llama-cpp-deepseek-r1.jinja

llama-server --jinja -fa -hf bartowski/DeepSeek-R1-Distill-Qwen-32B-GGUF:Q4_K_M \
    --chat-template-file models/templates/llama-cpp-deepseek-r1.jinja
```

### 需要正确模板的原生支持

```shell
# 原生支持需要这些 GGUF 的正确模板：

llama-server --jinja -fa -hf bartowski/functionary-small-v3.2-GGUF:Q4_K_M \
    --chat-template-file models/templates/meetkai-functionary-medium-v3.2.jinja

llama-server --jinja -fa -hf bartowski/Hermes-2-Pro-Llama-3-8B-GGUF:Q4_K_M \
    --chat-template-file models/templates/NousResearch-Hermes-2-Pro-Llama-3-8B-tool_use.jinja

llama-server --jinja -fa -hf bartowski/Hermes-3-Llama-3.1-8B-GGUF:Q4_K_M \
    --chat-template-file models/templates/NousResearch-Hermes-3-Llama-3.1-8B-tool_use.jinja

llama-server --jinja -fa -hf bartowski/firefunction-v2-GGUF -hff firefunction-v2-IQ1_M.gguf \
    --chat-template-file models/templates/fireworks-ai-llama-3-firefunction-v2.jinja

llama-server --jinja -fa -hf bartowski/c4ai-command-r7b-12-2024-GGUF:Q6_K_L \
    --chat-template-file models/templates/CohereForAI-c4ai-command-r7b-12-2024-tool_use.jinja
```

### 通用格式支持

```shell
# 通用格式支持
llama-server --jinja -fa -hf bartowski/phi-4-GGUF:Q4_0
llama-server --jinja -fa -hf bartowski/gemma-2-2b-it-GGUF:Q8_0
llama-server --jinja -fa -hf bartowski/c4ai-command-r-v01-GGUF:Q2_K
```

### 获取官方模板

要从原始 HuggingFace 仓库获取官方模板，可以使用 [scripts/get_chat_template.py](../scripts/get_chat_template.py)（参见 [models/templates/README.md](../models/templates/README.md) 中的示例调用）

> [!TIP]
> 如果没有官方 `tool_use` Jinja 模板，您可能需要设置 `--chat-template chatml` 来使用适用于许多模型的默认模板（效果因人而异！），或者编写自己的模板（例如，我们为 DeepSeek R1 提供了一个自定义 [llama-cpp-deepseek-r1.jinja](../models/templates/llama-cpp-deepseek-r1.jinja)）

> [!CAUTION]
> 小心极端的 KV 量化（例如 `-ctk q4_0`），它们可能会显著降低模型的工具调用性能。

## 测试方法

在 CLI 中测试（或使用任何可以使用 OpenAI 兼容 API 后端的库/软件）：

### 示例 1：Python 代码执行

```bash
curl http://localhost:8080/v1/chat/completions -d '{
    "model": "gpt-3.5-turbo",
    "tools": [
        {
        "type":"function",
        "function":{
            "name":"python",
            "description":"Runs code in an ipython interpreter and returns the result of the execution after 60 seconds.",
            "parameters":{
            "type":"object",
            "properties":{
                "code":{
                "type":"string",
                "description":"The code to run in the ipython interpreter."
                }
            },
            "required":["code"]
            }
        }
        }
    ],
    "messages": [
        {
        "role": "user",
        "content": "Print a hello world message with python."
        }
    ]
}'
```

### 示例 2：天气查询

```bash
curl http://localhost:8080/v1/chat/completions -d '{
    "model": "gpt-3.5-turbo",
    "messages": [
        {"role": "system", "content": "You are a chatbot that uses tools/functions. Dont overthink things."},
        {"role": "user", "content": "What is the weather in Istanbul?"}
    ],
    "tools": [{
        "type":"function",
        "function":{
            "name":"get_current_weather",
            "description":"Get the current weather in a given location",
            "parameters":{
                "type":"object",
                "properties":{
                    "location":{
                    "type":"string",
                    "description":"The city and country/state, e.g. `San Francisco, CA`, or `Paris, France`"
                    }
                },
                "required":["location"]
            }
        }
    }]
}'
```

### 输出示例

```json
{
"choices": [
    {
    "finish_reason": "tool",
    "index": 0,
    "message": {
        "content": null,
        "tool_calls": [
        {
            "name": "python",
            "arguments": "{\"code\":\" \\nprint(\\\"Hello, World!\\\")\"}"
        }
        ],
        "role": "assistant"
    }
    }
],
"created": 1727287211,
"model": "gpt-3.5-turbo",
"object": "chat.completion",
"usage": {
    "completion_tokens": 16,
    "prompt_tokens": 44,
    "total_tokens": 60
},
"id": "chatcmpl-Htbgh9feMmGM0LEH2hmQvwsCxq3c6Ni8"
}
```

## 函数调用的实现机制

### 原生格式处理

llama.cpp 识别并处理多种模型的原生函数调用格式，包括：

1. **Llama 3.x 系列**：支持内置工具和标准函数调用
2. **Hermes 系列**：包括 Hermes 2 Pro 和 Hermes 3
3. **Functionary 系列**：专门为函数调用训练的模型
4. **Mistral Nemo**：Mistral 的较新模型
5. **Qwen 2.5 系列**：包括通用的和代码专用的版本
6. **其他**：DeepSeek R1、Command R7B、Firefunction v2 等

### 通用格式处理

当模型不被原生格式处理程序识别时，llama.cpp 会回退到通用格式处理：

- 使用标准的 OpenAI 兼容 API
- 可能消耗更多 token
- 效率低于原生格式

## 函数调用的配置方法

### 服务器启动参数

- `--jinja`：启用 Jinja 模板处理（必需）
- `--chat-template-file`：覆盖默认聊天模板
- `--fa`：启用 Flash Attention（推荐）

### 模板选择

根据模型选择合适的模板：

1. 检查模型的原生支持
2. 如果没有原生支持，使用通用模板
3. 必要时创建自定义模板

### KV Cache 配置

- 避免极端的 KV 量化
- 推荐使用 `f16` 或更高精度的 KV cache

## 函数调用的使用示例

### 工具定义

```json
{
    "type": "function",
    "function": {
        "name": "function_name",
        "description": "Function description",
        "parameters": {
            "type": "object",
            "properties": {
                "param_name": {
                    "type": "string",
                    "description": "Parameter description"
                }
            },
            "required": ["param_name"]
        }
    }
}
```

### 并行工具调用

在请求中添加：
```json
{
    "parallel_tool_calls": true
}
```

### 响应格式

```json
{
    "choices": [{
        "finish_reason": "tool",
        "message": {
            "role": "assistant",
            "content": null,
            "tool_calls": [{
                "name": "function_name",
                "arguments": "{\"param\": \"value\"}"
            }]
        }
    }]
}
```

## 支持的模型列表摘要

### 完全原生支持的模型系列

- **Llama 3.x**：3.1、3.2、3.3
- **Hermes 2/3**
- **Qwen 2.5**：包括通用和 Coder 版本
- **Mistral Nemo**
- **Functionary v3.1/v3.2**
- **Firefunction v2**
- **Command R7B**
- **DeepSeek R1**

### 需要模板覆盖的模型

- 部分 Hermes 模型
- Functionary GGUF 版本
- DeepSeek R1（推荐使用自定义模板）
- Command R7B

### 通用格式支持的模型

- 大多数其他模型
- 包括 Gemma、Phi、多种微调模型等