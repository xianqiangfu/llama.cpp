# 模型转换示例

此目录包含帮助将 HuggingFace PyTorch 模型转换为 GGUF 格式的脚本和代码。

创建此工具的动机是，转换过程通常是一个迭代过程，需要检查原始模型、转换模型、更新 llama.cpp、再次转换等。一旦模型转换完成，需要根据原始模型进行验证，然后可选择量化，在某些情况下还需要检查量化模型的困惑度。最后，需要将模型上传到 ggml-org 的 Hugging Face。此工具/示例旨在帮助完成此过程。

> 📝 **注意：** 在添加来自现有系列的新模型时，请先验证前一版本通过 logits 验证。现有模型可能存在细微的数值差异，这些差异不会影响生成质量，但会导致 logits 不匹配。提前识别这些差异（无论是在 llama.cpp、转换脚本中，还是在上游实现中）可以节省大量调试时间。

### 概述

这里的 makefile 目标和脚本可以在开发/转换过程中使用，协助完成以下工作：

* 检查/运行原始模型以了解其工作原理
* 将原始模型转换为 GGUF 格式
* 检查/运行转换后的模型
* 验证原始模型和转换模型生成的 logits
* 将模型量化为 GGUF 格式
* 运行困惑度评估以验证量化模型是否按预期运行
* 将模型上传到 HuggingFace 供其他人使用

## 设置

创建 Python 虚拟环境
```console
$ python3.11 -m venv venv
$ source venv/bin/activate
(venv) $ pip install -r requirements.txt
```

## 因果语言模型转换

本节描述将因果语言模型转换为 GGUF 并验证转换是否成功的步骤。

### 下载原始模型
首先，将原始模型克隆到某个本地目录：
```console
$ mkdir models && cd models
$ git clone https://huggingface.co/user/model_name
$ cd model_name
$ git lfs install
$ git lfs pull
```

### 设置 MODEL_PATH
下载模型的路径可以通过两种方式提供：

**选项 1：环境变量（推荐用于迭代开发）**
```console
export MODEL_PATH=~/work/ai/models/some_model
```

**选项 2：命令行参数（用于一次性任务）**
```console
make causal-convert-model MODEL_PATH=~/work/ai/models/some_model
```

当同时提供两者时，命令行参数优先于环境变量。

在模型的 transformer 实现尚未发布的情况下，可以设置环境变量 `UNRELEASED_MODEL_NAME`，这将显式加载 transformer 实现而不使用 AutoModelForCausalLM：
```
export UNRELEASED_MODEL_NAME=SomeNewModel
```

### 检查原始张量
```console
# 使用环境变量
(venv) $ make causal-inspect-original-model

# 或使用命令行参数
(venv) $ make causal-inspect-original-model MODEL_PATH=~/work/ai/models/some_model
```

### 运行原始模型
这主要是为了验证原始模型是否工作，以及与转换模型的输出进行比较。
```console
# 使用环境变量
(venv) $ make causal-run-original-model

# 或使用命令行参数
(venv) $ make causal-run-original-model MODEL_PATH=~/work/ai/models/some_model
```
此命令将把两个文件保存到 `data` 目录，一个是包含 logits 的二进制文件（将用于稍后与转换模型进行比较），另一个是允许手动视觉检查的文本文件。

### 模型转换
在对 [gguf-py](../../gguf-py) 进行更新以添加对新模型的支持后，可以使用以下命令将模型转换为 GGUF 格式：
```console
# 使用环境变量
(venv) $ make causal-convert-model

# 或使用命令行参数
(venv) $ make causal-convert-model MODEL_PATH=~/work/ai/models/some_model
```

### 检查转换后的模型
可以使用以下命令检查转换后的模型：
```console
(venv) $ make causal-inspect-converted-model
```

### 运行转换后的模型
```console
(venv) $ make causal-run-converted-model
```

### 模型 logits 验证
以下目标将运行原始模型和转换模型并比较 logits：
```console
(venv) $ make causal-verify-logits
```

### 模型量化
可以使用以下命令将因果模型量化为 GGUF 格式：
```console
(venv) $ make causal-quantize-Q8_0
Quantized model saved to: /path/to/quantized/model-Q8_0.gguf
在环境中导出量化模型路径到 QUANTIZED_MODEL 变量
```
这将显示量化模型的路径，然后可用于设置 `QUANTIZED_MODEL` 环境变量：
```console
export QUANTIZED_MODEL=/path/to/quantized/model-Q8_0.gguf
```
然后可以使用以下命令运行量化模型：
```console
(venv) $ make causal-run-quantized-model
```

### 量化 QAT（量化感知训练）模型
当量化到 `Q4_0` 时，token 嵌入权重的默认数据类型为 `Q6_K`。对于要上传到 ggml-org 的模型，建议为嵌入和输出张量使用 `Q8_0`。原因是虽然 `Q6_K` 较小，但需要更多计算来解包，这在输出生成期间可能会损害性能，因为必须解量化整个嵌入矩阵来计算词汇 logits。`Q8_0` 提供几乎完整的质量和更好的计算效率。
```console
(venv) $ make causal-quantize-qat-Q4_0
```


