# llama-gguf-hash

用于对 GGUF 文件进行哈希计算的命令行工具，用于检测模型级别和张量级别的差异。

**命令行选项：**

- `--help`: 显示帮助消息
- `--xxh64`: 使用 xxhash 64位哈希模式（默认）
- `--sha1`: 使用 sha1
- `--uuid`: 使用 uuid
- `--sha256`: 使用 sha256
- `--all`: 使用所有哈希
- `--no-layer`: 排除每层哈希
- `--uuid`: 生成 UUIDv5 ID
- `-c`, `--check <manifest>`: 根据清单文件验证

## 关于

虽然大多数 POSIX 系统已经有 sha256sum 这样的哈希检查程序，但它们的设计是检查整个文件。如果我们要检查张量数据的一致性，即使 gguf KV 存储的元数据内容已更新，这并不理想。

此程序设计为在"每个张量层"哈希 gguf 张量负载，以及"整个张量模型"哈希。其意图是首先检查整个张量层，但如果检测到任何不一致，则可以使用每张量哈希来缩小具有不一致性的特定张量层。

对于维护者：
- 在开发和自动化测试期间检测张量不一致性
    - 这由 xxh64 提供支持，它很快
    - 这也由具有每张量层支持来提供，有助于缩小有问题的张量层的位置
    - 这也由 sha1 提供支持，尽管它慢得多但得到更广泛的支持

对于模型创建者：
- 基于模型张量内容的可选一致性 UUID 生成
    - 这由 UUIDv5 提供支持，对于数据库键很有用
        - llama.cpp UUIDv5 命名空间：`ef001206-dadc-5f6d-a15f-3359e577d4e5`
            - 通过 UUIDv5 URL 命名空间的 `en.wikipedia.org/wiki/Llama.cpp` 创建

对于模型用户：
- 即使元数据已更新，也能保证张量层的完整性
    - 这由 sha256 提供支持，截至 2024 年仍被认为非常安全

### 设计说明

- 如果没有提供参数，此程序的默认行为是使用 xxhash 的 xxh32 模式进行哈希，因为它非常快，并且主要针对可能希望在自动化测试中使用此程序的维护者。
- xxhash 支持 xxh32 和 xxh128，分别用于 32 位哈希和 128 位哈希，然而我们选择了 64 位 xxhash，因为截至 2024 年大多数计算机都是 64 位的，因此对计算 64 位大小的哈希具有更好的亲和性。

