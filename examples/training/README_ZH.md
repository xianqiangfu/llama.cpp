# llama.cpp/examples/training

此目录包含与使用 llama.cpp/GGML 进行语言模型训练相关的示例。
到目前为止，微调在技术上是可用的（对于 FP32 模型和有限的硬件设置），但代码仍处于 WIP（进行中工作）状态。
Stories 260K 和 LLaMA 3.2 1b 的微调似乎需要 24 GB 的内存才能工作。
**对于 CPU 训练，请在不添加任何额外后端（如 CUDA）的情况下编译 llama.cpp。**
**对于 CUDA 训练，请使用最大数量的 GPU 层。**

概念验证：

``` sh
export model_name=llama_3.2-1b && export quantization=f32
./build/bin/llama-finetune --file wikitext-2-raw/wiki.test.raw -ngl 999 --model models/${model_name}-${quantization}.gguf -c 512 -b 512 -ub 512
./build/bin/llama-perplexity --file wikitext-2-raw/wiki.test.raw -ngl 999 --model finetuned-model.gguf
```

在测试集上训练 2 个 epoch 后，微调模型的困惑度值应该更低。
