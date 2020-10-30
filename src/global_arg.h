#ifndef GLOBAL_ARG_H
#define GLOBAL_ARG_H


namespace check_system {
class Laser;
class CameraManager;
class StateMachine;
class KeyFile;
class Lcd;
class EventManager;
class LedController;


class GlobalArg {
 public:
  static GlobalArg* GetInstance();
  Laser* laser;
  CameraManager* camera;
  StateMachine* sm;
  KeyFile* key_file;
  Lcd* lcd;
  EventManager* em;
  LedController* led;

  int interrupt_flag;

 private:
  static GlobalArg* global_arg_;
};
}

#endif // GLOBAL_ARG_H
