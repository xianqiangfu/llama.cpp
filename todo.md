# llama.cpp 项目全方位深度调研与梳理任务清单

> **项目规模概览：**
> - 约 863 个 C/C++ 源文件
> - 核心代码：35个文件 (src/)
> - 公共库：50+个文件 (common/)
> - GGML核心库：多个硬件后端实现
> - 工具集：20+个独立工具
> - 示例：20+个子目录
> - 测试：60+个测试文件

---

## 第一阶段：项目架构调研与文档整理

### 1.1 项目整体架构分析
- [ ] 1.1.1 分析项目整体目录结构及其组织逻辑
- [ ] 1.1.2 梳理核心技术栈（C/C++、CMake、各硬件后端等）
- [ ] 1.1.3 分析 GGUF 文件格式及其作用
- [ ] 1.1.4 分析 GGML 张量计算库的架构设计
- [ ] 1.1.5 分析 llama.cpp 核心推理引擎的工作原理
- [ ] 1.1.6 分析各硬件后端（CUDA、Metal、Vulkan等）的实现策略
- [ ] 1.1.7 绘制项目总体架构图
- [ ] 1.1.8 绘制项目模块依赖关系图

### 1.2 文档阅读与整理
- [ ] 1.2.1 阅读并翻译 `AGENTS.md` 开发者指南
- [ ] 1.2.2 阅读并翻译 `CONTRIBUTING.md` 贡献指南
- [ ] 1.2.3 阅读并翻译 `SECURITY.md` 安全文档
- [ ] 1.2.4 阅读并整理 `docs/build.md` 构建文档
- [ ] 1.2.5 阅读并整理 `docs/install.md` 安装文档
- [ ] 1.2.6 阅读并整理 `docs/multi-gpu.md` 多GPU文档
- [ ] 1.2.7 阅读并整理 `docs/speculative.md` 推测解码文档
- [ ] 1.2.8 阅读并整理 `docs/function-calling.md` 函数调用文档
- [ ] 1.2.9 阅读并整理 `docs/autodraft.md` 自动草稿文档
- [ ] 1.2.10 阅读并整理 `docs/multimodal.md` 多模态文档
- [ ] 1.2.11 阅读并整理 `docs/android.md` Android平台文档
- [ ] 1.2.12 阅读并整理 `docs/docker.md` Docker部署文档
- [ ] 1.2.13 阅读并整理 `docs/ops.md` 运维文档
- [ ] 1.2.14 阅读并整理各后端文档（BLAS、SYCL、CUDA、HIP、Metal、Vulkan等）
- [ ] 1.2.15 阅读并整理 `docs/development/` 下的开发文档

### 1.3 核心原理与关键技术梳理
- [ ] 1.3.1 梳理 LLM 推理核心流程（前向传播、KV Cache、采样等）
- [ ] 1.3.2 梳理模型量化原理（Q4_0、Q4_K、Q5_K、Q8_0等格式）
- [ ] 1.3.3 梳理 KV Cache 实现原理（ISWA、Hybrid等策略）
- [ ] 1.3.4 梳理推测解码（Speculative Decoding）原理
- [ ] 1.3.5 梳理 Lookahead 解码优化技术
- [ ] 1.3.6 梳理 Mamba/RWKV 等非Transformer架构支持
- [ ] 1.3.7 梳理 LoRA 适配器支持机制
- [ ] 1.3.8 梳理 GBNF 语法约束实现
- [ ] 1.3.9 梳理多模态输入处理机制
- [ ] 1.3.10 梳理注意力机制实现（RoPE、ALiBi等）

---

## 第二阶段：为每个文件夹创建中文 README 文档

### 2.1 根目录与核心配置文件
- [ ] 2.1.1 创建 `.github/README_ZH.md` - GitHub Actions配置说明
- [ ] 2.1.2 创建 `.devops/README_ZH.md` - DevOps相关配置说明
- [ ] 2.1.3 创建 `.pi/README_ZH.md` - 性能分析相关说明
- [ ] 2.1.4 创建 `.gemini/README_ZH.md` - Gemini相关配置说明
- [ ] 2.1.5 创建 `app/README_ZH.md` - 应用程序说明
- [ ] 2.1.6 创建 `benches/README_ZH.md` - 基准测试说明
- [ ] 2.1.7 创建 `ci/README_ZH.md` - CI配置说明
- [ ] 2.1.8 创建 `cmake/README_ZH.md` - CMake模块说明

### 2.2 核心源代码目录
- [ ] 2.2.1 创建 `src/README_ZH.md` - 核心源代码目录总览
- [ ] 2.2.2 创建 `src/models/README_ZH.md` - 模型实现目录说明
- [ ] 2.2.3 创建 `include/README_ZH.md` - 公共头文件目录说明
- [ ] 2.2.4 创建 `ggml/README_ZH.md` - GGML张量计算库总览

