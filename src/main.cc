#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

#include "global_arg.h"
#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"
#include "state_machine.h"
#include "key_file.h"
#include "lcd.h"

void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->laser = new Laser("/dev/ttyUSB0");
  arg->camera = new CameraManager();
  arg->key_file = new KeyFile("./resource/PUFData");
  arg->lcd = new Lcd("");
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
  arg->sm->AuthPic(nullptr, 0, 0, nullptr, 0, 0);
//  InitSystem();
//  arg->sm->Register();
  return 0;
}