## 嵌入语言模型转换

### 下载原始模型
```console
$ mkdir models && cd models
$ git clone https://huggingface.co/user/model_name
$ cd model_name
$ git lfs install
$ git lfs pull
```

嵌入模型的路径可以通过两种方式提供：

**选项 1：环境变量（推荐用于迭代开发）**
```console
export EMBEDDING_MODEL_PATH=~/path/to/embedding_model
```

**选项 2：命令行参数（用于一次性任务）**
```console
make embedding-convert-model EMBEDDING_MODEL_PATH=~/path/to/embedding_model
```

当同时提供两者时，命令行参数优先于环境变量。

### 运行原始模型
这主要是为了验证原始模型是否工作，以及与转换模型的输出进行比较。
```console
# 使用环境变量
(venv) $ make embedding-run-original-model

# 或使用命令行参数
(venv) $ make embedding-run-original-model EMBEDDING_MODEL_PATH=~/path/to/embedding_model
```
此命令将把两个文件保存到 `data` 目录，一个是包含 logits 的二进制文件（将用于稍后与转换模型进行比较），另一个是允许手动视觉检查的文本文件。

#### 使用带有编号层的 SentenceTransformer
对于具有编号 SentenceTransformer 层（01_Pooling、02_Dense、03_Dense、04_Normalize）的模型，这些层将在运行转换后的模型时自动应用，但目前有一个单独的目标来运行原始版本：

```console
# 使用 SentenceTransformer 运行原始模型（应用所有编号层）
(venv) $ make embedding-run-original-model-st
```

这将使用 SentenceTransformer 库加载和运行模型，该库自动按正确顺序应用所有编号层。当与应包含这些额外转换层而不仅仅是基本模型输出的模型进行比较时，这特别有用。

可以为转换后的模型指定归一化类型，但这并不是严格必需的，因为验证使用余弦相似度，输出向量的大小不影响这一点。但是，可以将归一化类型指定为目标的参数，这对于手动检查可能很有用：
```console
(venv) $ make embedding-verify-logits-st EMBD_NORMALIZE=1
```
原始模型将根据 modules.json 配置文件中指定的归一化层应用归一化。

### 模型转换
在对 [gguf-py](../../gguf-py) 进行更新以添加对新模型的支持后，可以使用以下命令将模型转换为 GGUF 格式：
```console
(venv) $ make embedding-convert-model
```

### 运行转换后的模型
```console
(venv) $ make embedding-run-converted-model
```

### 模型 logits 验证
以下目标将运行原始模型和转换模型（这是在前面的步骤中手动完成的）并比较 logits：
```console
(venv) $ make embedding-verify-logits
```

对于带有 SentenceTransformer 层的模型，使用 `-st` 验证目标：
```console
(venv) $ make embedding-verify-logits-st
```
此便捷目标自动运行带有 SentenceTransformer 的原始模型和启用池化的转换模型，然后比较结果。

### llama-server 验证
要验证转换后的模型是否与 llama-server 一起工作，可以使用以下命令：
```console
(venv) $ make embedding-start-embedding-server
```
然后打开另一个终端并设置 `EMBEDDINGS_MODEL_PATH` 环境变量，因为这将不会被新终端继承：
```console
(venv) $ make embedding-curl-embedding-endpoint
```
这将调用 `embedding` 端点，输出将被传递到与目标 `embedding-verify-logits` 使用的相同的验证脚本。

因果模型也可用于生成嵌入，可以使用以下命令进行验证：
```console
(venv) $ make causal-start-embedding-server
```
然后打开另一个终端并设置 `MODEL_PATH` 环境变量，因为这将不会被新终端继承：
```console
(venv) $ make casual-curl-embedding-endpoint
```

### 模型量化
可以使用以下命令将嵌入模型量化为 GGUF 格式：
```console
(venv) $ make embedding-quantize-Q8_0
Quantized model saved to: /path/to/quantized/model-Q8_0.gguf
在环境中导出量化模型路径到 QUANTIZED_EMBEDDING_MODEL 变量
```
这将显示量化模型的路径，然后可用于设置 `QUANTIZED_EMBEDDING_MODEL` 环境变量：
```console
export QUANTIZED_EMBEDDING_MODEL=/path/to/quantized/model-Q8_0.gguf
```
然后可以使用以下命令运行量化模型：
```console
(venv) $ make embedding-run-quantized-model
```

