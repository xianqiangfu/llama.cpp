# llama-server 完整使用教程

本教程全面介绍 llama-server HTTP 服务器的部署、配置和高级应用。

## 目录

- [服务器概述](#服务器概述)
- [完整配置参考](#完整配置参考)
- [部署架构](#部署架构)
- [性能优化](#性能优化)
- [高级功能](#高级功能)
- [API 开发](#api-开发)
- [运维管理](#运维管理)
- [最佳实践](#最佳实践)

## 服务器概述

### llama-server 架构

```
llama-server
├── HTTP 服务层
│   ├── 请求路由
│   ├── 认证授权
│   └── 响应处理
├── 推理管理层
│   ├── 模型加载器
│   ├── 会话管理器
│   └── 批处理调度器
├── 性能优化层
│   ├── KV 缓存管理
│   ├── 内存池管理
│   └── GPU 调度
└── 监控管理层
    ├── 性能监控
    ├── 日志管理
    └── 健康检查
```

### 服务能力

1. **多模型支持**: 同时服务多个模型
2. **并发处理**: 支持多用户并发访问
3. **API 兼容**: OpenAI 和 Anthropic API 兼容
4. **流式响应**: 实时流式输出
5. **模版系统**: Jinja2 模板支持
6. **多模态**: 图像和音频处理

## 完整配置参考

### 基础配置

```bash
# 最小配置
llama-server -m model.gguf

# 基础配置
llama-server -m model.gguf \
    --host 0.0.0.0 \
    --port 8080 \
    -c 8192 \
    -t 8 \
    -np 4
```

### 性能配置

```bash
# 高性能配置
llama-server -m model.gguf \
    --host 0.0.0.0 \
    --port 8080 \
    -c 16384 \
    -t 16 \
    -b 1024 \
    -ub 512 \
    -np 8 \
    -ngl 99 \
    -fa on \
    --cache-prompt
```

### GPU 配置

```bash
# 单 GPU 配置
llama-server -m model.gguf \
    -ngl 99 \
    -dev cuda:0

# 多 GPU 配置
llama-server -m model.gguf \
    -ts 0.5,0.5 \
    -sm row \
    -dev cuda:0,cuda:1

# GPU + CPU 混合
llama-server -m model.gguf \
    -ngl 50 \
    -t 8 \
    -dev cuda:0,cuda:1
```

### 采样配置

```bash
# 精确控制
llama-server -m model.gguf \
    --temp 0.8 \
    --top-k 40 \
    --top-p 0.95 \
    --min-p 0.05 \
    --repeat-penalty 1.1 \
    --repeat-last-n 64

# 创意模式
llama-server -m model.gguf \
    --temp 1.2 \
    --top-p 0.98 \
    --min-p 0.02

# 确定模式
llama-server -m model.gguf \
    --temp 0.2 \
    --top-k 10 \
    --top-p 0.8
```

## 部署架构

### 单服务器部署

```yaml
# 单服务器架构
services:
  llama-server:
    image: ghcr.io/ggml-org/llama.cpp:server
    ports:
      - "8080:8080"
    volumes:
      - ./models:/models
    environment:
      - LLAMA_ARG_MODEL=/models/model.gguf
      - LLAMA_ARG_N_PARALLEL=4
      - LLAMA_ARG_N_GPU_LAYERS=99
    restart: unless-stopped
```

### 高可用部署

```yaml
# 高可用架构
services:
  llama-server-1:
    image: ghcr.io/ggml-org/llama.cpp:server
    ports:
      - "8081:8080"
    volumes:
      - ./models:/models
    environment:
      - LLAMA_ARG_MODEL=/models/model.gguf
      - LLAMA_ARG_PORT=8080
    restart: unless-stopped

  llama-server-2:
    image: ghcr.io/ggml-org/llama.cpp:server
    ports:
      - "8082:8080"
    volumes:
      - ./models:/models
    environment:
      - LLAMA_ARG_MODEL=/models/model.gguf
      - LLAMA_ARG_PORT=8080
    restart: unless-stopped

  nginx:
    image: nginx:alpine
    ports:
      - "80:80"
    volumes:
      - ./nginx/nginx.conf:/etc/nginx/nginx.conf:ro
    depends_on:
      - llama-server-1
      - llama-server-2
    restart: unless-stopped
```

### 负载均衡配置

```nginx
# nginx.conf
upstream llama_backend {
    least_conn;
    server llama-server-1:8080 weight=1 max_fails=3 fail_timeout=30s;
    server llama-server-2:8080 weight=1 max_fails=3 fail_timeout=30s;
    keepalive 32;
}

server {
    listen 80;
    
    location / {
        proxy_pass http://llama_backend;
        proxy_http_version 1.1;
        proxy_set_header Connection "";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        
        # 超时设置
        proxy_connect_timeout 300s;
        proxy_send_timeout 300s;
        proxy_read_timeout 300s;
        
        # SSE 支持
        proxy_buffering off;
        chunked_transfer_encoding off;
    }
}
```

## 性能优化

### 缓存优化

```bash
# 启用提示词缓存
llama-server -m model.gguf \
    --cache-prompt \
    --cache-reuse 128

# KV 缓存配置
llama-server -m model.gguf \
    -kvo \
    -ctk f16 \
    -ctv f16 \
    -cram 4096

# 上下文检查点
llama-server -m model.gguf \
    --context-shift \
    -ctxcp 32 \
    -cpent 8192
```

### 并发优化

```bash
# 并发配置
llama-server -m model.gguf \
    -np 8 \
    --cont-batching \
    -cb

# 线程配置
llama-server -m model.gguf \
    -t 8 \
    -tb 16 \
    --threads-http 4

# 超时配置
llama-server -m model.gguf \
    --timeout 600 \
    --poll 50
```

### GPU 优化

```bash
# GPU 内存优化
llama-server -m model.gguf \
    -ngl 99 \
    -cmoe \
    --repack \
    -fit

# 多 GPU 优化
llama-server -m model.gguf \
    -ts 0.3,0.7 \
    -sm tensor \
    -mg 0

# Flash Attention
llama-server -m model.gguf \
    -fa auto \
    --swa-full
```

## 高级功能

### 多模型路由

```bash
# 启动路由服务器
llama-server \
    --models-dir ./models \
    --models-preset ./presets.ini \
    --models-max 4 \
    --models-autoload
```

```ini
# presets.ini
[default]
model = /models/gemma-3-1b-it-Q4_K_M.gguf
n_parallel = 2
ctx_size = 8192

[code-assistant]
model = /models/code-model-Q4_K_M.gguf
n_parallel = 4
ctx_size = 16384
alias = code-assistant
tags = code,programming

[creative-writing]
model = /models/creative-model-Q5_K_M.gguf
n_parallel = 2
ctx_size = 8192
alias = creative
tags = creative,writing
```

### 投机解码

```bash
# 启用投机解码
llama-server -m model.gguf \
    -md draft-model.gguf \
    --spec-draft-n-max 16 \
    --spec-draft-n-min 4 \
    --spec-draft-p-min 0.9

# N-gram 投机解码
llama-server -m model.gguf \
    --spec-type ngram-mod \
    --spec-ngram-mod-n-min 48 \
    --spec-ngram-mod-n-max 64
```

### 语法约束

```bash
# JSON 模式
llama-server -m model.gguf \
    --json-schema '{"type": "object"}'

# 语法文件
llama-server -m model.gguf \
    --grammar-file grammars/json.gbnf

# 动态语法
curl http://localhost:8080/completion \
    -H "Content-Type: application/json" \
    -d '{
        "prompt": "生成数据",
        "grammar": "root ::= value",
        "n_predict": 100
    }'
```

## API 开发

### Python 客户端

```python
import requests
import json

class LlamaClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        
    def completion(self, prompt, max_tokens=100, temperature=0.8):
        """完成 API"""
        response = requests.post(
            f"{self.base_url}/completion",
            json={
                "prompt": prompt,
                "n_predict": max_tokens,
                "temperature": temperature
            }
        )
        return response.json()
    
    def chat_completion(self, messages, max_tokens=100, temperature=0.8):
        """聊天完成 API"""
        response = requests.post(
            f"{self.base_url}/v1/chat/completions",
            json={
                "messages": messages,
                "max_tokens": max_tokens,
                "temperature": temperature
            }
        )
        return response.json()
    
    def streaming_completion(self, prompt, max_tokens=100):
        """流式完成"""
        response = requests.post(
            f"{self.base_url}/completion",
            json={
                "prompt": prompt,
                "n_predict": max_tokens,
                "stream": True
            },
            stream=True
        )
        
        for line in response.iter_lines():
            if line:
                yield json.loads(line)

# 使用示例
client = LlamaClient()

# 基础完成
result = client.completion("你好，请介绍一下你自己")
print(result['content'])

# 聊天完成
messages = [
    {"role": "system", "content": "你是一个有帮助的助手"},
    {"role": "user", "content": "写一首关于春天的诗"}
]
result = client.chat_completion(messages)
print(result['choices'][0]['message']['content'])

# 流式完成
for chunk in client.streaming_completion("写一个短故事", max_tokens=200):
    if 'content' in chunk:
        print(chunk['content'], end='', flush=True)
print()
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
                messages,
                ...options
            })
        });
        return response.json();
    }

    async *streamingCompletion(prompt, options = {}) {
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
    const result = await client.completion('你好，请介绍一下你自己');
    console.log(result.content);
}

// 聊天完成
async function chatCompletion() {
    const messages = [
        { role: 'system', content: '你是一个有帮助的助手' },
        { role: 'user', content: '写一首关于春天的诗' }
    ];
    const result = await client.chatCompletion(messages);
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

## 运维管理

### 监控脚本

```bash
#!/bin/bash
# monitor.sh

server_url="http://localhost:8080"

# 健康检查
check_health() {
    response=$(curl -s "$server_url/health")
    
    if [ "$response" == '{"status":"ok"}' ]; then
        echo "✓ 服务器健康"
        return 0
    else
        echo "✗ 服务器不健康: $response"
        return 1
    fi
}

# 性能监控
monitor_performance() {
    echo "=== 性能监控 ==="
    
    # 获取属性
    props=$(curl -s "$server_url/props")
    
    # 提取指标
    total_slots=$(echo "$props" | jq '.total_slots')
    model_path=$(echo "$props" | jq -r '.model_path')
    
    echo "总插槽数: $total_slots"
    echo "模型路径: $model_path"
    
    # 获取插槽状态
    slots=$(curl -s "$server_url/slots")
    
    processing_slots=$(echo "$slots" | jq '[.[] | select(.is_processing == true)] | length')
    
    echo "正在处理的请求: $processing_slots"
}

# 资源使用
monitor_resources() {
    echo "=== 资源使用 ==="
    
    # CPU 使用
    cpu_usage=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'%' -f1)
    echo "CPU 使用率: ${cpu_usage}%"
    
    # 内存使用
    memory_info=$(free -h | grep Mem)
    echo "内存使用: $memory_info"
    
    # 磁盘使用
    disk_info=$(df -h / | tail -1)
    echo "磁盘使用: $disk_info"
}

# 主循环
while true; do
    clear
    echo "=== llama-server 监控 ==="
    echo "时间: $(date)"
    echo ""
    
    check_health
    monitor_performance
    monitor_resources
    
    sleep 5
done
```

### 日志管理

```bash
#!/bin/bash
# log-manager.sh

log_dir="/var/log/llama-server"
max_log_files=10

# 创建日志目录
mkdir -p "$log_dir"

# 日志轮转
rotate_logs() {
    # 压缩旧日志
    find "$log_dir" -name "*.log" -mtime +1 -exec gzip {} \;
    
    # 删除过多日志
    find "$log_dir" -name "*.log.gz" -type f | sort | head -n -$max_log_files | xargs rm
}

# 日志分析
analyze_logs() {
    echo "=== 日志分析 ==="
    
    # 错误统计
    error_count=$(grep -c "ERROR" "$log_dir"/llama-server.log)
    echo "错误数量: $error_count"
    
    # 警告统计
    warning_count=$(grep -c "WARNING" "$log_dir"/llama-server.log)
    echo "警告数量: $warning_count"
    
    # 性能统计
    avg_tokens_per_sec=$(grep "tokens per second" "$log_dir"/llama-server.log | \
        awk '{sum+=$NF; count++} END {print sum/count}')
    echo "平均 tokens/秒: $avg_tokens_per_sec"
}

# 清理日志
cleanup_logs() {
    echo "清理日志..."
    > "$log_dir"/llama-server.log
}

# 使用示例
case "$1" in
    rotate)
        rotate_logs
        ;;
    analyze)
        analyze_logs
        ;;
    cleanup)
        cleanup_logs
        ;;
    *)
        echo "使用方法: $0 {rotate|analyze|cleanup}"
        ;;
esac
```

### 备份恢复

```bash
#!/bin/bash
# backup.sh

backup_dir="/backup/llama-server"
timestamp=$(date +%Y%m%d_%H%M%S)

# 备份配置
backup_config() {
    mkdir -p "$backup_dir"
    
    # 备份模型配置
    cp -r /opt/llama.cpp/models "$backup_dir/models_$timestamp"
    
    # 备份配置文件
    cp /etc/default/llama-server "$backup_dir/config_$timestamp"
    
    # 备份日志
    cp -r /var/log/llama-server "$backup_dir/logs_$timestamp"
    
    echo "备份完成: $backup_dir"
}

# 恢复配置
restore_config() {
    local backup_id="$1"
    
    if [ -z "$backup_id" ]; then
        echo "请指定备份 ID"
        return 1
    fi
    
    # 恢复模型
    cp -r "$backup_dir/models_$backup_id" /opt/llama.cpp/models
    
    # 恢复配置
    cp "$backup_dir/config_$backup_id" /etc/default/llama-server
    
    # 重启服务
    systemctl restart llama-server
    
    echo "恢复完成: $backup_id"
}

# 使用示例
case "$1" in
    backup)
        backup_config
        ;;
    restore)
        restore_config "$2"
        ;;
    *)
        echo "使用方法: $0 {backup|restore <backup_id>}"
        ;;
esac
```

## 最佳实践

### 1. 部署策略

- 使用容器化部署提高可移植性
- 实施负载均衡提高可用性
- 配置自动扩缩容适应负载变化
- 建立灾备计划确保业务连续性

### 2. 性能优化

- 合理配置缓存策略减少响应时间
- 优化批处理提高吞吐量
- 启用 GPU 加速提升计算性能
- 监控性能指标及时发现问题

### 3. 安全管理

- 实施 API 密钥认证
- 配置防火墙规则限制访问
- 启用 SSL/TLS 加密传输
- 定期更新组件修复漏洞

### 4. 运维管理

- 建立监控告警系统
- 实施日志轮转和分析
- 定期备份重要数据
- 制定故障应急预案

---

*恭喜！你已经完成了 llama.cpp 的完整学习路径。现在你已经具备了使用 llama.cpp 进行 AI 推理的所有知识和技能。*