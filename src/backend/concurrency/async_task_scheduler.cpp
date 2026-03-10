#include <backend/concurrency/async_task_scheduler.hpp>
#include <iostream>

namespace hero::concurrency {

AsyncTaskScheduler& AsyncTaskScheduler::getInstance() {
    static AsyncTaskScheduler instance;
    return instance;
}

void AsyncTaskScheduler::initialize(int num_workers) {
    if (running_) return;

    num_workers_ = num_workers;
    running_ = true;

    for (int i = 0; i < num_workers_; ++i) {
        workers_.emplace_back([this](std::stop_token stop_token) {
            workerThreadFunction(stop_token);
        });
    }

    std::cout << "[AsyncScheduler] Initialized with " << num_workers << " workers" << std::endl;
}

void AsyncTaskScheduler::shutdown() {
    running_ = false;
    cv_.notify_all();
    workers_.clear();
    std::cout << "[AsyncScheduler] Shutdown complete" << std::endl;
}

bool AsyncTaskScheduler::isRunning() const {
    return running_;
}

void AsyncTaskScheduler::submitTask(Task task) {
    if (!running_) return;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(task);
    }
    cv_.notify_one();
}

void AsyncTaskScheduler::submitPriorityTask(Task task) {
    // For priority tasks, we could implement a priority queue
    // For now, just add to regular queue
    submitTask(task);
}

void AsyncTaskScheduler::setWorkerCount(int count) {
    if (count == num_workers_) return;
    if (count > num_workers_) {
        for (int i = num_workers_; i < count; ++i) {
            workers_.emplace_back([this](std::stop_token stop_token) {
                workerThreadFunction(stop_token);
            });
        }
    }
    num_workers_ = count;
}

int AsyncTaskScheduler::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return task_queue_.size();
}

int AsyncTaskScheduler::getWorkerCount() const {
    return num_workers_;
}

int AsyncTaskScheduler::getCompletedTaskCount() const {
    return completed_tasks_;
}

int AsyncTaskScheduler::getFailedTaskCount() const {
    return failed_tasks_;
}

AsyncTaskScheduler::~AsyncTaskScheduler() {
    shutdown();
}

void AsyncTaskScheduler::workerThreadFunction(std::stop_token stop_token) {
    while (!stop_token.stop_requested()) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            cv_.wait(lock, stop_token, [this]() {
                return !task_queue_.empty() || !running_;
            });

            if (task_queue_.empty()) {
                continue;
            }

            task = task_queue_.front();
            task_queue_.pop();
        }

        try {
            task();
            completed_tasks_++;
        } catch (const std::exception& e) {
            std::cerr << "[AsyncScheduler] Task failed: " << e.what() << std::endl;
            failed_tasks_++;
        }
    }
}

} // namespace hero::concurrency
