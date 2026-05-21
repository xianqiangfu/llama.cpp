# llama.cpp 线程安全与并发编程注意事项

本文档总结 llama.cpp 项目的线程安全和并发编程注意事项，帮助开发者编写线程安全的代码。

## 1. 线程模型概述

llama.cpp 支持多线程并行计算，主要用于：
- 矩阵运算并行化
- 批处理任务并行化
- 异步 I/O 操作

## 2. 线程管理机制

### 2.1 线程池

```cpp
ggml_threadpool_t threadpool       = nullptr;
ggml_threadpool_t threadpool_batch = nullptr;

// 附加线程池
void llama_context::attach_threadpool(
        ggml_threadpool_t threadpool,
        ggml_threadpool_t threadpool_batch) {
    this->threadpool       = threadpool;
    this->threadpool_batch = threadpool_batch;
}

// 分离线程池
void llama_context::detach_threadpool() {
    this->threadpool       = nullptr;
    this->threadpool_batch = nullptr;
}
```

**注意事项**：
- 线程池由 GGML 管理
- 不要在多个上下文间共享线程池
- 确保线程池在使用期间保持有效

### 2.2 线程数配置

```cpp
struct common_cpu_params {
    int      n_threads                   = -1;
    bool     cpumask[GGML_MAX_N_THREADS] = {false};
    bool     mask_valid                  = false;
    enum ggml_sched_priority  priority   = GGML_SCHED_PRIO_NORMAL;
    bool     strict_cpu                  = false;
    uint32_t poll                        = 50;  // 轮询级别
};

void llama_context::set_n_threads(int32_t n_threads, int32_t n_threads_batch) {
    cparams.n_threads       = n_threads;
    cparams.n_threads_batch = n_threads_batch;
}
```

**最佳实践**：
- 根据 CPU 核心数设置合理的线程数
- 区分编码和解码的线程数
- 考虑超线程因素

## 3. 线程安全机制

### 3.1 无共享状态设计

llama.cpp 主要通过避免共享状态来确保线程安全：

```cpp
struct llama_context {
private:
    const llama_model & model;  // 只读引用，线程安全
    llama_cparams cparams;      // 参数，配置时设置

    // 避免在多个线程间修改成员变量
public:
    // 提供线程安全的查询接口
    uint32_t n_threads() const;
    uint32_t n_threads_batch() const;
};
```

### 3.2 不可变数据

模型数据通常是不可变的：

```cpp
const llama_model & get_model() const;
const llama_cparams & get_cparams() const;
```

**注意事项**：
- 只读数据可以安全地从多个线程访问
- 确保数据在初始化后不再修改

### 3.3 图计算隔离

图计算在不同线程间隔离：

```cpp
// 每次计算使用独立的图
llm_graph_result * process_ubatch(
            const llama_ubatch & ubatch,
                llm_graph_type   gtype,
        llama_memory_context_i * mctx,
                   ggml_status & ret);
```

### 3.4 批处理隔离

批处理数据在不同请求间隔离：

```cpp
// 批处理数据
llama_batch batch_inp;
std::unique_ptr<llama_batch_allocr> balloc;
```

## 4. 并发编程注意事项

### 4.1 避免数据竞争

**错误示例**：
```cpp
// 危险：多线程访问共享变量
class Context {
private:
    int counter;  // 无保护的共享变量
public:
    void increment() { counter++; }  // 数据竞争！
};
```

**正确示例**：
```cpp
// 方案1：使用局部变量
void process() {
    int local_counter = 0;
    local_counter++;  // 线程安全
}

// 方案2：使用原子操作
class Context {
private:
    std::atomic<int> counter;
public:
    void increment() { counter.fetch_add(1); }  // 线程安全
};

// 方案3：使用互斥锁
class Context {
private:
    int counter;
    std::mutex mutex;
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex);
        counter++;
    }
};
```

### 4.2 避免 False Sharing

**问题**：不同线程访问同一缓存行的不同变量

**解决方案**：
```cpp
// 使用缓存行对齐
struct alignas(64) ThreadLocalData {
    int data;
    // 填充到缓存行大小
    char padding[64 - sizeof(int)];
};
```

