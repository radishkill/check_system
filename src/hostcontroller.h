#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#include "usart.h"

namespace check_system {

class HostController {
 public:
  HostController(const char* device_file);
  bool IsOpen() const {return usart_.IsOpen();}
  int GetFd() const {return usart_.GetFd();}
  void Open(const char* device_file);
  int RecvData();
  int CheckStatus();//状态查询
  int HandConfirm();//握手确认
  int AuthSuccess();//认证
  int AuthFail();
  int HandCancel();//握手取消
  int ResetSuccess();//复位
  int ResetFail();
  int RegisterSuccess();//注册
  int RegisterFail();
 private:
  Usart usart_;
  char data[1024];
};

}

#endif // HOSTCONTROLLER_H
