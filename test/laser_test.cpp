#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <iostream>

#include "laser.h"

using check_system::Laser;

int main() {
  Laser laser("/dev/ttyS0");
  if(!laser.IsOpen()) {
    std::cout << "cant open ttyS0" << std::endl;
  }
  int n;
  n = 12;
  int laser_flag;
  while (n--) {
    laser_flag = laser.SendCheckCmd();
    if (laser_flag == 0)
      break;
    Utils::MSleep(1000);
  }
  n = 20;
  while (n--) {
    if (0 == laser.SendOpenCmd())
      break;
    Utils::MSleep(1000);
  }
  return 0;
}
