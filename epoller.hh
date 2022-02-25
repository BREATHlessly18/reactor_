// epoller.hh
// epoller.hh
#ifndef __EPOLLER__
#define __EPOLLER__

#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <typeinfo>
#include <vector>

#include "copyability.hh"
#include "i_object.hh"
#include "threadpool.hh"
#include "timer.hh"

class Timer;
class Epoller : noncopyable {
 public:
  Epoller();
  void loop();
  void quit();

  void addEvent(int fd, int operartion, Object* callback);
  void removeEvent(int fd, Object* callback);

  bool isIOThread() const;

  // enable thread pool
  void enableThreadPool(int num = std::thread::hardware_concurrency());

  // async call. run in thread pool, return a future.
  template <class F, class... Args>
  auto asyncRun(F&& _func, Args&&... _args)
      -> std::future<decltype(_func(_args...))> {
    return threadpool_->enqueue(_func, _args...);
  }

  // async call. run a timer, return a future.
  template <class F, class... Args>
  auto runAfter(int second, F&& _func, Args&&... _args)
      -> std::future<decltype(_func(_args...))> {
    using rtype = decltype(_func(_args...));

    auto task = std::make_shared<std::packaged_task<rtype()>>(
        std::bind(std::forward<F>(_func), std::forward<Args>(_args)...));

    std::future<rtype> res = task->get_future();

    if (timer_) {
      timer_->setTimer(second, [task]() { (*task)(); });
    }

    return res;
  }

 private:
  const int kTimeoutMs_ = 1000;
  const pthread_t threadId_;
  bool quit_;
  int efd_;
  std::unique_ptr<Timer> timer_;
  std::vector<struct epoll_event> activeEvents_;
  std::unique_ptr<ThreadPool> threadpool_;
};

#endif
