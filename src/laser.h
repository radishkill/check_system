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
  int SetTemperature(int Temp);
  int SetCurrent(int cur);
  int SetMaxCurrent(int max_cur);
  int ReadBuffer(int timeout);
  int GetStatus() {return status_;}
  bool IsOpen() const {return usart_.IsOpen();}
  Usart& GetUsart() {return usart_;};
 private:
  char data_frame_[310];
  Usart usart_;
  int status_;
  int current_;
  int max_current_;
  int temperature_;
};
}



#endif // LASER_H
