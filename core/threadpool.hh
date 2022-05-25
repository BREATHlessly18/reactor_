#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

#include "copyability.hh"
#include "loopthread.hh"

// 前向声明class Epoller class LoopThread 以指针类型出现
// 在编译时只需计算指针大小
class Epoller;
class LoopThread;

/**
 * @brief 从反应堆线程池类
 *
 */
class ThreadPool : noncopyable {
 public:
  /**
   * @brief Construct a new Thread Pool object
   *
   * @param _mainLoop 主反应堆对象引用 如果不创建从反应堆 就会使用主反应堆
   * @param _num 从反应堆个数
   */
  explicit ThreadPool(Epoller* _mainLoop, uint8_t _num = 0);
  ~ThreadPool();

  /**
   * @brief 获取其中一个循环线程对象内的从反应堆引用
   *
   * @return Epoller*
   */
  Epoller* getOneLoop();

 private:
  Epoller* mainLoop_;
  const uint8_t threadsSize_;
  std::vector<std::unique_ptr<LoopThread>> threads_;
  uint8_t nextIdx_;
  std::mutex mtx_;
};
