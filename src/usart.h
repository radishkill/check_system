#ifndef USART_H
#define USART_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <iostream>
#include <stdio.h>
#include <string>

class Usart
{
  public:
  Usart();
  Usart(const char* name, int baud_rate, int databits, int stopbits, char parity, int flow_ctrl);
  int Open(const char* name, int baud_rate, int databits, int stopbits, char parity, int flow_ctrl);
  int SetParity(int baud_rate, int databits, int stopbits, char parity, int flow_ctrl);
  int GetFd();
  int SendData(char* buf, int len);
  int ReadData(char* buf, int len);
  void Close();
  private:
  std::string device_name_;
  int fd_;
};

#endif // USART_H
