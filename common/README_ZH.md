# 公共库总览

## 概述

`common/` 目录包含 llama.cpp 项目中广泛使用的公共库和工具函数。这些模块提供了命令行参数解析、日志记录、控制台交互、模型加载、采样、量化等核心功能，是整个项目的基础设施层。

## 目录结构

```
common/
├── CMakeLists.txt                    # CMake 构建配置
├── arg.h                             # 命令行参数定义
├── arg.cpp                           # 命令行参数解析 (195KB)
├── build-info.cpp.in                 # 构建信息模板
├── build-info.h                      # 构建信息头文件
├── base64.hpp                        # Base64 编解码
├── chat.h                            # 聊天接口
├── chat.cpp                          # 聊天实现 (113KB)
├── chat-auto-parser.h                # 自动解析器
├── chat-auto-parser-helpers.h        # 解析器辅助
├── chat-auto-parser-helpers.cpp      # 解析器辅助实现
├── chat-auto-parser-generator.cpp    # 解析器生成器
├── chat-diff-analyzer.cpp            # 差异分析器 (66KB)
├── chat-peg-parser.h                 # PEG 解析器
├── chat-peg-parser.cpp               # PEG 解析器实现
├── common.h                          # 公共头文件 (47KB)
├── common.cpp                        # 公共实现 (73KB)
├── console.h                         # 控制台接口
├── console.cpp                       # 控制台实现
├── debug.h                           # 调试接口
├── debug.cpp                         # 调试实现
├── download.h                        # 下载接口
├── download.cpp                      # 下载实现 (33KB)
├── fit.h                             # 拟合接口
├── fit.cpp                           # 拟合实现 (46KB)
├── hf-cache.h                        # HuggingFace 缓存
├── hf-cache.cpp                      # HF 缓存实现
├── http.h                            # HTTP 接口
├── jinja/                            # Jinja 模板引擎目录
├── json-partial.h                    # JSON 部分解析
├── json-partial.cpp                  # JSON 部分实现
├── json-schema-to-grammar.h          # JSON Schema 到语法
├── json-schema-to-grammar.cpp        # JSON Schema 转换实现 (51KB)
├── llguidance.cpp                    # LLM Guidance 实现
├── log.h                             # 日志接口
├── log.cpp                           # 日志实现 (12KB)
├── ngram-cache.h                     # N-gram 缓存
├── ngram-cache.cpp                   # N-gram 缓存实现
├── ngram-map.h                       # N-gram 映射
├── ngram-map.cpp                     // N-gram 映射实现
├── ngram-mod.h                       // N-gram 模块
├── ngram-mod.cpp                     // N-gram 模块实现
├── peg-parser.h                      // PEG 解析器
├── peg-parser.cpp                    // PEG 解析器实现 (84KB)
├── peg-parser.h                      // PEG 解析器头文件
├── preset.h                          // 预设
├── preset.cpp                        // 预设实现
├── reasoning-budget.h                # 推理预算
├── reasoning-budget.cpp              # 推理预算实现
├── regex-partial.h                   # 正则部分匹配
├── regex-partial.cpp                 # 正则部分匹配实现
├── sampling.h                        # 采样接口
├── sampling.cpp                      # 采样实现 (32KB)
├── speculative.h                     # 推测采样
├── speculative.cpp                   // 推测采样实现 (61KB)
├── unicode.cpp                       # Unicode 处理
├── unicode.h                         // Unicode 头文件
└── jinja/                            # Jinja 模板引擎 (见单独文档)
```

## 核心模块

### 1. 命令行参数解析 (arg.h/cpp)

#### 功能
- 命令行参数定义和解析
- 参数类型检查和验证
- 帮助信息生成

#### 主要类型
```cpp
// 常用参数类型
gpt_params              // GPT 模型参数
gpt_params_parse_result // 解析结果
```

#### 参数类别

1. **模型参数**：
   - 模型路径
   - 上下文大小
   - 批次大小
   - GPU 层数

2. **生成参数**：
   - 温度
   - Top-p / Top-k
   - 最大生成长度
   - 采样方法

3. **性能参数**：
   - 线程数
   - 内存限制
   - 缓存配置

4. **输出参数**：
   - 输出格式
   - 日志级别
   - 颜色输出

### 2. 聊天系统 (chat.h/cpp)

#### 功能
- 多轮对话管理
- 聊天模板处理
- 对话历史维护
- 消息格式化