### 2.3 GGML 各后端实现
- [ ] 2.3.1 创建 `ggml/src/ggml-blas/README_ZH.md` - BLAS后端说明
- [ ] 2.3.2 创建 `ggml/src/ggml-cann/README_ZH.md` - CANN后端说明
- [ ] 2.3.3 创建 `ggml/src/ggml-cpu/README_ZH.md` - CPU后端说明
- [ ] 2.3.4 创建 `ggml/src/ggml-cuda/README_ZH.md` - CUDA后端说明
- [ ] 2.3.5 创建 `ggml/src/ggml-hexagon/README_ZH.md` - Hexagon后端说明
- [ ] 2.3.6 创建 `ggml/src/ggml-hip/README_ZH.md` - HIP后端说明
- [ ] 2.3.7 创建 `ggml/src/ggml-metal/README_ZH.md` - Metal后端说明
- [ ] 2.3.8 创建 `ggml/src/ggml-musa/README_ZH.md` - MUSA后端说明
- [ ] 2.3.9 创建 `ggml/src/ggml-opencl/README_ZH.md` - OpenCL后端说明
- [ ] 2.3.10 创建 `ggml/src/ggml-openvino/README_ZH.md` - OpenVINO后端说明
- [ ] 2.3.11 创建 `ggml/src/ggml-rpc/README_ZH.md` - RPC后端说明
- [ ] 2.3.12 创建 `ggml/src/ggml-sycl/README_ZH.md` - SYCL后端说明
- [ ] 2.3.13 创建 `ggml/src/ggml-virtgpu/README_ZH.md` - VirtGPU后端说明
- [ ] 2.3.14 创建 `ggml/src/ggml-vulkan/README_ZH.md` - Vulkan后端说明
- [ ] 2.3.15 创建 `ggml/src/ggml-webgpu/README_ZH.md` - WebGPU后端说明
- [ ] 2.3.16 创建 `ggml/src/ggml-zdnn/README_ZH.md` - zDNN后端说明
- [ ] 2.3.17 创建 `ggml/src/ggml-zendnn/README_ZH.md` - ZenDNN后端说明

### 2.4 公共库目录
- [ ] 2.4.1 创建 `common/README_ZH.md` - 公共库总览
- [ ] 2.4.2 创建 `common/jinja/README_ZH.md` - Jinja模板引擎集成说明

### 2.5 工具目录
- [ ] 2.5.1 创建 `tools/README_ZH.md` - 工具集总览
- [ ] 2.5.2 创建 `tools/cli/README_ZH.md` - CLI工具说明
- [ ] 2.5.3 创建 `tools/server/README_ZH.md` - HTTP服务器说明
- [ ] 2.5.4 创建 `tools/quantize/README_ZH.md` - 量化工具说明
- [ ] 2.5.5 创建 `tools/perplexity/README_ZH.md` - 困惑度评估工具说明
- [ ] 2.5.6 创建 `tools/llama-bench/README_ZH.md` - 性能基准测试工具说明
- [ ] 2.5.7 创建 `tools/tokenize/README_ZH.md` - 分词工具说明
- [ ] 2.5.8 创建 `tools/imatrix/README_ZH.md` - 重要性矩阵计算工具说明
- [ ] 2.5.9 创建 `tools/fit-params/README_ZH.md` - 参数拟合工具说明
- [ ] 2.5.10 创建 `tools/export-lora/README_ZH.md` - LoRA导出工具说明
- [ ] 2.5.11 创建 `tools/gguf-split/README_ZH.md` - GGUF分割工具说明
- [ ] 2.5.12 创建 `tools/completion/README_ZH.md` - 补全工具说明
- [ ] 2.5.13 创建 `tools/batched-bench/README_ZH.md` - 批处理基准测试说明
- [ ] 2.5.14 创建 `tools/parser/README_ZH.md` - 解析器工具说明
- [ ] 2.5.15 创建 `tools/rpc/README_ZH.md` - RPC服务器说明
- [ ] 2.5.16 创建 `tools/tts/README_ZH.md` - TTS工具说明
- [ ] 2.5.17 创建 `tools/cvector-generator/README_ZH.md` - 控制向量生成器说明
- [ ] 2.5.18 创建 `tools/ui/README_ZH.md` - Web UI说明

