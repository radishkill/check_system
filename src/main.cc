#include <stddef.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include "camera_manager.h"
#include "cmdline.h"
#include "constant.h"
#include "eventmanager.h"
#include "global_arg.h"
#include "hostcontroller.h"
#include "key_file.h"
#include "laser.h"
#include "lcd.h"
#include "led.h"
#include "mutils.h"
#include "state_machine.h"
#include "usart.h"
#include "authpic.h"

using check_system::CameraManager;
using check_system::EventManager;
using check_system::GlobalArg;
using check_system::HostController;
using check_system::KeyFile;
using check_system::Laser;
using check_system::Lcd;
using check_system::LedController;
using check_system::StateMachine;

std::thread timer_thread_;

void SimpleTimer() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int i = 0;
  while (1) {
    if (global_arg->led->laser_blink_ &&
        ((i * 100) % global_arg->led->laser_blink_ == 0)) {
      if (global_arg->led->laser_status_) {
        global_arg->led->LaserLed(0);
      } else {
        global_arg->led->LaserLed(1);
      }
    }
    if (global_arg->led->lcd_blink_ &&
        ((i * 100) % global_arg->led->lcd_blink_ == 0)) {
      if (global_arg->led->lcd_status_) {
        global_arg->led->LcdLed(0);
      } else {
        global_arg->led->LcdLed(1);
      }
    }
    if (global_arg->led->cmos_blink_ &&
        ((i * 100) % global_arg->led->cmos_blink_ == 0)) {
      if (global_arg->led->cmos_status_) {
        global_arg->led->CmosLed(0);
      } else {
        global_arg->led->CmosLed(1);
      }
    }
    if (global_arg->led->error_blink_ &&
        ((i * 100) % global_arg->led->error_blink_ == 0)) {
      if (global_arg->led->error_status_) {
        global_arg->led->ErrorLed(0);
      } else {
        global_arg->led->ErrorLed(1);
      }
    }
    if (global_arg->laser) {
      if (global_arg->laser->ttl >= 0) {
        global_arg->laser->ttl--;
      }
      if (global_arg->laser->ttl == 0 && global_arg->laser->GetStatus()) {
        global_arg->laser->CloseLaser();
      }
    }

    Utils::MSleep(100);
    i++;
    if (i == 1000000) i = 0;
  }
}

