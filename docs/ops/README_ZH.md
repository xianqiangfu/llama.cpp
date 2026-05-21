# 运维文档总览

本目录包含 llama.cpp 的运维相关文档，涵盖部署、监控、调优等方面。

## 文档列表

| 文档 | 说明 |
|------|------|
| [ops.md](../ops.md) | 运维详细指南 |
| [README.csv](./README.csv) | 本文档 |
| [BLAS.csv](./BLAS.csv) | BLAS 后端性能数据 |
| [CANN.csv](./CANN.csv) | CANN 后端性能数据 |
| [CPU.csv](./CPU.csv) | CPU 后端性能数据 |
| [CUDA.csv](./CUDA.csv) | CUDA 后端性能数据 |
| [Metal.csv](./Metal.csv) | Metal 后端性能数据 |
| [OpenCL.csv](./OpenCL.csv) | OpenCL 后端性能数据 |
| [SYCL.csv](./SYCL.csv) | SYCL 后端性能数据 |
| [Vulkan.csv](./Vulkan.csv) | Vulkan 后端性能数据 |
| [WebGPU.csv](./WebGPU.csv) | WebGPU 后端性能数据 |
| [ZenDNN.csv](./ZenDNN.csv) | ZenDNN 后端性能数据 |
| [zDNN.csv](./zDNN.csv) | zDNN 后端性能数据 |

## 主要文档

### ops.md - 运维详细指南

[ops.md](../ops.md) 是主要的运维文档，包含以下内容：

#### 1. 服务器部署

- 使用 llama-server 部署 API 服务
- 配置选项和参数
- 反向代理设置
- 负载均衡

#### 2. 性能监控

- 关键指标监控
- 日志分析
- 性能瓶颈识别
- 资源使用优化

#### 3. 故障排查

- 常见问题和解决方案
- 调试技巧
- 错误日志分析
- 系统配置检查

#### 4. 安全配置

- API 认证
- 访问控制
- 数据加密
- 安全最佳实践

详细内容请参见 [ops.md](../ops.md)。

## 性能基准数据

本目录包含各后端的性能基准数据，可用于：

- 性能评估
- 硬件选型
- 配置优化
- 容量规划

### 后端性能数据

各后端的性能数据以 CSV 格式提供：

| 文件 | 内容 |
|------|------|
| [BLAS.csv](./BLAS.csv) | BLAS CPU 后端的性能指标 |
| [CANN.csv](./CANN.csv) | 华为昇腾 NPU 性能数据 |
| [CPU.csv](./CPU.csv) | 基础 CPU 后端性能 |
| [CUDA.csv](./CUDA.csv) | NVIDIA GPU 性能数据 |
| [Metal.csv](./Metal.csv) | Apple Metal 性能数据 |
| [OpenCL.csv](./OpenCL.csv) | OpenCL 兼容设备性能 |
| [SYCL.csv](./SYCL.csv) | Intel GPU/CPU 性能 |
| [Vulkan.csv](./Vulkan.csv) | Vulkan GPU 性能 |
| [WebGPU.csv](./WebGPU.csv) | 浏览器/WebGPU 性能 |
| [ZenDNN.csv](./ZenDNN.csv) | AMD GPU 性能 |
| [zDNN.csv](./zDNN.csv) | IBM Z 系列性能 |

### 查看基准数据

```bash
# 查看特定后端的性能数据
cat ops/CUDA.csv | less

# 对比不同后端
cat ops/CPU.csv ops/CUDA.csv ops/Metal.csv
```

## 运维场景

### 场景 1：部署推理服务

**目标：** 为多个用户提供 API 服务

**步骤：**

1. **选择硬件**
   - 根据基准数据选择合适的后端
   - 预估并发用户数和模型大小

2. **配置服务器**
   ```bash
   ./llama-server \
     --model model-q4.gguf \
     --host 0.0.0.0 \
     --port 8080 \
     --ctx-size 4096 \
     --n-gpu-layers 99
   ```

