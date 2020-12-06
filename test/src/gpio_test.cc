#include <fcntl.h>

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "constant.h"
#include "eventmanager.h"
#include "led.h"
#include "mutils.h"

namespace po = boost::program_options;
using check_system::EventManager;
using check_system::LedController;

class GlobalArg {
 public:
  EventManager *em;
  LedController *led;
  int button1, button2, button3, button4;
  int led1, led2, led3, led4;

 private:
};
GlobalArg *global_arg;

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()(
      "button1", po::value<int>(&global_arg->button1)->default_value(-1), "");
  desc.add_options()(
      "button2", po::value<int>(&global_arg->button2)->default_value(-1), "");
  desc.add_options()(
      "button3", po::value<int>(&global_arg->button3)->default_value(-1), "");
  desc.add_options()(
      "button4", po::value<int>(&global_arg->button4)->default_value(-1), "");
  desc.add_options()("led1",
                     po::value<int>(&global_arg->led1)->default_value(-1), "");
  desc.add_options()("led2",
                     po::value<int>(&global_arg->led2)->default_value(-1), "");
  desc.add_options()("led3",
                     po::value<int>(&global_arg->led3)->default_value(-1), "");
  desc.add_options()("led4",
                     po::value<int>(&global_arg->led4)->default_value(-1), "");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
}
std::vector<int> empty_pair_list_;
std::vector<int> available_pair_list_;

int main(int argc, char **argv) {
  global_arg = new GlobalArg();
  InitCmdLine(argc, argv);

  global_arg->em = new EventManager();
  global_arg->led = new LedController(global_arg->led1, global_arg->led2,
                                      global_arg->led3, global_arg->led4);
  int auth_button_id = global_arg->button1 == -1
                           ? check_system::kAuthButtonNumber
                           : global_arg->button1;
  int register_button_id = global_arg->button2 == -1
                               ? check_system::kRegisterButtonNumber
                               : global_arg->button2;
  int int_button_id = global_arg->button3 == -1
                          ? check_system::kInterruptButtonNumber
                          : global_arg->button3;
  int check_button_id = global_arg->button4 == -1
                            ? check_system::kCheckSelfButtonNumber
                            : global_arg->button4;

  int fd;
  char key;
  std::stringstream ss;
  //认证
  ss << "/sys/class/gpio/gpio" << std::to_string(auth_button_id) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << auth_button_id << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd, auth_button_id]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << auth_button_id << " " << key << std::endl;
    if (key == 0x31) {
      global_arg->led->LaserLed(1);
    } else {
      global_arg->led->LaserLed(0);
    }
  });
  ss.str("");

  //注册
  ss << "/sys/class/gpio/gpio" << std::to_string(register_button_id)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << register_button_id << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(
      fd, EventManager::kEventPri, [fd, register_button_id]() {
        char key;
        lseek(fd, 0, SEEK_SET);
        read(fd, &key, 1);
        std::cout << "button " << register_button_id << " " << key << std::endl;

        if (key == 0x31) {
          global_arg->led->CmosLed(1);
        } else {
          global_arg->led->CmosLed(0);
        }
      });
  ss.str("");

  //中断按钮
  ss << "/sys/class/gpio/gpio" << std::to_string(int_button_id) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << int_button_id << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd, int_button_id]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << int_button_id << " " << key << std::endl;

    if (key == 0x31) {
      global_arg->led->LcdLed(1);
    } else {
      global_arg->led->LcdLed(0);
    }
  });
  ss.str("");

  //自检
  ss << "/sys/class/gpio/gpio" << std::to_string(check_button_id) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_button_id << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(
      fd, EventManager::kEventPri, [fd, check_button_id]() {
        char key;
        lseek(fd, 0, SEEK_SET);
        read(fd, &key, 1);
        std::cout << "button " << check_button_id << " " << key << std::endl;
        if (key == 0x31) {
          global_arg->led->ErrorLed(1);
        } else {
          global_arg->led->ErrorLed(0);
        }
      });
  ss.str("");

  global_arg->em->Start(1);
}