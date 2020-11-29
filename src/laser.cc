#include "laser.h"

namespace check_system
{
  Laser::Laser(const char *device_name)
      : status_(0)
  {
    ttl = -1;
    usart_.Open(device_name, 9600, 8, 1, 'N', 0);
  }

  int Laser::ForceOpen()
  {
    int n = 20;
    while (n--)
    {
      int ret = SendOpenCmd();
      if (ret == 0)
        break;
      Utils::MSleep(1000);
    }
    if (n <= 0)
    {
      return -1;
    }
    return 0;
  }
  int Laser::ForceClose()
  {
    int n = 20;
    while (n--)
    {
      int ret = SendCloseCmd();
      if (ret == 0)
        break;
      Utils::MSleep(1000);
    }
    if (n <= 0)
    {
      return -1;
    }
    return 0;
  }
  int Laser::ForceCheck()
  {
    int n = 20;
    while (n--)
    {
      int ret = SendCheckCmd();
      if (ret == 0)
        break;
      Utils::MSleep(1000);
    }
    if (n <= 0)
    {
      return -1;
    }
    return 0;
  }

  int Laser::SendOpenCmd()
  {
    int i;
    int p = 0;
    ttl = 600;
    if (status_)
      return 0;
    std::cout << "open laser\n";
    data_frame_[p++] = 0x68;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p++] = 0x05;
    data_frame_[p++] = 0;
    data_frame_[p++] = 0x04;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
    p++;
    data_frame_[p++] = 0x16;
    data_frame_[p] = '\0';
    usart_.SendData(data_frame_, p);
    int ret = ReadBuffer(1);
    if (ret <= 0)
    {
      std::cout << "open laser wrong!!!" << std::endl;
      return -1;
    }
    status_ = 1;
    return 0;
  }

  int Laser::SendCloseCmd()
  {
    std::cout << "close laser\n";
    int i;
    int p = 0;
    int ret;

    if (status_ == 0)
      return 0;

    data_frame_[p++] = 0x68;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p++] = 0x06;
    data_frame_[p++] = 0;
    data_frame_[p++] = 0x04;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
    p++;
    data_frame_[p++] = 0x16;
    data_frame_[p] = '\0';
    usart_.SendData(data_frame_, p);
    i = 0;
    ret = ReadBuffer(1);
    if (ret <= 0)
    {
      std::cout << "close laser wrong!!!" << std::endl;
      return -1;
    }
    status_ = 0;
    return 0;
  }

  int Laser::SendCheckCmd()
  {
    std::cout << "check laser\n";
    int i;
    int p = 0;

    data_frame_[p++] = 0x68;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p++] = 0x01;
    data_frame_[p++] = 0;
    data_frame_[p++] = 0x04;
    for (i = 0; i < 4; i++)
    {
      data_frame_[i + p] = 0;
    }
    p += i;
    data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
    p++;
    data_frame_[p++] = 0x16;
    data_frame_[p] = '\0';
    usart_.SendData(data_frame_, p);
    i = 0;
    int ret = ReadBuffer(1);
    if (ret <= 0)
    {
      std::cout << "check laser fault!!" << std::endl;
      return -1;
    }
    //如果数据格式不对temperature
    if (data_frame_[5] != 0x01 && data_frame_[7] != 0x12)
    {
      perror("bad data!!!");
      return -1;
    }
    status_ = data_frame_[10];
    memcpy(temperature_, data_frame_ + 18, 4);
    // and etc...
    return 0;
  }

  int Laser::SetTemperature(int Temp)
  {
    std::cout << "set laser temperature\n";
    int ret;
    if (Temp == 20)
    {
      if (temperature_[0] == 0x41 && temperature_[1] == (char)0xa0 && temperature_[2] == 00 && temperature_[3] == 00)
      {
        return 0;
      }
      int i;
      int p = 0;
      data_frame_[p++] = 0x68;
      for (i = 0; i < 4; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p++] = 0x04;
      data_frame_[p++] = 0;
      data_frame_[p++] = 0x04;
      data_frame_[p++] = 0x41;
      data_frame_[p++] = 0xA0;
      for (i = 0; i < 2; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
      p++;
      data_frame_[p++] = 0x16;
      data_frame_[p] = '\0';
      usart_.SendData(data_frame_, p);
    }
    else
    {
      return -1;
    }
    ret = ReadBuffer(2);
    if (ret <= 0)
    {
      std::cout << "SetTemperature laser wrong!!!" << std::endl;
      return -1;
    }
    return 0;
  }

  int Laser::SetCurrent(int cur)
  {
    std::cout << "set laser current\n";
    int ret;
    if (cur == 3000)
    {
      int i;
      int p = 0;
      data_frame_[p++] = 0x68;
      for (i = 0; i < 4; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p++] = 0x02;
      data_frame_[p++] = 0;
      data_frame_[p++] = 0x04;
      data_frame_[p++] = 0x45;
      data_frame_[p++] = 0x3B;
      data_frame_[p++] = 0x80;
      for (i = 0; i < 1; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
      p++;
      data_frame_[p++] = 0x16;
      data_frame_[p] = '\0';
      usart_.SendData(data_frame_, p);
    }
    else
    {
      return -1;
    }
    ret = ReadBuffer(2);
    if (ret <= 0)
    {
      std::cout << "SetCurrent laser wrong!!!" << std::endl;
      return -1;
    }
    return 0;
  }

  int Laser::SetMaxCurrent(int max_cur)
  {
    std::cout << "set laser max current\n";
    int ret;
    if (max_cur == 5000)
    {
      int i;
      int p = 0;
      data_frame_[p++] = 0x68;
      for (i = 0; i < 4; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p++] = 0x02;
      data_frame_[p++] = 0;
      data_frame_[p++] = 0x04;
      data_frame_[p++] = 0x45;
      data_frame_[p++] = 0x9C;
      data_frame_[p++] = 0x40;
      for (i = 0; i < 1; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
      p++;
      data_frame_[p++] = 0x16;
      data_frame_[p] = '\0';
      usart_.SendData(data_frame_, p);
    }
    else if (max_cur == 7000)
    {
      int i;
      int p = 0;
      data_frame_[p++] = 0x68;
      for (i = 0; i < 4; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p++] = 0x02;
      data_frame_[p++] = 0;
      data_frame_[p++] = 0x04;
      data_frame_[p++] = 0x45;
      data_frame_[p++] = 0xEA;
      data_frame_[p++] = 0x60;
      for (i = 0; i < 1; i++)
      {
        data_frame_[i + p] = 0;
      }
      p += i;
      data_frame_[p] = Utils::CheckSum((unsigned char *)data_frame_ + 1, p - 1);
      p++;
      data_frame_[p++] = 0x16;
      data_frame_[p] = '\0';
      usart_.SendData(data_frame_, p);
    }
    else
    {
      return -1;
    }
    ret = ReadBuffer(2);
    if (ret <= 0)
    {
      std::cout << "SetMaxCurrent laser wrong!!!" << std::endl;
      return -1;
    }
    return 0;
  }

  int Laser::ReadBuffer(int timeout)
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
      int res_len = usart_.ReadData(data_frame_, 1024);
      return res_len;
    }
  }
} // namespace check_system
