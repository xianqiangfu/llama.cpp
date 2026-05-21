# Mamba/RWKV 等非 Transformer 架构支持

## 概述

llama.cpp 不仅支持传统的 Transformer 架构，还支持多种非 Transformer 架构，包括：

1. **Mamba 系列**：基于状态空间模型 (SSM) 的架构
2. **RWKV 系列**：基于循环神经网络的架构
3. **混合架构**：如 Jamba，结合 SSM 和注意力机制

## Mamba 架构

### 基本原理

Mamba 是基于选择性状态空间模型 (Selective State Space Models, S4) 的架构，与传统的 Transformer 注意力机制有本质区别：

1. **线性复杂度**：时间复杂度为 O(L)，而非 Transformer 的 O(L²)
2. **状态空间模型**：使用连续状态空间来建模序列
3. **选择性机制**：根据输入动态调整状态转移

### 核心组件

#### 1. 状态空间参数

```cpp
struct llama_layer {
    // SSM 相关参数
    ggml_tensor * ssm_in;         // 输入投影 [n_embd, 2*d_inner]
    ggml_tensor * ssm_conv1d;     // 1D 卷积权重 [d_conv, d_inner]
    ggml_tensor * ssm_conv1d_b;   // 1D 卷积偏置
    ggml_tensor * ssm_x;          // 状态投影 [d_inner, dt_rank + 2*d_state]
    ggml_tensor * ssm_dt;         // 时间步长投影 [dt_rank, d_inner]
    ggml_tensor * ssm_dt_b;       // 时间步长偏置
    ggml_tensor * ssm_a;          // 状态转移矩阵 [d_state, d_inner]
    ggml_tensor * ssm_d;          // 跳跃连接
    ggml_tensor * ssm_out;        // 输出投影 [d_inner, n_embd]
};
```

#### 2. 关键超参数

| 参数 | 描述 | 典型值 |
|------|------|--------|
| `d_conv` | 1D 卷积核大小 | 4 |
| `d_inner` | 内部维度 | 2*n_embd |
| `d_state` | 状态空间维度 | 16 |
| `dt_rank` | 时间步长低秩分解维度 | 16 |

### Mamba 层实现流程

```cpp
// 1. 输入投影
xz = matmul(ssm_in, cur)  // [n_embd, 2*d_inner] @ [n_embd, n_tokens]
x, z = split(xz)          // 分为 x 和 z

// 2. 1D 卷积
conv_x = concat(conv_state, x)
x = ssml(conv1d(conv_x))
update_conv_state(x)

// 3. SSM 计算
x_db = matmul(ssm_x, x)
dt, B, C = split(x_db)
dt = matmul(ssm_dt, dt) + ssm_dt_b

// 4. 状态空间扫描
y = ssm_scan(ssm_state, x, dt, A, B, C)
update_ssm_state(y)

// 5. 输出计算
y = y + x * ssm_d
y = swiglu_split(z, y)
out = matmul(ssm_out, y)
```

### Mamba1 vs Mamba2

**Mamba1**：
- 标准的 SSM 实现
- 简单的 B、C、Dt 投影
- 适用于大多数场景

**Mamba2**：
- 更高效的实现
- 引入分组机制 (`n_group`)
- 优化状态空间对偶性
- 改进的注意力机制集成

```cpp
// Mamba2 的关键区别
const int64_t n_group = hparams.ssm_n_group;
const int64_t n_head  = hparams.ssm_dt_rank;

// 分组的 B、C 投影
B = view_4d(xBC, d_state, n_group, ...)
C = view_4d(xBC, d_state, n_group, ...)

// 更复杂的状态管理
// 支持状态空间对偶性优化
```

## RWKV 架构

### 基本原理

RWKV (Receptance Weighted Key Value) 是一种结合了 RNN 和 Transformer 特点的架构：

1. **线性注意力机制**：使用 RWKV 操作替代标准注意力
2. **循环状态**：维护可训练的循环状态
3. **时间混合**：使用时间混合机制整合历史信息

### 核心组件

#### 1. 时间混合 (Time Mix)

```cpp
struct llama_layer {
    // 时间混合参数
    ggml_tensor * time_mix_w1;
    ggml_tensor * time_mix_w2;
    ggml_tensor * time_mix_lerp_x;
    ggml_tensor * time_mix_lerp_w;
    ggml_tensor * time_mix_lerp_k;
    ggml_tensor * time_mix_lerp_v;
    ggml_tensor * time_mix_lerp_r;
    ggml_tensor * time_mix_lerp_g;
    ggml_tensor * time_mix_first;
    ggml_tensor * time_mix_decay;
    ggml_tensor * time_mix_key;
    ggml_tensor * time_mix_value;
    ggml_tensor * time_mix_receptance;
    ggml_tensor * time_mix_gate;
    ggml_tensor * time_mix_output;
};
```

