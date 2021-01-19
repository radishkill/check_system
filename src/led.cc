#include "led.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
#include <string>

#include "mutils.h"

namespace check_system {

LedController::LedController(int laser_gpio, int cmos_gpio, int lcd_gpio, int error_gpio) {
  laser_gpio_id_ = laser_gpio==-1?kLaserGpioNumber:laser_gpio;
  cmos_gpio_id_ = cmos_gpio==-1?kCmosGpioNumber:cmos_gpio;
  lcd_gpio_id_ = lcd_gpio==-1?kLcdGpioNumber:lcd_gpio;
  error_gpio_id_ = error_gpio==-1?kErrorGpioNumber:error_gpio;
  

  std::string addr;
  addr = std::string("/sys/class/gpio/gpio") +
         std::to_string(laser_gpio_id_) + "/value";
  laser_fd_ = open(addr.c_str(), O_RDWR);
  if (laser_fd_ == -1) {
    std::cout << "can't open laser's led gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") + std::to_string(lcd_gpio_id_) +
         "/value";
  lcd_fd_ = open(addr.c_str(), O_RDWR);
  if (lcd_fd_ == -1) {
    std::cout << "can't open Lcd's led gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") + std::to_string(cmos_gpio_id_) +
         "/value";
  cmos_fd_ = open(addr.c_str(), O_RDWR);
  if (cmos_fd_ == -1) {
    std::cout << "can't open cmos's led gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") +
         std::to_string(error_gpio_id_) + "/value";
  error_fd_ = open(addr.c_str(), O_RDWR);
  if (error_fd_ == -1) {
    std::cout << "can't open Error'led gpio " << std::endl;
    exit(0);
    return;
  }

  laser_status_ = 0;
  lcd_status_ = 0;
  cmos_status_ = 0;
  error_status_ = 0;

  laser_blink_ = 0;
  lcd_blink_ = 0;
  cmos_blink_ = 0;
  error_blink_ = 0;
  LaserLed(0);
  LcdLed(0);
  CmosLed(0);
  ErrorLed(0);
}

LedController::~LedController() {
  ::close(laser_fd_);
  ::close(lcd_fd_);
  ::close(cmos_fd_);
  ::close(error_fd_);
}

int LedController::LaserLed(int s) {
  assert(s==0||s==1);
  //  std::cout << "laser :" << s << std::endl;
  char key;
  if (kLaserGpioReverse)
    key = '0' + ((s==1)?0:1);
  //重置读写位置到文件开头
  lseek(laser_fd_, 0, SEEK_SET);
  write(laser_fd_, &key, 1);

  laser_status_ = s;
  return 0;
}

int LedController::LcdLed(int s) {
  assert(s==0||s==1);
  //  std::cout << "lcd :" << s << std::endl;
  char key;
  if (kLcdGpioReverse)
    key = '0' + ((s==1)?0:1);
  //重置读写位置到文件开头
  lseek(lcd_fd_, 0, SEEK_SET);
  write(lcd_fd_, &key, 1);
  lcd_status_ = s;
  return 0;
}

int LedController::CmosLed(int s) {
  assert(s==0||s==1);
  //  std::cout << "cmos :" << s << std::endl;
  char key;
  if (kCmosGpioReverse)
    key = '0' + ((s==1)?0:1);
  //重置读写位置到文件开头
  lseek(cmos_fd_, 0, SEEK_SET);
  write(cmos_fd_, &key, 1);
  cmos_status_ = s;
  return 0;
}

int LedController::ErrorLed(int s) {
  assert(s==0||s==1);
  //  std::cout << "error :" << s << std::endl;
  char key;
  if (kErrorGpioReverse)
    key = '0' + ((s==1)?0:1);
  //重置读写位置到文件开头
  lseek(error_fd_, 0, SEEK_SET);
  write(error_fd_, &key, 1);
  error_status_ = s;
  return 0;
}

int LedController::CloseBlink() {
  int f = 0;
  if (laser_blink_ != 0) {
    laser_blink_ = 0;
    f = 1;
  }
  if (lcd_blink_ != 0) {
    lcd_blink_ = 0;
    f = 1;
  }
  if (cmos_blink_ != 0) {
    cmos_blink_ = 0;
    f = 1;
  }
  if (error_blink_ != 0) {
    error_blink_ = 0;
    f = 1;
  }
  if (f == 1) {
    Utils::MSleep(100);
  }
  return 0;
}

}  // namespace check_system
