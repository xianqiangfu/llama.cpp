# GBNF 语法约束实现分析

## 概述

GBNF (GGML BNF) 是 llama.cpp 实现的一种基于 Backus-Naur Form 的语法约束系统，用于约束模型输出格式。通过 GBNF，可以确保模型输出的文本符合特定的语法规则，如 JSON、XML、SQL 等格式。

## 语法定义

### 1. 语法元素类型

```cpp
enum llama_gretype {
    // 规则定义结束
    LLAMA_GRETYPE_END            = 0,

    // 替代定义开始
    LLAMA_GRETYPE_ALT            = 1,

    // 非终结符元素：规则引用
    LLAMA_GRETYPE_RULE_REF       = 2,

    // 终结符元素：字符
    LLAMA_GRETYPE_CHAR           = 3,

    // 反向字符
    LLAMA_GRETYPE_CHAR_NOT       = 4,

    // 字符范围
    LLAMA_GRETYPE_CHAR_RNG_UPPER = 5,

    // 替代字符
    LLAMA_GRETYPE_CHAR_ALT       = 6,

    // 任意字符 (.)
    LLAMA_GRETYPE_CHAR_ANY       = 7,

    // 终结符元素：token (<[token-id]>)
    LLAMA_GRETYPE_TOKEN          = 8,

    // 反向 token
    LLAMA_GRETYPE_TOKEN_NOT      = 9,
};
```

### 2. 语法元素结构

```cpp
typedef struct llama_grammar_element {
    enum llama_gretype type;  // 元素类型
    uint32_t           value; // Unicode 码点、规则 ID 或 token ID
} llama_grammar_element;
```

### 3. 语法规则

```cpp
// 单个规则
using llama_grammar_rule  = std::vector<llama_grammar_element>;

// 规则集合
using llama_grammar_rules      = std::vector<llama_grammar_rule>;

// 语法栈（用于解析）
using llama_grammar_stack     = std::vector<const llama_grammar_element *>;

// 语法栈集合
using llama_grammar_stacks     = std::vector<llama_grammar_stack>;
```

## 解析器实现

### 1. PEG 解析器

使用 PEG (Parsing Expression Grammar) 作为底层解析器：

```cpp
struct llama_grammar_parser {
    const llama_vocab * vocab;
    std::map<std::string, uint32_t> symbol_ids;

    llama_grammar_rules rules;

    llama_grammar_parser(const struct llama_vocab * vocab = nullptr)
        : vocab(vocab) {}

    // 生成符号 ID
    uint32_t get_symbol_id(const char * src, size_t len);
    uint32_t generate_symbol_id(const std::string & base_name);

    // 添加规则
    void add_rule(uint32_t rule_id, const llama_grammar_rule & rule);

    // 解析方法
    const char * parse_alternates(
        const char        * src,
        const std::string & rule_name,
        uint32_t            rule_id,
        bool                is_nested);

    const char * parse_sequence(
        const char         * src,
        const std::string  & rule_name,
        llama_grammar_rule & rule,
        bool               is_nested);

    const char * parse_rule(const char * src);
    bool parse(const char * src);
};
```

### 2. 解析流程

```cpp
bool llama_grammar_parser::parse(const char * src) {
    // 1. 跳过空白和注释
    const char * pos = parse_space(src, true);

    // 2. 解析规则
    while (*pos) {
        pos = parse_rule(pos);
        pos = parse_space(pos, true);
    }

    return true;
}
```

### 3. 规则解析

```cpp
const char * llama_grammar_parser::parse_rule(const char * src) {
    // 1. 解析规则名称
    const char * name_start = src;
    const char * name_end = parse_name(src);
    std::string name(name_start, name_end - name_start);

    // 2. 分配规则 ID
    uint32_t rule_id = get_symbol_id(name.c_str(), name.size());

    // 3. 解析 ::= 分隔符
    const char * pos = parse_space(name_end, true);
    if (*pos != ':' || *(pos+1) != ':' || *(pos+2) != '=') {
        throw std::runtime_error("expecting ::=");
    }
    pos += 3;
    pos = parse_space(pos, true);

    // 4. 解析规则内容
    llama_grammar_rule rule;
    pos = parse_alternates(pos, name, rule_id, false, rule);

    // 5. 添加规则
    add_rule(rule_id, rule);

    return pos;
}
```

## 语法约束应用

### 1. 语法结构

```cpp
struct llama_grammar {
    // 词表
    const llama_vocab * vocab;

    // 语法规则
    const llama_grammar_rules  rules;

    // 解析栈
    llama_grammar_stacks stacks;

    // UTF-8 部分解码
    llama_partial_utf8 partial_utf8;

    // 懒加载支持
    bool                     lazy;
    bool                     awaiting_trigger;
    std::string              trigger_buffer;
    std::vector<token_pos>   trigger_buffer_positions;
    std::vector<llama_token> trigger_tokens;
    std::vector<llama_grammar_trigger_pattern> trigger_patterns;
};
```

