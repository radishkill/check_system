#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

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
  arg->led->LcdLed(1);
  sleep(1);
  arg->led->LaserLed(1);
  arg->led->LcdLed(0);
  sleep(1);
  arg->led->LaserLed(0);
  //启动LED闪烁线程
//  arg->led->RunBlink();
  arg->em = new check_system::EventManager();

//  arg->laser = new Laser("/dev/ttyUSB1");
//  if(!arg->laser->IsOpen()) {
//    arg->led->laser_blink_ = 100;
//    arg->led->lcd_blink_ = 100;
//    arg->led->cmos_blink_ = 100;
//    arg->led->error_blink_ = 100;
//    return ;
//  }
//  arg->camera = new CameraManager();
//  if(arg->camera->is_open_flag_ == 0) {
//    arg->led->laser_blink_=100;
//    arg->led->lcd_blink_=100;
//    arg->led->cmos_blink_=100;
//    arg->led->error_blink_=100;
//    return ;
//  }
//  arg->lcd = new Lcd("/dev/fb");
//  arg->key_file = new KeyFile("./resource/PUFData");

//  arg->host = new HostController("");

//  for(int i = 0; i < 3; i++){
//    arg->led->CmosLed(0);
//    arg->led->LaserLed(0);
//    arg->led->LcdLed(0);
//    Utils::MSleep(250);
//    arg->led->CmosLed(1);
//    arg->led->LaserLed(1);
//    arg->led->LcdLed(1);
//    Utils::MSleep(250);
//  }

//  arg->sm->RunMachine(StateMachine::kSelfTest);

  //下面部分是用来打开button的
//  std::stringstream ss;
//  int fd;
//  ss << "/sys/class/gpio/gpio107/value";
//  fd = open(ss.str().c_str(), O_RDONLY);
//  arg->em->ListenFd(fd, EventManager::kEventPri, []() {
//    //处理正常流程应该要多线程
//    std::cout << "107 button" << std::endl;
//  });
//  ss.str("");

//  ss << "/sys/class/gpio/gpio107/value";
//  fd = open(ss.str().c_str(), O_RDONLY);
//  arg->em->ListenFd(fd, EventManager::kEventPri, []() {
//    //处理正常流程应该要多线程
//    std::cout << "171 button" << std::endl;
//  });
//  ss.str("");

//  ss << "/sys/class/gpio/gpio107/value";
//  fd = open(ss.str().c_str(), O_RDONLY);
//  arg->em->ListenFd(fd, EventManager::kEventPri, []() {
//    //处理正常流程应该要多线程
//    std::cout << "98 button" << std::endl;
//  });
//  ss.str("");

//  ss << "/sys/class/gpio/gpio107/value";
//  fd = open(ss.str().c_str(), O_RDONLY);
//  arg->em->ListenFd(fd, EventManager::kEventPri, []() {
//    //处理正常流程应该要多线程
//    std::cout << "165 button" << std::endl;
//  });
//  ss.str("");

//  arg->em->ListenFd(arg->host->GetFd(), EventManager::kEventRead, []() {
//    GlobalArg* arg = GlobalArg::GetInstance();
//    arg->host->RecvData();
//  });
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
  InitSystem();

//  arg->sm->Register();

  arg->em->Start(1);
//  arg->host = new HostController("fffff");
//  arg->host->CheckStatus();
  return 0;
}
