#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <iostream>
#include <stdio.h>
#include <string>


#include <iostream>

int fd_;



int SetParity(int baud_rate, int databits, int stopbits, char parity, int flow_ctrl)
{
    struct termios options;
    if  ( tcgetattr( fd_, &options)  !=  0)
    {
        return (-1);
    }
    bzero(&options, sizeof(options));
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    switch (baud_rate)
    {
    case 2400:
        cfsetispeed(&options, B2400);
        cfsetospeed(&options, B2400);
        break;
    case 4800:
        cfsetispeed(&options, B4800);
        cfsetospeed(&options, B4800);
        break;
    case 9600:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    case 115200:
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        break;
    default:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    }
    switch (databits) /*设置数据位数*/
    {
    case 5:
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    }
    /* 设置停止位*/
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported stop bits\n");
        return -1;
    }
    // 校验码
    switch (parity)
    {
    case '0':  /*奇校验*/
        options.c_cflag |= PARENB;/*开启奇偶校验*/
        options.c_iflag |= (INPCK | ISTRIP);/*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
        options.c_cflag |= PARODD;/*启用奇校验(默认为偶校验)*/
        break;
    case 'E':/*偶校验*/
        options.c_cflag |= PARENB; /*开启奇偶校验  */
        options.c_iflag |= ( INPCK | ISTRIP);/*打开输入奇偶校验并去除字符第八个比特*/
        options.c_cflag &= ~PARODD;/*启用偶校验*/
        break;
    case 'N': /*无奇偶校验*/
        options.c_cflag &= ~PARENB;
        break;
    }
    switch (flow_ctrl)
    {
    case 0:
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1://haidware
//         options.c_cflag |= CRTSCTS;
        break;
    case 2://software
        flow_ctrl = 0;
        options.c_iflag |= (IXON | IXOFF | IXANY);
        break;
    default:
        fprintf(stderr,"error\n");
        break;
    }
    /*设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时*/
    options.c_cc[VTIME] = 0;/*非规范模式读取时的超时时间；*/
    options.c_cc[VMIN]  = 0; /*非规范模式读取时的最小字符数*/
    tcflush(fd_ ,TCIFLUSH);/*tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

    return tcsetattr(fd_, TCSAFLUSH, &options);
}

char data_frame_[1024];

unsigned char  CheckSum(unsigned char *p, int datalen) {
  unsigned char cksum = 0;
  int i;
  for (i = 0; i < datalen; i++) {
    cksum += *(p+i);
  }
  return (~cksum);
}

int SendData(char* buf, int len) {
  std::cout << "send open" << std::endl;
  return write(fd_, buf, len);
}

int ReadData(char* buf, int len) {
  std::cout << "send close" << std::endl;
  return read(fd_, buf, len);
}

int ReadBuffer(int timeout) {
  fd_set fdsr;
  struct timeval tv;
  FD_ZERO(&fdsr);
  FD_SET(fd_, &fdsr);
  // timeout setting
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  int ret = select(fd_+1, &fdsr, NULL, NULL, &tv);
  if (ret < 0) {
    perror("select");
    return -1;
  } else if (ret == 0) {
    std::cout << "timeout or empty data" << std::endl;
    return 0;
  } else {
    int res_len = read(fd_, data_frame_, 1024);
    std::cout << "read:" << std::endl;
    for (int i = 0; i < res_len; i++){
      std::cout << std::hex << (int)data_frame_[i] << " ";
    }
    std::cout << std::endl;
    return res_len;
  }
}

int SendOpenCmd() {
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
  data_frame_[p] = CheckSum((unsigned char *)data_frame_+1, p-1);
  p++;
  data_frame_[p++] = 0x16;
  data_frame_[p] = '\0';

  SendData(data_frame_, p);
  int ret = ReadBuffer(6);
  if (ret <= 0) {
    std::cout << "open laser wrong!!!" << std::endl;
    return -1;
  } else {
    std::cout << "open sucess" << std::endl;
  }
  return 0;
}

int SendCloseCmd() {
  int i;
  int p = 0;
  int ret;

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
  data_frame_[p] = CheckSum((unsigned char *)data_frame_+1, p-1);
  p++;
  data_frame_[p++] = 0x16;
  data_frame_[p] = '\0';
  SendData(data_frame_, p);
  i = 0;
  ret = ReadBuffer(5);
  if (ret <= 0) {
    std::cout << "close laser wrong!!!" << std::endl;
    return -1;
  } else {
    std::cout << "close success" << std::endl;
  }
  return 0;
}

int main() {
  fd_ = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ == -1) {
      perror("open serialPort fail");
      return -1;
  } else {
    std::cout << "open fd : " << fd_ << std::endl;
  }
  SendCloseCmd();
  sleep(3);
  SendOpenCmd();
  sleep(3);
  SendCloseCmd();
  return 0;
}