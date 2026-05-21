# 贡献者指南

项目区分了3个级别的贡献者：

- 贡献者：之前有贡献过的人（无特殊权限）
- 协作者（Triage）：有重要贡献的人，可能负责代码的某些部分，并期望维护和审查其拥有的代码的贡献
- 维护者：负责审查和合并 PR，在获得代码所有者批准后

# AI 使用政策

> [!IMPORTANT]
> 本项目**不接受**完全或主要由 AI 生成的拉取请求。AI 工具只能以辅助角色使用。
>
> 重复违反此政策可能会导致您的账户被永久禁止参与项目。
>
> 有关允许和禁止使用 AI 的详细信息，可在 [AGENTS.md](AGENTS.md) 文件中找到。

最初由 AI 生成然后编辑的代码仍被视为 AI 生成。只有当大部分代码由人类贡献者编写，并且 AI 仅用于修正或扩展贡献者已经构思好的冗长修改（例如，生成有细微变化的重复行）时，才允许使用 AI 辅助。

如果使用 AI 生成代码的任何部分，贡献者必须遵守以下要求：

1. 明确披露使用 AI 的方式。
2. 在提交拉取请求之前进行全面的手动审查。
3. 准备在维护者询问时解释提交的每一行代码。
4. 严禁使用 AI 为您编写帖子（错误报告、功能请求、拉取请求描述、GitHub 讨论、回应人类等）。

有关更多信息，请参阅 [AGENTS.md](AGENTS.md) 文件。

# 拉取请求（针对贡献者和协作者）

