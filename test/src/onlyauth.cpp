#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <boost/program_options.hpp>

#include "authpic.h"
#include "camera_manager.h"
#include "constant.h"
#include "eventmanager.h"
#include "key_file.h"
#include "laser.h"
#include "lcd.h"
#include "led.h"
#include "usart.h"
#include "mutils.h"

namespace po = boost::program_options;
using check_system::CameraManager;
using check_system::EventManager;
using check_system::KeyFile;
using check_system::Laser;
using check_system::Lcd;
using check_system::LedController;

class GlobalArg {
 public:
  Laser *laser;
  CameraManager *camera;
  KeyFile *key_file;
  Lcd *lcd;
  EventManager *em;
  LedController *led;
  double exposion_time;
  int laser_current;
  int resolution_index;
  int roi_x, roi_y, roi_w, roi_h;
  bool no_button_flag;
  bool no_laser_flag;
  bool no_lcd_flag;
  bool no_led_flag;
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

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("no-button", "")("no-laser", "")("no-lcd", "");
  desc.add_options()("no-led", "");
  desc.add_options()("mid-save", po::value<std::string>(&global_arg->mid_save_addr),
                     "");
  desc.add_options()("resolution-index",
                     po::value<int>(&global_arg->resolution_index)->default_value(-1),
                     "4 8=320x240 15 19 -1=not set");
  desc.add_options()("ROI-x", po::value<int>(&global_arg->roi_x)->default_value(-1),
                     "");
  desc.add_options()("ROI-y", po::value<int>(&global_arg->roi_y)->default_value(-1),
                     "");
  desc.add_options()("ROI-w", po::value<int>(&global_arg->roi_w)->default_value(-1),
                     "");
  desc.add_options()("ROI-h", po::value<int>(&global_arg->roi_h)->default_value(-1),
                     "");
  desc.add_options()("exposion-time",
                     po::value<double>(&global_arg->exposion_time)->default_value(-1),
                     "us");
  desc.add_options()("laser-current",
                     po::value<int>(&global_arg->laser_current)->default_value(-1),
                     "uA");
  desc.add_options()("lcd-width",
                     po::value<int>(&global_arg->lcd_width)->default_value(-1), "");
  desc.add_options()("lcd-height",
                     po::value<int>(&global_arg->lcd_height)->default_value(-1), "");
  desc.add_options()("camera-gamma",
                     po::value<int>(&global_arg->camera_gamma)->default_value(-1), "");
  desc.add_options()("camera-contrast",
                     po::value<int>(&global_arg->camera_contrast)->default_value(-1),
                     "");
  desc.add_options()("camera-saturation",
                     po::value<int>(&global_arg->camera_saturation)->default_value(-1),
                     "");
  desc.add_options()("camera-sharpness",
                     po::value<int>(&global_arg->camera_sharpness)->default_value(-1),
                     "");
  desc.add_options()("camera-config-addr",
                     po::value<std::string>(&global_arg->camera_config_file_addr), "");
  desc.add_options()(
      "camera-lut-mode",
      po::value<int>(&global_arg->camera_lut_mode)->default_value(-1),
      "GAMMA_DYNAMIC_MODE = 0, GAMMA_PRESET_MODE, GAMMA_USER_MODE");
  desc.add_options()("stable-flag",po::value<int>(&global_arg->stable_flag)->default_value(-1), "no rand index");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
  if (vm.count("no-button")) {
    global_arg->no_button_flag = true;
  }
  if (vm.count("no-laser")) {
    global_arg->no_laser_flag = true;
  }
  if (vm.count("no-lcd")) {
    global_arg->no_lcd_flag = true;
  }
  if (vm.count("no-led")) {
    global_arg->no_led_flag = true;
  }
}
std::vector<int> empty_pair_list_;
std::vector<int> available_pair_list_;

