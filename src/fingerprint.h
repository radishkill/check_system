#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <sys/select.h>

#include "usart.h"
#include "mutils.h"

class FingerPrint
{
public:
  FingerPrint(const char *device_name);
  bool Connect();
  bool IsConnect() {return is_connect;}

  bool RecodeFinger(int index);
  bool CheckFinger();

  int ReadBuffer(int timeout);

private:
  bool is_connect = false;
  char data_frame_[1024];
  Usart usart_;
};

#endif // FINGERPRINT_H
