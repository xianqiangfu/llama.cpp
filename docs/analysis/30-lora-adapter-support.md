# LoRA 适配器支持机制分析

## 概述

LoRA (Low-Rank Adaptation) 是一种高效的模型微调方法，通过在预训练模型上添加低秩适配器来实现任务特定的微调。llama.cpp 提供了完整的 LoRA 适配器加载、管理和应用机制，支持单个或多个适配器同时使用。

## LoRA 基本原理

### 1. 数学原理

LoRA 通过将权重更新分解为两个低秩矩阵来减少参数量：

```
W' = W + ΔW = W + B × A
```

其中：
- `W`: 原始权重矩阵 (d × d)
- `A`: 低秩矩阵 (r × d)
- `B`: 低秩矩阵 (d × r)
- `r`: 秩 (rank)，通常 r << d

### 2. 缩放因子

实际应用时使用缩放因子控制影响：

```
W' = W + (α / r) × B × A
```

- `α`: alpha 缩放因子
- `r`: rank 秩
- `α/r`: 最终缩放系数

## 适配器类型

### 1. LoRA 适配器 (llama_adapter_lora)

标准的 LoRA 适配器，用于修改模型权重。

**数据结构**：

```cpp
struct llama_adapter_lora {
    llama_model * model;

    // 映射张量名称到 LoRA 权重对
    std::unordered_map<std::string, llama_adapter_lora_weight> ab_map;

    // GGUF 元数据
    std::unordered_map<std::string, std::string> gguf_kv;

    // aLoRA 激活 token
    std::vector<llama_token> alora_invocation_tokens;

    // 上下文和缓冲区
    std::vector<ggml_context_ptr> ctxs;
    std::vector<ggml_backend_buffer_ptr> bufs;

    float alpha;
};
```

**权重结构**：

```cpp
struct llama_adapter_lora_weight {
    ggml_tensor * a = nullptr;  // A 矩阵 (r × d)
    ggml_tensor * b = nullptr;  // B 矩阵 (d × r)

    // 计算实际缩放
    float get_scale(float alpha, float adapter_scale) const {
        const float rank = (float) b->ne[0];
        const float scale = alpha ? adapter_scale * alpha / rank : adapter_scale;
        return scale;
    }
};
```

### 2. CVec 适配器 (llama_adapter_cvec)

控制向量 (Control Vector) 适配器，用于修改激活值。

**数据结构**：

```cpp
struct llama_adapter_cvec {
    int32_t layer_start = -1;
    int32_t layer_end   = -1;

    std::vector<ggml_context_ptr> ctxs;
    std::vector<ggml_backend_buffer_ptr> bufs;

    std::vector<ggml_tensor *> tensors;  // 每层的张量

    // 应用到特定层
    bool apply(
        const llama_model & model,
        const float * data,
        size_t len,
        int32_t n_embd,
        int32_t il_start,
        int32_t il_end);
};
```

### 3. 激活式 LoRA (aLoRA)

根据特定 token 激活的 LoRA 适配器：

- 使用激活 token 序列决定是否应用适配器
- 支持条件适配器
- 可以用于多任务学习

**激活机制**：

```cpp
// 检查当前 token 序列是否匹配激活模式
bool should_activate(
    const std::vector<llama_token> & current_tokens,
    const std::vector<llama_token> & invocation_tokens);
```

## 加载流程

### 1. 初始化

```cpp
LLAMA_API struct llama_adapter_lora * llama_adapter_lora_init(
    struct llama_model * model,
    const char * path_lora);
```

**步骤**：
1. 打开 GGUF 文件
2. 读取元数据（alpha, rank 等）
3. 加载 LoRA 权重（A 和 B 矩阵）
4. 创建后端缓冲区
5. 建立 tensor 名称到权重的映射

### 2. 元数据访问

```cpp
// 获取字符串值
LLAMA_API int32_t llama_adapter_meta_val_str(
    const struct llama_adapter_lora * adapter,
    const char * key,
    char * buf,
    size_t buf_size);

// 获取元数据数量
LLAMA_API int32_t llama_adapter_meta_count(
    const struct llama_adapter_lora * adapter);

// 按索引获取键
LLAMA_API int32_t llama_adapter_meta_key_by_index(
    const struct llama_adapter_lora * adapter,
    int32_t i,
    char * buf,
    size_t buf_size);

// 按索引获取值
LLAMA_API int32_t llama_adapter_meta_val_str_by_index(
    const struct llama_adapter_lora * adapter,
    int32_t i,
    char * buf,
    size_t buf_size);
```

**常用元数据键**：
- `general.rank`: LoRA 秩
- `general.alpha`: Alpha 缩放因子
- `general.type`: 适配器类型
- `general.architecture`: 模型架构

### 3. aLoRA 信息

```cpp
// 获取激活 token 数量
LLAMA_API uint64_t llama_adapter_get_alora_n_invocation_tokens(
    const struct llama_adapter_lora * adapter);

// 获取激活 token 序列
LLAMA_API const llama_token * llama_adapter_get_alora_invocation_tokens(
    const struct llama_adapter_lora * adapter);
```

