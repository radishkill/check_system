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

  arg->laser = new Laser("/dev/ttyS0");
  if(!arg->laser->IsOpen()) {
    arg->led->laser_blink_ = 100;
    arg->led->lcd_blink_ = 100;
    arg->led->cmos_blink_ = 100;
    arg->led->error_blink_ = 100;
    return ;
  } else {
    std::cout << "laser connect ok!!" <<std::endl;
  }

  arg->camera = new CameraManager();
  if(arg->camera->is_open_flag_ == 0) {
//    arg->led->laser_blink_=100;
//    arg->led->lcd_blink_=100;
//    arg->led->cmos_blink_=100;
//    arg->led->error_blink_=100;
//    return ;
  } else {
    std::cout << "camera connect ok!!" << std::endl;
  }

  arg->lcd = new Lcd("/dev/fb0");
  if (!arg->lcd->IsOpen()) {
//    arg->led->laser_blink_=100;
//    arg->led->lcd_blink_=100;
//    arg->led->cmos_blink_=100;
//    arg->led->error_blink_=100;
//    return;
  } else {
    std::cout << "lcd buffer connect ok!!" << std::endl;
  }

  arg->host = new HostController("/dev/ttyS3");
  if (!arg->host->IsOpen()) {
//    arg->led->laser_blink_=100;
//    arg->led->lcd_blink_=100;
//    arg->led->cmos_blink_=100;
//    arg->led->error_blink_=100;
//    return;
  } else {
    std::cout << "host tty connect ok!!" << std::endl;
  }

  arg->key_file = new KeyFile("./resource/PUFData");
  if (!arg->key_file->Is_Open()) {
    std::cout << "key file have some wrong!!!" << std::endl;
  } else {
    std::cout << "key file ok" << std::endl;
  }


  for(int i = 0; i < 3; i++) {
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    Utils::MSleep(250);
    arg->led->CmosLed(1);
    arg->led->LaserLed(1);
    arg->led->LcdLed(1);
    Utils::MSleep(250);
  }

  //下面部分是用来打开button的

  //注册按钮
  std::stringstream ss;
  int fd;
  char key;
  ss << "/sys/class/gpio/gpio107/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 107");
    return;
  }

  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 107 " << key << std::endl;
    if (key == 0x31) {
      std::thread th([]() {
        GlobalArg* arg = GlobalArg::GetInstance();
        arg->sm->RunMachine(StateMachine::kRegister);
      });
      th.detach();
    }
  });
  ss.str("");

  //认证
  ss << "/sys/class/gpio/gpio171/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 171");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {

    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 171 " << key << std::endl;
    if (key == 0x31) {
      std::thread th([]() {
        GlobalArg* arg = GlobalArg::GetInstance();
        arg->sm->RunMachine(StateMachine::kAuth);
      });
      th.detach();
    }
  });
  ss.str("");

  //中断按钮
  ss << "/sys/class/gpio/gpio98/value";
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
  ss << "/sys/class/gpio/gpio165/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    perror("open gpio 165");
    return;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {

    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button 165 " << key << std::endl;
    if (key == 0x31) {
      std::thread th([]() {
        GlobalArg* arg = GlobalArg::GetInstance();
        arg->sm->RunMachine(StateMachine::kSelfTest);
      });
      th.detach();
    }
  });
  ss.str("");

  arg->em->ListenFd(arg->host->GetFd(), EventManager::kEventRead, []() {
    GlobalArg* arg = GlobalArg::GetInstance();
    arg->host->RecvData();
  });

//  std::thread th([&]() {
//    arg->sm->RunMachine(StateMachine::kSelfTest);
//  });
//  th.detach();
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->key_file = new KeyFile("./resource/PUFData");
  arg->sm = new StateMachine();
  InitSystem();
  arg->em->Start(1);
  return 0;
}