void SavePic(cv::Mat pic, int key_id, int index, std::string info) {
  if (global_arg->mid_save_addr.empty()) {
    return;
  }
  std::string addr = global_arg->mid_save_addr;
  addr = addr + std::string("/PUF") + Utils::DecToStr(key_id, 2) + "_Pic" +
         Utils::DecToStr(index, 4) + info + ".bmp";
  cv::imwrite(addr, pic);
}

//插入检测算法
int CheckKeyInsert() {
  std::srand(std::time(nullptr));
  int seed = std::rand();
  if (global_arg->lcd) global_arg->lcd->ShowBySeed(seed);
  global_arg->camera->GetPic();
  return global_arg->camera->CheckPic(30, 80);
}
int CheckPairStore(int id) {
  //清空empty_pairs
  std::vector<int>().swap(empty_pair_list_);
  std::vector<int>().swap(available_pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (global_arg->key_file->IsSeedAvailable(id, i)) {
      available_pair_list_.push_back(i);
    } else {
      empty_pair_list_.push_back(i);
    }
  }
  return 0;
}

int CheckKey(int key_id) {
  int ret = 0;
  CheckPairStore(key_id);
  if (available_pair_list_.empty()) {
    std::cout << "available pair is empty keyid = " << key_id << std::endl;
    return -1;
  }
  int n = 10;
  int available_num = available_pair_list_.size();
  double result;
  while (n--) {
    int index;
    if (global_arg->stable_flag == -1)
      index = std::rand() % available_pair_list_.size();
    else
      index = global_arg->stable_flag;
    int key_id_index = available_pair_list_[index];
    //如果随机到的是已经被删除了的
    if (key_id_index == -1) {
      int i = index;
      while (i++) {
        if (i == available_pair_list_.size()) i = 0;
        //找到了一个没有被删除的钥匙
        if (available_pair_list_[i] != -1) {
          index = i;
          key_id_index = available_pair_list_[index];
          break;
        }
        //钥匙已经被删完了
        if (i == index) {
          std::cout << "available pair is empty keyid = " << key_id
                    << std::endl;
          return 0;
        }
      }
    }
    int seed = global_arg->key_file->GetSeed(key_id, key_id_index);
    if (seed == -1) {
      std::cout << "fatal error seed = -1" << std::endl;
      return -1;
    }
    if (global_arg->lcd) global_arg->lcd->ShowBySeed(seed);
    ret = global_arg->key_file->ReadPicAsBmp(key_id, key_id_index);
    if (ret == -1) continue;
    ret = global_arg->camera->GetPic();
    if (ret == -1) continue;
    cv::Mat pic1 = global_arg->key_file->GetMatImage();
    cv::Mat pic2 = global_arg->camera->GetPicMat();
    cv::Mat pic3 = cv::Mat(global_arg->lcd->GetFbHeight(), global_arg->lcd->GetFbWidth(), CV_8UC4, global_arg->lcd->GetFrameBuffer());
    
    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(pic1, pic2);
    
    SavePic(pic1, key_id, key_id_index, std::string("_base") + std::to_string(seed) + std::string("_") + std::to_string(result));
    SavePic(pic2, key_id, key_id_index, std::string("_auth") + std::to_string(seed) + std::string("_") + std::to_string(result));
    SavePic(pic3, key_id, key_id_index, std::string("_lcd_") + std::to_string(seed) + std::string("_") + std::to_string(result));
    
    //删除本次循环使用的Seed文件及其对应的Pic文件
    std::cout << "delete old pair index = " << key_id_index << std::endl;
    global_arg->key_file->DeleteSeed(key_id, key_id_index);
    global_arg->key_file->DeletePic(key_id, key_id_index);
    available_pair_list_[index] = -1;

    //值越小说明两张图片越相似
    if (result <= check_system::kAuthThreshold) {
      std::cout << "generate new a pair index = " << key_id_index << std::endl;
      //认证通过
      //然后重新生成新的激励对
      int rand_seed = std::rand();
      if (global_arg->lcd) global_arg->lcd->ShowBySeed(rand_seed);
      global_arg->camera->GetPic();

      global_arg->key_file->SetMatImage(global_arg->camera->GetPicMat());
      global_arg->key_file->SavePicAndSeed(key_id, key_id_index, rand_seed);
      return 1;
    }
    available_num--;
    //用完了库里面所有的钥匙
    if (available_num == 0) {
      break;
    }
  }
  return 0;
}

