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

using namespace std;

class Usart
{
public:
    Usart(string name, int baud_rate, int databits, int stopbits, char parity, int flow_ctrl);
    bool SetParity(int baud_rate, int databits, int stopbits, char parity, int flow_ctrl);
    int GetFd();
    bool SendData(string msg);
    string ReadData();
    void CloseFd();
private:
    string device_name_;
    int fd_;
};

#endif // USART_H
