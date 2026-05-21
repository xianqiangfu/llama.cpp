# llama.cpp API 调用教程

本教程详细介绍 llama.cpp HTTP 服务器的 API 调用方法和最佳实践。

## 目录

- [API 概述](#api-概述)
- [原生 API](#原生-api)
- [OpenAI 兼容 API](#openai-兼容-api)
- [Anthropic 兼容 API](#anthropic-兼容-api)
- [其他 API](#其他-api)
- [客户端库](#客户端库)
- [高级功能](#高级功能)
- [性能优化](#性能优化)
- [错误处理](#错误处理)

## API 概述

### 支持的 API 类型

llama-server 提供以下 API：

1. **原生 API**: llama.cpp 特有的 API
2. **OpenAI 兼容 API**: 与 OpenAI API 兼容的接口
3. **Anthropic 兼容 API**: Anthropic Messages API
4. **嵌入 API**: 文本嵌入生成
5. **重排序 API**: 文档重排序

### API 基础

**基础 URL:**
```bash
http://localhost:8080
```

**认证:**
```bash
# API 密钥认证
Authorization: Bearer your-api-key
```

**响应格式:**
```json
{
  "status": "success",
  "data": {
    "content": "生成的内容",
    "tokens": [...],
    "timings": {...}
  }
}
```

## 原生 API

### 完成 API

```bash
# 基础完成请求
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "你好，请介绍一下你自己",
        "n_predict": 50
    }'

# 流式完成
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "请写一首关于春天的诗",
        "n_predict": 100,
        "stream": true
    }'
```

### 聊天完成 API

```bash
# 聊天完成（支持多模态）
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": {
            "prompt_string": "分析这张图片的内容",
            "multimodal_data": ["base64_image_data"]
        },
        "n_predict": 100,
        "stream": true
    }'
```

### 嵌入 API

```bash
# 文本嵌入
curl http://localhost:8080/embedding \
    -H "Content-Type: application/json" \
    -d '{
        "content": "这是一个测试文本",
        "embd_normalize": 2
    }'

# 批量嵌入
curl http://localhost:8080/embedding \
    -H "Content-Type: application/json" \
    -d '{
        "content": ["文本1", "文本2", "文本3"],
        "embd_normalize": 2
    }'
```

### 重排序 API

```bash
# 文档重排序
curl http://localhost:8080/reranking \
    -H "Content-Type: application/json" \
    -d '{
        "query": "什么是机器学习？",
        "documents": [
            "机器学习是一种人工智能技术",
            "深度学习是机器学习的一个分支",
            "自然语言处理是机器学习的应用"
        ],
        "top_n": 3
    }'
```

## OpenAI 兼容 API

### 聊天完成 API

```bash
# 基础对话完成
curl http://localhost:8080/v1/chat/completions \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "gpt-3.5-turbo",
        "messages": [
            {"role": "system", "content": "你是一个有帮助的助手"},
            {"role": "user", "content": "你好"}
        ],
        "max_tokens": 100,
        "temperature": 0.8
    }'

# 流式对话
curl http://localhost:8080/v1/chat/completions \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "gpt-3.5-turbo",
        "messages": [
            {"role": "user", "content": "写一首关于春天的诗"}
        ],
        "stream": true
    }'
```

### 完成 API

```bash
# 基础文本完成
curl http://localhost:8080/v1/completions \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "gpt-3.5-turbo",
        "prompt": "人工智能的未来",
        "max_tokens": 100
    }'
```

### 嵌入 API

```bash
# 图像 + 文本处理
curl http://localhost:8080/v1/chat/completions \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "gpt-4",
        "messages": [
            {
                "role": "user",
                "content": [
                    {"type": "text", "text": "请描述这张图片"},
                    {"type": "image_url", "image_url": {"url": "http://example.com/image.jpg"}}
                ]
            }
        ],
        "max_tokens": 200
    }'
```

### 嵌入 API

```bash
# 文本嵌入
curl http://localhost:8080/v1/embeddings \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "text-embedding-ada-002",
        "input": "这是一个测试文本",
        "encoding_format": "float"
    }'

# 批量嵌入
curl http://localhost:8080/v1/embeddings \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer your-api-key" \
    -d '{
        "model": "text-embedding-ada-002",
        "input": ["文本1", "文本2", "文本3"],
        "encoding_format": "float"
    }'
```

## Anthropic 兼容 API

### Messages API

```bash
# 基础消息处理
curl http://localhost:8080/v1/messages \
    -H "Content-Type: application/json" \
    -H "x-api-key: your-api-key" \
    -d '{
        "model": "claude-3-opus-20240229",
        "max_tokens": 200,
        "system": "你是一个专业的AI助手",
        "messages": [
            {"role": "user", "content": "你好，请介绍一下你自己"}
        ]
    }'

# 流式消息处理
curl http://localhost:8080/v1/messages \
    -H "Content-Type: application/json" \
    -H "x-api-key: your-api-key" \
    -d '{
        "model": "claude-3-opus-20240229",
        "max_tokens": 100,
        "stream": true,
        "messages": [
            {"role": "user", "content": "请写一段关于人工智能的内容"}
        ]
    }'
```

### Token 计数

```bash
# 计算 token 数量
curl http://localhost:8080/v1/messages/count_tokens \
    -H "Content-Type: application/json" \
    -H "x-api-key: your-api-key" \
    -d '{
        "model": "claude-3-opus-20240229",
        "messages": [
            {"role": "user", "content": "你好，请介绍一下你自己"}
        ]
    }'
```

## 其他 API

### 模板应用 API

```bash
# 应用聊天模板
curl http://localhost:8080/apply-template \
    -H "Content-Type: application/json" \
    -d '{
        "messages": [
            {"role": "system", "content": "你是一个专业的助手"},
            {"role": "user", "content": "你好"}
        ]
    }'
```

### Token化 API

```bash
# 文本 Token 化
curl http://localhost:8080/tokenize \
    -H "Content-Type: application/json" \
    -d '{
        "content": "你好，请介绍一下你自己",
        "add_special": true
    }'

# Token 解码
curl http://localhost:8080/detokenize \
    -H "Content-Type: application/json" \
    -d '{
        "tokens": [123, 456, 789]
    }'
```

## 客户端库

### Python 客户端

```python
import requests
import json

class LlamaClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        
    def completion(self, prompt, max_tokens=100, **kwargs):
        """原生完成 API"""
        response = requests.post(
            f"{self.base_url}/completion",
            json={
                "prompt": prompt,
                "n_predict": max_tokens,
                **kwargs
            }
        )
        return response.json()
    
    def chat_completion(self, messages, max_tokens=100, **kwargs):
        """OpenAI 聊天完成 API"""
        response = requests.post(
            f"{self.base_url}/v1/chat/completions",
            json={
                "model": "gpt-3.5-turbo",
                "messages": messages,
                "max_tokens": max_tokens,
                **kwargs
            }
        )
        return response.json()
    
    def streaming_completion(self, prompt, max_tokens=100, **kwargs):
        """流式完成 API"""
        response = requests.post(
            f"{self.base_url}/completion",
            json={
                "prompt": prompt,
                "n_predict": max_tokens,
                "stream": True,
                **kwargs
            },
            stream=True
        )
        
        for line in response.iter_lines():
            if line:
                yield json.loads(line)
    
    def embedding(self, content, **kwargs):
        """嵌入 API"""
        response = requests.post(
            f"{self.base_url}/v1/embeddings",
            json={
                "model": "text-embedding-ada-002",
                "input": content,
                **kwargs
            }
        )
        return response.json()

# 使用示例
client = LlamaClient()

# 基础完成
result = client.completion("你好，请介绍一下你自己", max_tokens=50)
print(result['content'])

# 聊天完成
messages = [
    {"role": "system", "content": "你是一个有帮助的助手"},
    {"role": "user", "content": "写一首关于春天的诗"}
]
result = client.chat_completion(messages, max_tokens=100)
print(result['choices'][0]['message']['content'])

# 流式完成
for chunk in client.streaming_completion("写一个短故事", max_tokens=200):
    if 'content' in chunk:
        print(chunk['content'], end='', flush=True)
```

### JavaScript 客户端

```javascript
class LlamaClient {
    constructor(baseUrl = 'http://localhost:8080') {
        this.baseUrl = baseUrl;
    }

    async completion(prompt, options = {}) {
        const response = await fetch(`${this.baseUrl}/completion`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                prompt,
                ...options
            })
        });
        return response.json();
    }

    async chatCompletion(messages, options = {}) {
        const response = await fetch(`${this.baseUrl}/v1/chat/completions`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                model: 'gpt-3.5-turbo',
                messages,
                ...options
            })
        });
        return response.json();
    }

    async streamingCompletion(prompt, options = {}) {
        const response = await fetch(`${this.baseUrl}/completion`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                prompt,
                stream: true,
                ...options
            })
        });

        const reader = response.body.getReader();
        const decoder = new TextDecoder();

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;

            const text = decoder.decode(value);
            const lines = text.split('\n').filter(line => line.trim());
            
            for (const line of lines) {
                try {
                    yield JSON.parse(line);
                } catch (e) {
                    // 忽略解析错误
                }
            }
        }
    }
}

// 使用示例
const client = new LlamaClient();

// 基础完成
async function basicCompletion() {
    const result = await client.completion('你好，请介绍一下你自己', { n_predict: 50 });
    console.log(result.content);
}

// 聊天完成
async function chatCompletion() {
    const messages = [
        { role: 'system', content: '你是一个有帮助的助手' },
        { role: 'user', content: '写一首关于春天的诗' }
    ];
    const result = await client.chatCompletion(messages, { max_tokens: 100 });
    console.log(result.choices[0].message.content);
}

// 流式完成
async function streamingCompletion() {
    for await (const chunk of client.streamingCompletion('写一个短故事', { n_predict: 200 })) {
        if (chunk.content) {
            process.stdout.write(chunk.content);
        }
    }
    console.log();
}

// 运行示例
basicCompletion();
chatCompletion();
streamingCompletion();
```

## 高级功能

### 语法约束

```bash
# JSON 约束
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "生成一个用户信息对象",
        "json_schema": "{\"type\": \"object\", \"properties\": {\"name\": {\"type\": \"string\"}, \"age\": {\"type\": \"number\"}}}",
        "n_predict": 100
    }'

# 语法文件约束
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "生成一个简单的算术表达式",
        "grammar_file": "grammars/arithmetic.gbnf",
        "n_predict": 50
    }'
```

### Logit 偏置

```bash
# 调整 token 概率
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "增加特定 token 的出现概率",
        "logit_bias": [[15043, 1.0], [15043, -1.0]],
        "n_predict": 100
    }'

# 禁止特定 token
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "生成不包含特定内容的文本",
        "logit_bias": [[15043, false]],
        "n_predict": 100
    }'
```

## 性能优化

### 批量请求

```bash
#!/bin/bash
# batch-requests.sh

model="gpt-3.5-turbo"
prompts=(
    "你好，请介绍一下你自己"
    "写一首关于春天的诗"
    "人工智能的应用领域有哪些"
)

# 并发请求
for prompt in "${prompts[@]}"; do
    (
        curl -s http://localhost:8080/v1/chat/completions \
            -H "Content-Type: application/json" \
            -d "{\"model\": \"$model\", \"messages\": [{\"role\": \"user\", \"content\": \"$prompt\"}], \"max_tokens\": 100}" > /dev/null
    ) &
done

wait
echo "所有批请求完成"
```

### 连接池

```python
import requests
from concurrent.futures import ThreadPoolExecutor

class LlamaConnectionPool:
    def __init__(self, base_url="http://localhost:8080", max_connections=10):
        self.base_url = base_url
        self.max_connections = max_connections
        self.executor = ThreadPoolExecutor(max_connections)
        
    def completion(self, prompt, **kwargs):
        """使用连接池的完成 API"""
        future = self.executor.submit(
            requests.post,
            f"{self.base_url}/completion",
            json={"prompt": prompt, **kwargs}
        )
        return future.result()
    
    def batch_completion(self, prompts, **kwargs):
        """批量完成 API"""
        futures = [self.completion(prompt, **kwargs) for prompt in prompts]
        return [future.result() for future in futures]

# 使用示例
pool = LlamaConnectionPool()

# 单个请求
result = pool.completion("你好，请介绍一下你自己", max_tokens=50)
print(result['content'])

# 批量请求
prompts = ["问题1", "问题2", "问题3", "问题4"]
results = pool.batch_completion(prompts, max_tokens=50)
for i, result in enumerate(results):
    print(f"结果 {i+1}: {result['content']}")
```

### 缓存策略

```python
import requests
import hashlib
import json
from functools import lru_cache

class LlamaCacheClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        # 响应缓存
        self.response_cache = lru_cache(maxsize=100)
        
    def _generate_cache_key(self, prompt, **kwargs):
        """生成缓存键"""
        cache_data = json.dumps({
            "prompt": prompt,
            **sorted(kwargs)
        })
        return hashlib.md5(cache_data.encode()).hexdigest()
    
    @lru_cache(maxsize=100)
    def completion(self, prompt, use_cache=True, **kwargs):
        """带缓存的完成 API"""
        cache_key = self._generate_cache_key(prompt, **kwargs)
        
        if use_cache and cache_key in self.response_cache:
            print(f"使用缓存结果: {cache_key}")
            return self.response_cache[cache_key]
        
        # 发起请求
        response = requests.post(
            f"{self.base_url}/completion",
            json={"prompt": prompt, **kwargs}
        )
        result = response.json()
        
        # 缓存响应
        if use_cache:
            self.response_cache[cache_key] = result
        
        return result
    
    def clear_cache(self):
        """清理缓存"""
        self.response_cache.clear()
        print("缓存已清理")

# 使用示例
client = LlamaCacheClient()

# 首次请求（使用缓存）
result1 = client.completion("你好，请介绍一下你自己", use_cache=True)
print(result1['content'])

# 相同请求（使用缓存）
result2 = client.completion("你好，请介绍一下你自己", use_cache=True)
print(result2['content'])  # 从缓存获取

# 清理缓存
client.clear_cache()

# 后续请求（重新缓存）
result3 = client.completion("你好，请介绍一下你自己", use_cache=False)
print(result3['content'])
```

## 错误处理

### 重试机制

```python
import requests
import time
from backoff import expo, on_exception

class LlamaRetryClient:
    def __init__(self, base_url="http://localhost:8080", max_retries=3):
        self.base_url = base_url
        self.max_retries = max_retries
        
    @expo(max_retries, on_exception(requests.exceptions.RequestException))
    def completion(self, prompt, **kwargs):
        """带重试的完成 API"""
        response = requests.post(
            f"{self.base_url}/completion",
            json={"prompt": prompt, **kwargs}
        )
        return response.json()
    
    def chat_completion(self, messages, **kwargs):
        """带重试的聊天完成 API"""
        response = requests.post(
            f"{self.base_url}/v1/chat/completions",
            json={
                "model": "gpt-3.5-turbo",
                "messages": messages,
                **kwargs
            }
        )
        return response.json()

# 使用示例
client = LlamaRetryClient(max_retries=3)

# 自动重试的请求
try:
    result = client.completion("你好，请介绍一下你自己", max_tokens=50)
    print(result['content'])
except Exception as e:
    print(f"请求失败: {e}")
    print("正在重试...")
```

### 错误处理

```python
import requests
from typing import Optional

class LlamaSafeClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        
    def completion(self, prompt: Optional[str] = None, **kwargs) -> dict:
        """安全的完成 API，带错误处理"""
        try:
            if not prompt:
                raise ValueError("提示词不能为空")
            
            response = requests.post(
                f"{self.base_url}/completion",
                json={"prompt": prompt, **kwargs}
            )
            response.raise_for_status()
            
            return {
                "success": True,
                "data": response.json(),
                "status_code": response.status_code
            }
            
        except requests.exceptions.RequestException as e:
            return {
                "success": False,
                "error": str(e),
                "error_type": "network_error"
            }
        except ValueError as e:
            return {
                "success": False,
                "error": str(e),
                "error_type": "validation_error"
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e),
                "error_type": "unknown_error"
            }
    
    def handle_response(self, result: dict) -> None:
        """处理响应结果"""
        if result['success']:
            print(f"成功: {result['data']['content']}")
        else:
            error_type = result.get('error_type', 'unknown')
            print(f"错误: {result['error']} (类型: {error_type})")
            
            # 根据错误类型采取不同措施
            if error_type == 'network_error':
                print("建议：检查网络连接，稍后重试")
            elif error_type == 'validation_error':
                print("建议：检查输入参数")
            else:
                print("建议：查看详细错误信息")

# 使用示例
client = LlamaSafeClient()

# 成功请求
result = client.completion("你好，请介绍一下你自己", max_tokens=50)
client.handle_response(result)

# 错误请求（空提示词）
result = client.completion("", max_tokens=50)
client.handle_response(result)

# 网络错误请求（假设）
# 注意：这里需要实际的错误情况来测试
print("测试网络错误处理...")

# 参数验证错误
result = client.completion(None, max_tokens=-1)  # 无效的参数
client.handle_response(result)
```

---

*恭喜！你已经完成了 llama.cpp API 调用的全面学习。现在你已经具备了使用各种 API 的技能，可以根据自己的需求选择合适的 API 接口和客户端库。*