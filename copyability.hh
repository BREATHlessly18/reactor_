#ifndef __COPYABILITY__
#define __COPYABILITY__

class copyable {
 protected:
  copyable() = default;
  ~copyable() = default;
};

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  void operator=(const noncopyable &) = delete;
  noncopyable(noncopyable &&) noexcept = delete;
  void operator=(noncopyable &&) noexcept = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

#endif