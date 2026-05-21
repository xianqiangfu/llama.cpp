# Parser 工具

本目录包含用于调试和分析聊天模板的工具。

## 工具列表

### 1. debug-template-parser

聊天模板调试工具，用于分析和验证 Jinja2 模板的渲染过程。

#### 功能

- 分析聊天模板的结构和变量
- 调试模板渲染过程
- 比较不同模板的输出
- 显示模板中的变量和参数

#### 使用方法

```bash
./debug-template-parser --template-path <模板路径> [选项]
```

#### 主要选项

- `--template-path PATH`: 指定要分析的模板文件路径
- `--with-tools`: 启用工具调用支持
- `--generation-prompt`: 使用生成提示词模式
- `--enable-reasoning`: 启用推理/思考模式
- `--debug-jinja`: 启用 Jinja 调试输出
- `--mode MODE`: 输出模式（ANALYSIS/TEMPLATE/BOTH）
- `--input-message TYPE`: 输入消息类型

### 2. template-analysis

模板分析工具，用于分析模板文件的结构和内容。

#### 功能

- 分析模板文件结构
- 提取模板变量和参数
- 生成模板使用报告
- 批量分析多个模板

#### 使用方法

```bash
./template-analysis [选项]
```

#### 主要选项

- `--template-path PATH`: 指定要分析的模板文件
- `--mode MODE`: 输出模式（ANALYSIS/TEMPLATE/BOTH）
- `--input-message TYPE`: 输入消息类型
- `--output-format FORMAT`: 输出格式

## 支持的模板

工具支持多种聊天模板，包括但不限于：

- Apertus, Apriel
- ByteDance-Seed, Cohere-Command
- GLM-4.6, GLM-4.7-Flash
- Kimi, MiniMax, Mistral
- NVIDIA-Nemotron, NousResearch-Hermes
- Qwen, Qwen2.5, Qwen3
- 深度搜索相关模型

## 常见用例

### 分析单个模板

```bash
./debug-template-parser --template-path models/templates/llama3.jinja
```

### 比较两个模板

```bash
./debug-template-parser --template-path template1.jinja --mode ANALYSIS
./debug-template-parser --template-path template2.jinja --mode ANALYSIS
```

### 调试模板渲染

```bash
./debug-template-parser --template-path models/templates/chatml.jinja --debug-jinja
```

### 分析工具调用支持

```bash
./debug-template-parser --template-path template.jinja --with-tools
```

## 开发说明

### 添加新模板支持

要为新模型添加模板支持：

1. 将模板文件放在 `models/templates/` 目录
2. 确保模板符合 Jinja2 语法规范
3. 使用模板分析工具验证模板结构
4. 更新相关配置文件

### 调试技巧

- 使用 `--debug-jinja` 选项查看详细的渲染过程
- 尝试不同的输入消息类型以测试各种场景
- 使用 `--mode BOTH` 同时查看分析和模板输出
- 检查模板中的变量是否正确传递

## 依赖项

- Jinja2 运行时支持
- nlohmann/json (JSON 处理)
- llama.cpp 核心库
- GBNF 语法解析器

## 相关文档

- [GBNF 语法指南](../../grammars/README.md)
- [Jinja2 模板文档](../../common/jinja/README.md)
- [聊天模板支持](https://github.com/ggml-org/llama.cpp/wiki/Templates-supported-by-llama_chat_apply_template)

## 注意事项

1. 这些工具主要用于开发和调试目的
2. 在生产环境中使用前请充分测试模板
3. 确保模板路径正确且文件可读
4. 某些高级功能可能需要特定的模型支持