## 编译示例

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DLLAMA_FATAL_WARNINGS=ON
make -C build clean
make -C build llama-gguf-hash VERBOSE=1
./build/bin/llama-gguf-hash test.gguf
./build/bin/llama-gguf-hash --xxh64 test.gguf
./build/bin/llama-gguf-hash --sha1 test.gguf
./build/bin/llama-gguf-hash --uuid test.gguf
./build/bin/llama-gguf-hash --sha256 test.gguf
```

## 生成和验证示例

要生成，我们可以使用此命令

```bash
./llama-gguf-hash --all test.gguf > test.gguf.manifest
```

这将生成如下所示的清单，其中包含多种哈希类型和每张量层哈希（不包括 UUID，因为它是 ID 而不是哈希）

```bash
xxh64     f66e9cd66a4396a0  test.gguf:tensor_0
sha1      59f79ecefd8125a996fdf419239051a7e99e5f20  test.gguf:tensor_0
sha256    c0510d38fa060c46265e0160a85c7243096b01dd31c2f355bdbb5516b20de1bd  test.gguf:tensor_0
xxh64     7d3a1f9ac04d0537  test.gguf:tensor_1
sha1      4765f592eacf096df4628ba59476af94d767080a  test.gguf:tensor_1
sha256    8514cbcc73692a2c56bd7a33a022edd5ff819614bd23b19915d7224387f397a7  test.gguf:tensor_1
xxh64     a0af5d700049693b  test.gguf:tensor_2
sha1      25cbfbad4513cc348e2c95ebdee69d6ff2fd8753  test.gguf:tensor_2
sha256    947e6b36e20f2cc95e1d2ce1c1669d813d574657ac6b5ac5196158d454d35180  test.gguf:tensor_2
xxh64     e83fddf559d7b6a6  test.gguf:tensor_3
sha1      a9cba73e2d90f2ee3dae2548caa42bef3fe6a96c  test.gguf:tensor_3
sha256    423b044e016d8ac73c39f23f60bf01bedef5ecb03c0230accd824c91fe86f1a1  test.gguf:tensor_3
xxh64     1257733306b7992d  test.gguf:tensor_4
sha1      d7bc61db93bb685ce9d598da97117c66729b7543  test.gguf:tensor_4
sha256    79737cb3912d4201384cf7f16a1a37ff7823f23ea796cb205b6ca361ab9e3ebf  test.gguf:tensor_4
xxh64     d238d16ba4711e58  test.gguf:tensor_5
sha1      0706566c198fe1072f37e0a5135b4b5f23654c52  test.gguf:tensor_5
sha256    60949be8298eced0ecdde64487643d018407bd261691e061d9e9c3dbc9fd358b  test.gguf:tensor_5
xxh64     3fbc3b65ab8c7f39  test.gguf:tensor_6
sha1      73922a0727226a409049f6fc3172a52219ca6f00  test.gguf:tensor_6
sha256    574f4c46ff384a3b9a225eb955d2a871847a2e8b3fa59387a8252832e92ef7b0  test.gguf:tensor_6
xxh64     c22021c29854f093  test.gguf:tensor_7
sha1      efc39cece6a951188fc41e354c73bbfe6813d447  test.gguf:tensor_7
sha256    4c0410cd3c500f078ae5b21e8dc9eb79e29112713b2ab58a882f82a3868d4d75  test.gguf:tensor_7
xxh64     936df61f5d64261f  test.gguf:tensor_8
sha1      c2490296d789a4f34398a337fed8377d943d9f06  test.gguf:tensor_8
sha256    c4401313feeba0261275c3b25bd2d8fe40ce04e0f440c2980ed0e9674c30ff01  test.gguf:tensor_8
xxh64     93fd20c64421c081  test.gguf:tensor_9
sha1      7047ce1e78437a6884337a3751c7ee0421918a65  test.gguf:tensor_9
sha256    23d57cf0d7a6e90b0b3616b41300e0cd354781e812add854a5f95aa55f2bc514  test.gguf:tensor_9
xxh64     5a54d3aad816f302  test.gguf
sha1      d15be52c4ff213e823cb6dd13af7ee2f978e7042  test.gguf
sha256    7dd641b32f59b60dbd4b5420c4b0f6321ccf48f58f6ae201a3dbc4a58a27c6e4  test.gguf
```

然后我们可以使用正常的检查命令，它默认将检查最高安全强度的哈希并对其进行验证：

```bash
$ ./llama-gguf-hash --check test.gguf.manifest test.gguf
manifest  test.gguf.manifest  sha256  sha1  xxh64
sha256    c0510d38fa060c46265e0160a85c7243096b01dd31c2f355bdbb5516b20de1bd  test.gguf:tensor_0  -  Ok
sha256    8514cbcc73692a2c56bd7a33a022edd5ff819614bd23b19915d7224387f397a7  test.gguf:tensor_1  -  Ok
sha256    947e6b36e20f2cc95e1d2ce1c1669d813d574657ac6b5ac5196158d454d35180  test.gguf:tensor_2  -  Ok
sha256    423b044e016d8ac73c39f23f60bf01bedef5ecb03c0230accd824c91fe86f1a1  test.gguf:tensor_3  -  Ok
sha256    79737cb3912d4201384cf7f16a1a37ff7823f23ea796cb205b6ca361ab9e3ebf  test.gguf:tensor_4  -  Ok
sha256    60949be8298eced0ecdde64487643d018407bd261691e061d9e9c3dbc9fd358b  test.gguf:tensor_5  -  Ok
sha256    574f4c46ff384a3b9a225eb955d2a871847a2e8b3fa59387a8252832e92ef7b0  test.gguf:tensor_6  -  Ok
sha256    4c0410cd3c500f078ae5b21e8dc9eb79e29112713b2ab58a882f82a3868d4d75  test.gguf:tensor_7  -  Ok
sha256    c4401313feeba0261275c3b25bd2d8fe40ce04e0f440c2980ed0e9674c30ff01  test.gguf:tensor_8  -  Ok
sha256    23d57cf0d7a6e90b0b3616b41300e0cd354781e812add854a5f95aa55f2bc514  test.gguf:tensor_9  -  Ok
sha256    7dd641b32f59b60dbd4b5420c4b0f6321ccf48f58f6ae201a3dbc4a58a27c6e4  test.gguf  -  Ok

Verification results for test.gguf.manifest - Success
```

或者我们可以明确要求更快的哈希，如：

```bash
$ ./llama-gguf-hash --check test.gguf.manifest --xxh64 test.gguf
manifest  test.gguf.manifest  sha256  sha1  xxh64
xxh64     f66e9cd66a4396a0  test.gguf:tensor_0  -  Ok
xxh64     7d3a1f9ac04d0537  test.gguf:tensor_1  -  Ok
xxh64     a0af5d700049693b  test.gguf:tensor_2  -  Ok
xxh64     e83fddf559d7b6a6  test.gguf:tensor_3  -  Ok
xxh64     1257733306b7992d  test.gguf:tensor_4  -  Ok
xxh64     d238d16ba4711e58  test.gguf:tensor_5  -  Ok
xxh64     3fbc3b65ab8c7f39  test.gguf:tensor_6  -  Ok
xxh64     c22021c29854f093  test.gguf:tensor_7  -  Ok
xxh64     936df61f5d64261f  test.gguf:tensor_8  -  Ok
xxh64     93fd20c64421c081  test.gguf:tensor_9  -  Ok
xxh64     5a54d3aad816f302  test.gguf  -  Ok

