# llama-eval

llama.cpp 的简单评估工具，支持多个数据集。

完整的描述、使用示例和示例结果请参见：

- [PR 21152](https://github.com/ggml-org/llama.cpp/pull/21152)

## 快速开始

```bash
# 单服务器
python3 llama-eval.py \
  --server http://localhost:8033 \
  --model my-model \
  --dataset gsm8k --n_cases 100 \
  --grader-type regex --threads 32

# 多服务器（逗号分隔的 URL 和线程计数）
python3 llama-eval.py \
  --server http://server1:8033,http://server2:8033 \
  --server-name server1,server2 \
  --threads 16,16 \
  --dataset aime2025 --n_cases 240 \
  --grader-type regex
```
