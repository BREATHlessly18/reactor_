#ifndef __CONNECTOR__
#define __CONNECTOR__

#include <vector>

#include "copyability.hh"
#include "epoller.hh"

// 前向声明class Epoller class Connector 以指针类型出现 在编译时只需计算指针大小
class Server;
class Connector;

// 声明回调函数对象类型
// 1. 处理消息回调
using OnMessageCb =
    std::function<void(std::shared_ptr<Connector> conn, uint8_t*, size_t sz)>;

// 2. 连接成功回调
using OnConnectionCb = std::function<void()>;

// 3. 断连回调
using OnDisconnectionCb = std::function<void()>;

// 4. 写完时回调
using OnWriteCompleteCb = std::function<void()>;

/**
 * @brief 连接对象，代表一个TCP连接，
 * 公有继承Object代表它可以被epoll关注，
 * 共有继承std::enable_shared_from_this
 * 表明该类型创建在堆上，并由shared_ptr接管生命周期
 */
class Connector : noncopyable,
                  public Object,
                  public std::enable_shared_from_this<Connector> {
 public:
  explicit Connector(int fd, Epoller* poller, Server* server);

  ~Connector() override;

  // 重写void Object::operator()(int events)
  void operator()(int events) override;

  void work();

  void send(const std::string& buf);

  void send(const void* buf, int len);

  /**
   * @brief 连接读事件处理函数
   *
   * @param events
   */
  void onMessage(int events);

  Connector& setMessageCallback(OnMessageCb cb) {
    msgCb_ = cb;
    return *this;
  }

  Connector& setConnCb(OnConnectionCb cb) {
    newConnCb_ = cb;
    return *this;
  }

  Connector& setDisconnCb(OnDisconnectionCb cb) {
    disConnCb_ = cb;
    return *this;
  }

  Connector& setWriteCompleteCb(OnWriteCompleteCb cb) {
    writeCompleteCb_ = cb;
    return *this;
  }

  struct Message {
    unsigned char data[30];
  };

 private:
  const uint16_t kBufferSz_ = 1000;
  Server* server_;
  std::vector<uint8_t> inputBuffer_;
  std::vector<uint8_t> outputBuffer_;
  OnConnectionCb newConnCb_;
  OnDisconnectionCb disConnCb_;
  OnMessageCb msgCb_;
  OnWriteCompleteCb writeCompleteCb_;
};

#endif