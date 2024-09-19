#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <type_traits>

class ThreadPool
{
public:
    explicit ThreadPool(const size_t numThreads);
    ~ThreadPool();

    // Task Enqueue function
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result_t<F, Args...>>;

private:
    std::vector<std::jthread> workers;        // Threads in the pool
    std::queue<std::function<void()>> tasks;  // Task queue

    std::mutex queueMutex;              // Synchronization
    std::condition_variable condition;  // Condition variable to notify workers
    std::atomic<bool> stop;             // To stop all threads
};

template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result_t<F, Args...>>
{
    using returnType = typename std::invoke_result_t<F, Args...>;

    // Package the task as a future
    auto task{std::make_shared<std::packaged_task<returnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...))};

    // Get future to return to the caller
    std::future<returnType> res{task->get_future()};

    {
        // Lock the queue to push the new task
        std::unique_lock<std::mutex> lock{queueMutex};

        // Don't allow enqueueing after stopping the pool
        if (stop)
        {
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        }

        // Add the task to the queue

        tasks.emplace([t = std::move(task)]() { (*t)(); });
    }

    // Notify one worker thread
    condition.notify_one();
    return res;
}

#endif