## 应用方法

### 1. 设置适配器

```cpp
LLAMA_API int32_t llama_set_adapters_lora(
    struct llama_context * ctx,
    struct llama_adapter_lora ** adapters,
    size_t n_adapters,
    const float * scales,
    size_t n_scales);
```

**参数说明**：
- `ctx`: llama 上下文
- `adapters`: LoRA 适配器数组
- `n_adapters`: 适配器数量
- `scales`: 缩放因子数组
- `n_scales`: 缩放因子数量

**行为**：
1. 检查适配器是否变化
2. 如果变化，重建计算图
3. 更新适配器列表

### 2. 设置 CVec 适配器

```cpp
LLAMA_API int32_t llama_set_adapter_cvec(
    struct llama_context * ctx,
    struct llama_adapter_cvec * cvec);
```

### 3. 图构建集成

在图构建过程中应用 LoRA：

```cpp
ggml_tensor * build_lora_mm(
    ggml_tensor * w,
    ggml_tensor * x,
    const llama_adapter_loras * loras) {
    // 1. 基础矩阵乘法
    ggml_tensor * result = ggml_mul_mat(ctx, w, x);

    // 2. 应用所有 LoRA 适配器
    for (auto & [lora, scale] : *loras) {
        auto * weight = lora->get_weight(w);
        if (weight) {
            // 计算 LoRA 增量: scale * (B @ A @ x)
            float lora_scale = weight->get_scale(lora->alpha, scale);
            ggml_tensor * lora_out = ggml_mul_mat(ctx, weight->a, x);
            lora_out = ggml_mul_mat(ctx, weight->b, lora_out);
            lora_out = ggml_scale(ctx, lora_out, lora_scale);

            // 加到结果上
            result = ggml_add(ctx, result, lora_out);
        }
    }

    return result;
}
```

### 4. 查找适配器权重

```cpp
llama_adapter_lora_weight * llama_adapter_lora::get_weight(ggml_tensor * w) {
    // 根据张量名称查找对应的 LoRA 权重
    const std::string name = ggml_get_name(w);

    auto it = ab_map.find(name);
    if (it != ab_map.end()) {
        return &it->second;
    }

    return nullptr;
}
```

## 多适配器支持

### 1. 适配器管理

```cpp
// 使用无序映射管理多个适配器
using llama_adapter_loras = std::unordered_map<llama_adapter_lora *, float>;
using llama_adapter_loras_ptr = std::unique_ptr<llama_adapter_loras>;
```

### 2. 缩放因子

每个适配器可以设置独立的缩放因子：

```cpp
std::vector<common_adapter_lora_info> lora_adapters;

struct common_adapter_lora_info {
    std::string path;      // 适配器文件路径
    float scale;           // 缩放因子
    struct llama_adapter_lora * ptr;  // 加载的适配器指针
};
```

### 3. 组合策略

适配器可以按以下方式组合：

**加法组合**：
```cpp
output = base_output + scale1 * lora1 + scale2 * lora2 + ...
```

**选择组合**（aLoRA）：
```cpp
if (token_matches_invocation(lora1)) {
    output = base_output + scale1 * lora1;
} else if (token_matches_invocation(lora2)) {
    output = base_output + scale2 * lora2;
}
```

## 性能影响分析

### 1. 计算开销

**LoRA 开销**：
- 每个适配器增加 2 次矩阵乘法（A @ x 和 B @ (A @ x)）
- 总开销 = O(n_adapters × r × d × n_tokens)

**CVec 开销**：
- 每层增加一次向量加法
- 开销较小，通常可忽略

### 2. 内存占用

**LoRA 内存**：
- A 矩阵：r × d × size(float)
- B 矩阵：d × r × size(float)
- 总内存 = 2 × r × d × size(float)

**对比**：
- 全量微调：d × d × size(float)
- LoRA (r=8)：16 × d × size(float)
- 减少比例：d / 16

### 3. 性能优化

**批处理**：
```cpp
// 将多个 LoRA 操作合并
result = base_output;
for (auto & [lora, scale] : *loras) {
    // ... 应用 LoRA
}
```

**融合算子**：
- 使用融合算子减少内存访问
- 支持 Metal/CUDA 后端优化

**缓存优化**：
- 缓存适配器权重到 GPU
- 减少主机-设备传输

## 使用示例

### 1. 命令行使用

```bash
# 加载单个 LoRA 适配器
./main -m model.gguf --lora adapter.gguf --lora-scale 1.0

# 加载多个适配器
./main -m model.gguf \
    --lora adapter1.gguf --lora-scale 1.0 \
    --lora adapter2.gguf --lora-scale 0.5

# 仅加载不应用
./main -m model.gguf \
    --lora adapter.gguf \
    --lora-init-without-apply
```

### 2. 代码中使用

