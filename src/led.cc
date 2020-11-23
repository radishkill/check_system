#include "led.h"


#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>

#include "utils.h"


namespace check_system {

LedController::LedController() {
  std::string addr;
  addr = std::string("/sys/class/gpio/gpio") + std::to_string(kLaserGpioNumber) + "/value";
  laser_fd_ = open(addr.c_str(), O_RDWR);
  if (laser_fd_ == -1) {
    std::cout << "can't open laser gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") + std::to_string(kLcdGpioNumber) + "/value";
  lcd_fd_ = open(addr.c_str(), O_RDWR);
  if (lcd_fd_ == -1) {
    std::cout << "can't open Lcd gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") + std::to_string(kCmosGpioNumber) + "/value";
  cmos_fd_ = open(addr.c_str(), O_RDWR);
  if (cmos_fd_ == -1) {
    std::cout << "can't open cmos gpio " << std::endl;
    exit(0);
    return;
  }

  addr = std::string("/sys/class/gpio/gpio") + std::to_string(kErrorGpioNumber) + "/value";
  error_fd_ = open(addr.c_str(), O_RDWR);
  if (error_fd_ == -1) {
    std::cout << "can't open Error gpio " << std::endl;
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
}

LedController::~LedController(){
  ::close(laser_fd_);
  ::close(lcd_fd_);
  ::close(cmos_fd_);
  ::close(error_fd_);
}

int LedController::LaserLed(int s) {
//  std::cout << "laser :" << s << std::endl;
  //重置读写位置到文件开头
  lseek(laser_fd_, 0, SEEK_SET);
  if (s == 0) {
    write(laser_fd_, "0", 1);
  } else if (s == 1) {
    write(laser_fd_, "1", 1);
  } else {
    std::cout << "bad status" << std::endl;
    return -1;
  }
  laser_status_ = s;
  return 0;
}

int LedController::LcdLed(int s) {
//  std::cout << "lcd :" << s << std::endl;
  //重置读写位置到文件开头
  lseek(lcd_fd_, 0, SEEK_SET);
  if (s == 0) {
    write(lcd_fd_, "0", 1);
  } else if (s == 1) {
    write(lcd_fd_, "1", 1);
  } else {
    std::cout << "bad status" << std::endl;
    return -1;
  }
  lcd_status_ = s;
  return 0;
}

int LedController::CmosLed(int s) {
//  std::cout << "cmos :" << s << std::endl;
  //重置读写位置到文件开头
  lseek(cmos_fd_, 0, SEEK_SET);
  if (s == 0) {
    write(cmos_fd_, "0", 1);
  } else if (s == 1) {
    write(cmos_fd_, "1", 1);
  } else {
    std::cout << "bad status" << std::endl;
    return -1;
  }
  cmos_status_ = s;
  return 0;
}

int LedController::ErrorLed(int s) {
//  std::cout << "error :" << s << std::endl;
  //重置读写位置到文件开头
  lseek(error_fd_, 0, SEEK_SET);
  if (s == 0) {
    write(error_fd_, "0", 1);
  } else if (s == 1) {
    write(error_fd_, "1", 1);
  } else {
    std::cout << "bad status" << std::endl;
    return -1;
  }
  error_status_ = s;
  return 0;
}

int LedController::RunBlink() {
  blink_thread_ = std::thread([&]() {
    int i = 0;
    while (1) {
      if (this->laser_blink_ && ((i*100)%this->laser_blink_ == 0)) {
        if (laser_status_) {
          LaserLed(0);
        } else {
          LaserLed(1);
        }
      }
      if (this->lcd_blink_ && ((i*100)%this->lcd_blink_ == 0)) {
        if (lcd_status_) {
          LcdLed(0);
        } else {
          LcdLed(1);
        }
      }
      if (this->cmos_blink_ && ((i*100)%this->cmos_blink_ == 0)) {
        if (cmos_status_) {
          CmosLed(0);
        } else {
          CmosLed(1);
        }
      }
      if (this->error_blink_ && ((i*100)%this->error_blink_ == 0)) {
        if (error_status_) {
          ErrorLed(0);
        } else {
          ErrorLed(1);
        }
      }
      Utils::MSleep(100);
      i++;
      if (i == 1000000)
        i = 0;
    }
  });
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

}
