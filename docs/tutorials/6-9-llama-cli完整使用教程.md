# llama-cli 完整使用教程

本教程全面介绍 llama-cli 命令行工具的所有功能和高级用法。

## 目录

- [工具概述](#工具概述)
- [完整参数参考](#完整参数参考)
- [采样策略详解](#采样策略详解)
- [模型管理](#模型管理)
- [性能优化](#性能优化)
- [高级功能](#高级功能)
- [脚本化应用](#脚本化应用)
- [故障排查](#故障排查)

## 工具概述

### llama-cli 功能架构

```
llama-cli
├── 模型加载系统
│   ├── 本地模型加载
│   ├── Hugging Face 下载
│   └── 模型缓存管理
├── 文本处理系统
│   ├── 提示词处理
│   ├── 模板应用
│   └── 多模态处理
├── 推理引擎
│   ├── CPU 后端
│   ├── GPU 后端
│   └── 混合后端
├── 采样系统
│   ├── 温度采样
│   ├── Top-k 采样
│   ├── Top-p 采样
│   └── 高级采样器
└── 输出系统
    ├── 流式输出
    ├── 彩色输出
    └── 性能统计
```

### 使用场景

1. **交互式对话**: 与 AI 进行多轮对话
2. **文本生成**: 创意写作、内容创作
3. **代码生成**: 编程辅助、代码补全
4. **批量处理**: 文件批量处理
5. **模型测试**: 性能基准测试

## 完整参数参考

### 核心参数

**模型相关:**
```bash
-m, --model           模型文件路径
-mu, --model-url      模型下载 URL
-hf, --hf-repo        Hugging Face 模型仓库
-hff, --hf-file       Hugging Face 模型文件
```

**上下文和生成:**
```bash
-c, --ctx-size        上下文大小 (0 = 从模型加载)
-n, --n-predict       生成 token 数量 (-1 = 无限)
-b, --batch-size      逻辑批处理大小
-ub, --ubatch-size    物理批处理大小
```

**线程和性能:**
```bash
-t, --threads         生成线程数
-tb, --threads-batch  批处理线程数
-dev, --device        使用的设备列表
-ngl, --n-gpu-layers  GPU 层数
```

### 采样参数

**基础采样:**
```bash
--temp, --temperature  温度参数 (默认 0.80)
--top-k               Top-k 采样 (默认 40)
--top-p               Top-p 采样 (默认 0.95)
--min-p               Min-p 采样 (默认 0.05)
```

**重复控制:**
```bash
--repeat-penalty      重复惩罚 (默认 1.00)
--repeat-last-n       考虑重复的最后 n 个 token (默认 64)
--presence-penalty    存在性惩罚 (默认 0.00)
--frequency-penalty   频率性惩罚 (默认 0.00)
```

**高级采样:**
```bash
--mirostat            Mirostat 采样 (0/1/2)
--mirostat-lr         Mirostat 学习率 (默认 0.10)
--mirostat-ent        Mirostat 目标熵 (默认 5.00)
--samplers            采样器顺序
```

### 对话参数

```bash
-cnv, --conversation  对话模式
-sys, --system-prompt 系统提示词
-st, --single-turn    单轮对话
-r, --reverse-prompt 反向提示词
```

### 模型管理参数

```bash
--mm, --mmproj        多模态投影器文件
--image, --audio      图像/音频文件
--lora                LoRA 适配器路径
--control-vector      控制向量路径
```

## 采样策略详解

### 温度采样

**原理:**
温度参数控制输出的随机性。值越高，输出越随机；值越低，输出越确定性。

**使用场景:**
```bash
# 创意写作 (高温度)
llama-cli -m model.gguf -p "创作一个科幻故事" \
    --temp 1.2 -n 300

# 代码生成 (低温度)
llama-cli -m model.gguf -p "编写一个排序函数" \
    --temp 0.2 -n 100

# 平衡模式 (中温度)
llama-cli -m model.gguf -p "写一篇技术文章" \
    --temp 0.7 -n 200
```

### Top-k 采样

**原理:**
只从概率最高的 k 个 token 中选择下一个 token。

**使用场景:**
```bash
# 严格选择 (小 k 值)
llama-cli -m model.gguf -p "技术文档" \
    --top-k 10 -n 100

# 平衡选择 (中等 k 值)
llama-cli -m model.gguf -p "一般内容" \
    --top-k 40 -n 100

# 灵活选择 (大 k 值)
llama-cli -m model.gguf -p "创意内容" \
    --top-k 100 -n 100
```

### Top-p 采样

**原理:**
从累计概率达到 p 的最小 token 集合中选择。

**使用场景:**
```bash
# 严格选择 (小 p 值)
llama-cli -m model.gguf -p "正式内容" \
    --top-p 0.8 -n 100

# 平衡选择 (中等 p 值)
llama-cli -m model.gguf -p "标准内容" \
    --top-p 0.95 -n 100

# 灵活选择 (大 p 值)
llama-cli -m model.gguf -p "创意内容" \
    --top-p 0.99 -n 100
```

### Mirostat 采样

**原理:**
动态调整采样参数以保持目标困惑度。

**使用场景:**
```bash
# Mirostat v1
llama-cli -m model.gguf -p "连贯文本" \
    --mirostat 1 \
    --mirostat-tau 5.0 \
    --mirostat-lr 0.1 \
    -n 300

# Mirostat v2 (推荐)
llama-cli -m model.gguf -p "连贯段落" \
    --mirostat 2 \
    --mirostat-tau 3.0 \
    --mirostat-lr 0.1 \
    -n 300
```

### 组合采样策略

```bash
# 平衡策略
llama-cli -m model.gguf -p "平衡内容" \
    --temp 0.8 \
    --top-k 40 \
    --top-p 0.95 \
    --min-p 0.05 \
    -n 200

# 创意策略
llama-cli -m model.gguf -p "创意内容" \
    --temp 1.0 \
    --top-k 50 \
    --top-p 0.98 \
    --min-p 0.02 \
    -n 200

# 确定策略
llama-cli -m model.gguf -p "确定内容" \
    --temp 0.2 \
    --top-k 10 \
    --top-p 0.8 \
    --min-p 0.1 \
    -n 200
```

## 模型管理

### 多模型切换

```bash
#!/bin/bash
# model-switcher.sh

declare -A MODELS
MODELS["1"]="models/gemma-3-1b-it-Q4_K_M.gguf"
MODELS["2"]="models/llama-3-8b-instruct-Q4_K_M.gguf"
MODELS["3"]="models/qwen-7b-chat-Q4_K_M.gguf"

echo "可用模型:"
for key in "${!MODELS[@]}"; do
    echo "  $key: ${MODELS[$key]}"
done

echo -n "选择模型 (1-3): "
read choice

if [[ -n "${MODELS[$choice]}" ]]; then
    model="${MODELS[$choice]}"
    echo "使用模型: $model"
    
    llama-cli -m "$model" -cnv
else
    echo "无效选择"
fi
```

### 模型缓存管理

```bash
#!/bin/bash
# cache-manager.sh

# 查看缓存
echo "=== 模型缓存 ==="
llama-cli --cache-list

# 清理特定模型缓存
echo "清理缓存..."
llama-cli --hf-repo ggml-org/gemma-3-1b-it-GGUF --offline

# 重新下载模型
echo "重新下载..."
llama-cli -hf ggml-org/gemma-3-1b-it-GGUF
```

### LoRA 管理

```bash
#!/bin/bash
# lora-manager.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

# 可用 LoRA 适配器
loras=(
    "code-assistant:0.8"
    "creative-writing:0.5"
    "technical-docs:1.0"
)

echo "可用 LoRA 适配器:"
select lora in "${loras[@]}" "不使用 LoRA"; do
    if [ "$lora" = "不使用 LoRA" ]; then
        llama-cli -m "$model" -cnv
        break
    fi
    
    lora_path="lora/${lora%%:*}.gguf"
    lora_scale="${lora##*:}"
    
    echo "应用 LoRA: $lora_path (缩放: $lora_scale)"
    llama-cli -m "$model" \
        --lora "$lora_path" \
        --lora-scaled "$lora_path:$lora_scale" \
        -cnv
    break
done
```

## 性能优化

### 硬件优化

```bash
# CPU 优化
llama-cli -m model.gguf -p "CPU 优化" \
    -t 8 \
    -tb 16 \
    -c 4096 \
    -b 512 \
    -n 100

# GPU 优化
llama-cli -m model.gguf -p "GPU 优化" \
    -ngl 99 \
    -dev cuda:0 \
    -t 4 \
    -n 100

# 混合优化
llama-cli -m model.gguf -p "混合优化" \
    -ngl 50 \
    -t 4 \
    -c 4096 \
    -b 256 \
    -n 100
```

### 内存优化

```bash
# 低内存配置
llama-cli -m model.gguf -p "低内存" \
    -c 1024 \
    -b 128 \
    --no-mmap \
    --mlock \
    -n 50

# 高内存配置
llama-cli -m model.gguf -p "高内存" \
    -c 16384 \
    -b 2048 \
    -ub 512 \
    -np 8 \
    -n 500
```

### 批处理优化

```bash
# 批量处理脚本
#!/bin/bash
# batch-optimizer.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"
input_dir="prompts"
output_dir="results"

# 根据硬件自动优化
detect_hardware() {
    local cpu_cores=$(nproc)
    local total_memory=$(free -g | awk '/^Mem:/{print $2}')
    
    # 自动配置线程数
    local threads=$((cpu_cores / 2))
    
    # 自动配置批处理大小
    local batch_size=$((total_memory / 4 * 1024))
    
    echo "$threads $batch_size"
}

# 获取硬件配置
read threads batch_size <<< $(detect_hardware)

echo "自动配置: 线程=$threads, 批处理=$batch_size"

# 批量处理
for prompt_file in "$input_dir"/*.txt; do
    filename=$(basename "$prompt_file" .txt)
    
    llama-cli -m "$model" \
        -f "$prompt_file" \
        -t "$threads" \
        -b "$batch_size" \
        -n 200 \
        > "$output_dir/$filename.txt"
    
    echo "完成: $filename"
done
```

## 高级功能

### 语法约束

```bash
# JSON 约束
llama-cli -m model.gguf \
    -p "生成用户信息" \
    --json-schema '{"type": "object", "properties": {"name": {"type": "string"}, "age": {"type": "number"}}}' \
    -n 100

# 自定义语法
llama-cli -m model.gguf \
    -p "生成电话号码" \
    --grammar "phone ::= \\d{3}-\\d{3}-\\d{4}" \
    -n 20

# 语法文件约束
llama-cli -m model.gguf \
    -p "生成 JSON 数据" \
    --grammar-file grammars/json.gbnf \
    -n 100
```

### 流式处理

```bash
#!/bin/bash
# stream-processor.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

# 流式生成并处理
llama-cli -m "$model" \
    -p "长文本生成" \
    -n 1000 \
    2>&1 | \
while IFS= read -r line; do
    # 实时处理输出
    if [[ $line == *tokens per second* ]]; then
        speed=$(echo "$line" | grep -o '[0-9.]* tokens per second')
        echo "当前速度: $speed"
    else
        echo "$line"
    fi
done
```

### 多模态处理

```bash
# 图像处理
llama-cli -m models/multimodal-model.gguf \
    --mmproj models/mmproj.gguf \
    --image images/photo.jpg \
    -p "描述这张图片的内容" \
    -n 100

# 音频处理
llama-cli -m models/audio-model.gguf \
    --audio audio/speech.wav \
    -p "总结这段音频的内容" \
    -n 100

# 多模态组合
llama-cli -m models/multimodal-model.gguf \
    --mmproj models/mmproj.gguf \
    --image images/photo1.jpg,image2.jpg \
    --audio audio/voice1.wav,voice2.wav \
    -p "分析这些多媒体内容" \
    -n 200
```

## 脚本化应用

### 交互式助手

```bash
#!/bin/bash
# interactive-assistant.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"
system_prompt="你是一个专业的AI助手，回答要准确、简洁、有帮助"

# 加载历史对话
history_file="conversation_history.txt"

# 检查历史文件
if [ -f "$history_file" ]; then
    echo "加载历史对话..."
fi

echo "=== 交互式助手 ==="
echo "系统提示词: $system_prompt"
echo "输入 'quit' 退出，'clear' 清除历史"

while true; do
    echo -n "用户: "
    read user_input
    
    if [ "$user_input" = "quit" ]; then
        break
    fi
    
    if [ "$user_input" = "clear" ]; then
        > "$history_file"
        echo "历史已清除"
        continue
    fi
    
    # 添加到历史
    echo "用户: $user_input" >> "$history_file"
    
    # 生成回复
    echo -n "助手: "
    response=$(llama-cli -m "$model" \
        -cnv \
        -sys "$system_prompt" \
        -p "$user_input" \
        -n 200 \
        --no-display-prompt 2>/dev/null)
    
    echo "$response"
    echo "助手: $response" >> "$history_file"
done

echo "对话结束，历史已保存到 $history_file"
```

### 文本分析工具

```bash
#!/bin/bash
# text-analysis-tool.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

analyze_sentiment() {
    local text="$1"
    
    llama-cli -m "$model" \
        -p "分析以下文本的情感（正面/负面/中性）：\n$text" \
        -n 50 \
        --temp 0.3 \
        2>/dev/null
}

extract_keywords() {
    local text="$1"
    
    llama-cli -m "$model" \
        -p "从以下文本中提取关键词：\n$text\n\n关键词：" \
        -n 100 \
        --temp 0.2 \
        2>/dev/null
}

summarize_text() {
    local text="$1"
    local length="${2:-200}"
    
    llama-cli -m "$model" \
        -p "总结以下文本的要点：\n$text" \
        -n "$length" \
        --temp 0.5 \
        2>/dev/null
}

# 使用示例
echo "=== 文本分析工具 ==="

sample_text="人工智能正在改变我们的生活方式，从智能手机到自动驾驶汽车，AI技术无处不在。"

echo "情感分析："
analyze_sentiment "$sample_text"

echo ""
echo "关键词提取："
extract_keywords "$sample_text"

echo ""
echo "文本摘要："
summarize_text "$sample_text" 150
```

### 代码生成工具

```bash
#!/bin/bash
# code-generator.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

generate_code() {
    local description="$1"
    local language="${2:-python}"
    
    llama-cli -m "$model" \
        -p "用$language编写以下功能的代码：\n$description\n\n代码：" \
        -n 300 \
        --temp 0.3 \
        2>/dev/null
}

explain_code() {
    local code="$1"
    
    llama-cli -m "$model" \
        -p "解释以下代码的作用和实现方式：\n$code" \
        -n 200 \
        --temp 0.2 \
        2>/dev/null
}

improve_code() {
    local code="$1"
    
    llama-cli -m "$model" \
        -p "改进以下代码的性能和可读性：\n$code\n\n改进后的代码：" \
        -n 400 \
        --temp 0.3 \
        2>/dev/null
}

# 使用示例
echo "=== 代码生成工具 ==="

echo "生成代码："
generate_code "快速排序算法" "python"

echo ""
echo "解释代码："
explain_code "def quick_sort(arr): return arr if len(arr) <= 1 else quick_sort([x for x in arr[1:] if x <= arr[0]]) + [arr[0]] + quick_sort([x for x in arr[1:] if x > arr[0]])"
```

## 故障排查

### 性能问题

```bash
# 性能诊断脚本
#!/bin/bash
# performance-diagnosis.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

echo "=== 性能诊断 ==="

# 测试不同配置
configs=(
    "基础配置: "
    "多线程: -t 8"
    "GPU加速: -ngl 99"
    "混合模式: -t 4 -ngl 50"
)

for config in "${configs[@]}"; do
    echo "测试: $config"
    
    time llama-cli -m "$model" \
        -p "性能测试文本" \
        -n 100 \
        ${config#*: } \
        --show-timings 2>&1 | grep "tokens per second"
    
    echo "---"
done
```

### 内存问题

```bash
# 内存诊断
#!/bin/bash
# memory-diagnosis.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"

echo "=== 内存诊断 ==="

# 监控内存使用
start_memory=$(free -m | grep Mem | awk '{print $3}')

llama-cli -m "$model" \
    -p "内存测试" \
    -n 500 \
    -c 8192 &

pid=$!
sleep 5

current_memory=$(free -m | grep Mem | awk '{print $3}')
memory_used=$((current_memory - start_memory))

echo "内存使用: ${memory_used}MB"

# 检查进程状态
if ps -p $pid > /dev/null; then
    echo "进程运行中"
    kill $pid
else
    echo "进程已结束"
fi
```

### 错误处理

```bash
# 健壮的错误处理
#!/bin/bash
# robust-error-handler.sh

model="models/gemma-3-1b-it-Q4_K_M.gguf"
max_retries=3
retry_delay=5

# 检查模型文件
check_model() {
    if [ ! -f "$model" ]; then
        echo "错误：模型文件不存在: $model"
        return 1
    fi
    
    # 验证模型格式
    if ! llama-cli -m "$model" -n 1 2>&1 | grep -q "load time"; then
        echo "错误：模型文件无效"
        return 1
    fi
    
    return 0
}

# 运行推理（带重试）
run_inference() {
    local prompt="$1"
    local max_tokens="${2:-100}"
    local attempt=1
    
    while [ $attempt -le $max_retries ]; do
        echo "尝试 $attempt/$max_retries"
        
        if llama-cli -m "$model" \
            -p "$prompt" \
            -n "$max_tokens" \
            2>&1; then
            echo "成功"
            return 0
        else
            echo "失败，等待 $retry_delay 秒后重试..."
            sleep $retry_delay
            ((attempt++))
        fi
    done
    
    echo "所有尝试都失败"
    return 1
}

# 主函数
main() {
    # 检查模型
    if ! check_model; then
        exit 1
    fi
    
    # 运行推理
    if ! run_inference "测试推理" 50; then
        exit 1
    fi
    
    echo "操作成功完成"
}

main
```

## 最佳实践

### 1. 性能调优

- 根据硬件配置合理设置线程数
- 启用 GPU 加速以提高速度
- 调整批处理大小以平衡内存和速度
- 使用适当的上下文大小

### 2. 质量控制

- 根据任务调整温度参数
- 使用重复惩罚减少冗余
- 应用 Mirostat 保持输出一致性
- 启用语法约束确保格式正确

### 3. 资源管理

- 定期清理模型缓存
- 监控内存和 CPU 使用
- 使用适当的量化级别
- 实施错误处理和重试机制

### 4. 脚本化

- 自动化常用任务
- 实现配置管理
- 添加日志记录
- 建立监控告警

---

*完成 llama-cli 教程后，你可以继续学习[llama-server 完整使用教程](./6-10-llama-server完整使用教程.md)。*