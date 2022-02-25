#include "timer.hh"

#include <assert.h>
#include <sys/timerfd.h>

#include <cstring>

Timer::Timer(Epoller *poller)
    : timefd_{::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)},
      poller_(poller),
      howlong_{},
      callback_{} {}

Timer::Timer(int second, std::function<void()> callback, Epoller *poller)
    : timefd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      poller_(poller),
      howlong_{},
      callback_(callback) {
  ::memset(&howlong_, 0, sizeof(itimerspec));
  howlong_.it_value.tv_sec = second;
  ::timerfd_settime(timefd_, 0, &howlong_, nullptr);
}

Timer::~Timer() {
  if (timefd_ > 0) {
    ::close(timefd_);
  }
}

void Timer::setTimer(int second, std::function<void()> callback,
                     bool rightNow) {
  callback_ = callback;

  ::memset(&howlong_, 0, sizeof(itimerspec));
  howlong_.it_value.tv_sec = second;

  if (rightNow) {
    ::timerfd_settime(timefd_, 0, &howlong_, nullptr);
    go();
  }
}

void Timer::operator()(int events) {
  assert(events | ~EPOLLOUT);

  uint64_t buf;
  ::read(timerfd(), &buf, sizeof(uint64_t));

  if (callback_) {
    std::cout << "Timer::operator():" << static_cast<void *>(this) << std::endl;

    callback_();
  }
}

void Timer::go() { poller_->addEvent(timerfd(), EPOLLIN | EPOLLOUT, this); }
