#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>

#include "global_arg.h"
#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"
#include "state_machine.h"
#include "key_file.h"

void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->laser = new Laser("/dev/ttyUSB0");
  arg->camera = new CameraManager();
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
  InitSystem();
  KeyFile* file_k = new KeyFile("./resource/PUFData");
//  file_k->GetSeed(0, 0);
//  file_k->GetPic(0, 0);
//  file_k->SavePic(0, 1);
//  file_k->SavePic(1, 1);

    file_k->AppendPufFile();
//  file_k->AppendSeed();
  return 0;
}
