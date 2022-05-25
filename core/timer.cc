#include "epoller.hh"
#include "timer.hh"

#include <cassert>
#include <cstring>

Timer::Timer(Epoller *poller) : Object(poller), howlong_{}, callback_{} {
  Object::setFd(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC));
}

Timer::Timer(int second, std::function<void()> callback, Epoller *poller)
    : Object(poller), howlong_{}, callback_(callback) {
  Object::setFd(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC));

  ::memset(&howlong_, 0, sizeof(itimerspec));
  howlong_.it_value.tv_sec = second;
  ::timerfd_settime(Object::fd(), 0, &howlong_, nullptr);
}

Timer::~Timer() {
  // 析构时 会触发Object析构 从epoll取关
}

void Timer::setTimer(int second, std::function<void()> callback,
                     bool rightNow) {
  callback_ = callback;

  ::memset(&howlong_, 0, sizeof(itimerspec));
  howlong_.it_value.tv_sec = second;

  if (rightNow) {
    ::timerfd_settime(Object::fd(), 0, &howlong_, nullptr);
    go();
  }
}

void Timer::operator()(int events) {
  assert(events | ~EPOLLOUT);

  uint64_t buf;
  ::read(Object::fd(), &buf, sizeof(uint64_t));

  if (callback_) {
    std::cout << "Timer::operator():" << static_cast<void *>(this) << std::endl;

    callback_();
  }
}

void Timer::go() { Object::addToEpoll(); }
