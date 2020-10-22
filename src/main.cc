#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>

#include "usart.h"
#include "laser.h"


void InitSystem() {
  Laser laser("ttyUSB0");
  laser.SendCloseCmd();
  sleep(1);
  laser.SendOpenCmd();
  sleep(10);
  laser.SendCloseCmd();
}
int main() {
  InitSystem();
  return 0;
}