### 2.6 示例目录
- [ ] 2.6.1 创建 `examples/README_ZH.md` - 示例总览
- [ ] 2.6.2 创建 `examples/simple/README_ZH.md` - 简单示例说明
- [ ] 2.6.3 创建 `examples/simple-chat/README_ZH.md` - 聊天示例说明
- [ ] 2.6.4 创建 `examples/batched/README_ZH.md` - 批处理示例说明
- [ ] 2.6.5 创建 `examples/batched.swift/README_ZH.md` - Swift批处理示例说明
- [ ] 2.6.6 创建 `examples/embedding/README_ZH.md` - 嵌入示例说明
- [ ] 2.6.7 创建 `examples/speculative/README_ZH.md` - 推测解码示例说明
- [ ] 2.6.8 创建 `examples/speculative-simple/README_ZH.md` - 简单推测解码示例说明
- [ ] 2.6.9 创建 `examples/lookahead/README_ZH.md` - Lookahead优化示例说明
- [ ] 2.6.10 创建 `examples/parallel/README_ZH.md` - 并行处理示例说明
- [ ] 2.6.11 创建 `examples/retrieval/README_ZH.md` - 检索增强示例说明
- [ ] 2.6.12 创建 `examples/passkey/README_ZH.md` - 密钥记忆测试示例说明
- [ ] 2.6.13 创建 `examples/llama-eval/README_ZH.md` - 评估示例说明
- [ ] 2.6.14 创建 `examples/llama.android/README_ZH.md` - Android应用示例说明
- [ ] 2.6.15 创建 `examples/llama.swiftui/README_ZH.md` - Swift UI示例说明
- [ ] 2.6.16 创建 `examples/convert-llama2c-to-ggml/README_ZH.md` - LLaMA2C转换示例说明
- [ ] 2.6.17 创建 `examples/model-conversion/README_ZH.md` - 模型转换示例说明
- [ ] 2.6.18 创建 `examples/gguf/README_ZH.md` - GGUF操作示例说明
- [ ] 2.6.19 创建 `examples/gguf-hash/README_ZH.md` - GGUF哈希示例说明
- [ ] 2.6.20 创建 `examples/diffusion/README_ZH.md` - 扩散模型示例说明
- [ ] 2.6.21 创建 `examples/training/README_ZH.md` - 训练示例说明
- [ ] 2.6.22 创建 `examples/debug/README_ZH.md` - 调试示例说明

### 2.7 测试目录
- [ ] 2.7.1 创建 `tests/README_ZH.md` - 测试总览
- [ ] 2.7.2 创建 `tests/peg-parser/README_ZH.md` - PEG解析器测试说明

### 2.8 文档目录
- [ ] 2.8.1 创建 `docs/README_ZH.md` - 文档目录总览
- [ ] 2.8.2 创建 `docs/android/README_ZH.md` - Android文档说明
- [ ] 2.8.3 创建 `docs/backend/README_ZH.md` - 后端文档总览
- [ ] 2.8.4 创建 `docs/development/README_ZH.md` - 开发文档总览
- [ ] 2.8.5 创建 `docs/multimodal/README_ZH.md` - 多模态文档总览
- [ ] 2.8.6 创建 `docs/ops/README_ZH.md` - 运维文档总览

### 2.9 其他目录
- [ ] 2.9.1 创建 `conversion/README_ZH.md` - 模型转换脚本说明
- [ ] 2.9.2 创建 `gguf-py/README_ZH.md` - GGUF Python库说明
- [ ] 2.9.3 创建 `grammars/README_ZH.md` - GBNF语法文件说明
- [ ] 2.9.4 创建 `licenses/README_ZH.md` - 许可证文件说明
- [ ] 2.9.5 创建 `media/README_ZH.md` - 媒体文件说明
- [ ] 2.9.6 创建 `models/README_ZH.md` - 模型文件说明
- [ ] 2.9.7 创建 `models/templates/README_ZH.md` - 聊天模板说明
- [ ] 2.9.8 创建 `pocs/README_ZH.md` - 概念验证代码说明
- [ ] 2.9.9 创建 `requirements/README_ZH.md` - Python依赖说明
- [ ] 2.9.10 创建 `scripts/README_ZH.md` - 辅助脚本说明
- [ ] 2.9.11 创建 `vendor/README_ZH.md` - 第三方库说明

---

## 第三阶段：为所有代码文件添加中文注释

