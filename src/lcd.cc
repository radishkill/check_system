#include "lcd.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

namespace check_system {
Lcd::Lcd(const char *device_name)
  : device_name_(device_name) {
  fd_ = open(device_name, O_RDWR);
  if (fd_ == -1) {
    perror("can't open framebuffer device");
    return;
  }
}
//显示seed到LCD 800 * 600
int Lcd::ShowBySeed(int seed) {

  return 0;
}

bool Lcd::IsOpen() const {
  if (fd_ <= 0) {
    return false;
  }
  return true;
}
}


