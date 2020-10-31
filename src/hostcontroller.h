#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#include "usart.h"

namespace check_system {

class HostController {
 public:
  HostController(const char* device_file);
  bool IsOpen() const;
  void Open(const char* device_file);
 private:
  Usart usart;
};

}

#endif // HOSTCONTROLLER_H