3. **设置负载均衡**
   - 使用 Nginx 反向代理
   - 配置多个实例

4. **监控性能**
   - 监控 GPU/CPU 使用率
   - 跟踪请求延迟
   - 查看错误率

### 场景 2：性能优化

**目标：** 提高推理速度

**步骤：**

1. **基准测试**
   ```bash
   ./tools/llama-bench -m model.gguf
   ```

2. **分析瓶颈**
   - 检查 CPU/GPU 利用率
   - 分析内存访问模式
   - 识别热点代码

3. **优化配置**
   - 调整批处理大小
   - 优化线程数
   - 使用更好的量化等级

4. **验证改进**
   - 运行基准测试
   - 对比性能数据
   - 确保质量不变

### 场景 3：容量规划

**目标：** 预估需要的硬件资源

**步骤：**

1. **评估需求**
   - 并发用户数
   - 平均请求长度
   - 响应时间要求

2. **参考基准数据**
   - 查看 [ops/](./) 中的 CSV 文件
   - 选择接近的硬件配置

3. **计算资源需求**
   - 模型大小 × 并发数
   - 预留缓冲
   - 考虑峰值负载

4. **预留扩展空间**
   - 20-30% 的额外容量
   - 考虑未来增长

## 监控指标

### 关键指标

| 指标 | 说明 | 目标值 |
|------|------|--------|
| 吞吐量 | tokens/秒 | 越高越好 |
| 延迟 | 首token时间 | < 500ms |
| GPU 利用率 | GPU 使用率 | > 80% |
| 内存使用 | 内存占用 | < 可用内存的 80% |
| 错误率 | 失败请求比例 | < 1% |

### 监控工具

- **htop** - CPU 和内存监控
- **nvidia-smi** - GPU 监控
- **iotop** - I/O 监控
- **prometheus/grafana** - 综合监控

## 故障排查

### 常见问题

#### 1. 内存不足

**症状：** 服务崩溃或变慢

**解决方案：**
- 使用量化模型
- 减小上下文长度
- 启用 `mmap` 加载
- 使用分层卸载

#### 2. GPU 利用率低

**症状：** GPU 使用率低，速度慢

**解决方案：**
- 检查 `n-gpu-layers` 设置
- 增加批处理大小
- 更新 GPU 驱动
- 检查后端是否正确启用

#### 3. 延迟高

**症状：** 首个 token 时间长

**解决方案：**
- 增加预填充缓冲
- 优化模型加载
- 使用更快的硬件
- 减少上下文切换

### 调试技巧

```bash
# 启用详细日志
./llama-server -m model.gguf --log-level debug

# 性能分析
./llama-server -m model.gguf --verbose

# 检查加载时间
time ./llama-cli -m model.gguf ...
```

## 安全最佳实践

### 1. 访问控制

- 使用 API 密钥
- 设置 IP 白名单
- 配置速率限制

### 2. 数据保护

- 启用 HTTPS
- 加密敏感数据
- 定期备份

### 3. 监控和审计

- 记录访问日志
- 监控异常行为
- 定期安全审计

## 相关文档

- [文档目录总览](../README_ZH.md)
- [后端文档](../backend/README_ZH.md)
- [构建指南](../build.md)
- [多 GPU 部署](../multi-gpu.md)
- [Docker 部署](../docker.md)

## 工具

### 监控工具

- [tools/llama-bench](../../tools/llama-bench/) - 基准测试工具
- [tools/server](../../tools/server/) - 服务器工具

### 脚本

- [scripts/server-bench.py](../../scripts/server-bench.py) - 服务器基准测试
- [scripts/tool_bench.py](../../scripts/tool_bench.py) - 工具基准测试

## 社区支持

- GitHub Issues - 报告问题
- GitHub Discussions - 讨论运维问题
- Discord - 实时交流

## 贡献

欢迎贡献运维相关的改进：

- 提供更多硬件的基准数据
- 添加部署脚本
- 改进监控工具
- 更新文档