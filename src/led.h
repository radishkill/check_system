#ifndef LED_H
#define LED_H

#include <thread>

namespace check_system {

const static int kLaserGpioNumber = 223;
const static int kLcdGpioNumber = 227;
const static int kCmosGpioNumber = 255;
const static int kErrorGpioNumber = 254;


class LedController {
 public:
  LedController();
  // s == 0关灯 s == 1 开灯
  int LaserLed(int s);
  int LcdLed(int s);
  int CmosLed(int s);
  int ErrorLed(int s);

  int RunBlink();

  int laser_blink_;
  int lcd_blink_;
  int cmos_blink_;
  int error_blink_;
 private:
  int laser_status_;
  int lcd_status_;
  int cmos_status_;
  int error_status_;

  int laser_fd_;
  int lcd_fd_;
  int cmos_fd_;
  int error_fd_;
  std::thread blink_thread_;
};
}

#endif // LED_H
