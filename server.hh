#ifndef __SERVER__
#define __SERVER__

#include <arpa/inet.h>  //sockaddr_in

#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "connector.hh"
#include "copyability.hh"
#include "epoller.hh"
#include "i_object.hh"

class Connector;

class Server : noncopyable, public Object {
 public:
  explicit Server(uint16_t listenPort, const char* ip, Epoller* poller);
  ~Server() override;
  void start();

  void operator()(int events) override;

  void accept();
  int listenfd() const;
  Epoller* poller() { return poller_; }
  void addClt(Connector* clt);
  void addClt(int cltfd) = delete;
  void removeClt(int cltfd);

 private:
  int listenfd_;
  Epoller* poller_;
  sockaddr_in sock_;
  using ClientList = std::vector<Connector*>;
  ClientList cltList_;
  std::mutex mtx_;
};

#endif