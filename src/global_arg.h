#pragma once

#include <string>

namespace check_system {
class Laser;
class CameraManager;
class StateMachine;
class KeyFile;
class Lcd;
class EventManager;
class LedController;
class HostController;

class GlobalArg {
 public:
  static GlobalArg* GetInstance();
  GlobalArg() {
    interrupt_flag = 0;
    hsk_flag = 0;
    is_fault = false;
    no_button_flag = false;
    no_laser_flag = false;
    no_lcd_flag = false;
    check_btn_flag = 0;
  }
  Laser* laser;
  CameraManager* camera;
  StateMachine* sm;
  KeyFile* key_file;
  Lcd* lcd;
  EventManager* em;
  LedController* led;
  HostController* host;

  int interrupt_flag;
  int check_btn_flag;
  int hsk_flag;  //上位机握手控制
  bool is_fault;

  double exposion_time;
  int analog_gain;
  int laser_current;
  int resolution_index;
  int roi_x, roi_y, roi_w, roi_h;
  bool no_button_flag;
  bool no_laser_flag;
  bool no_lcd_flag;
  bool no_led_flag;
  bool system_fault;
  int lcd_wh;

  int camera_gamma;
  int camera_contrast;
  int camera_saturation;
  int camera_sharpness;

  int pic_avg_high;
  int pic_avg_low;
  double auth_threshold;

  int camera_lut_mode;
  int stable_flag;

  std::string camera_config_file_addr;
  unsigned long check_btn_down;
  unsigned int interrupt_btn_down;
 private:
  static GlobalArg* global_arg_;
};
}  // namespace check_system
