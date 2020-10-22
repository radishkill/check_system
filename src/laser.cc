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
  i = 0;
  while (1) {
    if (ReadBuffer(3000)) {
      break;
    }
    if (i >= 3) {
      return -1;
    }
    i++;
  }
  status_ = 1;
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
  i = 0;
  while (1) {
    if (ReadBuffer(3000)) {
      break;
    }
    if (i >= 3) {
      return -1;
    }
    i++;
  }
  status_ = 0;
  return 0;
}

int Laser::SendCheckCmd() {
  return 0;
}

int Laser::ReadBuffer(int timeout) {
  fd_set fdsr;
  struct timeval tv;
  FD_ZERO(&fdsr);
  FD_SET(usart_.GetFd(), &fdsr);
  // timeout setting
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  int ret = select(usart_.GetFd()+1, &fdsr, NULL, NULL, &tv);
  if (ret < 0) {
    perror("select");
    return -1;
  } else if (ret == 0) {
    std::cout << "timeout or empty data" << std::endl;
    return 0;
  } else {
    int res_len = read(usart_.GetFd(), data_frame_, 1024);
    for (int i = 0; i < res_len; i++){
      std::cout << std::hex << (int)data_frame_[i] << " ";
    }
    std::cout << std::endl;
    return res_len;
  }
}
