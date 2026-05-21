// llama-cpp.h - LLaMA C++接口头文件
// 本文件提供了LLaMA模型的C++智能指针接口，用于自动资源管理

#pragma once

#ifndef __cplusplus
#error "此头文件仅用于C++"
#endif

#include <memory>

#include "llama.h"

// 模型资源删除器
struct llama_model_deleter {
    void operator()(llama_model * model) { llama_model_free(model); }
};

// 上下文资源删除器
struct llama_context_deleter {
    void operator()(llama_context * context) { llama_free(context); }
};

// 采样器资源删除器
struct llama_sampler_deleter {
    void operator()(llama_sampler * sampler) { llama_sampler_free(sampler); }
};

// LoRA适配器资源删除器
struct llama_adapter_lora_deleter {
    void operator()(llama_adapter_lora * adapter) { llama_adapter_lora_free(adapter); }
};

// 智能指针类型定义
typedef std::unique_ptr<llama_model, llama_model_deleter> llama_model_ptr;
typedef std::unique_ptr<llama_context, llama_context_deleter> llama_context_ptr;
typedef std::unique_ptr<llama_sampler, llama_sampler_deleter> llama_sampler_ptr;
typedef std::unique_ptr<llama_adapter_lora, llama_adapter_lora_deleter> llama_adapter_lora_ptr;
