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
#include "utils.h"

namespace po = boost::program_options;
using check_system::EventManager;
using check_system::LedController;


class GlobalArg {
 public:
  EventManager *em;
  LedController *led;
  int exposion_time;
  int laser_current;
  int resolution_index;
  int roi_x, roi_y, roi_w, roi_h;
  bool no_button_flag;
  bool no_laser_flag;
  bool no_lcd_flag;
  int lcd_width;
  int lcd_height;

  int camera_gamma;
  int camera_contrast;
  int camera_saturation;
  int camera_sharpness;

  int camera_lut_mode;
  int stable_flag;

  std::string camera_config_file_addr;
  std::string mid_save_addr;

 private:
};
GlobalArg *global_arg;

bool is_running = false;

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("no-button", "")("no-laser", "")("no-lcd", "");
  desc.add_options()("no-led", "");
  desc.add_options()("mid-save",
                     po::value<std::string>(&global_arg->mid_save_addr), "");
  desc.add_options()(
      "resolution-index",
      po::value<int>(&global_arg->resolution_index)->default_value(-1),
      "4 8=320x240 15 19 -1=not set");
  desc.add_options()("ROI-x",
                     po::value<int>(&global_arg->roi_x)->default_value(-1), "");
  desc.add_options()("ROI-y",
                     po::value<int>(&global_arg->roi_y)->default_value(-1), "");
  desc.add_options()("ROI-w",
                     po::value<int>(&global_arg->roi_w)->default_value(-1), "");
  desc.add_options()("ROI-h",
                     po::value<int>(&global_arg->roi_h)->default_value(-1), "");
  desc.add_options()(
      "exposion-time",
      po::value<int>(&global_arg->exposion_time)->default_value(-1), "us");
  desc.add_options()(
      "laser-current",
      po::value<int>(&global_arg->laser_current)->default_value(-1), "uA");
  desc.add_options()("lcd-width",
                     po::value<int>(&global_arg->lcd_width)->default_value(-1),
                     "");
  desc.add_options()("lcd-height",
                     po::value<int>(&global_arg->lcd_height)->default_value(-1),
                     "");
  desc.add_options()(
      "camera-gamma",
      po::value<int>(&global_arg->camera_gamma)->default_value(-1), "");
  desc.add_options()(
      "camera-contrast",
      po::value<int>(&global_arg->camera_contrast)->default_value(-1), "");
  desc.add_options()(
      "camera-saturation",
      po::value<int>(&global_arg->camera_saturation)->default_value(-1), "");
  desc.add_options()(
      "camera-sharpness",
      po::value<int>(&global_arg->camera_sharpness)->default_value(-1), "");
  desc.add_options()(
      "camera-config-addr",
      po::value<std::string>(&global_arg->camera_config_file_addr), "");
  desc.add_options()(
      "camera-lut-mode",
      po::value<int>(&global_arg->camera_lut_mode)->default_value(-1),
      "GAMMA_DYNAMIC_MODE = 0, GAMMA_PRESET_MODE, GAMMA_USER_MODE");
  desc.add_options()(
      "stable-flag",
      po::value<int>(&global_arg->stable_flag)->default_value(-1),
      "no rand index");

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

  global_arg->led = new LedController();
  global_arg->led->LaserLed(0);
  global_arg->led->CmosLed(0);
  global_arg->led->LcdLed(0);
  global_arg->led->ErrorLed(0);

  int fd;
  char key;
  std::stringstream ss;
  //认证
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kAuthButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kAuthButtonNumber
              << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kAuthButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      global_arg->led->LaserLed(1);
    } else {
      global_arg->led->LaserLed(0);
    }
  });
  ss.str("");
  //注册
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kRegisterButtonNumber) << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cout << "can not open :" << check_system::kRegisterButtonNumber
              << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kAuthButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      global_arg->led->LaserLed(1);
    } else {
      global_arg->led->LaserLed(0);
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
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kInterruptButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      global_arg->led->LaserLed(1);
    } else {
      global_arg->led->LaserLed(0);
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
    return 0;
  }
  read(fd, &key, 1);
  global_arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kCheckSelfButtonNumber << " " << key
              << std::endl;
    if (key == 0x31) {
      global_arg->led->LaserLed(1);
    } else {
      global_arg->led->LaserLed(0);
    }
  });
  ss.str("");

  global_arg->em->Start(1);
}