# HTTP 服务器请求处理流程图

```mermaid
flowchart TD
    A[HTTP 请求到达] --> B[监听端口]
    B --> C[接收连接]
    C --> D[解析 HTTP 请求]
    D --> E{请求类型}
    E -->|GET| F[路由到 GET 处理器]
    E -->|POST| G[路由到 POST 处理器]
    E -->|OPTIONS| H[处理 CORS 预检]
    E -->|其他| I[返回 405 错误]

    F --> J{请求路径}
    G --> J
    H --> K[返回 CORS 头]
    I --> L[返回错误响应]

    J -->|/health| M[健康检查]
    J -->|/v1/chat/completions| N[聊天补全]
    J -->|/v1/completions| O[文本补全]
    J -->|/v1/embeddings| P[嵌入计算]
    J -->|/v1/models| Q[模型信息]
    J -->|/slots| R[槽位信息]
    J -->|其他| S[返回 404]

    M --> T[返回服务状态]
    N --> U[解析请求体]
    O --> U
    P --> U
    Q --> V[返回模型列表]
    R --> W[返回槽位信息]
    S --> X[返回 404]

    U --> Y{验证请求}
    Y -->|成功| Z[创建任务]
    Y -->|失败| AA[返回 400 错误]

    Z --> AB[获取空闲槽位]
    AB --> AC{是否有空闲槽位?}
    AC -->|是| AD[分配槽位]
    AC -->|否| AE[排队等待]

    AD --> AF[添加到批次]
    AF --> AG[执行推理]
    AG --> AH[采样生成]
    AH --> AI[构建响应]
    AI --> AJ[返回 JSON 响应]

    AE --> AK{队列超时?}
    AK -->|是| AL[返回 503 错误]
    AK -->|否| AC

    T --> AM[发送响应]
    AA --> AM
    L --> AM
    V --> AM
    W --> AM
    X --> AM
    AJ --> AM
    AL --> AM

    AM --> AN[关闭连接]
    AN --> AO[等待下一个请求]

    subgraph 路由处理器
        AP[server_http_req<br/>请求对象]
        AQ[server_http_res<br/>响应对象]
        AR[handler_t<br/>处理函数]
    end

    F --> AP
    G --> AP
    AP --> AR
    AR --> AQ
    AQ --> AM

    subgraph 错误处理
        AS[异常捕获]
        AT[错误格式化]
        AU[错误响应]
    end

    Z --> AS
    AS --> AT
    AT --> AU
    AU --> AM
```

## HTTP 服务器说明

### 1. 请求处理流程
- **连接接收**: 接受客户端连接
- **请求解析**: 解析 HTTP 请求头和体
- **路由分发**: 根据路径和方法分发到处理器
- **响应构建**: 构建并返回响应

### 2. 支持的 API
- **/health**: 健康检查
- **/v1/chat/completions**: 聊天补全
- **/v1/completions**: 文本补全
- **/v1/embeddings**: 嵌入计算
- **/v1/models**: 模型信息
- **/slots**: 槽位信息

### 3. 请求类型
- **GET**: 查询操作
- **POST**: 创建操作
- **OPTIONS**: CORS 预检

### 4. 推理流程
- **请求验证**: 验证请求参数
- **任务创建**: 创建推理任务
- **槽位分配**: 分配推理槽位
- **批次处理**: 添加到推理批次
- **结果返回**: 返回生成结果

### 5. 并发控制
- **槽位管理**: 管理并发推理槽位
- **队列管理**: 处理超载情况
- **超时处理**: 超时返回错误

### 6. 错误处理
- **异常捕获**: 捕获处理异常
- **错误格式化**: 统一错误格式
- **错误响应**: 返回错误信息

### 7. CORS 支持
- **预检请求**: 处理 OPTIONS 请求
- **跨域头**: 添加 CORS 头部
- **安全控制**: 配置允许的来源