//库定位算法 判断一枚key是否已经建立过数据库了
int FindKey() {
  int ret = 0;
  std::srand(std::time(nullptr));
  int key_id = -1;
  double result;
  std::cout << "find key start " << std::endl;
  while (key_id++ < 100) {
    auto begin_tick = std::chrono::steady_clock::now();

    CheckPairStore(key_id);

    if (available_pair_list_.empty()) continue;
    std::cout << "key id = " << key_id << std::endl;
    std::cout << "available pair list = " << available_pair_list_.size()
              << std::endl;
    std::cout << "empty pair list = " << empty_pair_list_.size() << std::endl;
    int index;
    int key_id_index;
    if (global_arg->stable_flag == -1)
      index = std::rand() % available_pair_list_.size();
    else
      index = global_arg->stable_flag;
    key_id_index = available_pair_list_[index];
    int seed =
        global_arg->key_file->GetSeed(key_id, key_id_index);

    if (global_arg->lcd) global_arg->lcd->ShowBySeed(seed);
    ret = global_arg->key_file->ReadPicAsBmp(key_id, key_id_index);
    if (ret == -1) continue;
    ret = global_arg->camera->GetPic();
    if (ret == -1) continue;
    cv::Mat pic1 = global_arg->key_file->GetMatImage();
    cv::Mat pic2 = global_arg->camera->GetPicMat();
    cv::Mat pic3 = cv::Mat(global_arg->lcd->GetFbHeight(), global_arg->lcd->GetFbWidth(), CV_8UC4, global_arg->lcd->GetFrameBuffer());
    
    //验证两张图片
    result = AuthPic(pic1, pic2);

    SavePic(pic1, key_id, key_id_index, std::string("_base") + std::to_string(seed) + std::string("_") + std::to_string(result));
    SavePic(pic2, key_id, key_id_index, std::string("_auth") + std::to_string(seed) + std::string("_") + std::to_string(result));
    SavePic(pic3, key_id, key_id_index, std::string("_lcd_") + std::to_string(seed) + std::string("_") + std::to_string(result));
    
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "auth pic elapsed time :"
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_tick - begin_tick)
                     .count()
              << "ms" << std::endl;
    if (result <= check_system::kAuthThreshold) {
      std::cout << "auth pic ok" << std::endl;
      //找到相应的库
      break;
    } else {
      std::cout << "auth pic fault" << std::endl;
    }
  }
  if (key_id >= 100) key_id = -1;

  return key_id;
}

//认证
int Authentication() {
  int ret = 0;

  assert(global_arg->camera != nullptr);

  if (CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    //认证失败
    return -1;
  }
  std::cout << "key insert" << std::endl;
  auto begin_tick = std::chrono::steady_clock::now();
  int key_id = FindKey();
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  if (key_id == -1) {
    std::cout << "can't find key" << std::endl;
    //认证失败
    return -1;
  }

  begin_tick = std::chrono::steady_clock::now();
  ret = CheckKey(key_id);
  end_tick = std::chrono::steady_clock::now();
  std::cout << "check key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  //认证失败 或者发生错误check key
  if (ret == 0 || ret == -1) {
    return -1;
  }
  std::cout << "auth success!!!" << std::endl;
  return 0;
}

