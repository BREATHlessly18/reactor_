/**
 * @file timer.hh
 * @author yangzheng (you@domain.com)
 * @brief 定时器类型
 * @version 0.1
 * @date 2022-04-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __TIMER__
#define __TIMER__

#include <sys/timerfd.h>

#include <functional>

#include "copyability.hh"
#include "epoller.hh"

// 前向声明class Epoller 以指针类型出现 在编译时只需计算指针大小
class Epoller;

class Timer : noncopyable, public Object {
 public:
  explicit Timer(int second, std::function<void()> callback, Epoller *poller);
  explicit Timer(Epoller *poller);
  ~Timer() override;
  void operator()(int events) override;

  void go();

  /**
   * @brief Set the Timer object
   *
   * @param second 秒
   * @param callback 可调用对象
   * @param now 是都立马触发
   */
  void setTimer(int second, std::function<void()> callback, bool now = true);

 private:
  /**
   * @brief 默认调用对象
   *
   */
  void defaultCallBack() {}

 private:
  // 时间间隔
  struct itimerspec howlong_;

  // 调用对象
  std::function<void()> callback_;
};

#endif
