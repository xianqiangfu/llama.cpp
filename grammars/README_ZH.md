# GBNF 语法文件说明

本目录包含 GBNF (GGML BNF) 语法文件，用于约束 llama.cpp 的模型输出。

## 简介

GBNF (GGML BNF) 是一种用于定义形式文法的格式，可以在 llama.cpp 中约束模型输出。例如，可以用它强制模型生成有效的 JSON，或者只输出表情符号。

GBNF 语法在 `tools/cli`、`tools/completion` 和 `tools/server` 中以多种方式得到支持。

## 背景

[Backus-Naur Form (BNF)](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form) 是一种用于描述编程语言、文件格式和协议等形式语言语法的符号表示法。GBNF 是 BNF 的扩展，主要添加了一些类似现代正则表达式的功能。

## 基础概念

在 GBNF 中，我们定义*产生式规则*，指定*非终结符*（规则名）如何替换为*终结符*（字符，特别是 Unicode 码点）和其他非终结符的序列。产生式规则的基本格式是 `nonterminal ::= sequence...`。

## 示例

在深入之前，让我们看看 `grammars/chess.gbnf` 中演示的一些功能，这是一个国际象棋符号语法：

```
# `root` 指定整体输出的模式
root ::= (
    # 它必须以字符 "1. " 开头，后跟匹配 `move` 规则的字符序列，
    # 后跟空格，再跟另一个移动，然后是换行符
    "1. " move " " move "\n"

    # 后跟一个或多个后续移动，用一位或两位数字编号
    ([1-9] [0-9]? ". " move " " move "\n")+
)

# `move` 是一个抽象表示，可以是 pawn、nonpawn 或 castle。
# `[+#]?` 表示移动后可能有将军或将死符号
move ::= (pawn | nonpawn | castle) [+#]?

pawn ::= ...
nonpawn ::= ...
castle ::= ...
```

## 非终结符和终结符

非终结符符号（规则名）代表终结符和其他非终结符的模式。它们必须是带短划线的小写单词，如 `move`、`castle` 或 `check-mate`。

终结符是实际字符（码点）。它们可以指定为序列，如 `"1"` 或 `"O-O"`，或者范围，如 `[1-9]` 或 `[NBKQR]`。

## 字符和字符范围

终结符支持完整的 Unicode 范围。Unicode 字符可以在语法中直接指定，例如 `hiragana ::= [ぁ-ゟ]`，或使用转义：8位（`\xXX`）、16位（`\uXXXX`）或 32位（`\UXXXXXXXX`）。

字符范围可以取反：

```
single-line ::= [^\n]+ "\n"
```

## 序列和选择

序列中符号的顺序很重要。例如，在 `"1. " move " " move "\n"` 中，`"1. "` 必须在第一个 `move` 之前，依此类推。

选择，用 `|` 表示，给出不同的可接受序列。例如，在 `move ::= pawn | nonpawn | castle` 中，`move` 可以是 `pawn` 移动、`nonpawn` 移动或 `castle`。

括号 `()` 可以用于分组序列，这允许在更大的规则中嵌入选择，或对序列应用重复和可选符号（如下）。

## 重复和可选符号

- 符号或序列后的 `*` 表示可以重复零次或多次（等同于 `{0,}`）
- `+` 表示符号或序列应出现一次或多次（等同于 `{1,}`）
- `?` 使前面的符号或序列可选（等同于 `{0,1}`）
- `{m}` 精确重复前一个符号或序列 `m` 次
- `{m,}` 重复前一个符号或序列至少 `m` 次
- `{m,n}` 在 `m` 和 `n` 次之间（包含）重复前一个符号或序列
- `{0,n}` 重复前一个符号或序列最多 `n` 次（包含）

## Tokens

Tokens 允许语法匹配特定的 tokenizer tokens 而不是字符序列。这对于基于特殊 tokens（如 ````<|start_of_assistant|>````）约束输出很有用。

Tokens 可以通过两种方式指定：

1. **Token ID**：使用方括号内的 token ID 的尖括号：`<[token-id]>`。例如，`<[1000]>` 匹配 ID 为 1000 的 token。

2. **Token 字符串**：直接使用 token 文本的尖括号：````<token>````。例如，````<|start_of_assistant|>```` 将匹配文本正好是 ````<|start_of_assistant|>```` 的 token。这只在字符串在词汇表中完全标记为一个 token 时才有效，否则语法将无法解析。

你可以使用 `!` 前缀否定 token 匹配：`!<[1000]>` 或 ````!<|start_of_assistant|>```` 匹配除指定 token 之外的任何 token。

```
# 匹配一个思考块：````<|thinking|>...<|end_of_assistant|>````

# 使用 token 字符串（要求这些在词汇表中是单个 tokens）
root ::= ````<|thinking|>```` thinking ````<|end_of_assistant|>````
thinking ::= !````<|end_of_assistant|>````
```

## 注释和换行

注释可以用 `#` 指定：

```
# 定义可选空白
ws ::= [ \t\n]+
```

规则之间以及括号内嵌套的符号或序列之间允许换行。此外，选择标记 `|` 后的换行将继续当前规则，即使不在括号内。

## 根规则

在完整的语法中，`root` 规则总是定义语法的起点。换句话说，它指定整个输出必须匹配的内容。

```
# 列表的语法
root ::= ("- " item)+
item ::= [^\n]+ "\n"
```

## 使用 GBNF 语法

你可以使用 GBNF 语法：

- 在 [llama-server](../tools/server/) 的补全端点中，作为 `grammar` body 字段传递
- 在 [llama-cli](../tools/cli/) 和 [llama-completion](../tools/completion/) 中，作为 `--grammar` 和 `--grammar-file` 标志传递
- 与 [test-gbnf-validator](../tests/test-gbnf-validator.cpp) 一起使用，测试字符串

## JSON Schema 转 GBNF

`llama.cpp` 支持将 https://json-schema.org/ 的一个子集转换为 GBNF 语法：

- 在 [llama-server](../tools/server/) 中：
  - 对于任何补全端点，作为 `json_schema` body 字段传递
  - 对于 `/chat/completions` 端点，在 `response_format` body 字段内传递（例如 `{"type": "json_object", "schema": {"items": {}}}` 或 `{ type: "json_schema", json_schema: {"schema": ...} }`）
- 在 [llama-cli](../tools/cli/) 和 [llama-completion](../tools/completion/) 中，作为 `--json` / `-j` 标志传递
- 提前转换为语法：
  - 在 CLI 中，使用 [examples/json_schema_to_grammar.py](../examples/json_schema_to_grammar.py)

> [!NOTE]
> JSON schema 仅用于约束模型输出，不会注入到提示词中。模型看不到 schema，所以如果你想让它理解预期的结构，请在提示词中明确描述。这不适用于工具调用，其中 schema 被注入到提示词中。

查看 [tests](../tests/test-json-schema-to-grammar.cpp) 以了解可能支持的功能（你也会在 https://github.com/ggml-org/llama.cpp/pull/5978、https://github.com/ggml-org/llama.cpp/pull/6659 和 https://github.com/ggml-org/llama.cpp/pull/6555 中找到使用示例）。

## 语法文件列表

本目录包含以下预定义的语法文件：

| 文件 | 描述 |
|------|------|
| README.md | 英文文档 |
| README_ZH.md | 本文档（中文） |
| arithmetic.gbnf | 算术表达式语法 |
| c.gbnf | C 语言语法 |
| chess.gbnf | 国际象棋符号语法 |
| english.gbnf | 英语语法约束 |
| japanese.gbnf | 日语语法约束 |
| json.gbnf | JSON 格式语法 |
| json_arr.gbnf | JSON 数组语法 |
| list.gbnf | 列表格式语法 |

## 使用示例

### 使用语法文件

```bash
# 使用 chess.gbnf 约束国际象棋棋谱输出
./llama-cli \
  -m model.gguf \
  --grammar-file grammars/chess.gbnf \
  -p "Generate a chess opening"

# 使用 json.gbnf 生成有效的 JSON
./llama-cli \
  -m model.gguf \
  --grammar-file grammars/json.gbnf \
  -p "Generate a JSON object with name and age"
```

### 使用 JSON Schema

```bash
# 直接使用 JSON Schema
./llama-cli \
  -m model.gguf \
  -j '{"type": "object", "properties": {"name": {"type": "string"}}}' \
  -p "Generate a person object"
```

## 常见问题

### Q: 语法约束会影响性能吗？

A: 会有一定影响，但通常可以接受。复杂的语法可能会降低生成速度。

### Q: 如何调试语法错误？

A: 使用 `test-gbnf-validator` 测试语法：

```bash
./test-gbnf-validator grammar.gbnf
```

### Q: 可以混合多个语法吗？

A: 不直接支持，但可以创建一个包含多个规则的语法文件。

### Q: JSON Schema 支持哪些功能？

A: 支持基本的 JSON Schema 功能，如类型、属性、必需字段等。某些高级功能可能不支持。

## 性能注意事项

语法目前有一些性能陷阱（参见 https://github.com/ggml-org/llama.cpp/issues/4218）。

### 高效的可选重复

一个常见模式是允许模式 `x` 最多重复 N 次。

虽然在语义上是正确的，但语法 `x? x? x?.... x?`（有 N 个重复）可能会导致采样极其缓慢。相反，你可以写 `x{0,N}`（或在早期 llama.cpp 版本中使用 N 层嵌套的 `(x (x (x ... (x)?...)?)?)?`）。

## 相关文档

- [测试总览](../tests/README_ZH.md)
- [PEG 解析器测试](../tests/peg-parser/README_ZH.md)
- [llama-server](../tools/server/)
- [开发文档](../docs/development/README_ZH.md)

## 贡献

欢迎贡献新的语法文件：

1. 创建新的 `.gbnf` 文件
2. 添加使用示例
3. 测试语法正确性
4. 更新文档

## 参考资源

- [形式文法](https://en.wikipedia.org/wiki/Formal_grammar)
- [Backus-Naur Form](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form)
- [JSON Schema](https://json-schema.org/)