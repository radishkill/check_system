#include <fcntl.h>

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "constant.h"
#include "eventmanager.h"
#include "lcd.h"
#include "led.h"
#include "mutils.h"

namespace po = boost::program_options;
using check_system::EventManager;
using check_system::Lcd;
using check_system::LedController;

class GlobalArg {
 public:
  EventManager *em;
  LedController *led;
  Lcd *lcd;

  int button1, button2, button3, button4;
  int led1, led2, led3, led4;
  int with_lcd;
  std::vector<std::pair<int, int>> seeds;
  int cur_seed_index;
  bool reverse;

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
  desc.add_options()("with-lcd",
                     po::value<int>(&global_arg->with_lcd)->default_value(-1),
                     "to controll lcd");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file("./base.cfg", desc), vm);
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
  if (global_arg->with_lcd != -1) {
    global_arg->lcd = new Lcd("/dev/fb0");
    global_arg->lcd->SetRect(global_arg->with_lcd, global_arg->with_lcd);
  }
  std::srand(std::time(nullptr));
  global_arg->cur_seed_index = 0;
  global_arg->reverse = false;

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
      if (global_arg->lcd != nullptr) {
        global_arg->reverse = false;
        if (global_arg->seeds.empty()) {
          global_arg->seeds.push_back(
              std::make_pair(std::rand(), global_arg->lcd->rect_width_));
        }
        if (global_arg->reverse) {
          global_arg->cur_seed_index = global_arg->cur_seed_index == 0
                                           ? 0
                                           : global_arg->cur_seed_index - 1;
        } else {
          global_arg->cur_seed_index++;
          if (global_arg->cur_seed_index + 1 > global_arg->seeds.size()) {
            global_arg->seeds.push_back(
                std::make_pair(std::rand(), global_arg->lcd->rect_width_));
          }
        }
        global_arg->lcd->SetRect(
            global_arg->seeds[global_arg->cur_seed_index].second,
            global_arg->seeds[global_arg->cur_seed_index].second);
        global_arg->lcd->ShowBySeed(
            global_arg->seeds[global_arg->cur_seed_index].first);
      }
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
          if (global_arg->lcd != nullptr) {
            if (global_arg->lcd->rect_width_ > 1)
              global_arg->lcd->rect_width_--;
            if (global_arg->lcd->rect_height_ > 1)
              global_arg->lcd->rect_height_--;
            if (!global_arg->seeds.empty()) {
              global_arg->seeds[global_arg->cur_seed_index].second = global_arg->lcd->rect_width_;
              global_arg->lcd->ShowBySeed(
                  global_arg->seeds[global_arg->cur_seed_index].first);
            }
          }
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
      if (global_arg->lcd != nullptr) {
        global_arg->lcd->rect_width_++;
        global_arg->lcd->rect_height_++;
        if (!global_arg->seeds.empty()) {
          global_arg->seeds[global_arg->cur_seed_index].second = global_arg->lcd->rect_width_;
          global_arg->lcd->ShowBySeed(
              global_arg->seeds[global_arg->cur_seed_index].first);
        }
      }
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
          global_arg->reverse = true;
          if (global_arg->lcd != nullptr) {
            if (global_arg->seeds.empty()) {
              global_arg->seeds.push_back(
                  std::make_pair(std::rand(), global_arg->lcd->rect_width_));
            }
            if (global_arg->reverse) {
              global_arg->cur_seed_index = global_arg->cur_seed_index == 0
                                               ? 0
                                               : global_arg->cur_seed_index - 1;
            } else {
              global_arg->cur_seed_index++;
              if (global_arg->cur_seed_index + 1 > global_arg->seeds.size()) {
                global_arg->seeds.push_back(
                    std::make_pair(std::rand(), global_arg->lcd->rect_width_));
              }
            }
            global_arg->lcd->SetRect(
                global_arg->seeds[global_arg->cur_seed_index].second,
                global_arg->seeds[global_arg->cur_seed_index].second);
            global_arg->lcd->ShowBySeed(
                global_arg->seeds[global_arg->cur_seed_index].first);
          }
          global_arg->led->ErrorLed(1);
        } else {
          global_arg->led->ErrorLed(0);
        }
      });
  ss.str("");

  global_arg->em->Start(1);
}