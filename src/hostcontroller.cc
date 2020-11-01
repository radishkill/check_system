#include<ctime>

#include "hostcontroller.h"
#include "utils.h"

namespace check_system {

HostController::HostController(const char *device_file) {
  Open(device_file);
}

void HostController::Open(const char *device_file) {
  usart_.Open(device_file, 115200, 8, 1, 'N', 0);
}

int HostController::RecvData() {
  char recved_data[3];
  int len = usart_.ReadData(recved_data, 3);
  if (len != 3 || recved_data[0] != (char)0xDD || recved_data[1] != (char)0x7E) {
    perror("bad data");
    return -1;
  }
  switch (recved_data[2]) {
    case 0x01: {
      //状态查询
      CheckStatus();
      break;
    }
    case 0x02: {
      //握手确认
      break;
    }
    case 0x03: {

      break;
    }
    case 0x04: {
      //认证

      break;
    }
    case 0x05: {
      //握手取消

      break;
    }
    case 0x06: {
      //复位
      break;
    }
    case 0x07: {
      //注册
      break;
    }
    default: {
      perror("bad data");
      return -1;
    }
  }
  return 0;
}
//状态查询反馈
int HostController::CheckStatus() {
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
  usart_.SendData(data,i);
  return 0;
}

//握手确认反馈
int HostController::HandConfirm(){
  int i=0;
  data[i++] = (char) 0xDD;
  data[i++] = (char) 0x7E;
  data[i++] = 0x61;
  data[i++] = 0x0A;
  std::srand(std::time(nullptr));
  for(int p = 0 ; p < 8; p++){
  data[i++]=std::rand()%256;
  }
  auto result = Utils::Crc16AndXmodem((unsigned char *)data + 4 , 8 );
  data[i++] = result.first/256;
  data[i++] = result.first%256;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}

//认证反馈成功
int HostController::AuthenticationSuccess(){
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
  usart_.SendData(data,i);
  return 0;
}

//认证反馈失败
int HostController::AuthenticationFail(){
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
  usart_.SendData(data,i);
  return 0;
}

//握手取消反馈
int HostController::HandCancel(){
  int i=0;
  data[i++] = (char) 0xDD;
  data[i++] = (char) 0x7E;
  data[i++] = 0x64;
  data[i++] = 0x0A;
  std::srand(std::time(nullptr));
  for(int p = 0 ; p < 8; p++){
  data[i++]=std::rand()%256;
  }
  auto result = Utils::Crc16AndXmodem((unsigned char *)data + 4 , 8 );
  data[i++] = result.first/256;
  data[i++] = result.first%256;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}

//复位反馈成功
int HostController::ResetSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x65;
  data[i++] = 0x01;
  data[i++] = 0x01;
  data[i++] = 0x3C;
  data[i++] = 0xA5;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}

//复位反馈失败
int HostController::ResetFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x65;
  data[i++] = 0x01;
  data[i++] = 0xFF;
  data[i++] = 0xED;
  data[i++] = 0xAB;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}

//注册反馈成功
int HostController::RegisterSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x66;
  data[i++] = 0x01;
  data[i++] = 0x01;
  data[i++] = 0x6C;
  data[i++] = 0xFC;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}

//注册反馈失败
int HostController::RegisterFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x66;
  data[i++] = 0x01;
  data[i++] = 0xFF;
  data[i++] = 0xBD;
  data[i++] = 0xF2;
  data[i] = '\0';
  usart_.SendData(data,i);
  return 0;
}
}
