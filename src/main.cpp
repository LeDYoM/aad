#include <iostream>
#include <chrono>
#include "include/th_pool.h"

int exampleTask(int id, int sleepTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
//    std::cout << "Task " << id << " completed after " << sleepTime << " ms\n";
    return sleepTime;
}

int main()
{
    // Create a thread pool with 4 worker threads
    aad::ThreadPool pool(4);

    std::vector<std::future<int>> results;
    // Enqueue and execute some tasks
    for (int i = 0; i < 100; ++i)
    {
        results.emplace_back(pool.enqueue(exampleTask, i, i * 10));
    }

    for (int i = 0; i < 100; ++i)
    {
        const int result{results[i].get()};
        std::cout << "Task " << i << " returned " << result << " \n";
    }

    return 0;
}
