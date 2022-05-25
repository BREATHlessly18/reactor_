#ifndef __SERVER__
#define __SERVER__

#include <arpa/inet.h>  //sockaddr_in

#include <functional>
#include <mutex>
#include <vector>

#include "connector.hh"
#include "copyability.hh"
#include "epoller.hh"
#include "loopthread.hh"
#include "threadpool.hh"

// 前向声明class Connector 以指针类型出现
class Connector;

/**
 * @brief TCP服务类型，用于接收客户端连接
 *
 */
class Server : noncopyable, public Object {
 public:
  explicit Server(uint16_t listenPort, const char* ip, Epoller* poller);
  ~Server() override;
  void start();

  void operator()(int events) override;

  void accept();

  void addClt(std::shared_ptr<Connector> conn);

  void removeClt(int cltfd);

  int listenfd() const;

  void setConnCb(OnConnectionCb cb) { newConnCb_ = cb; }

  void setDisconnCb(OnDisconnectionCb cb) { disConnCb_ = cb; }

  void setMessageCallback(OnMessageCb cb) { msgCb_ = cb; }

  void setWriteCompleteCb(OnWriteCompleteCb cb) { writeCompleteCb_ = cb; }

  // 使能线程池功能
  void enableThreadPool(uint8_t num = std::thread::hardware_concurrency());

  using ClientList = std::vector<std::shared_ptr<Connector>>;

 private:
  sockaddr_in sock_;
  std::mutex mtx_;
  ClientList connections_;
  std::unique_ptr<ThreadPool> threadpool_;
  OnConnectionCb newConnCb_;
  OnDisconnectionCb disConnCb_;
  OnMessageCb msgCb_;
  OnWriteCompleteCb writeCompleteCb_;
};

#endif