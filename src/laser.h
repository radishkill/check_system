#ifndef LASER_H
#define LASER_H

#include "usart.h"
#include "utils.h"

class Laser {
 public:
  Laser(const char* device_name);
  int SendOpenCmd();
  int SendCloseCmd();
  int SendCheckCmd();
 private:
  char data_frame_[310];
  Usart usart_;
  int status_;
};

#endif // LASER_H