#### 主要组件

1. **自动解析器** (`chat-auto-parser.*`):
   - 自动解析用户输入
   - 识别指令和内容
   - 生成结构化输出

2. **PEG 解析器** (`chat-peg-parser.*`):
   - 解析表达式语法 (PEG)
   - 语法分析
   - 语法验证

3. **差异分析器** (`chat-diff-analyzer.cpp`):
   - 对话差异分析
   - 变更检测
   - 一致性检查

4. **聊天模板**:
   - 支持多种模型模板
   - 自定义模板
   - 模板变量替换

### 3. 公共功能 (common.h/cpp)

#### 核心数据结构

```cpp
// 模型上下文
struct llama_context;

// 模型加载
struct llama_model;

// 批次处理
struct llama_batch;

// 采样器
struct llama_sampler;
```

#### 主要功能

1. **模型管理**:
   - 模型加载和卸载
   - 上下文创建
   - 内存管理

2. **批次处理**:
   - 批次创建
   - Token 添加
   - 批次执行

3. **内存管理**:
   - 内存分配
   - 缓冲区管理
   - 性能监控

### 4. 日志系统 (log.h/cpp)

#### 功能
- 结构化日志输出
- 日志级别控制
- 彩色输出
- 文件日志

#### 日志级别

```cpp
enum log_level {
    LOG_LEVEL_DEBUG,    // 调试信息
    LOG_LEVEL_INFO,     // 一般信息
    LOG_LEVEL_WARN,     // 警告信息
    LOG_LEVEL_ERROR,    // 错误信息
    LOG_LEVEL_FATAL     // 致命错误
};
```

#### 使用示例

```cpp
// 设置日志级别
log_set_level(LOG_LEVEL_INFO);

// 输出日志
LOG_INFO("Model loaded successfully");
LOG_WARN("GPU not available, using CPU");
LOG_ERROR("Failed to load model");
```

### 5. 控制台交互 (console.h/cpp)

#### 功能
- 终端输出控制
- 进度条显示
- 交互式输入
- 颜色输出

#### 主要功能

```cpp
// 进度条
console_init_progress();
console_update_progress(progress, total);
console_cleanup_progress();

// 颜色输出
console_set_color(CONSOLE_COLOR_GREEN);
console_reset_color();
```

### 6. 采样系统 (sampling.h/cpp)

#### 功能
- 多种采样策略
- 熵控制
- 重复惩罚
- 采样温度

#### 采样方法

1. **基础采样**:
   - Greedy (贪婪采样)
   - Random (随机采样)

2. **Top-k 采样**:
   - 限制候选 token 数量
   - 提高多样性

3. **Top-p (Nucleus) 采样**:
   - 累积概率阈值
   - 平衡多样性和质量

4. **重复惩罚**:
   - 惩罚重复 token
   - 减少循环生成

5. **温度采样**:
   - 控制随机性
   - 0 = 确定性，>0 = 随机

### 7. 推测采样 (speculative.h/cpp)

#### 功能
- 推测解码优化
- 并行生成候选
- 快速验证

#### 工作原理

1. 草稿模型快速生成候选序列
2. 主模型验证候选序列
3. 接受正确部分，拒绝错误部分
4. 加速生成过程

### 8. JSON 处理 (json-partial.*)

#### 功能
- JSON 部分解析
- Schema 验证
- 语法约束

#### 组件

1. **部分解析器** (`json-partial.cpp`):
   - 解析不完整 JSON
   - 流式解析
   - 错误恢复

2. **Schema 转换** (`json-schema-to-grammar.cpp`):
   - JSON Schema 到语法转换
   - 生成语法约束
   - 验证输出

### 9. 正则表达式 (regex-partial.*)

#### 功能
- 正则表达式部分匹配
- 模式验证
- 语法约束

### 10. N-gram 处理

#### 组件

1. **N-gram 缓存** (`ngram-cache.*`):
   - N-gram 统计
   - 缓存管理
   - 查询优化

2. **N-gram 映射** (`ngram-map.*`):
   - N-gram 到 token 映射
   - 概率计算
   - 插值

3. **N-gram 模块** (`ngram-mod.*`):
   - N-gram 语言模型
   - 评分机制
   - 集成推理

### 11. 模型下载 (download.h/cpp)

#### 功能
- 模型文件下载
- 断点续传
- 进度显示
- 校验验证

#### 支持的协议

- HTTP/HTTPS
- HuggingFace Hub
- 文件系统

