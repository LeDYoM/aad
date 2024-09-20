#include "include/th_pool.h"

namespace aad
{
ThreadPool::ThreadPool(const size_t numThreads) : stop{false}
{
    auto main_worker{[this]() -> void {
        while (!stop) [[unlikely]]
        {
            std::function<void()> task;
            {
                // Lock the queue
                std::unique_lock<std::mutex> lock{queueMutex};
                // Wait until the queue has tasks or stop is requested
                condition.wait(lock, [this] { return stop || !tasks.empty(); });

                if (stop) [[unlikely]]
                {
                    return;  // If stopped and no tasks, exit the thread
                }
                // Get the next task from the queue
                task = std::move(tasks.front());
                tasks.pop();
            }
            // Execute the task
            task();
        }
    }};

    for (size_t i{0U}; i < numThreads; ++i)
    {
        workers.emplace_back(main_worker);
    }
}

// Destructor
ThreadPool::~ThreadPool()
{
    {
        // Stop all threads
        std::unique_lock<std::mutex> lock{queueMutex};
        stop = true;
    }
    condition.notify_all();  // Notify all threads to stop
}

}  // namespace aad