#### 2. 通道混合 (Channel Mix)

```cpp
struct llama_layer {
    // 通道混合参数
    ggml_tensor * channel_mix_lerp_k;
    ggml_tensor * channel_mix_lerp_r;
    ggml_tensor * channel_mix_key;
    ggml_tensor * channel_mix_value;
    ggml_tensor * channel_mix_receptance;
};
```

### RWKV 层实现流程

#### 1. 时间混合

```cpp
// 1. 计算时间差
sx = x_prev - cur

// 2. 动态混合
xxx = tanh(matmul(w1, cur + lerp_x * sx))
xxx = matmul(w2, xxx)

// 3. 分别计算 K、V、R、G
xw = cur + lerp_w * sx
xk = cur + lerp_k * sx
xv = cur + lerp_v * sx
xr = cur + lerp_r * sx
xg = cur + lerp_g * sx

// 4. 应用 RWKV 变换
k = matmul(time_mix_key, xk)
v = matmul(time_mix_value, xv)
r = matmul(time_mix_receptance, xr)
g = sigmoid(matmul(time_mix_gate, xg))

// 5. 计算衰减
w = exp(-exp(matmul(w2, tanh(matmul(w1, xw))) + time_mix_decay))

// 6. RWKV 操作
out = rwkv_wkv6(k, v, r, time_mix_first, w, wkv_state)

// 7. 更新状态
wkv_state = extract_state_from(out)
```

#### 2. 通道混合

```cpp
// 1. 计算时间差
sx = x_prev - cur

// 2. 混合输入
xk = cur + lerp_k * sx
xr = cur + lerp_r * sx

// 3. 计算输出
r = sigmoid(matmul(channel_mix_receptance, xr))
k = square(relu(matmul(channel_mix_key, xk)))
out = r * matmul(channel_mix_value, k)
```

### RWKV 版本差异

**RWKV6**：
- 标准实现
- 支持 QKV 注意力变体
- 使用 SiLU 激活函数

**RWKV7**：
- 更高效的实现
- 改进的状态管理
- 优化了数值稳定性

**RWKV6QWEN2**：
- 与 Qwen2 架构的混合
- 保持了 RWKV 的核心特性
- 适配了 Qwen2 的层归一化策略

## Jamba 混合架构

### 架构特点

Jamba 是一种混合架构，结合了：
- **Mamba 层**：处理长程依赖
- **注意力层**：处理局部上下文
- **MoE FFN**：混合专家网络

### 层配置

```cpp
// Jamba 使用异构层配置
for (uint32_t i = 0; i < n_layer; ++i) {
    hparams.recurrent_layer_arr[i] = (n_head_kv(i) == 0);
}
```

每个层根据 `n_head_kv` 决定使用哪种机制：
- `n_head_kv == 0`：Mamba 层
- `n_head_kv > 0`：注意力层

### 实现流程

```cpp
for (int il = 0; il < n_layer; ++il) {
    const int64_t n_head_kv = hparams.n_head_kv(il);

    // 归一化
    cur = norm(cur, layer.attn_norm);

    // 选择机制
    if (n_head_kv == 0) {
        // Mamba 层
        cur = build_mamba_layer(inp_hybrid->get_recr(), cur, model, ubatch, il);
    } else {
        // 注意力层
        auto [Qcur, Kcur, Vcur] = build_qkv(layer, cur, ...);
        cur = build_attn(inp_hybrid->get_attn(), layer.wo, Qcur, Kcur, Vcur, ...);
    }

    // FFN 或 MoE
    if (layer.ffn_gate_inp == nullptr) {
        cur = build_ffn(cur, layer.ffn_gate, layer.ffn_up, layer.ffn_down, ...);
    } else {
        cur = build_moe_ffn(cur, layer.ffn_gate_inp, layer.ffn_up_exps, ...);
    }
}
```

## 非 Transformer 架构与 Transformer 的差异

### 复杂度对比

| 特性 | Transformer | Mamba | RWKV |
|------|-------------|-------|------|
| 时间复杂度 | O(L²) | O(L) | O(L) |
| 空间复杂度 | O(L) | O(1) | O(1) |
| 长序列处理 | 困难 | 优秀 | 优秀 |
| 并行训练 | 是 | 是 | 否 |
| 推理效率 | 中 | 高 | 高 |

### KV Cache 差异

**Transformer**：
- 需要为每个位置存储 K 和 V
- 内存随序列长度线性增长
- 支持灵活的序列操作

**Mamba/RWKV**：
- 只需要维护固定大小的状态
- 内存占用恒定
- 状态更新更简单

### 循环状态管理

llama.cpp 使用 `llama_memory_recurrent_context` 来管理循环状态：

