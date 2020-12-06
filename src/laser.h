#ifndef LASER_H
#define LASER_H

#include <sys/select.h>

#include "usart.h"
#include "mutils.h"

namespace check_system {
class Laser {
 public:
  Laser(const char* device_name);
  int ForceOpen();
  int ForceClose();
  int ForceCheck();
  int ForceSetCurrent(int current);

  int OpenLaser();
  int CloseLaser();
  int CheckStatus();
  int SetTemperature(double Temp);
  int SetCurrent(int cur);
  int SetMaxCurrent(int max_cur);
  int ReadBuffer(int timeout);
  int GetStatus() {return status_;}
  bool IsOpen() const {return usart_.IsOpen();}
  Usart& GetUsart() {return usart_;};
  int ttl;
 private:
  char data_frame_[310];
  Usart usart_;
  int status_;
  char temperature_[4];
  char current_[2];
  char max_current_[2];
};
}



#endif // LASER_H