### 2. 初始化语法

```cpp
struct llama_grammar * llama_grammar_init_impl(
    const struct llama_vocab * vocab,
    const char * grammar_str,
    const char * grammar_root,
    bool lazy,
    const char ** trigger_patterns,
    size_t num_trigger_patterns,
    const llama_token * trigger_tokens,
    size_t num_trigger_tokens) {

    // 1. 创建解析器
    llama_grammar_parser parser(vocab);

    // 2. 解析语法字符串
    if (!parser.parse(grammar_str)) {
        return nullptr;
    }

    // 3. 创建语法对象
    llama_grammar * grammar = new llama_grammar();
    grammar->vocab = vocab;
    grammar->rules = std::move(parser.rules);

    // 4. 初始化栈
    uint32_t start_rule_id = parser.get_symbol_id(grammar_root, strlen(grammar_root));
    grammar->stacks.push_back({&grammar->rules[start_rule_id][0]});

    // 5. 设置懒加载
    grammar->lazy = lazy;
    grammar->awaiting_trigger = lazy;

    // 6. 设置触发模式
    for (size_t i = 0; i < num_trigger_patterns; i++) {
        llama_grammar_trigger_pattern pattern;
        pattern.pattern = trigger_patterns[i];
        pattern.regex = std::regex(trigger_patterns[i]);
        grammar->trigger_patterns.push_back(pattern);
    }

    // 7. 设置触发 tokens
    for (size_t i = 0; i < num_trigger_tokens; i++) {
        grammar->trigger_tokens.push_back(trigger_tokens[i]);
    }

    return grammar;
}
```

### 3. 应用约束

```cpp
void llama_grammar_apply_impl(
    const struct llama_grammar & grammar,
    llama_token_data_array * cur_p) {

    // 1. 获取候选 token
    std::vector<llama_grammar_candidate> candidates;
    for (size_t i = 0; i < cur_p->size; ++i) {
        candidates.push_back({
            i,
            get_code_points(vocab, cur_p->data[i].id),
            partial_utf8,
            cur_p->data[i].id
        });
    }

    // 2. 对每个栈，过滤候选
    for (const auto & stack : grammar.stacks) {
        auto rejected = llama_grammar_reject_candidates_for_stack(
            grammar.rules, stack, candidates);

        // 标记拒绝的候选
        for (auto idx : rejected) {
            cur_p->data[idx].logit = -INFINITY;
        }
    }
}
```

### 4. 接受 token

```cpp
void llama_grammar_accept_impl(
    struct llama_grammar & grammar,
    llama_token token) {

    // 1. 获取 token 的代码点
    auto [code_points, partial_utf8] =
        decode_utf8(get_token_text(grammar.vocab, token),
                   grammar.partial_utf8);

    // 2. 更新部分 UTF-8
    grammar.partial_utf8 = partial_utf8;

    // 3. 对每个代码点更新栈
    for (auto code_point : code_points) {
        llama_grammar_accept(&grammar, code_point);
    }
}
```

### 5. 栈更新

```cpp
void llama_grammar_accept(struct llama_grammar * grammar, uint32_t chr) {
    // 1. 创建新栈集合
    llama_grammar_stacks new_stacks;

    // 2. 对每个栈，尝试接受字符
    for (const auto & stack : grammar.stacks) {
        auto new_stack = llama_grammar_advance_stack(grammar->rules, stack, chr);
        if (!new_stack.empty()) {
            new_stacks.insert(new_stacks.end(),
                             new_stack.begin(), new_stack.end());
        }
    }

    // 3. 更新语法栈
    grammar.stacks = std::move(new_stacks);
}
```

## 懒加载语法

### 1. 基本概念

懒加载语法等待触发条件满足后才应用约束：

```cpp
bool lazy             = false;  // 是否为懒加载
bool awaiting_trigger = false;  // 是否等待触发
std::string trigger_buffer;    // 触发缓冲区
```

### 2. 触发机制

**Token 触发**：
```cpp
// 检查是否遇到触发 token
for (auto trigger_token : grammar.trigger_tokens) {
    if (token == trigger_token) {
        grammar.awaiting_trigger = false;
        // 应用缓冲的输出
        break;
    }
}
```

**模式触发**：
```cpp
// 检查输出是否匹配触发模式
for (auto & pattern : grammar.trigger_patterns) {
    size_t pos = pattern.find(trigger_buffer);
    if (pos != std::string::npos) {
        grammar.awaiting_trigger = false;
        // 从匹配位置开始应用语法
        break;
    }
}
```

