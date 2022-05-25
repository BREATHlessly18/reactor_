#ifndef __INTERFACE_OBJECT__
#define __INTERFACE_OBJECT__

#include <sys/epoll.h>

#include <cassert>

// 前向声明class Epoller 以指针类型出现 在编译时只需计算指针大小
class Epoller;

/**
 * @brief 接口类 如果一个对象想被事件驱动就必须公有继承该类并实现
 *
 */
class Object {
 public:
  Object(Epoller *poller);

  virtual ~Object();

  virtual void operator()(int events) = 0;

  inline void enableRead() {
    events_ |= (EPOLLIN | EPOLLPRI);
    update();
  }

  inline void disableRead() {
    events_ &= ~(EPOLLIN | EPOLLPRI);
    update();
  }

  inline void enableWrite() {
    events_ |= EPOLLOUT;
    update();
  }

  inline void disableWrite() {
    events_ &= ~EPOLLOUT;
    update();
  }

  /**
   * @brief 将this作为回调，加入epoll
   *
   */
  void addToEpoll();

  void setFd(int fd) { fd_ = fd; }

  int fd() const { return fd_; }

  Epoller *poller() { return poller_; }

  int events() { return events_; }

 private:
  /**
   * @brief 更新关注的事件类型
   *
   */
  void update();

 private:
  // 事件循环对象，本类不可以控制其生命周期
  Epoller *poller_;

  // 关注的事件们
  int events_;

  // 自身fd
  int fd_;
};

#endif