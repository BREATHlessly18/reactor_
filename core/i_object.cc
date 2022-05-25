#include "i_object.hh"

#include <sys/socket.h>

#include <iostream>

#include "epoller.hh"

Object::Object(Epoller *poller) : poller_{poller}, events_{0}, fd_{-1} {
  assert(poller_ != nullptr);
}

/**
 * @brief 析构时取消对事件的关注 自动从epoll中移除 关闭连接
 *
 */
Object::~Object() {
  disableRead();
  disableWrite();

  if (fd_ != -1) {
    poller_->removeEvent(fd_, this);
    ::shutdown(fd_, SHUT_WR);
  }
}

void Object::addToEpoll() {
  // 首先关注读事件
  events_ = Epoller::EpollOpr::kReadEvent;
  poller_->addEvent(fd_, events_, this);
  std::cout << "add fd:" << fd() << std::endl;
}

void Object::update() { poller_->modifyEvent(fd_, events_, this); }