### 3.1 核心源代码文件 (src/) - 35个文件
- [ ] 3.1.1 为 `src/llama.cpp` 添加中文注释
- [ ] 3.1.2 为 `src/llama-model.cpp` 添加中文注释
- [ ] 3.1.3 为 `src/llama-model.h` 添加中文注释
- [ ] 3.1.4 为 `src/llama-context.cpp` 添加中文注释
- [ ] 3.1.5 为 `src/llama-context.h` 添加中文注释
- [ ] 3.1.6 为 `src/llama-vocab.cpp` 添加中文注释
- [ ] 3.1.7 为 `src/llama-vocab.h` 添加中文注释
- [ ] 3.1.8 为 `src/llama-sampler.cpp` 添加中文注释
- [ ] 3.1.9 为 `src/llama-sampler.h` 添加中文注释
- [ ] 3.1.10 为 `src/llama-grammar.cpp` 添加中文注释
- [ ] 3.1.11 为 `src/llama-grammar.h` 添加中文注释
- [ ] 3.1.12 为 `src/llama-quant.cpp` 添加中文注释
- [ ] 3.1.13 为 `src/llama-quant.h` 添加中文注释
- [ ] 3.1.14 为 `src/llama-graph.cpp` 添加中文注释
- [ ] 3.1.15 为 `src/llama-graph.h` 添加中文注释
- [ ] 3.1.16 为 `src/llama-batch.cpp` 添加中文注释
- [ ] 3.1.17 为 `src/llama-batch.h` 添加中文注释
- [ ] 3.1.18 为 `src/llama-chat.cpp` 添加中文注释
- [ ] 3.1.19 为 `src/llama-chat.h` 添加中文注释
- [ ] 3.1.20 为 `src/llama-arch.cpp` 添加中文注释
- [ ] 3.1.21 为 `src/llama-arch.h` 添加中文注释
- [ ] 3.1.22 为 `src/llama-adapter.cpp` 添加中文注释
- [ ] 3.1.23 为 `src/llama-adapter.h` 添加中文注释
- [ ] 3.1.24 为 `src/llama-kv-cache.cpp` 添加中文注释
- [ ] 3.1.25 为 `src/llama-kv-cache.h` 添加中文注释
- [ ] 3.1.26 为 `src/llama-kv-cache-iswa.cpp` 添加中文注释
- [ ] 3.1.27 为 `src/llama-kv-cache-iswa.h` 添加中文注释
- [ ] 3.1.28 为 `src/llama-kv-cells.h` 添加中文注释
- [ ] 3.1.29 为 `src/llama-memory.cpp` 添加中文注释
- [ ] 3.1.30 为 `src/llama-memory.h` 添加中文注释
- [ ] 3.1.31 为 `src/llama-memory-hybrid.cpp` 添加中文注释
- [ ] 3.1.32 为 `src/llama-memory-hybrid.h` 添加中文注释
- [ ] 3.1.33 为 `src/llama-memory-recurrent.cpp` 添加中文注释
- [ ] 3.1.34 为 `src/llama-memory-recurrent.h` 添加中文注释
- [ ] 3.1.35 为 `src/unicode.cpp` 添加中文注释
- [ ] 3.1.36 为 `src/unicode.h` 添加中文注释
- [ ] 3.1.37 为 `src/unicode-data.cpp` 添加中文注释
- [ ] 3.1.38 为 `src/unicode-data.h` 添加中文注释

### 3.2 公共头文件 (include/) - 2个文件
- [ ] 3.2.1 为 `include/llama.h` 添加中文注释
- [ ] 3.2.2 为 `include/llama-cpp.h` 添加中文注释

