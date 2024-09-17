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

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Task Enqueue function
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
        using returnType = typename std::invoke_result_t<F, Args...>;

        // Package the task as a future
        auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<returnType> res = task->get_future(); // Get future to return to the caller

        {
            // Lock the queue to push the new task
            std::unique_lock<std::mutex> lock(queueMutex);

            // Don't allow enqueueing after stopping the pool
            if (stop) {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); }); // Add the task to the queue
        }

        condition.notify_one(); // Notify one worker thread
        return res;
    }

private:
    std::vector<std::thread> workers;           // Threads in the pool
    std::queue<std::function<void()>> tasks;    // Task queue

    std::mutex queueMutex;                      // Synchronization
    std::condition_variable condition;          // Condition variable to notify workers
    std::atomic<bool> stop;                     // To stop all threads
};

#endif
