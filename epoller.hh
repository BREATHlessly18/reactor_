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
#include "timer.hh"

class Timer;
class Epoller : noncopyable {
 public:
  Epoller();
  void loop();
  void quit();

  void addEvent(int fd, int operartion, Object* callback);
  void removeEvent(int fd, Object* callback);

  // async call. return a future.
  template <class F, class... Args>
  auto runAfter(int second, F&& _func, Args&&... _args)
      -> std::future<decltype(_func(_args...))> {
    using rtype = decltype(_func(_args...));

    auto task = std::make_shared<std::packaged_task<rtype()>>(
        std::bind(std::forward<F>(_func), std::forward<Args>(_args)...));

    std::future<rtype> res = task->get_future();

    {
      if (timerTie_) {
        timerTie_->setTimer(second, [task]() { (*task)(); });
      }
    }

    return res;
  }

 private:
  const int kTimeoutMs_ = 1000;
  bool quit_;
  int efd_;
  std::unique_ptr<Timer> timerTie_;
  std::vector<struct epoll_event> activeEvents_;
};

#endif