### 3. 缓冲机制

```cpp
// 等待触发时，缓冲输出
if (grammar.awaiting_trigger) {
    grammar.trigger_buffer += text;
    grammar.trigger_buffer_positions.push_back(
        {token, {start, end}});

    // 不应用语法约束
    return;
}

// 触发后，应用约束
apply_grammar(grammar, text);
```

## JSON Schema 转换

### 1. 转换接口

```cpp
std::string json_schema_to_grammar(
    const nlohmann::ordered_json & schema,
    bool force_gbnf = false);
```

### 2. Schema 信息探测

```cpp
class common_schema_info {
    // 解析引用
    void resolve_refs(nlohmann::ordered_json & schema);

    // 检查是否解析为字符串
    bool resolves_to_string(const nlohmann::ordered_json & schema);
};
```

### 3. 语法构建器

```cpp
struct common_grammar_builder {
    // 添加规则
    std::function<std::string(const std::string &,
                           const std::string &)> add_rule;

    // 添加 schema
    std::function<std::string(const std::string &,
                             const nlohmann::ordered_json &)> add_schema;

    // 解析引用
    std::function<void(nlohmann::ordered_json &)> resolve_refs;
};
```

### 4. 示例转换

**JSON Schema**：
```json
{
    "type": "object",
    "properties": {
        "name": {"type": "string"},
        "age": {"type": "integer"}
    }
}
```

**GBNF 语法**：
```
root ::= "{" ws string ":" ws value "}"
string ::= "\"" ([^"\\] | "\\" .)* "\""
integer ::= "-"? ([0-9])+
ws ::= [ \t\n\r]*
```

## 应用场景

### 1. 结构化输出

**JSON 输出**：
```cpp
const char * json_grammar = R"(
    root ::= "{" ws string ":" ws value "}"
    value ::= object | array | string | number | "true" | "false" | "null"
    object ::= "{" ws "}" | "{" ws member ("," ws member)* ws "}"
    member ::= string ":" ws value
    array ::= "[" ws "]" | "[" ws value ("," ws value)* ws "]"
    string ::= "\"" ([^"\\] | "\\" .)* "\""
    number ::= "-"? ([0-9])+
    ws ::= [ \t\n\r]*
)";

llama_grammar * grammar = llama_grammar_init(
    vocab, json_grammar, "root", false, nullptr, 0, nullptr, 0);
```

### 2. 代码生成

**Python 代码**：
```
root ::= (statement)*
statement ::= "if" ws condition ":" ws block |
              "for" ws identifier "in" ws iterable ":" ws block |
              identifier "=" ws expression
block ::= statement | "{" ws (statement)* ws "}"
```

### 3. 格式验证

**SQL 查询**：
```
root ::= select_stmt
select_stmt ::= "SELECT" ws columns "FROM" ws table
               ("WHERE" ws condition)?
columns ::= column ("," ws column)*
condition ::= column ws operator ws value
```

### 4. 协议交互

**API 请求**：
```
root ::= "{" ws "method" ":" ws string "," ws
        "params" ":" ws object "}"
method ::= "GET" | "POST" | "PUT" | "DELETE"
```

## 性能优化

### 1. 栈去重

```cpp
// 去除重复的栈
std::set<llama_grammar_stack> unique_stacks(
    grammar.stacks.begin(),
    grammar.stacks.end());
grammar.stacks.assign(unique_stacks.begin(),
                     unique_stacks.end());
```

### 2. 候选过滤

```cpp
// 提前过滤不可能的候选
auto rejected = llama_grammar_reject_candidates_for_stack(
    rules, stack, candidates);

// 只保留可能接受的候选
for (auto idx : rejected) {
    candidates[idx].logit = -INFINITY;
}
```

### 3. 缓存优化

```cpp
// 缓存代码点分解
std::unordered_map<llama_token, std::vector<uint32_t>> token_codepoints_cache;

auto get_code_points = [&](llama_token token) {
    auto it = token_codepoints_cache.find(token);
    if (it != token_codepoints_cache.end()) {
        return it->second;
    }

    auto code_points = decode_code_points(token);
    token_codepoints_cache[token] = code_points;
    return code_points;
};
```

## 使用示例

### 1. 命令行使用

```bash
# 使用语法约束
./main -m model.gguf \
    --grammar 'root ::= "Hello, " name "!"'

# 从文件加载语法
./main -m model.gguf \
    --grammar-file json.gbnf

# 使用 JSON Schema
./main -m model.gguf \
    --json-schema '{"type": "object", "properties": {"name": {"type": "string"}}}'

# 使用懒加载语法
./main -m model.gguf \
    --grammar 'root ::= json' \
    --grammar-trigger-pattern '"tool":'
```

### 2. 代码中使用

