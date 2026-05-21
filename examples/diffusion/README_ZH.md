# 扩散文本生成

此目录包含扩散语言模型（DLLMs）的实现

更多信息：
- https://github.com/ggml-org/llama.cpp/pull/14644
- https://github.com/ggml-org/llama.cpp/pull/14771

## 参数
扩散 CLI 支持各种参数来控制生成过程：

### 核心扩散参数
- `--diffusion-steps`: 扩散步数（默认：256）
- `--diffusion-algorithm`: 令牌选择算法
  - `0`: DIFFUSION_ALGORITHM_ORIGIN - 从 https://arxiv.org/abs/2107.03006 以完全随机的顺序生成令牌。
  - `1`: DIFFUSION_ALGORITHM_ENTROPY_BASED - 基于熵的选择
  - `2`: DIFFUSION_ALGORITHM_MARGIN_BASED - 基于边距的选择
  - `3`: DIFFUSION_ALGORITHM_RANDOM - 随机选择
  - `4`: DIFFUSION_ALGORITHM_CONFIDENCE_BASED - 基于置信度的选择（默认）
  - 更多文档在这里 https://github.com/DreamLM/Dream
- `--diffusion-visual`: 在生成期间启用实时可视化

### 调度参数
选择以下调度方法之一：

**基于时间步的调度：**
- `--diffusion-eps`: 时间步调度的 Epsilon 值（例如 0.001）

**基于块的调度：**
- `--diffusion-block-length`: 基于块调度的块大小（例如 32）

### 采样参数
- `--temp`: 采样温度（0.0 = 贪婪/确定性，越高 = 越随机）
- `--top-k`: 用于采样的 Top-k 过滤
- `--top-p`: 用于采样的 Top-p（核心）过滤
- `--seed`: 用于可重现性的随机种子

### 模型参数
- `-m`: GGUF 模型文件的路径
- `-p`: 输入提示文本
- `-ub`: 最大序列长度（ubatch 大小）
- `-c`: 上下文大小
- `-b`: 批大小

### 示例
#### Dream 架构：
```
llama-diffusion-cli -m dream7b.gguf -p "write code to train MNIST in pytorch" -ub 512 --diffusion-eps 0.001 --diffusion-algorithm 3 --diffusion-steps 256 --diffusion-visual
```

#### LLaDA 架构：
```
llama-diffusion-cli -m llada-8b.gguf -p "write code to train MNIST in pytorch" -ub 512 --diffusion-block-length 32 --diffusion-steps 256 --diffusion-visual
```

#### RND1 架构：
```
llama-diffusion-cli -m RND1-Base-0910.gguf -p "write code to train MNIST in pytorch" -ub 512 --diffusion-algorithm 1 --diffusion-steps 256 --diffusion-visual --temp 0.5 --diffusion-eps 0.001
```
