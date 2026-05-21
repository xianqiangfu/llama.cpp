# 多 GPU 部署最佳实践

## 概述

多 GPU 部署是扩展 llama.cpp 能力和性能的重要手段。本文档提供了多 GPU 部署的详细指南，包括部署策略、负载均衡、性能优化和故障排查。

## 多 GPU 模式对比

### 1. 流水线并行

**工作原理：**
- 不同 GPU 处理不同的层
- Token 顺序通过流水线
- KV 缓存在对应 GPU 上

**优势：**
- 设备间通信少
- 兼容性好
- 适合慢速互联

**劣势：**
- 生成延迟高
- 设备利用率不均
- 需要大量 token 扩展

**配置：**
```bash
./llama-cli -m model.gguf --split-mode layer
./llama-cli -m model.gguf --split-mode layer --tensor-split 3,2,1
```

### 2. 张量并行

**工作原理：**
- 每层在 GPU 间分割
- 通过"元设备"抽象
- 跨设备归约操作

**优势：**
- 生成延迟低
- 设备利用率高
- 适合快速互联

**劣势：**
- 设备间通信频繁
- 对互联速度敏感
- 实验性功能

**配置：**
```bash
./llama-cli -m model.gguf --split-mode tensor --flash-attn on
```

### 3. 混合模式

**特点：**
- 结合两种模式优势
- 复杂配置
- 高级优化

**配置：**
```bash
# 部分层使用张量并行
# 部分层使用流水线并行
# 需要手动配置
```

## 硬件配置建议

### 1. GPU 选择

**推荐配置：**
- **同型号 GPU**：简化配置和优化
- **相同代际**：避免性能差异
- **充足显存**：单卡 >= 16GB
- **快速互联**：NVLink 或 PCIe 4.0+

### 2. 系统配置

**关键要素：**
- **PCIe 配置**：充足的 PCIe 通道
- **内存配置**：充足的系统内存
- **散热系统**： adequate cooling
- **电源供应**：稳定的电源

### 3. 网络配置（分布式）

**考虑因素：**
- **网络带宽**：>= 10GbE
- **延迟**：尽可能低
- **网络拓扑**：优化路径
- **协议**：RDMA 如果可用

## 部署策略

### 1. 负载均衡

**策略选择：**

1. **均匀分配**
```bash
# 基于显存均匀分配
./llama-cli -m model.gguf --split-mode layer --tensor-split 1,1,1,1
```

2. **基于性能分配**
```bash
# 性能高的 GPU 分配更多
./llama-cli -m model.gguf --split-mode layer --tensor-split 2,1,1
```

3. **动态调整**
```bash
# 监控性能并动态调整
# 需要自定义脚本
```

### 2. 模型分割

**分割原则：**
- 相邻层在同一 GPU
- KV 缓存随层分配
- 考虑层的大小差异

**配置：**
```bash
# 自动分割（推荐）
./llama-cli -m model.gguf --split-mode layer

# 手动分割
./llama-cli -m model.gguf --split-mode layer --tensor-split 3,2,1
```

### 3. 内存管理

**策略：**
```bash
# KV 缓存管理
./llama-cli -m model.gguf \
  --cache-type-k f16 \
  --cache-type-v f16

# 上下文管理
./llama-cli -m model.gguf -c 8192

# GPU 层数管理
./llama-cli -m model.gguf -ngl all
```

## 性能优化

### 1. 通信优化

**NCCL 优化：**
```bash
# 编译时启用 NCCL
cmake -DGGML_CUDA_NCCL=ON ..
cmake --build . --config Release

# 验证 NCCL 使用
./llama-cli -m model.gguf --split-mode tensor --verbose
```

**P2P 访问：**
```bash
# 启用 P2P（如果支持）
GGML_CUDA_P2P=1 ./llama-cli -m model.gguf --split-mode tensor
```

### 2. GPU 利用率优化

