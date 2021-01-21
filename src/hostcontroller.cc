#include "hostcontroller.h"

#include <ctime>
#include <thread>
#include <functional>

#include "mutils.h"
#include "global_arg.h"
#include "state_machine.h"
#include "led.h"

namespace check_system {

HostController::HostController(const char *device_file) {
  Open(device_file);
}

HostController::~HostController() {
  ::close(fd_ctl_gpio_);
}

void HostController::Open(const char *device_file) {
  usart_.Open(device_file, 115200, 8, 1, 'N', 0);

  std::string addr = std::string("/sys/class/gpio/gpio") + std::to_string(kRs485CtlGpio) + "/value";
  fd_ctl_gpio_ = open(addr.c_str(), O_RDWR);
  if (fd_ctl_gpio_ == -1) {
    perror("open gpio");
  }
  write(fd_ctl_gpio_, "0", 1);
}

int HostController::RecvData() {
  GlobalArg* arg = GlobalArg::GetInstance();
  char recved_data[3];
  int len = usart_.ReadData(recved_data, 3);
  if (len != 3 || recved_data[0] != (char)0xDD || recved_data[1] != (char)0x7E) {
    std::cout << "bad data" << std::endl;
    return -1;
  }
  Utils::ShowRawString(recved_data, 3);
  std::cout << std::endl;

  switch (recved_data[2]) {
    case 0x01: {
      //状态查询
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kSelfTest));
      th.detach();
      break;
    }
    case 0x02: {
      //握手确认
      HandConfirm();
      arg->hsk_flag = true;
      std::cout << "set hsk flag" << std::endl;
      break;
    }
    case 0x03: {
      //未定义
      break;
    }
    case 0x04: {
      //认证
      if (!arg->hsk_flag) {
        std::cout << "not set hsk flag" << std::endl;
        break;
      }
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kAuth));
      th.detach();
      break;
    }
    case 0x05: {
      //握手取消
      HandCancel();
      arg->hsk_flag = false;
      std::cout << "reset hsk flag" << std::endl;
      break;
    }
    case 0x06: {
      //复位
      arg->interrupt_flag = 1;
      ResetSuccess();
      std::cout << "set interrupt flag" << std::endl;
      break;
    }
    case 0x07: {
      //注册
      if (!arg->hsk_flag) {
        std::cout << "not set hsk flag" << std::endl;
        break;
      }
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kRegister));
      th.detach();
      break;
    }
  case 0x08:{
    //初始化
    std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kSystemInit));
    th.detach();
    break;
    }
    default: {
     std::cout << "bad data" << std::endl;
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
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 6 );
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  auto result_da = Utils::Crc16AndXmodem((unsigned char *)data , 12 );
  data[i++] = result_da.first%256;
  data[i++] = result_da.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
  return 0;
}

//认证反馈成功
int HostController::AuthSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x63;
  data[i++] = 0x04;
  data[i++] = 0x17;
  data[i++] = 0x49;
  data[i++] = 0x8F;
  data[i++] = 0x00;
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 8 );
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
  return 0;
}

//认证反馈失败
int HostController::AuthFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x63;
  data[i++] = 0x04;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  data[i++] = 0xFF;
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 8);
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  auto result_da = Utils::Crc16AndXmodem((unsigned char *)data , 12 );
  data[i++] = result_da.first%256;
  data[i++] = result_da.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 5 );
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 5 );
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 5);
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
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
  auto result = Utils::Crc16AndXmodem((unsigned char *)data , 5);
  data[i++] = result.first%256;
  data[i++] = result.first/256;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
  return 0;
}

//初始化完成
int HostController::InitializeSuccess(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x67;
  data[i++] = 0x01;
  data[i++] = 0x01;
  data[i++] = 0x5C;
  data[i++] = 0xCB;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
  return 0;
}

//初始化失败
int HostController::InitializeFail(){
  int i=0;
  data[i++] = 0xDD;
  data[i++] = 0x7E;
  data[i++] = 0x67;
  data[i++] = 0x01;
  data[i++] = 0xff;
  data[i++] = 0x8D;
  data[i++] = 0xC5;
  data[i] = '\0';
  write(fd_ctl_gpio_, "1", 1);
  Utils::MSleep(100);
  usart_.SendData(data,i);
  Utils::MSleep(100);
  write(fd_ctl_gpio_, "0", 1);
  return 0;
}
}