### 4.3 死锁预防

**死锁条件**：
1. 互斥条件
2. 占有并等待
3. 不可抢占
4. 循环等待

**预防策略**：
```cpp
// 策略1：固定锁顺序
class SafeContext {
private:
    std::mutex mutex1;
    std::mutex mutex2;
public:
    void safe_method() {
        // 始终按相同顺序获取锁
        std::lock(mutex1, mutex2);
        std::lock_guard<std::mutex> lock1(mutex1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mutex2, std::adopt_lock);
    }
};

// 策略2：使用 std::lock
class SafeContext {
private:
    std::mutex mutex1;
    std::mutex mutex2;
public:
    void safe_method() {
        std::unique_lock<std::mutex> lock1(mutex1, std::defer_lock);
        std::unique_lock<std::mutex> lock2(mutex2, std::defer_lock);
        std::lock(lock1, lock2);  // 避免死锁
    }
};
```

### 4.4 条件变量使用

```cpp
std::mutex mutex;
std::condition_variable cv;
bool ready = false;

void producer() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        ready = true;
    }
    cv.notify_one();
}

void consumer() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, []{ return ready; });  // 防止虚假唤醒
}
```

## 5. 异步操作

### 5.1 异步图计算

```cpp
ggml_status llama_context::graph_compute(ggml_cgraph * gf, bool batched) {
    // 异步计算
    ggml_status ret = ggml_backend_sched_graph_compute_async(sched.get(), gf);

    // 等待完成
    if (ret == GGML_STATUS_SUCCESS) {
        ret = ggml_backend_sched_graph_compute_async(sched.get(), nullptr);
    }

    return ret;
}
```

### 5.2 中止回调

```cpp
ggml_abort_callback abort_callback      = nullptr;
void *              abort_callback_data = nullptr;

void llama_context::set_abort_callback(
        bool (*abort_callback)(void * data),
        void * abort_callback_data) {
    this->abort_callback      = abort_callback;
    this->abort_callback_data = abort_callback_data;
}
```

**注意事项**：
- 中止回调应该是线程安全的
- 避免在回调中执行耗时操作

## 6. 性能计数器

### 6.1 线程本地统计

```cpp
// 使用 mutable 修饰性能计数器，允许在 const 方法中更新
mutable int64_t t_start_us  = 0;
mutable int64_t t_load_us   = 0;
mutable int64_t t_p_eval_us = 0;
mutable int64_t t_eval_us   = 0;

mutable int32_t n_p_eval = 0;
mutable int32_t n_eval   = 0;
mutable int32_t n_reused = 0;
```

**注意事项**：
- 性能计数器可能从多个线程更新
- 使用原子操作或线程本地存储
- 注意统计的准确性

### 6.2 性能数据

```cpp
llama_perf_context_data perf_get_data() const {
    return llama_perf_context_data{
        t_start_us,
        t_load_us,
        t_p_eval_us,
        t_eval_us,
        n_p_eval,
        n_eval,
        n_reused,
    };
}

void perf_reset() {
    t_start_us  = 0;
    t_load_us   = 0;
    t_p_eval_us = 0;
    t_eval_us   = 0;
    n_p_eval = 0;
    n_eval   = 0;
    n_reused = 0;
}
```

## 7. 并发模式

### 7.1 生产者-消费者模式

```cpp
// 批处理队列
std::queue<Batch> batch_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;

void producer() {
    while (running) {
        Batch batch = create_batch();
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            batch_queue.push(batch);
        }
        queue_cv.notify_one();
    }
}

void consumer() {
    while (running) {
        Batch batch;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [&]{ return !batch_queue.empty() || !running; });
            if (!running) break;
            batch = batch_queue.front();
            batch_queue.pop();
        }
        process_batch(batch);
    }
}
```

### 7.2 工作窃取模式

```cpp
// 使用线程池实现工作窃取
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop = false;

public:
    ThreadPool(size_t n_threads) {
        for (size_t i = 0; i < n_threads; ++i) {
            workers.emplace_back([this]{
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [this]{ return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
};
```

