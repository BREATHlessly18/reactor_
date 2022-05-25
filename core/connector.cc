#include "connector.hh"

#include <arpa/inet.h>  //sockaddr_in
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include "server.hh"

Connector::Connector(int fd, Epoller* poller, Server* server)
    : Object(poller),
      server_(server),
      inputBuffer_(kBufferSz_),
      outputBuffer_(kBufferSz_),
      newConnCb_{},
      disConnCb_{},
      msgCb_{},
      writeCompleteCb_{} {
  assert(poller != nullptr);
  assert(server_ != nullptr);

  int opt = ::fcntl(fd, F_GETFL, 0);
  ::fcntl(fd, F_SETFL, opt | O_NONBLOCK);

  // 构造时将fd保存如基类 并且加入epoll关注
  Object::setFd(fd);
  Object::addToEpoll();
}

Connector::~Connector() {
  // 析构时 会触发Object析构 从epoll取关
}

void Connector::operator()(int events) {
  // 多线程下 connector对象会被复制，一旦遇到read
  // 0触发对象析构，其余线程就会崩溃
  onMessage(events);
}

void Connector::work() {
  // 为了服务器类能死如泉涌的开发 这里自动添加自己到服务器中进行管理
  server_->addClt(shared_from_this());
}

void Connector::onMessage(int events) {
  if (events & (EPOLLIN | EPOLLPRI)) {
    inputBuffer_.clear();

    while (true) {
      int ret = ::read(Object::fd(), inputBuffer_.data(), kBufferSz_);

      if (ret < 0) {
        if (errno == EWOULDBLOCK or errno == EINTR) {
          break;
        }
      }

      if (0 == ret) {
        assert(Object::poller() != nullptr);
        assert(server_ != nullptr);

        // FIXME: it kills itself?
        // FIXME: have to run in one of loop threads
        // to ensure object be destroied by one loop

        // 为了服务器类能死如泉涌的开发 这里自动移除将自己从服务器中移除
        // 当shared_ptr减到1时，析构自身，又会触发从epoll中取关的操作
        // 很棒吧 上面FIXME也是我写的  这里对象析构不会引起本函数行为的错误
        // 但是貌似也不太好
        server_->removeClt(Object::fd());
        break;
      }

      static int loop = 0;
      std::cout << "loop" << ++loop << " recv msg:["
                << std::string((char*)inputBuffer_.data(), ret - 1)
                << "] from:" << Object::fd() << std::endl;

      if (msgCb_) {
        // 把读到的东西通知到上层业务
        msgCb_(shared_from_this(), inputBuffer_.data(), ret);
      }
      break;
    }
  }
}

void Connector::send(const void* buf, int len) {
  size_t remaining = len;

  // 在没有关注写事件时直接write
  const auto nwrote = ::write(Object::fd(), buf, len);
  if (nwrote >= 0) {
    remaining = len - nwrote;
    if (remaining == 0) {
      std::cout << "write compeleted.\n";
    }

    // else 还有剩余，稍后处理
  } else {
    // nwrote < 0
    assert(errno & EWOULDBLOCK);
  }

  assert(remaining <= len);
  // 处理剩余部分 一定要用缓冲区先暂存下来 这里数据结构类型没选好
  // 感觉要自己把数组再封装一层
  //  std::copy((unsigned char)((unsigned char)buf + nwrote),
  //           (unsigned char)((unsigned char)buf + len), outputBuf_.rbegin());
  Object::poller()->addEvent(Object::fd(), Object::events() | EPOLLOUT, this);
}

void Connector::send(const std::string& buf) { send(buf.data(), buf.size()); }