Verification results for test.gguf.manifest - Success
```

或者我们可能只想检查所有哈希是否有效：

```bash
$./llama-gguf-hash --check test.gguf.manifest --all test.gguf.manifest
manifest  test.gguf.manifest  sha256  sha1  xxh64
xxh64     f66e9cd66a4396a0  test.gguf:tensor_0  -  Ok
sha1      59f79ecefd8125a996fdf419239051a7e99e5f20  test.gguf:tensor_0  -  Ok
sha256    c0510d38fa060c46265e0160a85c7243096b01dd31c2f355bdbb5516b20de1bd  test.gguf:tensor_0  -  Ok
xxh64     7d3a1f9ac04d0537  test.gguf:tensor_1  -  Ok
sha1      4765f592eacf096df4628ba59476af94d767080a  test.gguf:tensor_1  -  Ok
sha256    8514cbcc73692a2c56bd7a33a022edd5ff819614bd23b19915d7224387f397a7  test.gguf:tensor_1  -  Ok
xxh64     a0af5d700049693b  test.gguf:tensor_2  -  Ok
sha1      25cbfbad4513cc348e2c95ebdee69d6ff2fd8753  test.gguf:tensor_2  -  Ok
sha256    947e6b36e20f2cc95e1d2ce1c1669d813d574657ac6b5ac5196158d454d35180  test.gguf:tensor_2  -  Ok
xxh64     e83fddf559d7b6a6  test.gguf:tensor_3  -  Ok
sha1      a9cba73e2d90f2ee3dae2548caa42bef3fe6a96c  test.gguf:tensor_3  -  Ok
sha256    423b044e016d8ac73c39f23f60bf01bedef5ecb03c0230accd824c91fe86f1a1  test.gguf:tensor_3  -  Ok
xxh64     1257733306b7992d  test.gguf:tensor_4  -  Ok
sha1      d7bc61db93bb685ce9d598da97117c66729b7543  test.gguf:tensor_4  -  Ok
sha256    79737cb3912d4201384cf7f16a1a37ff7823f23ea796cb205b6ca361ab9e3ebf  test.gguf:tensor_4  -  Ok
xxh64     d238d16ba4711e58  test.gguf:tensor_5  -  Ok
sha1      0706566c198fe1072f37e0a5135b4b5f23654c52  test.gguf:tensor_5  -  Ok
sha256    60949be8298eced0ecdde64487643d018407bd261691e061d9e9c3dbc9fd358b  test.gguf:tensor_5  -  Ok
xxh64     3fbc3b65ab8c7f39  test.gguf:tensor_6  -  Ok
sha1      73922a0727226a409049f6fc3172a52219ca6f00  test.gguf:tensor_6  -  Ok
sha256    574f4c46ff384a3b9a225eb955d2a871847a2e8b3fa59387a8252832e92ef7b0  test.gguf:tensor_6  -  Ok
xxh64     c22021c29854f093  test.gguf:tensor_7  -  Ok
sha1      efc39cece6a951188fc41e354c73bbfe6813d447  test.gguf:tensor_7  -  Ok
sha256    4c0410cd3c500f078ae5b21e8dc9eb79e29112713b2ab58a882f82a3868d4d75  test.gguf:tensor_7  -  Ok
xxh64     936df61f5d64261f  test.gguf:tensor_8  -  Ok
sha1      c2490296d789a4f34398a337fed8377d943d9f06  test.gguf:tensor_8  -  Ok
sha256    c4401313feeba0261275c3b25bd2d8fe40ce04e0f440c2980ed0e9674c30ff01  test.gguf:tensor_8  -  Ok
xxh64     93fd20c64421c081  test.gguf:tensor_9  -  Ok
sha1      7047ce1e78437a6884337a3751c7ee0421918a65  test.gguf:tensor_9  -  Ok
sha256    23d57cf0d7a6e90b0b3616b41300e0cd354781e812add854a5f95aa55f2bc514  test.gguf:tensor_9  -  Ok
xxh64     5a54d3aad816f302  test.gguf
sha1      d15be52c4ff213e823cb6dd13af7ee2f978e7042  test.gguf
sha256    7dd641b32f59b60dbd4b5420c4b0f6321ccf48f58f6ae201a3dbc4a58a27c6e4  test.gguf

Verification results for test.gguf.manifest - Success
```


## 使用的加密/哈希库

这些微型 C 库依赖项通过 [clib c 包管理器](https://github.com/clibs) 安装

- https://github.com/Cyan4973/xxHash
- https://github.com/clibs/sha1/
- https://github.com/jb55/sha256.c
