# LLM 推理核心流程分析

## 概述

本文档详细分析了 llama.cpp 项目中 LLM 推理的核心流程，包括前向传播、KV Cache 管理、采样生成和批处理推理等关键环节。

## 1. 前向传播计算流程

### 1.1 核心架构

llama.cpp 采用 GGML (GPT-Generated Model Language) 作为底层张量计算框架，提供统一的张量运算抽象。

**主要组件：**
- `llama_context`：推理上下文，管理模型状态和计算资源
- `llama_model`：模型结构和权重
- `llama_kv_cache`：键值缓存
- `ggml_context`：张量计算上下文
- `ggml_cgraph`：计算图

### 1.2 前向传播流程

```
输入批次 → Embedding → Transformer 层 → 输出 Logits → 采样
              ↓
          KV Cache 更新
```

**详细步骤：**

1. **输入处理阶段**
   - 接收 `llama_batch` 结构，包含 token 列表、位置信息、序列 ID 等
   - 通过 `llama_decode()` 处理批次
   - 支持多序列并行推理

2. **Embedding 层**
   - Token IDs 转换为 Embedding 向量
   - 支持位置编码 (RoPE、ALiBi 等)
   - 处理 LoRA 适配器和控制向量

3. **Transformer 层循环**
   ```cpp
   for (int il = 0; il < n_layers; il++) {
       // 自注意力计算
       // 使用 KV Cache 缓存历史键值对
       // 前馈网络计算
       // 残差连接和层归一化
   }
   ```

4. **输出生成**
   - 计算最后的 logits (词汇表大小的概率分布)
   - 返回给采样器进行 token 选择

### 1.3 计算图构建与执行

llama.cpp 使用计算图优化推理性能：

**计算图特点：**
- 静态图重用：相同结构的批次可以重用计算图
- 后端调度器：自动分配计算任务到 CPU/GPU
- 内存管理：优化的张量内存分配

**关键函数：**
- `graph_reserve()`：预留计算图资源
- `graph_compute()`：执行计算图
- `graph_get_cb()`：获取图构建回调

## 2. KV Cache 管理流程

### 2.1 KV Cache 基本原理

KV Cache 缓存注意力机制中的键 (Key) 和值 (Value) 张量，避免重复计算历史 token 的注意力。

**核心优势：**
- 减少计算量：从 O(n²) 降到 O(n)
- 降低延迟：每个新 token 只需计算当前 token 的注意力
- 节省内存：量化后的 KV Cache 大幅减少内存占用

### 2.2 KV Cache 结构

```cpp
struct llama_kv_cache {
    const llama_model & model;
    llama_hparams hparams;
    
    // 每层的 K 和 V 缓存
    std::vector<layer_t> layers;
    
    // 单元格管理
    std::vector<llama_kv_cells> v_cells;
    
    // 内存后端
    std::vector<ggml_backend_buffer_t> buffers;
    
    // 支持量化类型
    ggml_type type_k;  // K 缓存数据类型
    ggml_type type_v;  // V 缓存数据类型
};
```

### 2.3 KV Cache 操作流程

**1. 初始化阶段**
```cpp
llama_kv_cache kv(model, type_k, type_v, kv_size, n_seq_max);
```
- 根据 KV 大小和序列数分配内存
- 为每层创建 K、V 缓存张量
- 支持不同量化格式 (Q4_0, Q8_0, FP16 等)

**2. Token 处理阶段**
```cpp
// 查找可用缓存槽位
slot_info sinfo = kv.find_slot(ubatch);

// 更新 KV Cache
kv.apply_ubatch(sinfo, ubatch);

// 复制新的 K、V 到缓存
kv.cpy_k(ctx, k_cur, k_idxs, il);
kv.cpy_v(ctx, v_cur, v_idxs, il);
```

**3. 注意力计算阶段**
```cpp
// 获取历史 K、V
ggml_tensor * K = kv.get_k(ctx, il, n_kv, sinfo);
ggml_tensor * V = kv.get_v(ctx, il, n_kv, sinfo);

// 计算注意力
ggml_tensor * QK = ggml_mul_mat(ctx, K, Q);
ggml_tensor * attn = ggml_soft_max(ctx, QK);
ggml_tensor * out = ggml_mul_mat(ctx, V, attn);
```

### 2.4 内存管理策略

**单元格分配：**
- 使用环形缓冲区管理 KV 单元格
- 支持多序列共享和独立缓存
- 自动回收过期的 KV 单元格

