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
  int AuthenticationSuccess();//认证
  int AuthenticationFail();
  int HandCancel();//握手取消
  int ResetSuccess();//复位
  int ResetFail();
  int RegisterSuccess();//注册
  int RegisterFail();
 private:
  Usart usart;
  char data[300];
};

}

#endif // HOSTCONTROLLER_H
