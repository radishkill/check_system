#ifndef GLOBAL_ARG_H
#define GLOBAL_ARG_H

class Laser;
class CameraManager;

class GlobalArg {
 public:
  static GlobalArg* GetInstance();
  Laser* laser;
  CameraManager* camera;
 private:
  static GlobalArg* global_arg_;
};

#endif // GLOBAL_ARG_H