**内存优化：**
- 支持 KV Cache 量化 (Q4_K, Q8_0)
- 支持 Flash Attention 优化
- 支持 ISWA (Infinite Sliding Window Attention)

### 2.5 KV Cache 读写操作

**序列管理：**
```cpp
// 删除序列的 KV
kv.seq_rm(seq_id, p0, p1);

// 复制序列的 KV
kv.seq_cp(seq_id_src, seq_id_dst, p0, p1);

// 保留特定序列
kv.seq_keep(seq_id);

// 添加序列位置偏移
kv.seq_add(seq_id, p0, p1, shift);
```

**状态保存与恢复：**
```cpp
// 保存 KV Cache 状态
kv.state_write(io, seq_id, flags);

// 恢复 KV Cache 状态
kv.state_read(io, seq_id, flags);
```

## 3. 采样生成流程

### 3.1 采样器架构

llama.cpp 提供灵活的采样器链系统：

```cpp
struct llama_sampler {
    struct llama_sampler_i * iface;
    llama_sampler_context_t ctx;
};
```

**采样器类型：**
- `greedy`：贪婪采样 (选择概率最大的 token)
- `dist`：分布采样 (根据概率分布采样)
- `top_k`：Top-K 采样
- `top_p`：Top-P (Nucleus) 采样
- `min_p`：Minimum-P 采样
- `typical`：Typical 采样
- `temp`：温度调整
- `mirostat`：Mirostat 采样

### 3.2 采样流程

**完整流程：**
```cpp
// 1. 获取 logits
float * logits = llama_get_logits_ith(ctx, -1);

// 2. 构建候选 token 数组
llama_token_data_array candidates;
// 填充 token_id, logit, p

// 3. 应用采样器链
llama_sampler_apply(sampler, &candidates);

// 4. 获取选中的 token
llama_token selected = candidates.data[candidates.selected].id;

// 5. 接受采样结果
llama_sampler_accept(sampler, selected);
```

### 3.3 具体采样算法

**Top-K 采样：**
```cpp
void llama_sampler_top_k_apply(llama_token_data_array * cur_p, int32_t k) {
    // 1. 排序得到前 k 个候选
    llama_token_data_array_partial_sort_inplace(cur_p, k);
    cur_p->size = k;  // 只保留前 k 个
}
```

**Top-P 采样：**
```cpp
void llama_sampler_top_p_apply(llama_token_data_array * cur_p, float p) {
    // 1. 计算 softmax 概率
    llama_sampler_softmax_impl(cur_p, false);
    
    // 2. 计算累积概率
    float cum_sum = 0.0f;
    size_t last_idx = cur_p->size;
    for (size_t i = 0; i < cur_p->size; i++) {
        cum_sum += cur_p->data[i].p;
        if (cum_sum >= p) {
            last_idx = i + 1;
            break;
        }
    }
    
    // 3. 只保留累积概率达到 p 的 tokens
    cur_p->size = last_idx;
}
```

**温度采样：**
```cpp
void llama_sampler_temp_impl(llama_token_data_array * cur_p, float temp) {
    if (temp <= 0.0f) {
        // 选择最大 logit 的 token
        // ...
    } else {
        // 调整 logits: logit / temp
        for (size_t i = 0; i < cur_p->size; i++) {
            cur_p->data[i].logit /= temp;
        }
    }
}
```

### 3.4 采样器链

支持将多个采样器串联使用：

```cpp
// 创建采样器链
auto * sampler = llama_sampler_chain_init(params);

// 添加采样器
llama_sampler_chain_add(sampler, llama_sampler_init_top_k(50));
llama_sampler_chain_add(sampler, llama_sampler_init_top_p(0.9, 1));
llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.8));
llama_sampler_chain_add(sampler, llama_sampler_init_dist(seed));

// 执行采样
llama_token token = llama_sampler_sample(sampler, ctx, -1);
```

## 4. 批处理推理流程

### 4.1 批次结构

```cpp
typedef struct llama_batch {
    int32_t n_tokens;
    
    llama_token  *  token;   // token ids
    float        *  embd;    // embeddings (可选)
    llama_pos    *  pos;     // token positions
    int32_t      *  n_seq_id;// 每个 token 的序列 ID 数量
    llama_seq_id ** seq_id;  // 序列 ID 数组
    int8_t       *  logits;  // 是否需要输出 logits
} llama_batch;
```

### 4.2 批次处理策略

