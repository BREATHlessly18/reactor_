#include "epoller.hh"

#include <arpa/inet.h>
#include <assert.h>

#include <cstring>
#include <iostream>

#include "timer.hh"

Epoller::Epoller()
    : threadId_(::pthread_self()),
      quit_(false),
      efd_{-1},
      timer_{},
      activeEvents_(10) {
  // 创建efd
  efd_ = ::epoll_create(10);
  assert(efd_ >= 0);

  timer_.reset(new Timer(this));
}

void Epoller::addEvent(int fd, int operartion, Object* callback) {
  assert(callback != nullptr);

  struct epoll_event ev;
  ::memset(&ev, 0, sizeof(struct epoll_event));
  ev.events = operartion;
  ev.data.fd = fd;
  ev.data.ptr = callback;

  assert(::epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev) >= 0);
  std::cout << "add clt:" << fd << " callback:" << ev.data.ptr
            << " to event_loop." << std::endl;
}

void Epoller::modifyEvent(int fd, int operartion, Object* callback) {
  assert(callback != nullptr);

  struct epoll_event ev;
  ::memset(&ev, 0, sizeof(struct epoll_event));
  ev.events = operartion;
  ev.data.fd = fd;
  ev.data.ptr = callback;

  assert(::epoll_ctl(efd_, EPOLL_CTL_MOD, fd, &ev) >= 0);
  std::cout << "mod clt:" << fd << " callback:" << ev.data.ptr
            << " to event_loop." << std::endl;
}

void Epoller::removeEvent(int fd, Object* callback) {
  // 从红黑树上摘下
  struct epoll_event event;
  ::memset(&event, 0, sizeof event);

  assert(::epoll_ctl(efd_, EPOLL_CTL_DEL, fd, &event) >= 0);
  std::cout << "del clt:" << fd << " callback:" << event.data.ptr
            << " from event_loop." << std::endl;
}

bool Epoller::isIOThread() const { return threadId_ == ::pthread_self(); }

void Epoller::loop() {
  while (!quit_) {
    // 阻塞等待 1s超时退出阻塞
    int ready = ::epoll_wait(efd_, &*activeEvents_.begin(),
                             activeEvents_.size(), kTimeoutMs_);
    std::cout << "wait....ready:" << ready << std::endl;

    if (1 <= ready) {
      for (size_t i = 0; i != ready; ++i) {
        if (nullptr != activeEvents_[i].data.ptr) {
          // 1. 将获取回调对象转型为抽象基类类型Object*
          auto cb = static_cast<Object*>(activeEvents_[i].data.ptr);

          // 2. 调用每个子类重写的仿函数operate()
          // 注意: 这里拿到的是实际对象 如果对象在上面这一行时尚未从epoll中取关
          // 在下面这一行前 在其他线程被销毁 那么下一行函数会导致崩溃
          // 因此注意多线程同步问题
          (*cb)(activeEvents_[i].events);
        }
      }

      // 如果一次性获取满了 那么就扩容一倍 迎接后续的并发
      if (ready == activeEvents_.size()) {
        activeEvents_.resize(activeEvents_.size() * 2);
      }
    }
  }
}

void Epoller::quit() { quit_ = true; }