void InitSystem() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  global_arg->led = new LedController();

  global_arg->em = new check_system::EventManager();

  InitAuth();

  if (!global_arg->no_laser_flag) {
    global_arg->laser = new Laser(check_system::kLaserAddr);
    if (!global_arg->laser->IsOpen()) {
      global_arg->led->laser_blink_ = 200;
      global_arg->led->lcd_blink_ = 200;
      global_arg->led->cmos_blink_ = 200;
      global_arg->led->error_blink_ = 200;
      std::cout << "laser connect error!!" << std::endl;
      // return;
    } else {
      std::cout << "laser connect ok!!" << std::endl;
    }
  }

  global_arg->camera = new CameraManager();
  if (global_arg->camera->is_open_flag_ == -1) {
    global_arg->led->laser_blink_ = 200;
    global_arg->led->lcd_blink_ = 200;
    global_arg->led->cmos_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "camera connect error!!" << std::endl;
  } else {
    std::cout << "camera connect ok!!" << std::endl;
  }
  //从配置文件中读相机的配置
  if (!global_arg->camera_config_file_addr.empty()) {
    // std::cout << "config addr:" << global_arg->camera_config_file_addr <<
    // std::endl;
    global_arg->camera->ReadParameterFromFile(
        global_arg->camera_config_file_addr.c_str());
  }
  global_arg->camera->InitCameraByDefault();
  global_arg->camera->ShowCameraBaseConfig();

  //设置曝光时间
  if (global_arg->exposion_time != -1) {
    std::cout << std::fixed << global_arg->exposion_time << " "
              << global_arg->analog_gain << std::endl;
    global_arg->camera->SetExposureTimeAndAnalogGain(global_arg->exposion_time,
                                                     global_arg->analog_gain);
  }

  //设置兴趣区域
  (global_arg->roi_x == -1 || global_arg->roi_y == -1 ||
   global_arg->roi_w == -1 || global_arg->roi_h == -1)
      ?: global_arg->camera->SetRoi(global_arg->roi_x, global_arg->roi_y,
                                    global_arg->roi_w, global_arg->roi_h);
  global_arg->camera->Play();

  global_arg->lcd = new Lcd("/dev/fb0");
  if (!global_arg->lcd->IsOpen()) {
    global_arg->led->laser_blink_ = 200;
    global_arg->led->lcd_blink_ = 200;
    global_arg->led->cmos_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "lcd buffer connect error!!" << std::endl;
    return;
  } else {
    std::cout << "lcd buffer connect ok!!" << std::endl;

    if (global_arg->lcd_wh != -1)
      global_arg->lcd->SetRect(global_arg->lcd_wh, global_arg->lcd_wh);
  }

  global_arg->host = new HostController(check_system::kHostAddr);
  if (!global_arg->host->IsOpen()) {
    global_arg->led->laser_blink_ = 200;
    global_arg->led->lcd_blink_ = 200;
    global_arg->led->cmos_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "host tty connect error!!" << std::endl;
    return;
  } else {
    std::cout << "host tty connect ok!!" << std::endl;
  }

  global_arg->key_file = new KeyFile("./resource/PUFData");
  if (!global_arg->key_file->Is_Open()) {
    std::cout << "key file have some wrong!!!" << std::endl;
  } else {
    std::cout << "key file ok" << std::endl;
  }

  // led闪烁提示结果匹配
  for (int i = 0; i < 3; i++) {
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);
    global_arg->led->CmosLed(1);
    global_arg->led->LaserLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
  }
  //开始自检
  std::thread th(std::bind(&StateMachine::RunMachine, global_arg->sm,
                           StateMachine::kSelfTest));
  th.detach();

  //下面部分是用来打开button的

  //认证按钮
  std::stringstream ss;
  int fd;
  char key;
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kRegisterButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kRegisterButtonNumber
              << std::endl;
    return;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* global_arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kAuthButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      std::thread th(std::bind(&StateMachine::RunMachine, global_arg->sm,
                               StateMachine::kAuth));
      th.detach();
    }
  });
  ss.str("");

  //注册
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kAuthButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kAuthButtonNumber
              << std::endl;
    return;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* global_arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kAuthButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      std::thread th(std::bind(&StateMachine::RunMachine, global_arg->sm,
                               StateMachine::kRegister));
      th.detach();
    }
  });
  ss.str("");

  //中断按钮
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kInterruptButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kInterruptButtonNumber
              << std::endl;
    return;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* global_arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kInterruptButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      //抬起
      std::cout << "set interrupt flag" << std::endl;
      global_arg->interrupt_flag = 1;
    } else {
      
    }
  });
  ss.str("");

  //自检
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kCheckSelfButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kCheckSelfButtonNumber
              << std::endl;
    return;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    GlobalArg* global_arg = GlobalArg::GetInstance();
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kCheckSelfButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      unsigned long up_time = std::time(nullptr);
      //抬起按钮
      //[10, 20)时触发删除操作
      if ((up_time - global_arg->check_btn_down >= 10) &&
          (up_time - global_arg->check_btn_down < 20) &&
          global_arg->interrupt_flag) {
        std::cout << "delete all key!!\n";
        global_arg->key_file->DeleteAll();
        //如果删除成功 闪错误灯3秒
        for (int i = 0; i < 5; i++) {
          global_arg->led->CmosLed(1);
          global_arg->led->LaserLed(1);
          global_arg->led->LcdLed(1);
          global_arg->led->ErrorLed(1);
          Utils::MSleep(500);
          global_arg->led->CmosLed(0);
          global_arg->led->LaserLed(0);
          global_arg->led->LcdLed(0);
          global_arg->led->ErrorLed(0);
          Utils::MSleep(500);
        }
      }
      //抬起时间大于10s
      //[10, 20)时触发删除操作
      if ((up_time - global_arg->check_btn_down >= 10) &&
          (up_time - global_arg->check_btn_down < 20) &&
          !global_arg->interrupt_flag) {
        std::cout << "delete all key except admin!!\n";
        global_arg->key_file->DeleteAllExceptAdmin();
        //如果删除成功 闪错误灯3秒
        for (int i = 0; i < 3; i++) {
          global_arg->led->CmosLed(1);
          global_arg->led->LaserLed(1);
          global_arg->led->LcdLed(1);
          global_arg->led->ErrorLed(1);
          Utils::MSleep(500);
          global_arg->led->CmosLed(0);
          global_arg->led->LaserLed(0);
          global_arg->led->LcdLed(0);
          global_arg->led->ErrorLed(0);
          Utils::MSleep(500);
        }
      }
      //[2, 10)
      if ((up_time - global_arg->check_btn_down) >= 2 &&
          (up_time - global_arg->check_btn_down) < 10) {
        std::thread th(std::bind(&StateMachine::RunMachine, global_arg->sm,
                                 StateMachine::kOther));
        th.detach();
      }
      if ((up_time - global_arg->check_btn_down) <= 1) {
        std::thread th(std::bind(&StateMachine::RunMachine, global_arg->sm,
                                 StateMachine::kSelfTest));
        th.detach();
      }
      global_arg->check_btn_down = 0;
    } else {
      //按下自检按钮
      global_arg->check_btn_down = std::time(nullptr);
      global_arg->interrupt_flag = 0;
    }
  });
  ss.str("");

  //定时器线程
  timer_thread_ = std::thread(SimpleTimer);

  global_arg->em->ListenFd(global_arg->host->GetFd(), EventManager::kEventRead,
                           []() {
                             GlobalArg* global_arg = GlobalArg::GetInstance();
                             std::cout << "recv data " << std::endl;
                             global_arg->host->RecvData();
                           });
}

int main(int argc, char** argv) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  global_arg->no_laser_flag = true;
  check_system::InitCmdLine(argc, argv);
  global_arg->sm = new StateMachine();
  InitSystem();
  global_arg->em->Start(1);
  pause();
  return 0;
}
