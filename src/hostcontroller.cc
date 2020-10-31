#include "hostcontroller.h"





check_system::HostController::HostController(const char *device_file) {
  Open(device_file);
}

bool check_system::HostController::IsOpen() const {
}

void check_system::HostController::Open(const char *device_file) {
  usart.Open(device_file, 115200, 8, 1, 0, 0);
}
//状态查询反馈
int check_system::HostController::CheckStatus(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x60;
  data[i++] = 0x02;
  data[i++] = 0x01;
  data[i++] = 0x00;
  data[i++] = 0x5A;
  data[i++] = 0x3C;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//握手确认反馈
int check_system::HostController::HandConfirm(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0xDA;
//?????
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;

}

//认证反馈成功
int check_system::HostController::AuthenticationSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x63;
  data[i++] = 0x04;
  data[i++] = 0x17;
  data[i++] = 0x49;
  data[i++] = 0x8F;
  data[i++] = 0x00;
  data[i++] = 0x86;
  data[i++] = 0x0C;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//认证反馈失败
int check_system::HostController::AuthenticationFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x63;
  data[i++] = 0x04;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  data[i++] = 0x59;
  data[i++] = 0x57;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//握手取消反馈
int check_system::HostController::HandCancel(){

}

//复位反馈成功
int check_system::HostController::ResetSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x65;
  data[i++] = 0x01;
  data[i++] = 0x01;
  data[i++] = 0x3C;
  data[i++] = 0xA5;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//复位反馈失败
int check_system::HostController::ResetFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x65;
  data[i++] = 0x01;
  data[i++] = 0xFF;
  data[i++] = 0xED;
  data[i++] = 0xAB;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//注册反馈成功
int check_system::HostController::RegisterSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x66;
  data[i++] = 0x01;
  data[i++] = 0x01;
  data[i++] = 0x6C;
  data[i++] = 0xFC;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}

//注册反馈失败
int check_system::HostController::RegisterFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x66;
  data[i++] = 0x01;
  data[i++] = 0xFF;
  data[i++] = 0xBD;
  data[i++] = 0xF2;
  data[i] = '\0';
  usart.SendData(data,i);
  return 0;
}
