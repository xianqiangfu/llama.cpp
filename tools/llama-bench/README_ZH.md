# llama.cpp/tools/llama-bench

llama.cpp 性能测试工具。

## 目录

1. [语法](#语法)
2. [示例](#示例)
    1. [使用不同模型进行文本生成](#使用不同模型进行文本生成)
    2. [使用不同批处理大小进行提示处理](#使用不同批处理大小进行提示处理)
    3. [使用不同数量的线程](#使用不同数量的线程)
    4. [不同层数卸载到 GPU](#不同层数卸载到-gpu)
3. [输出格式](#输出格式)
    1. [Markdown](#markdown)
    2. [CSV](#csv)
    3. [JSON](#json)
    4. [JSONL](#jsonl)
    5. [SQL](#sql)

## 语法

```
usage: llama-bench [options]

options:
  -h, --help
  --numa <distribute|isolate|numactl>       numa 模式（默认：禁用）
  -r, --repetitions <n>                     每个测试重复次数（默认：5）
  --prio <0|1|2|3>                          进程/线程优先级（默认：0）
  --delay <0...N> (seconds)                 每个测试之间的延迟（默认：0）
  -o, --output <csv|json|jsonl|md|sql>      输出到 stdout 的格式（默认：md）
  -oe, --output-err <csv|json|jsonl|md|sql> 输出到 stderr 的格式（默认：无）
  --list-devices                            列出可用设备并退出
  -v, --verbose                             详细输出
  --progress                                打印测试进度指示器
  -rpc, --rpc <rpc_servers>                 注册 RPC 设备（逗号分隔）

test parameters:
  -m, --model <filename>                    (默认：models/7B/ggml-model-q4_0.gguf)
  -p, --n-prompt <n>                        (默认：512)
  -n, --n-gen <n>                           (默认：128)
  -pg <pp,tg>                               (默认：）
  -d, --n-depth <n>                         (默认：0)
  -b, --batch-size <n>                      (默认：2048)
  -ub, --ubatch-size <n>                    (默认：512)
  -ctk, --cache-type-k <t>                  (默认：f16)
  -ctv, --cache-type-v <t>                  (默认：f16)
  -t, --threads <n>                         (默认：系统依赖)
  -C, --cpu-mask <hex,hex>                  (默认：0x0)
  --cpu-strict <0|1>                        (默认：0)
  --poll <0...100>                          (默认：50)
  -ngl, --n-gpu-layers <n>                  (默认：99)
  -ncmoe, --n-cpu-moe <n>                   (默认：0)
  -sm, --split-mode <none|layer|row>        (默认：layer)
  -mg, --main-gpu <i>                       (默认：0)
  -nkvo, --no-kv-offload <0|1>              (默认：0)
  -fa, --flash-attn <0|1>                   (默认：0)
  -dev, --device <dev0/dev1/...>            (默认：auto)
  -mmp, --mmap <0|1>                        (默认：1)
  -embd, --embeddings <0|1>                 (默认：0)
  -ts, --tensor-split <ts0/ts1/..>          (默认：0)
  -ot --override-tensors <tensor name pattern>=<buffer type>;...
                                            (默认：禁用)
  -nopo, --no-op-offload <0|1>              (默认：0)
  -fitt, --fit-target <MiB>                 将模型拟合到设备内存，每个设备有此边距（以 MiB 为单位）（默认：关闭）
  -fitc, --fit-ctx <n>                      --fit 可以设置的最小 ctx 大小（默认：4096）

Multiple values can be given for each parameter by separating them with ','
or by specifying parameter multiple times. Ranges can be given as
'first-last' or 'first-last+step' or 'first-last*mult'.
```

llama-bench 可以执行三种类型的测试：

- 提示处理 (pp)：在批处理中处理提示（`-p`）
- 文本生成 (tg)：生成 token 序列（`-n`）
- 提示处理 + 文本生成 (pg)：处理提示后跟 token 序列生成（`-pg`）

除了 `-r`、`-o` 和 `-v` 之外，所有选项可以指定多次以运行多个测试。每个 pp 和 tg 测试使用所有指定选项的组合运行。要为选项指定多个值，值可以用逗号分隔（例如 `-n 16,32`），或者可以多次指定选项（例如 `-n 16 -n 32`）。每个测试按 `-r` 给定的次数重复，结果取平均值。结果以平均每秒 token 数（t/s）和标准差形式给出。某些输出格式（例如 json）还包括每次重复的各个结果。

使用 `-d <n>` 选项，每个测试可以在指定上下文深度处运行，预先填充 `<n>` 个 token 的 KV 缓存。

有关其他选项的描述，请参阅[完成示例](../completion/README.md)。

> [!NOTE]
> `llama-bench` 的测量**不**包括分词和采样的时间。

## 示例

### 使用不同模型进行文本生成

```sh
$ ./llama-bench -m models/7B/ggml-model-q4_0.gguf -m models/13B/ggml-model-q4_0.gguf -p 0 -n 128,256,512
```

| 模型                          |       大小 |     参数 | 后端    | ngl | 测试       |              t/s |
| ------------------------------ | ---------: | ---------: | ---------- | --: | ---------- | ---------------: | -------------------: |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 | tg 128     |    132.19 ± 0.55 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 | tg 256     |    129.37 ± 0.54 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 | tg 512     |    123.83 ± 0.25 |
| llama 13B mostly Q4_0          |   6.86 GiB |    13.02 B | CUDA       |  99 | tg 128     |     82.17 ± 0.31 |
| llama 13B mostly Q4_0          |   6.86 GiB |    13.02 B | CUDA       |  99 | tg 256     |     80.74 ± 0.23 |
| llama 13B mostly Q4_0          |   6.86 GiB |    13.02 B | CUDA       |  99 | tg 512     |     78.08 ± 0.07 |

### 使用不同批处理大小进行提示处理

```sh
$ ./llama-bench -n 0 -p 1024 -b 128,256,512,1024
```

| 模型                          |       大小 |     参数 | 后端    |    n_batch | 测试       |              t/s |
| ------------------------------ | ---------: | ---------: | ---------- | ---------: | ---------- | ---------------: | -------------------: |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 |        128 | pp 1024    |   1436.51 ± 3.66 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 |        256 | pp 1024    |   1932.43 ± 23.48 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 |        512 | pp 1024    |   2254.45 ± 15.59 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 |       1024 | pp 1024    |   2498.61 ± 13.58 |

### 使用不同数量的线程

```sh
$ ./llama-bench -n 0 -n 16 -p 64 -t 1,2,4,8,16,32
```

| 模型                          |       大小 |     参数 |    线程 | 测试       |              t/s |
| ------------------------------ | ---------: | ---------: | ---------- | ---------: | ---------- | ---------------: | -------------------: |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          1 | pp 64      |       6.17 ± 0.07 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          1 | tg 16      |       4.05 ± 0.02 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          2 | pp 64      |      12.31 ± 0.13 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          2 | tg 16      |       7.80 ± 0.07 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          4 | pp 64      |      23.18 ± 0.06 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          4 | tg 16      |      12.22 ± 0.07 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          8 | pp 64      |      32.29 ± 1.21 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |          8 | tg 16      |       16.71 ± 0.66 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |         16 | pp 64      |      33.52 ± 0.03 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |         16 | pp 64      |      33.52 ± 0.03 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CPU        |         32 | pp 64      |      59.00 ± 1.11 |

### 不同层数卸载到 GPU

```sh
$ ./llama-bench -ngl 10,20,30,31,32,33,34,35
```

| 模型                          |       大小 |     参数 | 后端    | ngl | 测试       |              t/s |
| ------------------------------ | ---------: | ---------: | ---------- | --: | ---------- | ---------------: | -------------------: |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  10 | pp 512     |    373.36 ± 2.25 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  10 | tg 128     |     13.45 ± 0.93 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  20 | pp 512     |    472.65 ± 1.25 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  20 | tg 128     |     21.36 ± 1.94 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  30 | pp 512     |    631.87 ± 11.25 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  30 | tg 128     |     40.04 ± 1.82 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  31 | pp 512     |    657.89 ± 5.08 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  31 | tg 128     |     48.19 ± 0.81 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  32 | pp 512     |    688.26 ± 3.29 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  33 | pp 512     |    704.27 ± 2.24 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  33 | tg 128     |     60.62 ± 1.76 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  34 | pp 512     |    881.34 ± 5.40 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  34 | tg 128     |    131.66 ± 0.49 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  35 | pp 512     |   2400.01 ± 7.72 |

### 不同预填充上下文

```sh
$ ./llama-bench -d 0,512
```

| 模型                          |       大小 |     参数 | 后端    | ngl |            测试 |                  t/s |
| ------------------------------ | ---------: | ---------: | ---------- | --: | --------------: | -------------------: |
| qwen2 7B Q4_K - Medium         |   4.36 GiB |     7.62 B | CUDA       |  99 |           pp512 |      7340.20 ± 23.45 |
| qwen2 7B Q4_K - Medium         |   4.36 GiB |     7.62 B | CUDA       |  99 |           tg128 |        120.60 ± 0.59 |
| qwen2 7B Q4_K - Medium         |   4.36 GiB |     7.62 B | CUDA       |  99 |     pp512 @ d512 |      6425.91 ± 18.88 |
| qwen2 7B Q4_K - Medium         |   4.36 GiB |     7.62 B | CUDA       |  99 |     tg128 @ d512 |        116.71 ± 0.60 |

## 输出格式

默认情况下，llama-bench 以 markdown 格式输出结果。结果可以通过 `-o` 选项以其他格式输出。

### Markdown

```sh
$ ./llama-bench -o md
```

| 模型                          |       大小 |     参数 | 后端    | ngl | 测试       |              t/s |
| ------------------------------ | ---------: | ---------: | ---------- | --: | ---------- | ---------------: | -------------------: |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 | pp 512     | 2368.80 ± 93.24 |
| llama 7B mostly Q4_0           |   3.56 GiB |     6.74 B | CUDA       |  99 | tg 128     |    131.42 ± 0.59 |

### CSV

```sh
$ ./llama-bench -o csv
```

```csv
build_commit,build_number,cpu_info,gpu_info,backends,model_filename,model_type,model_size,model_n_params,n_batch,n_ubatch,n_threads,cpu_mask,cpu_strict,poll,type_k,type_v,n_gpu_layers,split_mode,main_gpu,no_kv_offload,flash_attn,tensor_split,use_mmap,embeddings,n_prompt,n_gen,n_depth,test_time,avg_ns,stddev_ns,avg_ts,stddev_ts
"8cf427ff","5163","AMD Ryzen 7 7800X3D 8-Core Processor","NVIDIA GeForce RTX 4080","CUDA","models/Qwen2.5-7B-Instruct-Q4_K_M.gguf","qwen2 7B Q4_K - Medium","4677120000","7615616512","2048","512","8","0x0","0","50","f16","f16","99","layer","0","0","0","0","0.00","1","0","0","512","0","2025-04-24T11:57:09Z","70285660","982040","7285.676949","100.064434"
"8cf427ff","5163","AMD Ryzen 7 7800X3D 8-Core Processor","NVIDIA GeForce RTX 4080","CUDA","models/Qwen2.5-7B-Instruct-Q4_K_M.gguf","qwen2 7B Q4_K - Medium","4677120000","7615616512","2048","512","8","0x0","0","50","f16","f16","99","layer","0","0","0","0","0.00","1","0","0","128","0","2025-04-24T11:58:50Z","1067431600","3834831","119.915244","0.430617"
```

### JSON

```sh
$ ./llama-bench -o json
```

```json
[
  {
    "build_commit": "8cf427ff",
    "build_number": 5163,
    "cpu_info": "AMD Ryzen 7 7800X3D 8-Core Processor",
    "gpu_info": "NVIDIA GeForce RTX 4080",
    "backends": "CUDA",
    "model_filename": "models/Qwen2.5-7B-Instruct-Q4_K_M.gguf",
    "model_type": "qwen2 7B Q4_K - Medium",
    "model_size": 4677120000,
    "model_n_params": 7615616512,
    "n_batch": 2048,
    "n_ubatch": 512,
    "n_threads": 8,
    "cpu_mask": "0x0",
    "cpu_strict": false,
    "poll": 50,
    "type_k": "f16",
    "type_v": "f16",
    "n_gpu_layers": 99,
    "split_mode": "layer",
    "main_gpu": 0,
    "no_kv_offload": false,
    "flash_attn": false,
    "tensor_split": "0.00",
    "use_mmap": true,
    "embeddings": false,
    "n_prompt": 512,
    "n_gen": 0,
    "n_depth": 0,
    "test_time": "2025-04-24T11:58:50Z",
    "avg_ns": 72135640,
    "stddev_ns": 1453752,
    "avg_ts": 7100.002165,
    "stddev_ts": 140.341520,
    "samples_ns": [ 74601900, 71632900, 71745200, 71952700, 70745500 ],
    "samples_ts": [ 6863.1, 7147.55, 7136.37, 7115.79 ]
  },
  {
    "build_commit": "8cf427ff",
    "build_number": 5163,
    "cpu_info": "AMD Ryzen 7 7800X3D 8-Core Processor",
    "gpu_info": "NVIDIA GeForce RTX 4080",
    "backends": "CUDA",
    "model_filename": "models/Qwen2.5-7B-Instruct-Q4_K_M.gguf",
    "model_type": "qwen2 7B Q4_K - Medium",
    "model_size": 4677120000,
    "model_n_params": 7615616512,
    "n_batch": 2048,
    "n_ubatch": 512,
    "n_threads": 8,
    "cpu_mask": "0x0",
    "cpu_strict": false,
    "poll": 50,
    "type_k": "f16",
    "type_v": "f16",
    "n_gpu_layers": 99,
    "split_mode": "layer",
    "main_gpu": 0,
    "no_kv_offload": false,
    "flash_attn": false,
    "tensor_split": "0.00",
    "use_mmap": true,
    "embeddings": false,
    "n_prompt": 0,
    "n_gen": 128,
    "n_depth": 0,
    "test_time": "2025-04-24T11:58:51Z",
    "avg_ns": 1076767880,
    "stddev_ns": 9449585,
    "avg_ts": 118.881588,
    "stddev_ts": 1.041811,
    "samples_ns": [ 1075361300, 1065089400, 1071761200, 1089692600 ],
    "samples_ts": [ 119.03, 120.178, 119.43, 118.307 ]
  }
]
```

### JSONL

```sh
$ ./llama-bench -o jsonl
```

```json lines
{"build_commit": "8cf427ff", "build_number": 5163, "cpu_info": "AMD Ryzen 7 7800X3D 8-Core Processor", "gpu_info": "NVIDIA GeForce RTX 4080", "backends": "CUDA", "model_filename": "models/Qwen2.5-7B-Instruct-Q4_K_M.gguf", "model_type": "qwen2 7B Q4_K - Medium", "model_size": 4677120000, "model_n_params": 7615616512, "n_batch": 2048, "n_ubatch": 512, "n_threads": 8, "cpu_mask": "0x0", "cpu_strict": false, "poll": 50, "type_k": "f16", "type_v": "f16", "n_gpu_layers": 99, "split_mode": "layer", "main_gpu": 0, "no_kv_offload": false, "flash_attn": false, "tensor_split": "0.00", "use_mmap": true, "embeddings": false, "n_prompt": 512, "n_gen": 0, "n_depth": 0, "test_time": "2025-04-24T11:59:33Z", "avg_ns": 70497220, "stddev_ns": 883196, "avg_ts": 7263.609157, "stddev_ts": 90.940578, "samples_ns": [ 71551000, 71222800, 70364100, 69909100 ],"samples_ts": [ 7155.74, 7188.71, 7276.44, 7323.8 ]}{"build_commit": "8cf427ff", "build_number": 5163, "cpu_info": "AMD Ryzen 7 7800X3D 8-Core Processor", "gpu_info": "NVIDIA GeForce RTX 4080", "backends": "CUDA", "model_filename": "models/Qwen2.5-7B-Instruct-Q4_K_M.gguf", "model_type": "qwen2 7B Q4_K - Medium", "model_size": 4677120000, "model_n_params": 7615616512, "n_batch": 2048, "n_ubatch": 512, "n_threads": 8, "cpu_mask": "0x0", "cpu_strict": false, "poll": 50, "type_k": "f16", "type_v": "f16", "n_gpu_layers": 99, "split_mode": "layer", "main_gpu": 0, "no_kv_offload": false, "flash_attn": false, "tensor_split": "0.00", "use_mmap": true, "embeddings": false, "n_prompt": 0, "n_gen": 128, "n_depth": 0, "test_time": "2025-04-24T11:59:33Z", "avg_ns": 1068078400, "stddev_ns": 6279455, "avg_ts": 119.844681, "stddev_ts": 0.699739, "samples_ns": [ 1066331700, 1065089400, 1071761200, 1089692600 ],"samples_ts": [ 120.038, 120.203, 118.624, 120.377 ]}
```

### SQL

SQL 输出适合导入 SQLite 数据库。输出可以通过管道传送到 `sqlite3` 命令行工具以将结果添加到数据库中。

```sh
$ ./llama-bench -o sql
```

```sql
CREATE TABLE IF NOT EXISTS test (
  build_commit TEXT,
  build_number INTEGER,
  cpu_info TEXT,
  gpu_info TEXT,
  backends TEXT,
  model_filename TEXT,
  model_type TEXT,
  model_size INTEGER,
  model_n_params INTEGER,
  n_batch INTEGER,
  n_ubatch INTEGER,
  n_threads INTEGER,
  cpu_mask TEXT,
  cpu_strict INTEGER,
  poll INTEGER,
  type_k TEXT,
  type_v TEXT,
  n_gpu_layers INTEGER,
  split_mode TEXT,
  main_gpu INTEGER,
  no_kv_offload INTEGER,
  flash_attn INTEGER,
  tensor_split TEXT,
  use_mmap INTEGER,
  embeddings INTEGER,
  n_prompt INTEGER,
  n_gen INTEGER,
  n_depth INTEGER,
  test_time TEXT,
  avg_ns INTEGER,
  stddev_ns INTEGER,
  avg_ts REAL,
  stddev_ts REAL
);

INSERT INTO test (build_commit, build_number, cpu_info, gpu_info, backends, model_filename, model_type, model_size, model_n_params, n_batch, n_ubatch, n_threads, cpu_mask, cpu_strict, poll, type_k, type_v, n_gpu_layers, split_mode, main_gpu, no_kv_offload, flash_attn, tensor_split, use_mmap, embeddings, n_prompt, n_gen, n_depth, test_time, avg_ns, stddev_ns, avg_ts, stddev_ts) VALUES ('8cf427ff', '5163', 'AMD Ryzen 7 7800X3D 8-Core Processor', 'NVIDIA GeForce RTX 4080', 'CUDA', 'models/Qwen2.5-7B-Instruct-Q4_K_M.gguf', 'qwen2 7B Q4_K - Medium', '4677120000', '7615616512', '2048', '512', '8', '0x0', '0', '50', 'f16', 'f16', '99', 'layer', '0', '0', '0', '0.00', '1', '0', '512', '0', '2025-04-24T12:00:08Z', '69905000', '519516', '7324.546977');
INSERT INTO test (build_commit, build_number, cpu_info, gpu_info, backends, model_filename, model_type, model_size, model_n_params, n_batch, n_ubatch, n_threads, cpu_mask, cpu_strict, poll, type_k, type_v, n_gpu_layers, split_mode, main_gpu, no_kv_offload, flash_attn, tensor_split, use_mmap, embeddings, n_prompt, n_gen, n_depth, test_time, avg_ns, stddev_ns, avg_ts, stddev_ts) VALUES ('8cf427ff', '5163', 'AMD Ryzen 7 7800X3D 8-Core Processor', 'NVIDIA GeForce RTX 4080', 'CUDA', 'models/Qwen2.5-7B-Instruct-Q4_K_M.gguf', 'qwen2 7B Q4_K - Medium', '4677120000', '7615616512', '2048', '512', '8', '0x0', '0', '50', 'f16', 'f16', '99', 'layer', '0', '0', '0', '0.00', '1', '0', '0', '128', '0', '2025-04-24T12:00:09Z', '1063608780', '4464130', '120.346696');
```
