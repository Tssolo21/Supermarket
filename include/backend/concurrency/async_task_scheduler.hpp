#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <vector>

namespace hero::concurrency {

/**
 * @class AsyncTaskScheduler
 * @brief Task scheduler for non-blocking operations
 * 
 * Uses std::jthread for modern async tasking.
 * Implements worker pool for real-time stock updates and async checkouts.
 */
class AsyncTaskScheduler {
public:
    using Task = std::function<void()>;

    static AsyncTaskScheduler& getInstance();

    // Lifecycle
    void initialize(int num_workers = 4);
    void shutdown();
    bool isRunning() const;

    // Task submission
    void submitTask(Task task);
    void submitPriorityTask(Task task);  // Higher priority in queue

    // Worker management
    void setWorkerCount(int count);
    int getQueueSize() const;
    int getWorkerCount() const;

    // Statistics
    int getCompletedTaskCount() const;
    int getFailedTaskCount() const;

private:
    AsyncTaskScheduler() = default;
    ~AsyncTaskScheduler();

    void workerThreadFunction(std::stop_token stop_token);

    std::queue<Task> task_queue_;
    std::vector<std::jthread> workers_;
    mutable std::mutex queue_mutex_;
    std::condition_variable_any cv_;
    bool running_ = false;
    int completed_tasks_ = 0;
    int failed_tasks_ = 0;
    int num_workers_ = 4;
};

} // namespace hero::concurrency