**1. 逻辑批次 vs 物理批次**
- 逻辑批次 (`n_batch`)：用户提交的最大批次大小
- 物理批次 (`n_ubatch`)：实际计算的批次大小
- 自动将大批次分解为多个微批次处理

**2. 批次分割策略**

llama.cpp 提供多种批次分割算法：

```cpp
// 简单分割：按顺序分割
auto ubatch = balloc.split_simple(n_ubatch);

// 均等分割：确保每个序列有相等 token
auto ubatch = balloc.split_equal(n_ubatch, false);
```

**3. 多序列处理**
- 支持在单个批次中处理多个独立序列
- 每个序列可以有不同的长度
- 自动处理序列间的 KV Cache 隔离

### 4.3 批次执行流程

**完整流程：**
```cpp
// 1. 创建批次
llama_batch batch = llama_batch_init(n_tokens, embd, n_seq_max);

// 2. 填充批次数据
batch.token[i] = token_id;
batch.pos[i] = position;
batch.seq_id[i] = &seq_id;
batch.logits[i] = (i == n_tokens - 1);  // 最后一个 token 输出 logits

// 3. 执行推理
int ret = llama_decode(ctx, batch);

// 4. 获取结果
float * logits = llama_get_logits_ith(ctx, -1);
```

**内部处理：**
1. 批次预处理：分割为微批次
2. KV Cache 预留：为微批次预留缓存槽位
3. 微批次执行：逐个执行微批次
4. 结果汇总：整合所有微批次结果

### 4.4 批次优化技术

**1. 计算图重用**
```cpp
// 相同结构的批次可以重用计算图
if (can_reuse_graph) {
    gf = lctx->get_gf_res_reserve();
} else {
    gf = lctx->graph_reserve(n_tokens, n_seqs, n_outputs, nullptr);
}
```

**2. 内存预分配**
- 预先分配足够的输出缓冲区
- 避免推理过程中的内存分配
- 支持流式输出

**3. 多线程执行**
- 主线程：控制逻辑
- 工作线程：执行张量计算
- 线程池：管理工作线程

## 5. 性能优化技术

### 5.1 Flash Attention

llama.cpp 集成了 Flash Attention 优化：

**优势：**
- 减少内存访问次数
- 提高 GPU 利用率
- 支持更大的批次大小

**实现要点：**
- 优化注意力计算的内存布局
- 减少中间结果的存储
- 支持 KV Cache 量化

### 5.2 KV Cache 量化

支持多种量化格式：
- Q4_0, Q4_K, Q5_K, Q8_0
- FP16, BF16
- 自定义量化策略

**内存节省：**
- Q4_0：4 比特，节省约 75% 内存
- Q8_0：8 比特，节省约 50% 内存

### 5.3 计算后端优化

**多后端支持：**
- CPU (GGML CPU)
- GPU (CUDA, Metal, Vulkan, ROCm 等)
- 专用加速器 (Hexagon, OpenVINO 等)

**自动调度：**
- 自动选择最优后端
- 支持 CPU/GPU 混合执行
- 动态负载均衡

## 6. 状态管理与持久化

### 6.1 状态保存

```cpp
// 获取状态大小
size_t size = llama_state_get_size(ctx);

// 保存状态到缓冲区
size_t written = llama_state_get_data(ctx, dst, size);

// 保存到文件
bool success = llama_state_save_file(ctx, filepath, tokens, n_tokens);
```

### 6.2 状态恢复

```cpp
// 从缓冲区恢复
size_t read = llama_state_set_data(ctx, src, size);

// 从文件恢复
bool success = llama_state_load_file(ctx, filepath, tokens, capacity, &count);
```

### 6.3 单序列状态

支持保存/恢复单个序列的状态：

```cpp
// 保存单序列状态
size_t size = llama_state_seq_get_size(ctx, seq_id);
size_t written = llama_state_seq_get_data(ctx, dst, size, seq_id);

// 恢复单序列状态
size_t read = llama_state_seq_set_data(ctx, src, size, dest_seq_id);
```

## 7. 总结

llama.cpp 的推理核心流程设计体现了以下原则：

1. **模块化设计**：清晰的组件划分和接口定义
2. **性能优化**：多层次的优化策略
3. **灵活性**：支持多种配置和扩展
4. **内存效率**：精心设计的内存管理
5. **可扩展性**：易于添加新的功能和优化

核心组件的协同工作实现了高效的 LLM 推理，为用户提供了快速、可靠的文本生成能力。