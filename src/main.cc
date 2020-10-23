#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>

#include "usart.h"
#include "laser.h"

#include"camera_manager.h"

void InitSystem() {
  Laser laser("/dev/ttyUSB0");
  laser.SendCloseCmd();
  sleep(5);
  laser.SendOpenCmd();
  sleep(20);
  laser.SendCheckCmd();
  sleep(20);
  laser.SendCloseCmd();
}
void InitCamera() {
  CameraManager();
}
int main() {
  InitSystem();
  InitCamera();
  return 0;
}