提交 PR 之前：
- 搜索现有 PR 以防止重复工作
- llama.cpp 使用 ggml 张量库进行模型评估。如果您不熟悉 ggml，请考虑查看 [ggml 仓库中的示例](https://github.com/ggml-org/ggml/tree/master/examples)。[simple](https://github.com/ggml-org/ggml/tree/master/examples/simple) 展示了使用 ggml 的最低要求。[gpt-2](https://github.com/ggml-org/ggml/tree/master/examples/gpt-2) 有使用 GPT-2 进行语言模型推理的最小实现。[mnist](https://github.com/ggml-org/ggml/tree/master/examples/mnist) 演示了如何训练和评估简单的图像分类器
- 测试您的更改：
  - 发布前在本地计算机上[运行完整的 CI](ci/README.md)
  - 验证困惑度和性能不会因您的更改而受到负面影响（使用 `llama-perplexity` 和 `llama-bench`）
  - 如果修改了 `ggml` 源代码，运行 `test-backend-ops` 工具以检查 `ggml` 运算符的不同后端实现是否产生一致的结果（这需要访问至少两个不同的 `ggml` 后端）
  - 如果修改了 `ggml` 运算符或添加了新运算符，请将相应的测试用例添加到 `test-backend-ops`
- 为每个功能或修复创建单独的 PR：
  - 避免在单个 PR 中合并无关的更改
  - 对于复杂的功能，考虑首先打开功能请求以讨论并统一期望
  - 添加新模型或功能支持时，在初始 PR 中专注于**仅 CPU 支持**，除非有充分的理由不这样做。在后续 PR 中添加对其他后端（如 CUDA）的支持
  - 特别是，添加新数据类型（`ggml_type` 枚举的扩展）带来了不成比例的维护负担。因此，要添加新的量化类型，您至少需要满足以下*额外*标准：
    - 使用新类型将小型模型转换为 GGUF 并上传到 HuggingFace
    - 提供与 FP16/BF16（以原生精度为准）以及相似大小类型的[困惑度](https://github.com/ggml-org/llama.cpp/tree/master/tools/perplexity)比较
    - 提供新类型以及相似大小类型相对于 FP16/BF16（以原生精度为准）版本计算的 KL 散度数据
    - 在纯 CPU 上提供新类型与相似大小类型的[性能数据](https://github.com/ggml-org/llama.cpp/tree/master/tools/llama-bench)
- 考虑允许对您的分支的写入访问以加快审查速度，因为审查者可以直接推送提交
- 如果您是新贡献者
    - 将打开的 PR 限制为 1 个
    - 不要提交琐碎的修复（例如拼写错误、格式更改）

提交 PR 后：
- 期望收到修改请求，以确保代码符合 llama.cpp 的质量和长期可维护性标准
- 维护者在做出批准和合并 PR 的最终决定时将依赖您的见解和批准
- 如果您的 PR 变得过时，将其重新建立在最新的 `master` 之上以引起维护者的注意
- 考虑将自己添加到 [CODEOWNERS](CODEOWNERS) 以表明您有可用性来修复相关问题并审查相关 PR

# 拉取请求（针对维护者）

- Squash-merge PR
- 对压缩提交标题使用以下格式：`<module> : <commit title> (#<issue_number>)`。例如：`utils : fix typo in utils.py (#1234)`
- 可选地从这里选择 `<module>`：https://github.com/ggml-org/llama.cpp/wiki/Modules
- 让其他维护者合并自己的 PR
- 合并 PR 时，确保您很好地理解更改
- 注意维护：大部分工作在 PR 合并后进行。如果 PR 作者不承诺长期贡献，则需要其他人承担责任（您）

维护者有权出于任何原因拒绝审查或关闭拉取请求，而无需提出任何问题，特别是在以下任何情况下：
- 提议的更改已在路线图或现有问题中提及，并且已分配给某人。
- 拉取请求与现有 PR 重复。
- 贡献者未遵守此贡献指南或 AI 政策。

# 编码指南

- 避免添加第三方依赖项、额外文件、额外头文件等。
- 始终考虑与其他操作系统和架构的跨兼容性
- 避免花哨的现代 STL 构造，使用基本的 `for` 循环，避免模板，保持简单
- 垂直对齐使内容更易读且易于批量编辑
- 清理任何尾随空白，使用 4 个空格缩进，括号在同一行，`void * ptr`，`int & a`
- 在公共 API 中使用大小整型类型，如 `int32_t`，例如 `size_t` 也可能适用于分配大小或字节偏移量
- 使用 `struct foo {}` 而不是 `typedef struct foo {} foo` 声明结构体
    - 在 C++ 代码中，尽可能省略可选的 `struct` 和 `enum` 关键字
    ```cpp
    // OK
    llama_context * ctx;
    const llama_rope_type rope_type;

    // not OK
    struct llama_context * ctx;
    const enum llama_rope_type rope_type;
    ```

    _（注意：此指南尚未应用于 `llama.cpp` 代码库。新代码应遵循此指南。）_

- 尝试遵循代码中的现有模式（缩进、空格等）。如果有疑问，使用 `clang-format`（来自 clang-tools v15+）来格式化添加的代码
- 对于当前指南未涵盖的任何内容，请参阅 [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- 张量按行主序存储数据。我们将维度 0 称为列，1 称为行，2 称为矩阵
- 矩阵乘法非常规：[`C = ggml_mul_mat(ctx, A, B)`](https://github.com/ggml-org/llama.cpp/blob/880e352277fc017df4d5794f0c21c44e1eae2b84/ggml.h#L1058-L1064) 表示 $C^T = A B^T \Leftrightarrow C = B A^T.$

![matmul](media/matmul.png)

# 命名指南

- 使用 `snake_case` 作为函数、变量和类型名称
- 命名通常优化最长公共前缀（参见 https://github.com/ggml-org/ggml/pull/302#discussion_r1243240963）

    ```cpp
    // not OK
    int small_number;
    int big_number;

    // OK
    int number_small;
    int number_big;
    ```

- 枚举值始终为大写并以枚举名称为前缀

    ```cpp
    enum llama_vocab_type {
        LLAMA_VOCAB_TYPE_NONE = 0,
        LLAMA_VOCAB_TYPE_SPM  = 1,
        LLAMA_VOCAB_TYPE_BPE  = 2,
        LLAMA_VOCAB_TYPE_WPM  = 3,
        LLAMA_VOCAB_TYPE_UGM  = 4,
        LLAMA_VOCAB_TYPE_RWKV = 5,
    };
    ```

- 通用命名模式是 `<class>_<method>`，其中 `<method>` 是 `<action>_<noun>`

    ```cpp
    llama_model_init();           // class: "llama_model",         method: "init"
    llama_sampler_chain_remove(); // class: "llama_sampler_chain", method: "remove"
    llama_sampler_get_seed();     // class: "llama_sampler",       method: "get_seed"
    llama_set_embeddings();       // class: "llama_context",       method: "set_embeddings"
    llama_n_threads();            // class: "llama_context",       method: "n_threads"
    llama_adapter_lora_free();    // class: "llama_adapter_lora",  method: "free"
    ```

    - 可以省略 `get` `<action>`
    - 如果不需要，可以省略 `<noun>`
    - `<class>` 的 `_context` 后缀是可选的。在需要时使用它来消除符号歧义
    - 使用 `init`/`free` 作为构造函数/析构函数 `<action>`

- 当类型应对用户不透明时使用 `_t` 后缀——用户并不关心它是结构体还是其他东西

    ```cpp
    typedef struct llama_context * llama_context_t;

    enum llama_pooling_type llama_pooling_type(const llama_context_t ctx);
    ```

    _（注意：此指南尚未应用于 `llama.cpp` 代码库。新代码应遵循此指南）_

- C/C++ 文件名全部小写带破折号。头文件使用 `.h` 扩展名。源文件使用 `.c` 或 `.cpp` 扩展名
- Python 文件名全部小写带下划线

- _（TODO：缩写使用）_

# 预处理指令

- _（TODO：添加带示例的指南并将其应用于代码库）_

    ```cpp
    #ifdef FOO
    #endif // FOO
    ```

# 代码维护

- 现有代码应在 [CODEOWNERS](CODEOWNERS) 文件中指定指定的协作者和/或维护者，负责：
  - 审查和合并相关 PR
  - 修复相关错误
  - 提供开发者指导/支持

- 添加或修改大块代码时：
  - 如果您是协作者，请务必将自己添加到 [CODEOWNERS](CODEOWNERS) 以表明您有可用性来审查相关 PR
  - 如果您是贡献者，请找到愿意长期审查和维护您代码的现有协作者
  - 提供必要的 CI 工作流（和硬件）来测试您的更改（参见 [ci/README.md](https://github.com/ggml-org/llama.cpp/tree/master/ci)）

- 新代码应遵循本文档概述的指南（编码、命名等）。允许在不直接与 `ggml` 接口交互的孤立、特定于后端的代码部分中出现例外。
  _（注意：出于遗留原因，现有代码不要求遵循此指南）_

- 对于服务器中的更改，请务必参阅 [服务器开发文档](./tools/server/README-dev.md)

# 文档

- 文档是社区的努力成果
- 当您需要查看源代码以弄清楚如何使用 API 时，考虑在头文件中添加简短摘要以供将来参考
- 当您注意到不正确或过时的文档时，请更新它

# 资源

GitHub 问题、PR 和讨论包含大量信息，这些信息对于熟悉代码库很有用。为方便起见，从 GitHub 项目引用了一些更重要的信息：

https://github.com/ggml-org/llama.cpp/projects