#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>

#include "global_arg.h"
#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"
#include "state_machine.h"
#include "key_file.h"
#include "lcd.h"
#include "eventmanager.h"
#include "led.h"
#include "hostcontroller.h"
#include "constant.h"

using check_system::GlobalArg;
using check_system::Laser;
using check_system::CameraManager;
using check_system::KeyFile;
using check_system::Lcd;
using check_system::EventManager;
using check_system::StateMachine;
using check_system::LedController;
using check_system::HostController;

void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->led = new LedController();
  //启动LED闪烁线程
  arg->led->RunBlink();
  arg->em = new check_system::EventManager();

  arg->laser = new Laser(check_system::kLaserAddr);
  if(!arg->laser->IsOpen()) {
    arg->led->laser_blink_ = 100;
    arg->led->lcd_blink_ = 100;
    arg->led->cmos_blink_ = 100;
    arg->led->error_blink_ = 100;
    std::cout << "laser connect error!!" <<std::endl;
    return ;
  } else {
    std::cout << "laser connect ok!!" <<std::endl;
  }

  arg->camera = new CameraManager();
  if(arg->camera->is_open_flag_ == 0) {
    arg->led->laser_blink_=100;
    arg->led->lcd_blink_=100;
    arg->led->cmos_blink_=100;
    arg->led->error_blink_=100;
    std::cout << "camera connect error!!" << std::endl;
    return ;
  } else {
    std::cout << "camera connect ok!!" << std::endl;
  }

  arg->lcd = new Lcd("/dev/fb0");
  if (!arg->lcd->IsOpen()) {
    arg->led->laser_blink_=200;
    arg->led->lcd_blink_=200;
    arg->led->cmos_blink_=200;
    arg->led->error_blink_=200;
    std::cout << "lcd buffer connect error!!" << std::endl;
    return;
  } else {
    std::cout << "lcd buffer connect ok!!" << std::endl;
  }

  arg->host = new HostController(check_system::kHostAddr);
  if (!arg->host->IsOpen()) {
    arg->led->laser_blink_ = 200;
    arg->led->lcd_blink_ = 200;
    arg->led->cmos_blink_ = 200;
    arg->led->error_blink_ = 200;
    std::cout << "host tty connect error!!" << std::endl;
    return;
  } else {
    std::cout << "host tty connect ok!!" << std::endl;
  }

  arg->key_file = new KeyFile("./resource/PUFData");
  if (!arg->key_file->Is_Open()) {
    std::cout << "key file have some wrong!!!" << std::endl;
  } else {
    std::cout << "key file ok" << std::endl;
  }

  //led闪烁提示
  for(int i = 0; i < 3; i++) {
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    Utils::MSleep(500);
    arg->led->CmosLed(1);
    arg->led->LaserLed(1);
    arg->led->LcdLed(1);
    Utils::MSleep(500);
  }

  //下面部分是用来打开button的

  //注册按钮
  std::stringstream ss;
  int fd;
  char key;
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kRegisterButtonNumber)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 107");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 107 " << key << std::endl;
    if (key == 0x31) {
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kRegister));
      th.detach();
    }
  });
  ss.str("");

  //认证
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kAuthButtonNumber)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 171");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 171 " << key << std::endl;
    if (key == 0x31) {
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kAuth));
      th.detach();
    }
  });
  ss.str("");

  //中断按钮
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kInterruptButtonNumber)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 98");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 98 " << key << std::endl;
    if (key == 0x31) {
      arg->interrupt_flag = 1;
    }
  });
  ss.str("");

  //自检
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kCheckSelfButtonNumber)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 165");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 165 " << key << std::endl;
    if (key == 0x31) {
      std::thread th(std::bind(&StateMachine::RunMachine, arg->sm, StateMachine::kSelfTest));
      th.detach();
    }
  });
  ss.str("");

  arg->em->ListenFd(arg->host->GetFd(), EventManager::kEventRead, []() {
    GlobalArg* arg = GlobalArg::GetInstance();
    std::cout << "recv data " << std::endl;
    arg->host->RecvData();
  });


//  std::thread th([&]() {
//    arg->sm->RunMachine(StateMachine::kSelfTest);
//  });
//  th.detach();
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
  InitSystem();
  arg->em->Start(1);
  pause();
  return 0;
}