```cpp
// 1. 加载模型
llama_model_params mparams = llama_model_default_params();
llama_model * model = llama_load_model_from_file("model.gguf", mparams);

// 2. 加载 LoRA 适配器
llama_adapter_lora * lora1 = llama_adapter_lora_init(model, "adapter1.gguf");
llama_adapter_lora * lora2 = llama_adapter_lora_init(model, "adapter2.gguf");

// 3. 获取元数据
char rank_str[32];
llama_adapter_meta_val_str(lora1, "general.rank", rank_str, sizeof(rank_str));
printf("LoRA rank: %s\n", rank_str);

// 4. 创建上下文
llama_context_params cparams = llama_context_default_params();
llama_context * ctx = llama_new_context_with_model(model, cparams);

// 5. 应用适配器
llama_adapter_lora * adapters[] = {lora1, lora2};
float scales[] = {1.0f, 0.5f};
llama_set_adapters_lora(ctx, adapters, 2, scales, 2);

// 6. 使用适配器
// ... 正常生成 ...

// 7. 清理
llama_adapter_lora_free(lora1);
llama_adapter_lora_free(lora2);
llama_free_model(model);
```

### 3. C++ 接口

```cpp
#include "llama-cpp.h"

// 使用智能指针管理适配器
llama_adapter_lora_ptr lora = llama_adapter_lora_init(model, "adapter.gguf");

// 应用到上下文
std::vector<llama_adapter_lora_ptr> loras;
loras.push_back(std::move(lora));

std::vector<float> scales = {1.0f};
llama_set_adapters_lora(ctx, loras, scales);
```

## 最佳实践

### 1. 适配器选择

**使用场景**：
- 任务微调：标准 LoRA
- 风格控制：CVec 适配器
- 多任务学习：aLoRA

**参数选择**：
- Rank: 4-16（平衡质量和效率）
- Alpha: 与 rank 相同或稍大
- Scale: 0.5-2.0（根据任务调整）

### 2. 性能优化

**减少适配器数量**：
- 合并相似功能的适配器
- 使用 aLoRA 进行条件选择

**调整 Rank**：
- 小任务：rank=4
- 中等任务：rank=8
- 复杂任务：rank=16

**量化支持**：
- 使用量化适配器减少内存
- 注意精度损失

### 3. 调试技巧

**检查适配器信息**：
```cpp
// 打印所有适配器权重
for (auto & [name, weight] : lora->ab_map) {
    printf("Tensor: %s\n", name.c_str());
    printf("  A: %ld x %ld\n", weight.a->ne[0], weight.a->ne[1]);
    printf("  B: %ld x %ld\n", weight.b->ne[0], weight.b->ne[1]);
}
```

**验证适配器效果**：
- 对比有/无适配器的输出
- 调整缩放因子
- 使用控制组测试

## 常见问题

### 1. 适配器不生效

**可能原因**：
- 适配器权重名称不匹配
- 缩放因子设置过小
- 适配器加载失败

**解决方案**：
- 检查适配器的 tensor 名称
- 增大缩放因子
- 查看加载日志

### 2. 性能下降

**可能原因**：
- 适配器数量过多
- Rank 设置过大
- 后端不支持优化

**解决方案**：
- 减少适配器数量
- 降低 Rank
- 使用支持 LoRA 优化的后端

### 3. 内存不足

**可能原因**：
- 适配器数量过多
- Rank 设置过大
- 模型本身过大

**解决方案**：
- 使用量化适配器
- 减少 Rank
- 分批加载适配器

## 扩展功能

### 1. 动态适配器切换

```cpp
// 运行时切换适配器
llama_adapter_lora * new_adapters[] = {new_lora};
float new_scales[] = {1.0f};
llama_set_adapters_lora(ctx, new_adapters, 1, new_scales, 1);
```

### 2. 适配器融合

```cpp
// 将多个适配器融合为一个
llama_adapter_lora * merge_adapters(
    llama_adapter_lora ** adapters,
    float * scales,
    size_t n_adapters);
```

### 3. 适配器导出

```cpp
// 将训练好的适配器导出为 GGUF
bool export_adapter_to_gguf(
    llama_adapter_lora * adapter,
    const char * output_path);
```

## 结论

llama.cpp 的 LoRA 适配器支持机制提供了灵活高效的模型微调方案。通过支持多种适配器类型、多适配器组合和动态切换，用户可以轻松实现任务特定的模型适配，同时保持高效的推理性能。系统提供了完整的 API 和命令行工具，便于集成到各种应用场景中。

## 相关文件

- `include/llama.h`: C API 接口定义
- `include/llama-cpp.h`: C++ API 接口定义
- `src/llama-adapter.h`: 适配器数据结构定义
- `src/llama-context.h`: 上下文适配器管理
- `src/llama-graph.h`: 图构建中 LoRA 应用
- `common/common.h`: 公共适配器工具
- `tools/server/server-common.h`: 服务器适配器工具