```cpp
class llama_memory_recurrent_context {
    // 状态缓存
    ggml_tensor * get_r_l(int il) const;  // 卷积状态
    ggml_tensor * get_s_l(int il) const;  // SSM 状态

    // 状态更新
    void update_r_l(int il, ggml_tensor * new_state);
    void update_s_l(int il, ggml_tensor * new_state);

    // 多序列支持
    int get_head() const;
    int get_size() const;
};
```

### 适用的后端操作

不同架构需要不同的后端支持：

**Mamba 特有的操作**：
- `ggml_ssm_conv`: 1D 卷积
- `ggml_ssm_scan`: 状态空间扫描

**RWKV 特有的操作**：
- `ggml_rwkv_wkv6`: RWKV 核心操作
- `ggml_gated_linear_attn`: 门控线性注意力

## 支持的架构类型

llama.cpp 支持以下非 Transformer 架构：

```cpp
enum llm_arch {
    LLM_ARCH_MAMBA,      // Mamba1
    LLM_ARCH_MAMBA2,     // Mamba2
    LLM_ARCH_JAMBA,      // Jamba 混合架构
    LLM_ARCH_RWKV6,      // RWKV6
    LLM_ARCH_RWKV6QWEN2, // RWKV6 + Qwen2
    LLM_ARCH_RWKV7,      // RWKV7
    LLM_ARCH_ARWKV7,     // 自适应 RWKV7
    // ...
};
```

## 模型转换

llama.cpp 提供了多种非 Transformer 模型的转换脚本：

```bash
# Mamba 模型转换
python conversion/mamba.py --model mamba-790m --output mamba-790m.gguf

# RWKV 模型转换
python conversion/rwkv.py --model rwkv-6b --output rwkv-6b.gguf

# Jamba 模型转换
python conversion/jamba.py --model jamba-1b --output jamba-1b.gguf
```

## 性能特点

### Mamba 性能优势

1. **长序列处理**：
   - 内存占用不随序列长度增长
   - 适合超长上下文任务
   - 推理速度快

2. **批处理效率**：
   - 状态管理简单
   - 批处理开销小
   - GPU 利用率高

### RWKV 性能优势

1. **推理速度**：
   - 无需计算注意力矩阵
   - 状态更新高效
   - 适合实时应用

2. **内存效率**：
   - 固定大小的状态
   - KV Cache 开销小
   - 支持大量并发请求

### 混合架构性能

1. **平衡性**：
   - 结合了两种架构的优势
   - 适应不同类型的任务
   - 灵活的配置选项

## 应用场景

### Mamba 适用场景

- 长文档处理
- 代码生成
- 时间序列分析
- 需要长程依赖的任务

### RWKV 适用场景

- 实时对话
- 流式生成
- 边缘设备部署
- 资源受限环境

### Jamba 适用场景

- 混合类型任务
- 需要灵活配置的场景
- 研究和实验
- 需要平衡性能和效率

## 实现细节

### 状态初始化

```cpp
// 循环状态初始化
void llama_init_recurrent_state(llama_context * lctx) {
    for (int il = 0; il < n_layer; ++il) {
        // 初始化卷积状态
        ggml_tensor * r_l = mctx->get_r_l(il);
        fill_tensor(r_l, 0.0f);

        // 初始化 SSM 状态
        ggml_tensor * s_l = mctx->get_s_l(il);
        fill_tensor(s_l, 0.0f);
    }
}
```

### 状态更新

```cpp
// 在推理过程中更新状态
void llama_update_recurrent_state(llama_context * lctx) {
    for (int il = 0; il < n_layer; ++il) {
        if (is_recurrent_layer(il)) {
            // 更新 Mamba 状态
            update_ssm_state(il, new_state);
            update_conv_state(il, new_conv);
        } else {
            // 更新注意力状态
            update_kv_cache(il, new_k, new_v);
        }
    }
}
```

## 总结

llama.cpp 对非 Transformer 架构的支持体现了其灵活性和前瞻性：

### 主要优势

1. **多样性**：支持多种创新架构
2. **效率**：针对不同架构优化实现
3. **统一性**：提供一致的 API 接口
4. **可扩展性**：易于添加新架构支持

### 技术亮点

1. **状态管理**：高效的循环状态管理机制
2. **后端支持**：针对 SSM 和 RWKV 的优化操作
3. **混合架构**：支持多种架构的混合使用
4. **内存效率**：优化的状态存储和更新

### 未来方向

1. 更多非 Transformer 架构支持
2. 更高效的状态空间操作
3. 混合架构的自动优化
4. 跨架构的知识迁移

非 Transformer 架构在长序列处理和推理效率方面展现出巨大潜力，llama.cpp 对这些架构的完善支持为研究者和开发者提供了强大的工具。