#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

// The ThreadPool class that manages worker threads and a task queue.
// Created with the help of Gemini(thanks to Google)
class ThreadPool {
public:
    // The constructor creates and starts the worker threads.
    ThreadPool(size_t numThreads);

    // The destructor joins all threads.
    ~ThreadPool();

    // Adds a new task to the queue.
    // It uses a template to accept any function and its arguments.
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // A vector to store the worker threads.
    std::vector<std::thread> workers;

    // The queue of tasks. Each task is a function with no arguments.
    std::queue<std::function<void()>> tasks;

    // Mutex for synchronizing access to the task queue.
    std::mutex queue_mutex;

    // Condition variable to notify waiting threads.
    std::condition_variable condition;

    // A boolean flag to stop the threads.
    bool stop;
};