### 3.3 公共库文件 (common/) - 50+个文件
- [ ] 3.3.1 为 `common/common.cpp` 添加中文注释
- [ ] 3.3.2 为 `common/common.h` 添加中文注释
- [ ] 3.3.3 为 `common/arg.cpp` 添加中文注释
- [ ] 3.3.4 为 `common/arg.h` 添加中文注释
- [ ] 3.3.5 为 `common/chat.cpp` 添加中文注释
- [ ] 3.3.6 为 `common/chat.h` 添加中文注释
- [ ] 3.3.7 为 `common/console.cpp` 添加中文注释
- [ ] 3.3.8 为 `common/console.h` 添加中文注释
- [ ] 3.3.9 为 `common/download.cpp` 添加中文注释
- [ ] 3.3.10 为 `common/download.h` 添加中文注释
- [ ] 3.3.11 为 `common/json-partial.cpp` 添加中文注释
- [ ] 3.3.12 为 `common/json-partial.h` 添加中文注释
- [ ] 3.3.13 为 `common/json-schema-to-grammar.cpp` 添加中文注释
- [ ] 3.3.14 为 `common/json-schema-to-grammar.h` 添加中文注释
- [ ] 3.3.15 为 `common/llguidance.cpp` 添加中文注释
- [ ] 3.3.16 为 `common/log.cpp` 添加中文注释
- [ ] 3.3.17 为 `common/log.h` 添加中文注释
- [ ] 3.3.18 为 `common/ngram-cache.cpp` 添加中文注释
- [ ] 3.3.19 为 `common/ngram-cache.h` 添加中文注释
- [ ] 3.3.20 为 `common/ngram-map.cpp` 添加中文注释
- [ ] 3.3.21 为 `common/ngram-map.h` 添加中文注释
- [ ] 3.3.22 为 `common/sampling.cpp` 添加中文注释
- [ ] 3.3.23 为 `common/sampling.h` 添加中文注释
- [ ] 3.3.24 为 `common/speculative.cpp` 添加中文注释
- [ ] 3.3.25 为 `common/speculative.h` 添加中文注释
- [ ] 3.3.26 为 `common/peg-parser.cpp` 添加中文注释
- [ ] 3.3.27 为 `common/peg-parser.h` 添加中文注释
- [ ] 3.3.28 为 `common/chat-peg-parser.cpp` 添加中文注释
- [ ] 3.3.29 为 `common/chat-peg-parser.h` 添加中文注释
- [ ] 3.3.30 为 `common/chat-auto-parser.cpp` 添加中文注释
- [ ] 3.3.31 为 `common/chat-auto-parser.h` 添加中文注释
- [ ] 3.3.32 为 `common/chat-auto-parser-generator.cpp` 添加中文注释
- [ ] 3.3.33 为 `common/chat-auto-parser-helpers.cpp` 添加中文注释
- [ ] 3.3.34 为 `common/chat-auto-parser-helpers.h` 添加中文注释
- [ ] 3.3.35 为 `common/chat-diff-analyzer.cpp` 添加中文注释
- [ ] 3.3.36 为 `common/fit.cpp` 添加中文注释
- [ ] 3.3.37 为 `common/fit.h` 添加中文注释
- [ ] 3.3.38 为 `common/hf-cache.cpp` 添加中文注释
- [ ] 3.3.39 为 `common/hf-cache.h` 添加中文注释
- [ ] 3.3.40 为 `common/ngram-mod.cpp` 添加中文注释
- [ ] 3.3.41 为 `common/ngram-mod.h` 添加中文注释
- [ ] 3.3.42 为 `common/preset.cpp` 添加中文注释
- [ ] 3.3.43 为 `common/preset.h` 添加中文注释
- [ ] 3.3.44 为 `common/reasoning-budget.cpp` 添加中文注释
- [ ] 3.3.45 为 `common/reasoning-budget.h` 添加中文注释
- [ ] 3.3.46 为 `common/regex-partial.cpp` 添加中文注释
- [ ] 3.3.47 为 `common/regex-partial.h` 添加中文注释
- [ ] 3.3.48 为 `common/unicode.cpp` 添加中文注释
- [ ] 3.3.49 为 `common/unicode.h` 添加中文注释
- [ ] 3.3.50 为 `common/jinja/` 目录下文件添加中文注释

### 3.4 GGML核心库文件 (ggml/)
- [ ] 3.4.1 为 `ggml/src/ggml.c` 添加中文注释
- [ ] 3.4.2 为 `ggml/src/ggml.cpp` 添加中文注释
- [ ] 3.4.3 为 `ggml/src/ggml-alloc.c` 添加中文注释
- [ ] 3.4.4 为 `ggml/src/ggml-quants.c` 添加中文注释
- [ ] 3.4.5 为 `ggml/src/ggml-quants.h` 添加中文注释
- [ ] 3.4.6 为 `ggml/src/ggml-opt.cpp` 添加中文注释
- [ ] 3.4.7 为 `ggml/src/ggml-common.h` 添加中文注释
- [ ] 3.4.8 为 `ggml/src/ggml-backend.cpp` 添加中文注释
- [ ] 3.4.9 为 `ggml/src/ggml-backend-meta.cpp` 添加中文注释
- [ ] 3.4.10 为 `ggml/src/ggml-backend-reg.cpp` 添加中文注释
- [ ] 3.4.11 为 `ggml/src/ggml-backend-impl.h` 添加中文注释
- [ ] 3.4.12 为 `ggml/src/ggml-backend-dl.cpp` 添加中文注释
- [ ] 3.4.13 为 `ggml/src/ggml-backend-dl.h` 添加中文注释
- [ ] 3.4.14 为 `ggml/src/ggml-threading.cpp` 添加中文注释
- [ ] 3.4.15 为 `ggml/src/ggml-threading.h` 添加中文注释
- [ ] 3.4.16 为 `ggml/src/ggml-impl.h` 添加中文注释
- [ ] 3.4.17 为 `ggml/src/ggml-gguf.cpp` 添加中文注释
- [ ] 3.4.18 为 `ggml/include/` 目录下头文件添加中文注释

