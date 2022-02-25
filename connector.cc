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

Connector::Connector(int fd, Epoller* poller, Server* server)
    : cltFd_(fd),
      buffer_{new unsigned char[bufferSz_]{}},
      poller_(poller),
      server_(server) {
  assert(poller != nullptr);
  assert(server_ != nullptr);

  int opt = ::fcntl(cltFd_, F_GETFL, 0);
  ::fcntl(cltFd_, F_SETFL, opt | O_NONBLOCK);
}

Connector::~Connector() {}

int Connector::cltFd() const { return cltFd_; }

void Connector::set_cltFd(int fd) { cltFd_ = fd; }

void Connector::operator()(int events) {
  poller_->asyncRun([=]() { this->onMessage(events); });
  // onMessage(events);
}

void Connector::onMessage(int events) {
  if (events & (EPOLLIN | EPOLLPRI)) {
    ::memset(buffer_.get(), 0, bufferSz_);

    while (true) {
      int ret = ::read(cltFd_, buffer_.get(), sizeof(Message));

      if (ret < 0) {
        if (errno == EWOULDBLOCK or errno == EINTR) {
          // std::cout << "read again plz.\n";
          // continue;
          break;
        }
      }

      if (0 == ret) {
        assert(poller_ != nullptr);
        assert(server_ != nullptr);

        poller_->removeEvent(cltFd_, this);
        server_->removeClt(cltFd_);
        break;
      }

      static int loop = 0;
      std::cout << "loop" << ++loop << " recv msg:["
                << std::string((char*)buffer_.get(), ret - 1)
                << "] from:" << cltFd_ << std::endl;
      break;
    }
  }
}
