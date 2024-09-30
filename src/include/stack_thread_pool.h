#ifndef STACK_THREAD_POOL_INCLUDE_H
#define STACK_THREAD_POOL_INCLUDE_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <type_traits>
#include <array>

namespace aad
{
template <std::size_t kMaxQueueSize>
class StackThreadPool
{
public:
    explicit StackThreadPool(const size_t numThreads = kMaxQueueSize)
    {
        for (size_t i{0U}; i < numThreads; ++i)
        {
            std::jthread p{[this]() -> void {
                while (!stop)
                    [[likely]]
                    {
                        std::function<void()> task;
                        {
                            // Lock the queue
                            std::unique_lock<std::mutex> lock{queueMutex};
                            // Wait until the queue has tasks or stop is
                            // requested
                            condition.wait(lock, [this] {
                                return stop || !tasks.empty();
                            });

                            if (stop) [[unlikely]]
                            {
                                return;  // If stopped and no tasks, exit the
                                         // thread
                            }
                            // Get the next task from the queue
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        // Execute the task
                        task();
                    }
            }};

            workers[i] = std::move(p);
        }
    }

    ~StackThreadPool()
    {
        {
            // Stop all threads
            std::unique_lock<std::mutex> lock{queueMutex};
            stop = true;
        }
        condition.notify_all();  // Notify all threads to stop
    }

    // Task Enqueue function
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
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

            // Add the task to the queue
            tasks.emplace([t = std::move(task)]() { (*t)(); });
        }

        // Notify one worker thread
        condition.notify_one();
        return res;
    }

private:
    std::array<std::jthread, kMaxQueueSize> workers;  // Threads in the pool
    std::queue<std::function<void()>> tasks;          // Task queue

    std::mutex queueMutex;              // Synchronization
    std::condition_variable condition;  // Condition variable to notify workers
    std::atomic<bool> stop{false};            // To stop all threads
};

}  // namespace aad

#endif
