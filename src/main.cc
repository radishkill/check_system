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

using check_system::GlobalArg;
using check_system::Laser;
using check_system::CameraManager;
using check_system::KeyFile;
using check_system::Lcd;
using check_system::EventManager;
using check_system::StateMachine;
using check_system::LedController;

void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
//  arg->laser = new Laser("/dev/ttyUSB0");
//  arg->camera = new CameraManager();
//  arg->key_file = new KeyFile("./resource/PUFData");
//  arg->lcd = new Lcd("");
  arg->em = new check_system::EventManager();
  arg->led = new LedController();
  arg->led->RunBlink();

//  std::stringstream ss;
//  int fd;

//  ss << "/sys/class/gpio/gpio";
//  ss << "152";
//  ss << "/value";
//  fd = open(ss.str().c_str(), O_RDONLY);
//  arg->em->ListenFd(fd, EventManager::kEventPri, []() {
//    std::cout << "152 button" << std::endl;
//  });
}


int main() {
  GlobalArg
      * arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
//  arg->sm->AuthPic(nullptr, 0, 0, nullptr, 0, 0);
  InitSystem();

//  arg->sm->Register();

  arg->em->Start(1);
  return 0;
}
