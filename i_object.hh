#ifndef __INTERFACE_OBJECT__
#define __INTERFACE_OBJECT__

class Object {
 public:
  virtual ~Object() = default;
  virtual void operator()(int events) = 0;
};

#endif