### 3.5 GGML各后端实现文件
- [ ] 3.5.1 为 `ggml/src/ggml-cpu/` 目录下文件添加中文注释
- [ ] 3.5.2 为 `ggml/src/ggml-cuda/` 目录下文件添加中文注释
- [ ] 3.5.3 为 `ggml/src/ggml-metal/` 目录下文件添加中文注释
- [ ] 3.5.4 为 `ggml/src/ggml-hip/` 目录下文件添加中文注释
- [ ] 3.5.5 为 `ggml/src/ggml-vulkan/` 目录下文件添加中文注释
- [ ] 3.5.6 为 `ggml/src/ggml-blas/` 目录下文件添加中文注释
- [ ] 3.5.7 为 `ggml/src/ggml-sycl/` 目录下文件添加中文注释
- [ ] 3.5.8 为 `ggml/src/ggml-opencl/` 目录下文件添加中文注释
- [ ] 3.5.9 为 `ggml/src/ggml-openvino/` 目录下文件添加中文注释
- [ ] 3.5.10 为 `ggml/src/ggml-cann/` 目录下文件添加中文注释
- [ ] 3.5.11 为 `ggml/src/ggml-musa/` 目录下文件添加中文注释
- [ ] 3.5.12 为 `ggml/src/ggml-rpc/` 目录下文件添加中文注释
- [ ] 3.5.13 为 `ggml/src/ggml-zdnn/` 目录下文件添加中文注释
- [ ] 3.5.14 为 `ggml/src/ggml-zendnn/` 目录下文件添加中文注释
- [ ] 3.5.15 为 `ggml/src/ggml-hexagon/` 目录下文件添加中文注释
- [ ] 3.5.16 为 `ggml/src/ggml-virtgpu/` 目录下文件添加中文注释
- [ ] 3.5.17 为 `ggml/src/ggml-webgpu/` 目录下文件添加中文注释

### 3.6 工具文件 (tools/)
- [ ] 3.6.1 为 `tools/cli/` 目录下文件添加中文注释
- [ ] 3.6.2 为 `tools/server/` 目录下文件添加中文注释
- [ ] 3.6.3 为 `tools/quantize/` 目录下文件添加中文注释
- [ ] 3.6.4 为 `tools/perplexity/` 目录下文件添加中文注释
- [ ] 3.6.5 为 `tools/llama-bench/` 目录下文件添加中文注释
- [ ] 3.6.6 为 `tools/tokenize/` 目录下文件添加中文注释
- [ ] 3.6.7 为 `tools/imatrix/` 目录下文件添加中文注释
- [ ] 3.6.8 为 `tools/fit-params/` 目录下文件添加中文注释
- [ ] 3.6.9 为 `tools/export-lora/` 目录下文件添加中文注释
- [ ] 3.6.10 为 `tools/gguf-split/` 目录下文件添加中文注释
- [ ] 3.6.11 为 `tools/completion/` 目录下文件添加中文注释
- [ ] 3.6.12 为 `tools/batched-bench/` 目录下文件添加中文注释
- [ ] 3.6.13 为 `tools/parser/` 目录下文件添加中文注释
- [ ] 3.6.14 为 `tools/rpc/` 目录下文件添加中文注释
- [ ] 3.6.15 为 `tools/tts/` 目录下文件添加中文注释
- [ ] 3.6.16 为 `tools/cvector-generator/` 目录下文件添加中文注释
- [ ] 3.6.17 为 `tools/ui/` 目录下文件添加中文注释

### 3.7 示例文件 (examples/)
- [ ] 3.7.1 为 `examples/simple/` 目录下文件添加中文注释
- [ ] 3.7.2 为 `examples/simple-chat/` 目录下文件添加中文注释
- [ ] 3.7.3 为 `examples/batched/` 目录下文件添加中文注释
- [ ] 3.7.4 为 `examples/embedding/` 目录下文件添加中文注释
- [ ] 3.7.5 为 `examples/speculative/` 目录下文件添加中文注释
- [ ] 3.7.6 为 `examples/lookahead/` 目录下文件添加中文注释
- [ ] 3.7.7 为 `examples/parallel/` 目录下文件添加中文注释
- [ ] 3.7.8 为 `examples/retrieval/` 目录下文件添加中文注释
- [ ] 3.7.9 为 `examples/passkey/` 目录下文件添加中文注释
- [ ] 3.7.10 为 `examples/llama-eval/` 目录下文件添加中文注释
- [ ] 3.7.11 为 `examples/llama.android/` 目录下文件添加中文注释
- [ ] 3.7.12 为 `examples/llama.swiftui/` 目录下文件添加中文注释
- [ ] 3.7.13 为其他示例目录下文件添加中文注释

### 3.8 转换脚本文件
- [ ] 3.8.1 为 `convert_hf_to_gguf.py` 添加中文注释
- [ ] 3.8.2 为 `convert_hf_to_gguf_update.py` 添加中文注释
- [ ] 3.8.3 为 `convert_llama_ggml_to_gguf.py` 添加中文注释
- [ ] 3.8.4 为 `convert_lora_to_gguf.py` 添加中文注释
- [ ] 3.8.5 为 `conversion/` 目录下其他转换脚本添加中文注释

