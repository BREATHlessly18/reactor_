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

/**
 * @brief 事件循环类型
 *
 */
class Epoller : noncopyable {
 public:
  enum EpollOpr {
    kNoneEvent = 0,
    kReadEvent = EPOLLIN | EPOLLPRI,
    kWriteEvent = EPOLLOUT,
  };

  Epoller();

  /**
   * @brief 事件循环函数 消费事件
   *
   */
  void loop();
  void quit();

  /**
   * @brief 添加事件 修改事件 移除事件
   *
   * @param fd
   * @param operartion 添加/修改/移除
   * @param callback 回调上下文
   */
  void addEvent(int fd, int operartion, Object* callback);
  void modifyEvent(int fd, int operartion, Object* callback);
  void removeEvent(int fd, Object* callback);

  bool isIOThread() const;

  /**
   * @brief 跑一个定时器
   *
   * @tparam F 函数类型
   * @tparam Args 函数参数们的类型
   * @param second 秒
   * @param _func 函数
   * @param _args 函数参数们
   * @return 返回一个包含注入函数返回值类型的futrue
   */
  template <class F, class... Args>
  auto runAfter(int second, F&& _func, Args&&... _args)
      -> std::future<decltype(_func(_args...))> {
    using rtype = decltype(_func(_args...));

    auto task = std::make_shared<std::packaged_task<rtype()>>(
        std::bind(std::forward<F>(_func), std::forward<Args>(_args)...));

    std::future<rtype> res = task->get_future();

    // 这个函数有问题 如果前面的定时器没走完 会被后面的更新掉
    if (timer_) {
      timer_->setTimer(second, [task]() { (*task)(); });
    }

    return res;
  }

 private:
  // 超时时间
  const int kTimeoutMs_ = 1000;
  const pthread_t threadId_;

  // 退出标志
  bool quit_;

  // epoll文件描述符
  int efd_;

  // 定时器对象以组合方式含入 生命周期由Epoll决定
  std::unique_ptr<Timer> timer_;

  // 实际被唤醒的事件们
  std::vector<struct epoll_event> activeEvents_;
};

#endif
