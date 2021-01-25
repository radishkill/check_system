#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#include "usart.h"
#include "constant.h"

namespace check_system {



class HostController {
 public:
  HostController(const char* device_file);
  ~HostController();
  bool IsOpen() const {return usart_.IsOpen()&&(fd_ctl_gpio_!=-1);}
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
  int InitializeSuccess(); //初始化完成
  int InitializeFail();  // 初始化失败
  int SetCtl(int s);
 private:
  int fd_ctl_gpio_;
  Usart usart_;
  char data[1024];
};

}

#endif // HOSTCONTROLLER_H
