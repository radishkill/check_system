#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>

#include "global_arg.h"
#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"


void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->laser = new Laser("/dev/ttyUSB0");
  arg->camera = new CameraManager();
}

void SelfTest() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->laser->SendCloseCmd();
  Utils::MSleep(3000);
  arg->laser->SendOpenCmd();
  Utils::MSleep(3000);
  arg->camera->GetPic();
  arg->camera->CheckPic();
  arg->laser->SendCloseCmd();
}


int main() {
  InitSystem();
  return 0;
}
