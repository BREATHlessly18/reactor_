#include "loopthread.hh"

#include <iostream>

#include "epoller.hh"

LoopThread::LoopThread()
    : internalLoop_{new Epoller},
      task_{new std::thread(&LoopThread::threadFunction, this)} {}

Epoller* LoopThread::internalLoop() { return internalLoop_.get(); }

void LoopThread::threadFunction() { internalLoop_->loop(); }