int main(int argc, char **argv) {
  global_arg = new GlobalArg();
  InitCmdLine(argc, argv);
  if (!global_arg->no_button_flag) {
    global_arg->em = new EventManager();
  }
  if (!global_arg->no_led_flag) {
    global_arg->led = new LedController();
    global_arg->led->LaserLed(0);
    global_arg->led->CmosLed(0);
    global_arg->led->LcdLed(0);
    global_arg->led->ErrorLed(0);
  }
  if (!global_arg->no_laser_flag) {
    global_arg->laser = new Laser("/dev/ttyS0");
    global_arg->laser->ForceCheck();
    if (global_arg->laser_current != -1) global_arg->laser->SetCurrent(global_arg->laser_current);
    global_arg->laser->ForceOpen();
  }
  if (!global_arg->no_lcd_flag) {
    global_arg->lcd = new Lcd("/dev/fb0");
    if (global_arg->lcd_width != -1 && global_arg->lcd_height != -1)
      global_arg->lcd->SetRect(global_arg->lcd_width, global_arg->lcd_height);
  }
  global_arg->camera = new CameraManager();
  if (!global_arg->camera->IsOpen()) {
    return -1;
  }
  //设置lut模式
  global_arg->camera_lut_mode == -1 ?: global_arg->camera->SetLutMode(global_arg->camera_lut_mode);
  //设置分辨率
  global_arg->resolution_index == -1
      ?: global_arg->camera->SetResolution(global_arg->resolution_index);
  //设置曝光时间
  global_arg->exposion_time == -1 ?: global_arg->camera->SetExposureTimeAndAnalogGain(global_arg->exposion_time, -1);
  //设置兴趣区域
  (global_arg->roi_x == -1 || global_arg->roi_y == -1 || global_arg->roi_w == -1 || global_arg->roi_h == -1)
      ?: global_arg->camera->SetRoi(global_arg->roi_x, global_arg->roi_y, global_arg->roi_w, global_arg->roi_h);
  global_arg->camera_contrast == -1 ?: global_arg->camera->SetContrast(global_arg->camera_contrast);
  global_arg->camera_gamma == -1 ?: global_arg->camera->SetGamma(global_arg->camera_gamma);
  //设置饱和度 这个值对于黑白相机无效
  global_arg->camera_saturation == -1
      ?: global_arg->camera->SetSaturation(global_arg->camera_saturation);
  global_arg->camera_sharpness == -1
      ?: global_arg->camera->SetSharpness(global_arg->camera_sharpness);
  if (!global_arg->camera_config_file_addr.empty())
    global_arg->camera->ReadParameterFromFile(global_arg->camera_config_file_addr.c_str());

  global_arg->camera->Play();

  global_arg->key_file = new KeyFile("../res/PUFData");

  if (global_arg->em == nullptr) {
    //直接运行
    int ret;
    auto begin_tick = std::chrono::steady_clock::now();
    ret = Authentication();
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "authentication sum elapsed time :"
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                                         begin_tick).count()
              << "ms" << std::endl;
    if (ret != 0) {
      if (global_arg->led) global_arg->led->ErrorLed(1);
    } else {
      if (global_arg->led) global_arg->led->ErrorLed(1);
      Utils::MSleep(500);
      if (global_arg->led) global_arg->led->ErrorLed(0);
      Utils::MSleep(500);
      if (global_arg->led) global_arg->led->ErrorLed(1);
      Utils::MSleep(500);
      if (global_arg->led) global_arg->led->ErrorLed(0);
      Utils::MSleep(500);
    }
    if (global_arg->laser) global_arg->laser->ForceClose();
    global_arg->camera->Pause();
    global_arg->camera->Uninit();
    return 0;
  }
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
      int ret = Authentication();
      if (ret != 0) {
        if (global_arg->led) global_arg->led->ErrorLed(1);
      } else {
        if (global_arg->led) global_arg->led->ErrorLed(1);
        Utils::MSleep(500);
        if (global_arg->led) global_arg->led->ErrorLed(0);
        Utils::MSleep(500);
        if (global_arg->led) global_arg->led->ErrorLed(1);
        Utils::MSleep(500);
        if (global_arg->led) global_arg->led->ErrorLed(0);
        Utils::MSleep(500);
      }
    }
  });
  ss.str("");
  global_arg->em->Start(1);
}