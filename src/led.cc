#include "led.h"

#include <iostream>

#include "utils.h"


namespace check_system {

LedController::LedController() {
  laser_status_ = 0;
  lcd_status_ = 0;
  cmos_status_ = 0;
  error_status_ = 0;
  laser_blink_ = 0;
  lcd_blink_ = 0;
  cmos_blink_ = 0;
  error_blink_ = 0;
}

int LedController::LaserLed(int s) {
  std::cout << "laser :" << s << std::endl;
  laser_status_ = s;
  return 0;
}

int LedController::LcdLed(int s) {
  std::cout << "lcd :" << s << std::endl;
  lcd_status_ = s;
  return 0;
}

int LedController::CmosLed(int s) {
  std::cout << "cmos :" << s << std::endl;
  cmos_status_ = s;
  return 0;
}

int LedController::ErrorLed(int s) {
  std::cout << "error :" << s << std::endl;
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

}