### 3.9 测试文件 (tests/)
- [ ] 3.9.1 为 `tests/test-*.cpp` 主要测试文件添加中文注释
- [ ] 3.9.2 为 `tests/peg-parser/` 目录下文件添加中文注释

### 3.10 其他文件
- [ ] 3.10.1 为 `gguf-py/` 目录下Python文件添加中文注释
- [ ] 3.10.2 为 `scripts/` 目录下脚本文件添加中文注释
- [ ] 3.10.3 为其他辅助文件添加中文注释

---

## 第四阶段：绘制项目流程图与架构图

### 4.1 核心流程图
- [ ] 4.1.1 绘制 LLM 推理完整流程图
- [ ] 4.1.2 绘制模型加载与初始化流程图
- [ ] 4.1.3 绘制前向传播计算流程图
- [ ] 4.1.4 绘制 KV Cache 管理流程图
- [ ] 4.1.5 绘制采样生成流程图
- [ ] 4.1.6 绘制批处理推理流程图
- [ ] 4.1.7 绘制推测解码流程图
- [ ] 4.1.8 绘制 Lookahead 优化流程图

### 4.2 模块架构图
- [ ] 4.2.1 绘制 llama.cpp 总体模块架构图
- [ ] 4.2.2 绘制 llama 核心库模块依赖图
- [ ] 4.2.3 绘制 GGML 张量计算库架构图
- [ ] 4.2.4 绘制多后端系统架构图
- [ ] 4.2.5 绘制量化模块架构图
- [ ] 4.2.6 绘制词表处理模块架构图
- [ ] 4.2.7 绘制语法约束模块架构图

### 4.3 业务逻辑图
- [ ] 4.3.1 绘制聊天对话业务逻辑图
- [ ] 4.3.2 绘制补全生成业务逻辑图
- [ ] 4.3.3 绘制嵌入计算业务逻辑图
- [ ] 4.3.4 绘制重排序业务逻辑图
- [ ] 4.3.5 绘制 LoRA 适配器应用逻辑图
- [ ] 4.3.6 绘制多模态输入处理逻辑图

### 4.4 依赖关系图
- [ ] 4.4.1 绘制核心模块依赖关系图
- [ ] 4.4.2 绘制工具链依赖关系图
- [ ] 4.4.3 绘制后端实现依赖关系图
- [ ] 4.4.4 绘制第三方库依赖关系图

### 4.5 其他图表
- [ ] 4.5.1 绘制数据流向图
- [ ] 4.5.2 绘制内存管理架构图
- [ ] 4.5.3 绘制线程池与任务调度图
- [ ] 4.5.4 绘制 RPC 通信架构图
- [ ] 4.5.5 绘制 HTTP 服务器请求处理流程图

---

## 第五阶段：梳理开发注意事项与最佳实践

### 5.1 开发注意事项
- [ ] 5.1.1 梳理 C/C++ 编码规范与代码风格
- [ ] 5.1.2 梳理内存管理与资源释放注意事项
- [ ] 5.1.3 梳理线程安全与并发编程注意事项
- [ ] 5.1.4 梳理跨平台兼容性注意事项
- [ ] 5.1.5 梳理性能优化注意事项
- [ ] 5.1.6 梳理错误处理与调试注意事项

### 5.2 踩坑点总结
- [ ] 5.2.1 总结常见构建错误与解决方案
- [ ] 5.2.2 总结模型加载常见问题
- [ ] 5.2.3 总结推理性能问题排查方法
- [ ] 5.2.4 总结内存溢出与内存泄漏问题
- [ ] 5.2.5 总结量化精度损失问题
- [ ] 5.2.6 总结后端兼容性问题
- [ ] 5.2.7 总结多设备协同问题

### 5.3 最佳实践
- [ ] 5.3.1 总结模型选择与量化最佳实践
- [ ] 5.3.2 总结推理配置调优最佳实践
- [ ] 5.3.3 总结批处理与并发最佳实践
- [ ] 5.3.4 总结多 GPU 部署最佳实践
- [ ] 5.3.5 总结服务器部署与运维最佳实践
- [ ] 5.3.6 总结二次开发与扩展最佳实践
- [ ] 5.3.7 总结测试与验证最佳实践

---

## 第六阶段：补充项目理解资料

### 6.1 入门指南
- [ ] 6.1.1 编写项目快速入门指南
- [ ] 6.1.2 编写环境搭建指南（Windows、Linux、macOS）
- [ ] 6.1.3 编写从源码构建详细教程
- [ ] 6.1.4 编写模型获取与准备教程
- [ ] 6.1.5 编写第一个推理示例教程
- [ ] 6.1.6 编写命令行工具使用教程
- [ ] 6.1.7 编写服务器部署教程
- [ ] 6.1.8 编写 API 调用教程