### 量化 QAT（量化感知训练）模型
当量化到 `Q4_0` 时，token 嵌入权重的默认数据类型为 `Q6_K`。对于要上传到 ggml-org 的模型，建议为嵌入和输出张量使用 `Q8_0`。原因是虽然 `Q6_K` 较小，但需要更多计算来解包，这在输出生成期间可能会损害性能，因为必须解量化整个嵌入矩阵来计算词汇 logits。`Q8_0` 提供几乎完整的质量和更好的计算效率。
```console
(venv) $ make embedding-quantize-qat-Q4_0
```

## 困惑度评估

### 简单困惑度评估
这允许在不生成 token/logits 文件的情况下运行困惑度评估：
```console
(venv) $ make perplexity-run QUANTIZED_MODEL=~/path/to/quantized/model.gguf
```
这将使用 wikitext 数据集运行困惑度评估，并将困惑度分数输出到终端。然后可以将此值与未量化模型的困惑度分数进行比较。

### 完整困惑度评估
首先使用转换后的、未量化的模型使用以下命令生成困惑度评估数据集：
```console
$ make perplexity-data-gen CONVERTED_MODEL=~/path/to/converted/model.gguf
```
这将在 `data` 目录中生成一个以模型命名并带有 `.kld` 后缀的文件，其中包含 wikitext 数据集的 token 和 logits。

生成数据集后，可以使用量化模型运行困惑度评估：
```console
$ make perplexity-run-full QUANTIZED_MODEL=~/path/to/quantized/model-Qxx.gguf LOGITS_FILE=data/model.gguf.ppl
```

> 📝 **注意：** `LOGITS_FILE` 是上一个命令生成的文件，可能非常大，因此请确保有足够的磁盘空间可用。

## HuggingFace 工具
以下目标有助于在 HuggingFace 上的 ggml-org 中创建集合和模型仓库。这些可用于准备发布时为新模型发布编写脚本。

对于以下目标，需要 `HF_TOKEN` 环境变量。

> 📝 **注意：** 运行这些命令后不要忘记从 Hugging Face 注销，否则在拉取/克隆仓库时可能会遇到问题，因为令牌仍将处于使用状态：
> $ huggingface-cli logout
> $ unset HF_TOKEN

### 创建新的 HuggingFace 模型（模型仓库）
这将在 HuggingFace 上使用指定的模型名称创建一个新的模型仓库。
```console
(venv) $ make hf-create-model MODEL_NAME='TestModel' NAMESPACE="danbev" ORIGINAL_BASE_MODEL="some-base-model"
Repository ID:  danbev/TestModel-GGUF
Repository created: https://huggingface.co/danbev/TestModel-GGUF
```
注意，我们在模型名称后面附加了 `-GGUF` 后缀，以确保 GGUF 模型的一致命名约定。

可以使用以下命令创建嵌入模型：
```console
(venv) $ make hf-create-model-embedding MODEL_NAME='TestEmbeddingModel' NAMESPACE="danbev" ORIGINAL_BASE_MODEL="some-base-model"
```
唯一的区别是，嵌入模型的模型卡在 llama-server 命令方面会有所不同，在访问/调用嵌入端点方面也是如此。

### 将 GGUF 模型上传到模型仓库
以下目标将模型上传到现有的 HuggingFace 模型仓库。
```console
(venv) $ make hf-upload-gguf-to-model MODEL_PATH=dummy-model1.gguf REPO_ID=danbev/TestModel-GGUF
📤 Uploading dummy-model1.gguf to danbev/TestModel-GGUF/dummy-model1.gguf
✅ Upload successful!
🔗 File available at: https://huggingface.co/danbev/TestModel-GGUF/blob/main/dummy-model1.gguf
```
此命令也可用于更新仓库中的现有模型文件。

### 创建新集合
```console
(venv) $ make hf-new-collection NAME=TestCollection DESCRIPTION="Collection for testing scripts" NAMESPACE=danbev
🚀 Creating Hugging Face Collection
Title: TestCollection
Description: Collection for testing scripts
Namespace: danbev
Private: False
✅ Authenticated as: danbev
📚 Creating collection: 'TestCollection'...
✅ Collection created successfully!
📋 Collection slug: danbev/testcollection-68930fcf73eb3fc200b9956d
🔗 Collection URL: https://huggingface.co/collections/danbev/testcollection-68930fcf73eb3fc200b9956d

🎉 Collection created successfully!
Use this slug to add models: danbev/testcollection-68930fcf73eb3fc200b9956d
```

### 将模型添加到集合
```console
(venv) $ make hf-add-model-to-collection COLLECTION=danbev/testcollection-68930fcf73eb3fc200b9956d MODEL=danbev/TestModel-GGUF
✅ Authenticated as: danbev
🔍 Checking if model exists: danbev/TestModel-GGUF
✅ Model found: danbev/TestModel-GGUF
📚 Adding model to collection...
✅ Model added to collection successfully!
🔗 Collection URL: https://huggingface.co/collections/danbev/testcollection-68930fcf73eb3fc200b9956d

🎉 Model added successfully!

```
