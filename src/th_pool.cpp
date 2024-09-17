#include "include/th_pool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() {
            for (;;) {
                std::function<void()> task;
                {
                    // Lock the queue
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    // Wait until the queue has tasks or stop is requested
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                    if (this->stop && this->tasks.empty()) {
                        return; // If stopped and no tasks, exit the thread
                    }
                    // Get the next task from the queue
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                // Execute the task
                task();
            }
        });
    }
}

// Destructor
ThreadPool::~ThreadPool() {
    {
        // Stop all threads
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();  // Notify all threads to stop
    for (std::thread &worker : workers) {
        worker.join();        // Join all threads
    }
}
