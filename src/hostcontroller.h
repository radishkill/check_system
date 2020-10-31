#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#include "usart.h"

namespace check_system {

class HostController {
 public:
  HostController(const char* device_file);
  bool IsOpen() const {return usart.IsOpen();};
  void Open(const char* device_file);
  int CheckStatus();//状态查询
  int HandConfirm();//握手确认
  int Authentication();//认证
  int HandCancel();//握手取消
  int Reset();//复位
  int Register();//注册
 private:
  Usart usart;
  char data[300];
};

}

#endif // HOSTCONTROLLER_H
