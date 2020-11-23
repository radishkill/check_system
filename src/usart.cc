#include "usart.h"

#include "utils.h"

Usart::Usart()
    : fd_(-1), device_name_("") {
}

Usart::Usart(const char* name, int baud_rate, int databits, int stopbits, char parity, int flow_ctrl) {
  Open(name, baud_rate, databits, stopbits, parity, flow_ctrl);
}

int Usart::Open(const char* name, int baud_rate, int databits, int stopbits, char parity, int flow_ctrl) {
  device_name_ = name;

  fd_ = open(device_name_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ == -1) {
      perror("open serialPort fail");
      return -1;
  }
  SetParity(baud_rate, databits, stopbits, parity, flow_ctrl);
  return 0;
}

int Usart::SetParity(int baud_rate, int databits, int stopbits, char parity, int flow_ctrl)
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

int Usart::SendData(char* buf, int len) {
//  std::cout << "send:";
//  Utils::ShowRawString(buf, len);
//  std::cout << std::endl;
  return write(fd_, buf, len);
}

int Usart::ReadData(char* buf, int len) {
  std::cout << "recv:";
  int ret = read(fd_, buf, len);
  Utils::ShowRawString(buf, ret);
  std::cout << std::endl;
  return ret;
}

bool Usart::IsOpen() const  {
  if (fd_ <= 0)
    return false;
  return true;
}

int Usart::GetFd() const {
    return fd_;
}

void Usart::Close() {
    close(fd_);
    fd_ = -1;
}
