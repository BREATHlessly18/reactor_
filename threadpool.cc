#include "threadpool.hh"

ThreadPool::ThreadPool(uint8_t _num) : stop_{false} {
  for (auto i{0}; i != _num; ++i) {
    this->threads_.emplace_back([this] {
      while (true) {
        auto task = std::function<void()>();

        {
          std::unique_lock<std::mutex> lock(this->mtx_);
          // 停止时不阻塞，运行时任务队列非空时不阻塞
          this->cv_.wait(lock, [this] {
            if (this->stop_.load() or !this->tasks_.empty()) {
              return true;
            }
            return false;
          });

          // 保证受到停止信号后，消费完所有任务
          if (this->stop_.load() and this->tasks_.empty()) {
            break;
          }

          task = std::move(this->tasks_.front());
          this->tasks_.pop();
        }

        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  this->stop_.store(true);
  this->cv_.notify_all();

  for (auto&& thread : this->threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}
