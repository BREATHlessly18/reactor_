#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool final {
 public:
  ThreadPool(uint8_t _num = 0);
  ~ThreadPool();

  ThreadPool(ThreadPool const&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool const&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  // 函数模板声明在class体中，调用处实例化
  // 采用返回值后置，因为前置时形参还没声明，无法推导类型
  template <class F, class... Args>
  auto enqueue(F&& _func, Args&&... _args)
      -> std::future<decltype(_func(_args...))> {
    if (this->stop_.load()) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    using rtype = decltype(_func(_args...));

    auto task = std::make_shared<std::packaged_task<rtype()>>(
        std::bind(std::forward<F>(_func), std::forward<Args>(_args)...));

    std::future<rtype> res = task->get_future();

    {
      std::lock_guard<std::mutex> lock(mtx_);

      this->tasks_.emplace([task]() { (*task)(); });
    }

    this->cv_.notify_all();
    return res;
  }

 private:
  std::atomic<bool> stop_;
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
};
