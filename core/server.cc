#include "server.hh"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

#include "connector.hh"
#include "threadpool.hh"

Server::Server(uint16_t listenPort, const char* ip, Epoller* poller)
    : Object(poller),
      sock_{},
      mtx_{},
      connections_{},
      threadpool_{},
      newConnCb_{},
      disConnCb_{},
      msgCb_{},
      writeCompleteCb_{} {
  assert(ip != nullptr);

  sock_.sin_family = AF_INET;
  sock_.sin_port = ::htons(listenPort);
  sock_.sin_addr.s_addr = ::inet_addr(ip);

  auto listenfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  assert(listenfd_ != -1);

  int optval = 1;
  ::setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
               static_cast<socklen_t>(sizeof optval));

  int opt = ::fcntl(listenfd_, F_GETFL, 0);
  ::fcntl(listenfd_, F_SETFL, opt | O_NONBLOCK);

  assert(::bind(listenfd_, reinterpret_cast<sockaddr*>(std::addressof(sock_)),
                sizeof(sockaddr_in)) >= 0);

  assert(::listen(listenfd_, listenPort) >= 0);

  Object::setFd(listenfd_);

  enableThreadPool(2);
}

Server::~Server() {
  // 析构时 会触发Object析构 从epoll取关
}

void Server::enableThreadPool(uint8_t num) {
  threadpool_.reset(new ThreadPool(Object::poller(), num));
}

void Server::start() { Object::addToEpoll(); }

void Server::accept() {
  struct sockaddr_in clientInfo {};
  int size = sizeof clientInfo;

  // 从accept队列中pop一个建立好连接的fd出来
  int cltFd = ::accept(Object::fd(), (sockaddr*)&clientInfo, (socklen_t*)&size);
  assert(-1 != cltFd);

  std::cout << "Client port:" << clientInfo.sin_port << std::endl;

  // 从线程池中拿一个循环对象的事件循环引用出来创建连接对象
  std::shared_ptr<Connector> conn(
      new Connector(cltFd, threadpool_->getOneLoop(), this));

  // 设置业务回调
  if (msgCb_ and (newConnCb_ or disConnCb_ or writeCompleteCb_)) {
    conn->setMessageCallback(msgCb_)
        .setConnCb(newConnCb_)
        .setDisconnCb(disConnCb_)
        .setWriteCompleteCb(writeCompleteCb_);
  }

  conn->work();
}

int Server::listenfd() const { return Object::fd(); }

void Server::addClt(std::shared_ptr<Connector> conn) {
  connections_.push_back(conn);
}

void Server::removeClt(int cltfd) {
  for (auto it = connections_.begin(); it != connections_.end(); ++it) {
    if (cltfd == (*it)->fd()) {
      connections_.erase(it);
      break;
    }
  }
}

void Server::operator()(int events) {
  if (events & (EPOLLIN | EPOLLPRI)) {
    std::cout << "Register client\n";
    accept();
  }
}