### 7.3 Pipeline 模式

```cpp
// 流水线处理
class Pipeline {
private:
    Stage1 stage1;
    Stage2 stage2;
    Stage3 stage3;
    std::queue<Data> queue1, queue2;
    std::mutex mutex1, mutex2;
    std::condition_variable cv1, cv2;

public:
    void run() {
        // Stage 1: Tokenize
        std::thread t1([&]{
            while (running) {
                Data data = input();
                data = stage1.process(data);
                {
                    std::lock_guard<std::mutex> lock(mutex1);
                    queue1.push(data);
                }
                cv1.notify_one();
            }
        });

        // Stage 2: Encode
        std::thread t2([&]{
            while (running) {
                Data data;
                {
                    std::unique_lock<std::mutex> lock(mutex1);
                    cv1.wait(lock, [&]{ return !queue1.empty(); });
                    data = queue1.front();
                    queue1.pop();
                }
                data = stage2.process(data);
                {
                    std::lock_guard<std::mutex> lock(mutex2);
                    queue2.push(data);
                }
                cv2.notify_one();
            }
        });

        // Stage 3: Decode
        std::thread t3([&]{
            while (running) {
                Data data;
                {
                    std::unique_lock<std::mutex> lock(mutex2);
                    cv2.wait(lock, [&]{ return !queue2.empty(); });
                    data = queue2.front();
                    queue2.pop();
                }
                data = stage3.process(data);
                output(data);
            }
        });
    }
};
```

## 8. 调试与测试

### 8.1 线程检测器

**使用 ThreadSanitizer 检测线程问题**：

```bash
cmake -B build -DLLAMA_SANITIZE_THREAD=ON
cmake --build build
./your_program
```

### 8.2 竞态检测

**使用 Helgrind (Valgrind)**：

```bash
valgrind --tool=helgrind ./your_program
```

### 8.3 死锁检测

**使用工具检测死锁**：
- Valgrind DRD
- Intel Inspector
- ThreadSanitizer

## 9. 最佳实践

### 9.1 最小化共享状态

```cpp
// 好的做法：每个线程有自己的数据
void thread_function() {
    ThreadLocalData data;
    // 使用 data
}

// 避免：全局共享数据
static SharedData global_data;  // 危险！
```

### 9.2 使用高级并发工具

```cpp
// 使用标准库的并发工具
std::async(std::launch::async, task_function);
std::promise, std::future
std::atomic
std::mutex, std::shared_mutex
std::condition_variable
```

### 9.3 避免忙等待

```cpp
// 不好：忙等待
while (!ready) { /* 空循环 */ }

// 好：使用条件变量
std::unique_lock<std::mutex> lock(mutex);
cv.wait(lock, []{ return ready; });
```

### 9.4 使用线程局部存储

```cpp
thread_local std::mt19937 rng;
thread_local int thread_id = get_thread_id();
```

## 10. 常见问题与解决方案

### 10.1 数据竞争

**问题**：多个线程同时访问共享数据

**解决方案**：
- 使用互斥锁保护共享数据
- 使用原子操作
- 避免共享数据

### 10.2 死锁

**问题**：多个线程互相等待对方释放锁

**解决方案**：
- 固定锁获取顺序
- 使用 std::lock
- 设置超时
- 避免嵌套锁

### 10.3 活锁

**问题**：线程不断尝试获取锁但总是失败

**解决方案**：
- 使用退避策略
- 添加随机延迟
- 改进锁获取策略

### 10.4 饥饿

**问题**：某些线程长期得不到资源

**解决方案**：
- 使用公平锁
- 设置优先级
- 限制等待时间

## 11. 总结

llama.cpp 线程安全要点：

1. **避免共享状态**：优先使用线程本地数据
2. **不可变数据**：只读数据可以安全共享
3. **正确同步**：使用适当的同步机制
4. **避免死锁**：遵循锁顺序，使用 std::lock
5. **性能考虑**：避免过度同步，使用细粒度锁
6. **测试验证**：使用工具检测线程问题

遵循这些注意事项可以编写出线程安全、高性能的并发代码。