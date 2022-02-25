// timer.hh 定时器，时间到了出发写事件
#ifndef __TIMER__
#define __TIMER__

#include <functional>

#include "copyability.hh"
#include "epoller.hh"
#include "i_object.hh"

class Epoller;
class Timer : noncopyable, public Object {
 public:
  explicit Timer(int second, std::function<void()> callback, Epoller *poller);
  explicit Timer(Epoller *poller);
  ~Timer() override;
  void operator()(int events) override;

  void go();
  int timerfd() const { return timefd_; }

  void setTimer(int second, std::function<void()> callback, bool go = true);

 private:
  void defaultCallBack() {}

 private:
  int timefd_;
  Epoller *poller_;
  struct itimerspec howlong_;
  std::function<void()> callback_;
};

#endif
