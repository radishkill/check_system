#ifndef LED_H
#define LED_H


#include "constant.h"

namespace check_system {


class LedController {
 public:
  LedController(int laser_gpio=-1, int cmos_gpio=-1, int lcd_gpio=-1, int error_gpio=-1);
  ~LedController();
  // s == 0关灯 s == 1 开灯
  int LaserLed(int s);
  int LcdLed(int s);
  int CmosLed(int s);
  int ErrorLed(int s);

  int CloseBlink();

  int laser_blink_;
  int lcd_blink_;
  int cmos_blink_;
  int error_blink_;

  int laser_status_;
  int lcd_status_;
  int cmos_status_;
  int error_status_;

  int laser_fd_;
  int lcd_fd_;
  int cmos_fd_;
  int error_fd_;

  int laser_gpio_id_;
  int lcd_gpio_id_;
  int cmos_gpio_id_;
  int error_gpio_id_;
  private:
  // std::thread blink_thread_;
};
}

#endif // LED_H
