#ifndef GLOBAL_ARG_H
#define GLOBAL_ARG_H

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
  GlobalArg();
  Laser* laser;
  CameraManager* camera;
  StateMachine* sm;
  KeyFile* key_file;
  Lcd* lcd;
  EventManager* em;
  LedController* led;
  HostController* host;

  int interrupt_flag;
  int hsk_flag;//上位机握手控制
  int is_fault;

  int exposion_time;
  int laser_current;
  int resolution_index;
  int roi_x, roi_y, roi_w, roi_h;
  bool no_button_flag;
  bool no_laser_flag;
  bool no_lcd_flag;
  std::string mid_save_addr;
 private:
  static GlobalArg* global_arg_;
};
}

#endif // GLOBAL_ARG_H