### 12. HuggingFace 缓存 (hf-cache.*)

#### 功能
- HF 模型缓存
- 自动下载
- 版本管理
- 存储优化

### 13. HTTP 支持 (http.h)

#### 功能
- HTTP 客户端
- 请求处理
- 响应解析
- 错误处理

### 14. PEG 解析器 (peg-parser.*)

#### 功能
- 解析表达式语法 (PEG)
   - 语法定义
   - 解析引擎
   - 语法验证
   - 错误报告

### 15. 参数拟合 (fit.h/cpp)

#### 功能
- 模型参数微调
- LoRA 适配器训练
- 超参数搜索
- 损失计算

### 16. Unicode 支持 (unicode.*)

#### 功能
- Unicode 编解码
- 字符规范化
- 字符计数
- 字符边界处理

### 17. 基础功能

#### Base64 编解码 (base64.hpp)
- Base64 编码
- Base64 解码
- URL 安全编码

#### 构建信息 (build-info.*)
- 编译时间
- Git 版本
- 构建配置
- 编译器信息

#### 调试工具 (debug.*)
- 断言检查
- 内存调试
- 性能分析
- 状态转储

#### LLM Guidance (llguidance.cpp)
- 引导生成
- 格式约束
- 结构化输出
- 验证机制

#### 预设 (preset.*)
- 预设配置
- 配置加载
- 默认设置
- 配置验证

#### 推理预算 (reasoning-budget.*)
- 推理时间控制
- Token 限制
- 成本管理
- 资源分配

## 使用示例

### 基本使用

```cpp
#include "common/common.h"
#include "common/sampling.h"
#include "common/log.h"

int main() {
    // 初始化日志
    log_set_level(LOG_LEVEL_INFO);

    // 解析参数
    gpt_params params;
    params.model = "model.gguf";

    // 加载模型
    llama_model* model = llama_load_model_from_file(params);
    llama_context* ctx = llama_new_context_with_model(model, params);

    // 创建采样器
    llama_sampler* smpl = llama_sampler_init_greedy();

    // 执行推理
    llama_batch batch = llama_batch_init(512, 0);
    // ... 添加 tokens
    llama_decode(ctx, batch);

    // 采样
    llama_token token = llama_sampler_sample(smpl, ctx, -1);

    // 清理
    llama_sampler_free(smpl);
    llama_free(ctx);
    llama_free_model(model);
}
```

### 聊天示例

```cpp
#include "common/chat.h"

int main() {
    // 创建聊天上下文
    chat_context chat;
    chat_init(&chat);

    // 添加用户消息
    chat_add_message(&chat, "user", "Hello!");

    // 生成回复
    std::string reply = chat_generate(&chat, params);

    // 输出回复
    printf("Assistant: %s\n", reply.c_str());

    chat_cleanup(&chat);
}
```

## 性能考虑

### 内存管理

- 使用对象池减少分配
- 及时释放不用的资源
- 监控内存使用

### 缓存策略

- N-gram 缓存加速推理
- HF 缓存减少下载
- 采样结果缓存

### 并发处理

- 多线程采样
- 异步下载
- 并行计算

## 扩展性

### 添加新的采样方法

1. 在 `sampling.h` 中定义接口
2. 在 `sampling.cpp` 中实现
3. 注册到采样器

### 添加新的聊天模板

1. 在 `chat.cpp` 中定义模板
2. 实现模板解析
3. 集成到聊天系统

### 添加新的解析器

1. 继承 PEG 解析器
2. 实现语法规则
3. 集成到系统

## 调试支持

### 日志

- 设置日志级别
- 启用调试输出
- 记录关键操作

### 性能分析

- 计时测量
- 内存使用监控
- 性能计数器

### 状态检查

- 对象状态验证
- 资源泄漏检测
- 一致性检查

## 未来发展

### 计划中的特性

1. **更多采样方法**：扩展采样策略
2. **增强的聊天功能**：更多聊天特性
3. **更好的缓存**：智能缓存策略
4. **分布式支持**：多机器协作

### 社区参与

- 贡献新的采样方法
- 改进现有功能
- 报告问题和建议

## 相关文档

- [common/jinja/README_ZH.md](./jinja/README_ZH.md) - Jinja 模板引擎
- [采样系统文档](./sampling.h)
- [聊天系统文档](./chat.h)

## 许可证

遵循 llama.cpp 项目许可证。