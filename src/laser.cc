#include "laser.h"

Laser::Laser(const char* device_name)
  : status_(0) {
  usart_.Open(device_name, 9600, 8, 1, 'N', 0);
}

int Laser::SendOpenCmd() {
  int i;
  int p = 0;
  data_frame_[p++] = 0x68;
  for (i = 0; i < 4; i++) {
    data_frame_[i+p] = 0;
  }
  p += i;
  data_frame_[p++] = 0x05;
  data_frame_[p++] = 0;
  data_frame_[p++] = 0x04;
  for (i = 0; i < 4; i++) {
    data_frame_[i+p] = 0;
  }
  p += i;
  data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_+1, p-1);
  p++;
  data_frame_[p++] = 0x16;
  data_frame_[p] = '\0';
  usart_.SendData(data_frame_, p);
  return 0;
}

int Laser::SendCloseCmd() {
  int i;
  int p = 0;
  data_frame_[p++] = 0x68;
  for (i = 0; i < 4; i++) {
    data_frame_[i+p] = 0;
  }
  p += i;
  data_frame_[p++] = 0x06;
  data_frame_[p++] = 0;
  data_frame_[p++] = 0x04;
  for (i = 0; i < 4; i++) {
    data_frame_[i+p] = 0;
  }
  p += i;
  data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_+1, p-1);
  p++;
  data_frame_[p++] = 0x16;
  data_frame_[p] = '\0';
  usart_.SendData(data_frame_, p);
  return 0;
}

int Laser::SendCheckCmd() {
  return 0;
}
