#ifndef __CONNECTOR__
#define __CONNECTOR__

#include <memory>
#include <string>

#include "copyability.hh"
#include "epoller.hh"
#include "i_object.hh"
#include "server.hh"

class Server;

class Connector : noncopyable, public Object {
 public:
  explicit Connector(int fd, Epoller* poller, Server* server);
  ~Connector() override;
  void operator()(int events) override;

  int cltFd() const;
  void set_cltFd(int fd);

  void onMessage(int events);

  struct Message {
    unsigned char data[30];
  };

 private:
  const uint8_t bufferSz_ = 100;
  int cltFd_;
  std::unique_ptr<unsigned char[]> buffer_;
  Epoller* poller_;
  Server* server_;
};

#endif