#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>

#include "global_arg.h"
#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"
#include "state_machine.h"
#include "key_file.h"
#include "lcd.h"
#include "epollrepertory.h"

void InitSystem() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->laser = new Laser("/dev/ttyUSB0");
  arg->camera = new CameraManager();
  arg->key_file = new KeyFile("./resource/PUFData");
  arg->lcd = new Lcd("");
  arg->epoll = new EpollRepertory();

  std::stringstream ss;
  int fd;
  FdContext fc;
  int key;

  ss << "/sys/class/gpio/gpio";
  ss << "123";
  ss << "/value";
  fd = open(ss.str().c_str(), O_RDONLY);
  ss.str("");
  if (fd < 0) {
    perror("error");
  } else {
      fc.type = DATATYPE_GPIO;
      fc.fd = fd;
      fc.addr[0] = 123;
      fc.status = 1;
      fc.events = EPOLLPRI;
      fc.call_back = [](int, uint32_t, void *) {
        std::cout << "button1";
      };
          //reinterpret_cast<void (*)(int, uint32_t, void *)>(getGpioMsg);
      read(fc.fd, &key, 1);
      arg->epoll->EventAdd(fc);
  }
}

void StartEpoll() {
  GlobalArg* arg = GlobalArg::GetInstance();
  struct epoll_event events[10];
  int nfds = epoll_wait(arg->epoll->GetEpollFd(), events, 10, -1);//time ms 时间无穷
  if (nfds > 0) {
    for (int i = 0; i < nfds; i++) {
      FdContext *fm = static_cast<FdContext *>(events[i].data.ptr);
      fm->call_back(fm->fd, events[i].events, fm);
    }
  } else if (nfds == 0) {
    std::cout << __FILE__ << "time  out" << std::endl;
  } else {
    switch (errno) {
      case EBADF: {
        std::cout << __FILE__ << "epfd is not a valid file descriptor." << std::endl;
        break;
      }
      case EFAULT: {
        std::cout << __FILE__ << "The memory area pointed to by events is not accessible with write permissions." << std::endl;
        break;
      }
      case EINTR: {
        std::cout << __FILE__ << "The call was interrupted by a signal." << std::endl;
        break;
      }
      case EINVAL: {
        std::cout << __FILE__ << "epfd is not an epoll file descriptor." << std::endl;
        break;
      }
      default: {
        std::cout << __FILE__ << "epoll unkonw error." << std::endl;
      }
    }
  }
}

int main() {
  GlobalArg* arg = GlobalArg::GetInstance();
  arg->sm = new StateMachine();
  arg->sm->AuthPic(nullptr, 0, 0, nullptr, 0, 0);
//  InitSystem();
//  arg->sm->Register();

  return 0;
}
