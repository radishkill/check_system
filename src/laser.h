#ifndef LASER_H
#define LASER_H

#include <sys/select.h>

#include "usart.h"
#include "utils.h"

namespace check_system {
class Laser {
 public:
  Laser(const char* device_name);
  int SendOpenCmd();
  int SendCloseCmd();
  int SendCheckCmd();
  int ReadBuffer(int timeout);
  int IsOpen() {return status_;}
  Usart& GetUsart() {return usart_;};
 private:
  char data_frame_[310];
  Usart usart_;
  int status_;
  int target_exist_;
  int target_temperature_status;
  int target_move_;
  int env_temperature_status_;
  int back_current_unit_;
  int is_back_current_empty_;
  int max_current_;
  int temperature_;
};
}



#endif // LASER_H
