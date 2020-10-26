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
  InitSystem();

//  arg->key_file->GetSeed(0, 0);
//  arg->key_file->GetPic(0, 0);
//  arg->key_file->SavePic(0, 1);
//  arg->key_file->SavePic(1, 1);

    arg->key_file->AppendPufFile();
//  arg->key_file->AppendSeed();
  return 0;
}
