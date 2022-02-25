#include "server.hh"

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>

Server::Server(uint16_t listenPort, const char* ip, Epoller* poller)
    : listenfd_{}, poller_(poller), sock_{}, cltList_{}, mtx_{} {
  assert(ip != nullptr);
  assert(poller_ != nullptr);

  sock_.sin_family = AF_INET;
  sock_.sin_port = ::htons(listenPort);
  sock_.sin_addr.s_addr = ::inet_addr(ip);

  listenfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  assert(listenfd_ != -1);

  int optval = 1;
  ::setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
               static_cast<socklen_t>(sizeof optval));

  int opt = ::fcntl(listenfd_, F_GETFL, 0);
  ::fcntl(listenfd_, F_SETFL, opt | O_NONBLOCK);

  assert(::bind(listenfd_, reinterpret_cast<sockaddr*>(std::addressof(sock_)),
                sizeof(sockaddr_in)) >= 0);

  assert(::listen(listenfd_, listenPort) >= 0);
}

Server::~Server() {
  if (!cltList_.empty()) {
    for (auto& clt : cltList_) {
      if (clt->cltFd() != -1) {
        ::close(clt->cltFd());
      }
    }
  }

  ::close(listenfd_);
}

void Server::start() { poller_->addEvent(listenfd_, EPOLLIN | EPOLLPRI, this); }

void Server::accept() {
  struct sockaddr_in clientInfo {};
  int size = sizeof clientInfo;

  // 从accept队列中pop一个建立好连接的fd出来
  int cltFd = ::accept(listenfd_, (sockaddr*)&clientInfo, (socklen_t*)&size);
  assert(-1 != cltFd);

  Connector* newConnector = new Connector(cltFd, poller_, this);
  addClt(newConnector);

  poller_->addEvent(cltFd, EPOLLIN | EPOLLPRI, newConnector);
}

int Server::listenfd() const { return listenfd_; }

void Server::addClt(Connector* clt) { cltList_.emplace_back(clt); }

void Server::removeClt(int cltfd) {
  for (auto it = cltList_.begin(); it != cltList_.end(); ++it) {
    ::close(cltfd);

    if (cltfd == (*it)->cltFd()) {
      auto clt = *it;
      delete clt;

      cltList_.erase(it);
      break;
    }
  }
}

void Server::operator()(int events) {
  if (events & (EPOLLIN | EPOLLPRI)) {
    std::cout << "register client\n";
    accept();
  }
}