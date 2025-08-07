#include "ThreadPool.h"

// Constructor: Initializes the thread pool with a given number of threads.
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    // This loop creates the worker threads.
    for(size_t i = 0; i < numThreads; ++i) {
        // Each worker thread is created with a lambda function.
        workers.emplace_back([this] {
            // This is the main loop for each worker thread.
            while(true) {
                std::function<void()> task;

                { // This block is a critical section, protected by the mutex.
                    // A std::unique_lock is used to lock the mutex. It unlocks automatically when it goes out of scope.
                    std::unique_lock<std::mutex> lock(this->queue_mutex);

                    // The thread waits on the condition variable. It will sleep until:
                    // 1. The 'stop' flag is true.
                    // 2. The 'tasks' queue is not empty.
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

                    // If the pool is stopped and there are no tasks left, the thread can exit.
                    if(this->stop && this->tasks.empty()) {
                        return;
                    }

                    // If a task is available, move it from the queue to the 'task' variable.
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                } // The lock is released here.

                // Execute the task.
                task();
            }
        });
    }
}

// Enqueue: Adds a task to the queue and returns a std::future to get the result later.
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    // Create a packaged_task, which wraps the function and its arguments.
    // This allows us to get a future from it.
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
        
    // Get the future from the packaged_task.
    std::future<return_type> res = task->get_future();
    
    { // Critical section to add the task to the queue.
        std::unique_lock<std::mutex> lock(queue_mutex);

        // Don't allow enqueueing after stopping the pool.
        if(stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        // Add the task (as a void function) to the queue.
        tasks.emplace([task](){ (*task)(); });
    } // Lock is released here.
    
    // Notify one waiting worker thread that a task is available.
    condition.notify_one();
    return res;
}

// Destructor: Joins all worker threads to ensure they finish before the pool is destroyed.
ThreadPool::~ThreadPool() {
    { // Critical section to set the 'stop' flag.
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    } // Lock is released here.
    
    // Notify all waiting threads to wake them up.
    condition.notify_all();
    
    // Join all worker threads. This waits for each thread to finish its execution.
    for(std::thread &worker: workers) {
        worker.join();
    }
}