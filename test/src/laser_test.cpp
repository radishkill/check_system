#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include "laser.h"

using check_system::Laser;

int main(int argc, char **argv)
{
  if (argc == 1)
  {
    std::cout << "pealse input 0 to close, 1 to open\n";
  }
  std::string laser_addr = "/dev/ttyS0";
  if (argc == 3) {
    laser_addr = argv[2];
  }
  Laser laser(laser_addr.c_str());
  if (!laser.IsOpen())
  {
    std::cout << "cant open ttyS0" << std::endl;
  }
  int n;
  n = 12;
  int laser_flag;
  while (n--)
  {
    laser_flag = laser.CheckStatus();
    if (laser_flag == 0)
      break;
    Utils::MSleep(1000);
  }
  if (std::atoi(argv[1]) == 0)
  {
    std::cout << "close laser" << std::endl;
    n = 20;
    while (n--)
    {
      if (0 == laser.CloseLaser())
        break;
      Utils::MSleep(1000);
    }
  }
  else
  {
    std::cout << "open laser" << std::endl;
    n = 20;
    while (n--)
    {
      if (0 == laser.OpenLaser())
        break;
      Utils::MSleep(1000);
    }
  }
  return 0;
}
