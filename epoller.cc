// epoller.cc
//  epoller.cc
#include "epoller.hh"

#include <arpa/inet.h>
#include <assert.h>

#include <cstring>
#include <functional>
#include <iostream>

Epoller::Epoller() : quit_(false), efd_{-1}, timerTie_{}, activeEvents_(10) {
  efd_ = ::epoll_create(10);
  assert(efd_ >= 0);

  timerTie_.reset(new Timer(this));
  std::cout << "timerTie_:" << static_cast<void*>(timerTie_.get()) << std::endl;
}

void Epoller::addEvent(int fd, int operartion, Object* callback) {
  assert(callback != nullptr);

  struct epoll_event ev;
  ::memset(&ev, 0, sizeof(struct epoll_event));
  ev.events = operartion;
  ev.data.fd = fd;
  ev.data.ptr = callback;

  assert(::epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev) >= 0);
  std::cout << "add clt:" << fd << " to event_loop." << std::endl;
}

void Epoller::removeEvent(int fd, Object* callback) {
  // 从红黑树上摘下
  struct epoll_event event;
  ::memset(&event, 0, sizeof event);

  assert(::epoll_ctl(efd_, EPOLL_CTL_DEL, fd, &event) >= 0);
  std::cout << "delete clt:" << fd << " callback:" << event.data.ptr
            << " from event_loop." << std::endl;
}

void Epoller::loop() {
  while (!quit_) {
    int ready = ::epoll_wait(efd_, &*activeEvents_.begin(),
                             activeEvents_.size(), kTimeoutMs_);

    if (1 <= ready) {
      for (size_t i = 0; i != ready; ++i) {
        if (nullptr != activeEvents_[i].data.ptr) {
          auto cb = static_cast<Object*>(activeEvents_[i].data.ptr);
          (*cb)(activeEvents_[i].events);
        }
      }

      if (ready == activeEvents_.size()) {
        activeEvents_.resize(activeEvents_.size() * 2);
      }
    }
  }
}

void Epoller::quit() { quit_ = true; }
