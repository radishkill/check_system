#ifndef GLOBAL_ARG_H
#define GLOBAL_ARG_H

class Laser;
class CameraManager;
class StateMachine;

class GlobalArg {
 public:
  static GlobalArg* GetInstance();
  Laser* laser;
  CameraManager* camera;
  StateMachine* sm;
 private:
  static GlobalArg* global_arg_;
};

#endif // GLOBAL_ARG_H