```cpp
// 1. 创建语法
const char * grammar_str = R"(
    root ::= "{" ws string ":" ws value "}"
    value ::= object | array | string | number
    object ::= "{" ws "}"
    string ::= "\"" ([^"\\] | "\\" .)* "\""
    number ::= "-"? ([0-9])+
    ws ::= [ \t\n\r]*
)";

llama_grammar * grammar = llama_grammar_init(
    vocab, grammar_str, "root",
    false, nullptr, 0, nullptr, 0);

// 2. 应用语法到采样器
llama_sampler * sampler = llama_sampler_init_grammar(grammar);

// 3. 采样
llama_token token = llama_sampler_sample(sampler, ctx, token);

// 4. 接受 token
llama_grammar_accept(grammar, token);

// 5. 清理
llama_sampler_free(sampler);
llama_grammar_free(grammar);
```

### 3. JSON Schema 使用

```cpp
// 1. 定义 JSON Schema
const char * schema_str = R"({
    "type": "object",
    "properties": {
        "name": {"type": "string"},
        "age": {"type": "integer", "minimum": 0}
    },
    "required": ["name", "age"]
})";

// 2. 转换为 GBNF
auto schema = nlohmann::ordered_json::parse(schema_str);
std::string grammar_str = json_schema_to_grammar(schema);

// 3. 创建语法
llama_grammar * grammar = llama_grammar_init(
    vocab, grammar_str.c_str(), "root",
    false, nullptr, 0, nullptr, 0);
```

## 最佳实践

### 1. 语法设计

**简洁性**：
- 使用规则复用减少重复
- 避免嵌套过深

**可读性**：
- 使用有意义的规则名称
- 添加注释说明复杂规则

**性能**：
- 避免回溯
- 使用确定性的选择

### 2. 错误处理

**语法错误**：
```cpp
llama_grammar * grammar = llama_grammar_init(...);
if (!grammar) {
    fprintf(stderr, "Failed to parse grammar\n");
    return 1;
}
```

**运行时错误**：
```cpp
if (grammar.stacks.empty()) {
    fprintf(stderr, "Grammar constraint violated\n");
    // 回退到无约束采样
}
```

### 3. 调试技巧

**打印语法规则**：
```cpp
llama_grammar_parser parser(vocab);
parser.parse(grammar_str);
parser.print(stderr);
```

**监控栈状态**：
```cpp
printf("Number of active stacks: %zu\n", grammar.stacks.size());
printf("Partial UTF-8: %d bytes remaining\n",
       grammar.partial_utf8.n_remain);
```

## 常见问题

### 1. 语法不生效

**可能原因**：
- 语法字符串格式错误
- 根规则名称不匹配
- 词表不支持

**解决方案**：
- 检查语法字符串格式
- 验证根规则名称
- 使用正确的词表

### 2. 性能下降

**可能原因**：
- 语法规则过于复杂
- 栈数量爆炸
- 没有使用懒加载

**解决方案**：
- 简化语法规则
- 使用懒加载语法
- 缓存代码点分解

### 3. 输出不符合预期

**可能原因**：
- 语法规则不完整
- 字符集限制过严
- 没有考虑所有情况

**解决方案**：
- 完善语法规则
- 放宽字符集限制
- 测试边界情况

## 扩展功能

### 1. 自定义语法元素

```cpp
// 添加自定义语法元素
enum llama_gretype_custom {
    LLAMA_GRETYPE_CUSTOM_REGEX = 10,
    LLAMA_GRETYPE_CUSTOM_LOOKUP = 11,
};
```

### 2. 语法组合

```cpp
// 组合多个语法
llama_grammar * combine_grammars(
    llama_grammar * grammar1,
    llama_grammar * grammar2);
```

### 3. 语法优化

```cpp
// 优化语法规则
llama_grammar * optimize_grammar(
    const llama_grammar * grammar);
```

## 结论

GBNF 语法约束系统为 llama.cpp 提供了强大的输出格式控制能力。通过基于 BNF 的语法定义和灵活的触发机制，可以确保模型输出符合各种结构化格式要求。系统支持懒加载、JSON Schema 转换等高级功能，并提供了良好的性能优化和扩展性。

## 相关文件

- `src/llama-grammar.h`: 语法数据结构定义
- `src/llama-grammar.cpp`: 语法解析和应用实现
- `common/peg-parser.h`: PEG 解析器接口
- `common/peg-parser.cpp`: PEG 解析器实现
- `common/json-schema-to-grammar.h`: JSON Schema 转换
- `common/json-schema-to-grammar.cpp`: JSON Schema 转换实现
- `common/chat.h`: Chat 语法约束
- `common/sampling.h`: 采样接口
- `src/llama-sampler.h`: 采样器实现
