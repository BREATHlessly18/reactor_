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

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

class nonmovable {
 public:
  nonmovable(nonmovable &&) noexcept = delete;
  void operator=(nonmovable &&) noexcept = delete;

 protected:
  nonmovable() = default;
  ~nonmovable() = default;
};

#endif