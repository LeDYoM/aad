#include <iostream>
#include <chrono>
#include "include/th_pool.h"

void exampleTask(int id, int sleepTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    std::cout << "Task " << id << " completed after " << sleepTime << " ms\n";
}

int main()
{
    // Create a thread pool with 4 worker threads
    aad::ThreadPool pool(4);

    // Enqueue and execute some tasks
    for (int i = 1; i <= 8; ++i)
    {
        pool.enqueue(exampleTask, i, i * 100);
    }

    for (int i = 9; i <= 180; ++i)
    {
        pool.enqueue(exampleTask, i, 10);
    }

    // The thread pool will automatically clean up in its destructor
    std::this_thread::sleep_for(
        std::chrono::seconds(2));  // Wait for tasks to finish

    // Enqueue and execute some tasks
    for (int i = 1; i <= 8; ++i)
    {
        pool.enqueue(exampleTask, i, i * 100);
    }

    for (int i = 9; i <= 18; ++i)
    {
        pool.enqueue(exampleTask, i, i * 10);
    }

    return 0;
}
