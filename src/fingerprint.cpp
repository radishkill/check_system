#include "fingerprint.h"

#include <array>

FingerPrint::FingerPrint(const char* device_name)
{
  usart_.Open(device_name, 115200, 8, 1, 'N', 0);
  is_connect = Connect();
}

bool FingerPrint::Connect()
{
 //握手
  int p = 0;
  int len = 0;
  data_frame_[0] = 0xef;
  data_frame_[1] = 0x01;
  data_frame_[2] = 0xff;
  data_frame_[3] = 0xff;
  data_frame_[4] = 0xff;
  data_frame_[5] = 0xff;
  data_frame_[6] = 0x01;
  data_frame_[7] = 0x00;
  data_frame_[8] = 0x07;
  data_frame_[9] = 0x13;
  data_frame_[10] = 0x00;
  data_frame_[11] = 0x00;
  data_frame_[12] = 0x00;
  data_frame_[13] = 0x00;
  data_frame_[14] = 0x00;
  data_frame_[15] = 0x1b;
  p = 16;

  data_frame_[p] = 0x00;
  usart_.SendData(data_frame_, p);
  len = ReadBuffer(1);
  if (len <= 0) {
    return false;
  }
  if (data_frame_[8] != 0x03) {
    return false;
  }
  std::cout << "finger print connect ok!!\n";
  return true;
}

bool FingerPrint::RecodeFinger(int index)
{
  //recode
  int p = 0;
  int len = 0;
  data_frame_[0] = 0xef;
  data_frame_[1] = 0x01;
  data_frame_[2] = 0xff;
  data_frame_[3] = 0xff;
  data_frame_[4] = 0xff;
  data_frame_[5] = 0xff;
  data_frame_[6] = 0x01;
  data_frame_[7] = 0x00;
  data_frame_[8] = 0x03;
  data_frame_[9] = 0x01;
  data_frame_[10] = 0x00;
  data_frame_[11] = 0x05;
  p = 12;

  data_frame_[p] = 0x00;
  usart_.SendData(data_frame_, p);
  len = ReadBuffer(2);
  if (len <= 0) {
    return false;
  }
  if (data_frame_[8] != 0x03 || data_frame_[9] != 0x00) {
    return false;
  }

  //save
  data_frame_[0] = 0xef;
  data_frame_[1] = 0x01;
  data_frame_[2] = 0xff;
  data_frame_[3] = 0xff;
  data_frame_[4] = 0xff;
  data_frame_[5] = 0xff;
  data_frame_[6] = 0x01;
  data_frame_[7] = 0x00;
  data_frame_[8] = 0x04;
  data_frame_[9] = 0x02;
  data_frame_[10] = index;
  data_frame_[11] = 0x00;
  data_frame_[12] = 0x07+index;
  p = 13;

  data_frame_[p] = 0x00;
  usart_.SendData(data_frame_, p);
  len = ReadBuffer(2);
  if (len <= 0) {
    return false;
  }
  if (data_frame_[8] != 0x03 || data_frame_[9] != 0x00) {
    return false;
  }
  return true;
}

bool FingerPrint::CheckFinger()
{
  int len = 0;
  int p = 0;
  //check
  data_frame_[0] = 0xef;
  data_frame_[1] = 0x01;
  data_frame_[2] = 0xff;
  data_frame_[3] = 0xff;
  data_frame_[4] = 0xff;
  data_frame_[5] = 0xff;
  data_frame_[6] = 0x01;
  data_frame_[7] = 0x00;
  data_frame_[8] = 0x03;
  data_frame_[9] = 0x03;
  data_frame_[10] = 0x00;
  data_frame_[11] = 0x07;
  p = 12;

  data_frame_[p] = 0x00;
  usart_.SendData(data_frame_, p);
  len = ReadBuffer(2);
  if (len <= 0) {
    return false;
  }
  if (data_frame_[11] < 0x50) {
    return false;
  }
  return true;
}

int FingerPrint::ReadBuffer(int timeout)
{
  fd_set fdsr;
  struct timeval tv;

  FD_ZERO(&fdsr);
  FD_SET(usart_.GetFd(), &fdsr);
  // timeout setting
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  int ret = select(usart_.GetFd() + 1, &fdsr, NULL, NULL, &tv);
  if (ret < 0)
  {
    perror("select");
    return -1;
  }
  else if (ret == 0)
  {
    return 0;
  }
  else
  {
    int len = usart_.ReadData(data_frame_, 1024);
    return len;
  }
}