### 6.2 使用教程
- [ ] 6.2.1 编写 llama-cli 完整使用教程
- [ ] 6.2.2 编写 llama-server 完整使用教程
- [ ] 6.2.3 编写量化工具使用教程
- [ ] 6.2.4 编写困惑度评估教程
- [ ] 6.2.5 编写性能基准测试教程
- [ ] 6.2.6 编写 LoRA 使用教程
- [ ] 6.2.7 编写语法约束使用教程
- [ ] 6.2.8 编写多模态使用教程
- [ ] 6.2.9 编写函数调用使用教程

### 6.3 开发文档
- [ ] 6.3.1 编写新增模型支持教程
- [ ] 6.3.2 编写新增后端教程
- [ ] 6.3.3 编写 C API 使用文档
- [ ] 6.3.4 编写 libllama 库集成文档
- [ ] 6.3.5 编写 Python 绑定使用文档
- [ ] 6.3.6 编写代码结构导航文档
- [ ] 6.3.7 编写调试技巧文档
- [ ] 6.3.8 编写性能分析文档

---

## 第七阶段：整合输出技术调研报告

### 7.1 报告结构设计
- [ ] 7.1.1 设计报告整体结构
- [ ] 7.1.2 创建报告目录与章节规划
- [ ] 7.1.3 设计报告模板与格式规范

### 7.2 报告内容整合
- [ ] 7.2.1 整合项目概述与背景
- [ ] 7.2.2 整合架构设计与核心原理
- [ ] 7.2.3 整合技术栈与依赖分析
- [ ] 7.2.4 整合各模块详细说明
- [ ] 7.2.5 整合使用教程与最佳实践
- [ ] 7.2.6 整合开发指南与注意事项
- [ ] 7.2.7 整合图表与流程说明
- [ ] 7.2.8 整合参考文献与资源链接

### 7.3 报告编写
- [ ] 7.3.1 编写报告前言
- [ ] 7.3.2 编写第一章：项目概述
- [ ] 7.3.3 编写第二章：架构设计
- [ ] 7.3.4 编写第三章：核心原理
- [ ] 7.3.5 编写第四章：技术实现
- [ ] 7.3.6 编写第五章：使用指南
- [ ] 7.3.7 编写第六章：开发指南
- [ ] 7.3.8 编写第七章：最佳实践
- [ ] 7.3.9 编写第八章：常见问题
- [ ] 7.3.10 编写总结与展望

### 7.4 报告完善
- [ ] 7.4.1 添加图表与示意图
- [ ] 7.4.2 添加代码示例
- [ ] 7.4.3 添加交叉引用
- [ ] 7.4.4 校对与修正
- [ ] 7.4.5 格式美化与排版

---

## 任务执行说明

### 任务优先级
1. **高优先级**：第一、二阶段（架构调研与文档整理）
2. **中优先级**：第三、四、五阶段（代码注释、图表绘制、最佳实践）
3. **低优先级**：第六、七阶段（补充资料、整合报告）

### 任务执行顺序建议
1. 先完成第一阶段，对项目整体有充分理解
2. 按目录优先级完成第二阶段的README文档
3. 按模块重要性完成第三阶段的代码注释
4. 在前三阶段过程中逐步完成第四、五阶段
5. 最后完成第六、七阶段的资料补充与报告整合

### 注意事项
- 所有中文注释仅增加内容，严禁修改原有业务代码逻辑
- README文档统一使用 `README_ZH.md` 命名
- 保持代码风格统一，不引入格式问题
- 图表使用 Mermaid 等格式，便于版本管理
- 报告采用 Markdown 格式，便于阅读与维护

### 预估工作量
- 第一阶段：约 20-30 工作时
- 第二阶段：约 40-50 工作时
- 第三阶段：约 100-150 工作时
- 第四阶段：约 30-40 工作时
- 第五阶段：约 20-30 工作时
- 第六阶段：约 30-40 工作时
- 第七阶段：约 20-30 工作时
- **总计：约 260-370 工作时**

---

## 附录

### A. 参考资料
- [llama.cpp GitHub 仓库](https://github.com/ggml-org/llama.cpp)
- [GGML 文档](https://github.com/ggml-org/ggml)
- [Hugging Face GGUF 模型库](https://huggingface.co/models?library=gguf)

### B. 关键术语表
- GGUF：GPT-Generated Unified Format
- GGML：Georgi Gerganov's Machine Learning library
- KV Cache：键值缓存
- ISWA：Implicit State Without Action
- LoRA：Low-Rank Adaptation
- MTP：Multi-Task Parallel
- BNF：Backus-Naur Form
- GBNF：GGML BNF

### C. 联系与反馈
如有任何问题或建议，请通过 GitHub Issues 联系。

---

**最后更新时间：2026-05-21**