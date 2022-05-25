#ifndef __ASYNCQUE__
#define __ASYNCQUE__

#include <condition_variable>
#include <queue>

#include "copyability.hh"
#include "epoller.hh"

template <typename T>
class AsyncQue : noncopyable {
 public:
  AsyncQue() = default;
  ~AsyncQue() = default;

  void push_back(T &&e) {
    std::unique_lock<std::mutex> lock(mtx_);

    //  超过高水位就延时
    if (elements_.size() > kMaxSize_) {
      cv_.wait_for(lock, 3s);
    }

    elements_.push(std::forward(e));

    cv_.notify_one();
  }

  void pop_front(T &e) {
    std::unique_lock<std::mutex> lock(mtx_);

    cv_.wait(lock, [=] { return elements_.empty() ? false : true; });

    // 此时非空!
    e = std::move(elements_.front());
    elements_.pop();
  }

 private:
  const uint8_t kMaxSize_ = 30;
  std::queue<T> elements_;
  std::condition_variable cv_;
  std::mutex mtx_;
};

#endif