#ifndef __LOOPTHREAD_
#define __LOOPTHREAD_

#include <condition_variable>
#include <thread>

#include "copyability.hh"

// 前向声明class Epoller 以指针类型出现 在编译时只需计算指针大小
class Epoller;

/**
 * @brief 循环线程类型 用作从反应堆
 *
 */
class LoopThread : noncopyable {
 public:
  explicit LoopThread();

  ~LoopThread() = default;

  Epoller* internalLoop();

 private:
  void threadFunction();

 private:
  // Epoll的生命由我来掌握！
  std::unique_ptr<Epoller> internalLoop_;

  // 线程对象
  std::unique_ptr<std::thread> task_;
};

#endif