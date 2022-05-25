/**
 * @file threadpool.cc
 * @author yangseng (you@domain.com)
 * @brief keep order: #include "epoller.hh"
 *                    #include "threadpool.hh"
 * @version 0.1
 * @date 2022-04-06
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "epoller.hh"
#include "loopthread.hh"
#include "threadpool.hh"

#include <cassert>
#include <iostream>


ThreadPool::ThreadPool(Epoller* _mainLoop, uint8_t _num)
    : mainLoop_{_mainLoop},
      threadsSize_{_num},
      threads_{},
      nextIdx_{0},
      mtx_{} {
  // 这里的线程池保存的是 从IO线程
  // 主线程监听和建立连接，并添加连接关注的读事件到 从IO线程
  std::cout << "Make thread num:" << (int)threadsSize_ << "\n";

  for (uint8_t i = 0; i < _num; ++i) {
    threads_.push_back(std::make_unique<LoopThread>());
  }
}

ThreadPool::~ThreadPool() {
  for (auto& thread : threads_) {
    thread->internalLoop()->quit();
  }
}

Epoller* ThreadPool::getOneLoop() {
  if (threadsSize_ == 0) {
    return mainLoop_;
  }

  std::lock_guard<std::mutex> lock(mtx_);

  if (nextIdx_ >= threadsSize_) {
    nextIdx_ -= threadsSize_;
  }

  std::cout << "Get thread index:" << (int)nextIdx_ << "\n";
  auto p = threads_[nextIdx_]->internalLoop();
  ++nextIdx_;
  return p;
}