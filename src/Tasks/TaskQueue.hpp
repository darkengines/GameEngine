#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// Thread-safe task queue
class TaskQueue
{
 public:
  using Task = std::packaged_task<void()>;

  void push(Task task)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(std::move(task));
    cond_var_.notify_one();
  }

  Task pop()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this] { return !tasks_.empty(); });  // Blocking wait
    Task task = std::move(tasks_.front());
    tasks_.pop();
    return task;
  }

 private:
  std::queue<Task> tasks_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};