**策略：**
```bash
# 最大化 GPU 利用
./llama-cli -m model.gguf --split-mode layer -ngl all

# 优化批处理
./llama-cli -m model.gguf --split-mode layer -b 1024

# 启用 Flash Attention
./llama-cli -m model.gguf --split-mode tensor -fa on
```

### 3. 内存优化

**策略：**
```bash
# 量化 KV 缓存
./llama-cli -m model.gguf \
  --cache-type-k q8_0 \
  --cache-type-v q8_0

# 启用 mmap
./llama-cli -m model.gguf --mmap

# 优化上下文大小
./llama-cli -m model.gguf -c 4096
```

## 监控和诊断

### 1. 性能监控

**监控脚本：**
```bash
#!/bin/bash
# monitor_multi_gpu.sh

while true; do
    echo "=== $(date) ==="
    
    # GPU 状态
    nvidia-smi --query-gpu=index,utilization.gpu,memory.used,memory.total,temperature.gpu --format=csv
    
    # 进程信息
    ps aux | grep llama-cli
    
    # 网络状态（如果分布式）
    # netstat -i
    
    sleep 5
done
```

### 2. 性能分析

**分析工具：**
```bash
# 使用 nvprof
nvprof ./llama-cli -m model.gguf --split-mode tensor -p "test" -n 1000

# 使用 Nsight Systems
nsys profile ./llama-cli -m model.gguf --split-mode tensor -p "test" -n 1000

# 使用 Nsight Compute
ncu ./llama-cli -m model.gguf --split-mode tensor -p "test" -n 1000
```

## 故障排查

### 常见问题

#### 1. 性能低于单 GPU

**可能原因：**
- 设备间通信成为瓶颈
- NCCL 未启用
- P2P 访问未启用
- 分割模式不当

**解决方案：**
```bash
# 检查 NCCL
./llama-cli -m model.gguf --split-mode tensor --verbose

# 启用 P2P
GGML_CUDA_P2P=1 ./llama-cli -m model.gguf --split-mode tensor

# 尝试不同分割模式
./llama-cli -m model.gguf --split-mode layer
```

#### 2. 内存不足

**可能原因：**
- 上下文太大
- 批处理太大
- 模型太大

**解决方案：**
```bash
# 减少上下文
./llama-cli -m model.gguf -c 2048

# 减少批处理
./llama-cli -m model.gguf -b 256

# 减少 GPU 层数
./llama-cli -m model.gguf -ngl 30
```

#### 3. 稳定性问题

**可能原因：**
- 过热
- 电源不稳定
- 驱动问题

**解决方案：**
```bash
# 检查温度
nvidia-smi --query-gpu=temperature.gpu --format=csv

# 限制功耗
sudo nvidia-smi -pl 250

# 更新驱动
sudo apt update
sudo apt upgrade nvidia-driver
```

## 最佳实践总结

### 1. 部署前准备

- 验证硬件兼容性
- 准备合适的模型
- 测试单 GPU 性能
- 建立性能基准

### 2. 部署过程

- 从流水线并行开始
- 逐步优化配置
- 监控系统状态
- 记录配置参数

### 3. 运行监控

- 持续监控性能
- 收集性能数据
- 定期优化配置
- 预防性维护

### 4. 故障处理

- 建立故障处理流程
- 准备降级方案
- 备份配置和数据
- 定期测试恢复

## 结论

多 GPU 部署可以显著提升 llama.cpp 的能力和性能。通过选择合适的分割模式、优化负载均衡、持续监控和优化，可以充分发挥多 GPU 系统的优势。

关键要点：
1. 根据硬件选择合适的分割模式
2. 优化负载均衡和内存管理
3. 持续监控性能和资源使用
4. 建立故障处理和降级机制

## 相关资源

- [多设备协同问题总结](./多设备协同问题总结.md)
- [后端兼容性问题总结](./后端兼容性问题总结.md)
- [多 GPU 文档](../multi-gpu.md)
- [推理配置调优最佳实践](./推理配置调优最佳实践.md)