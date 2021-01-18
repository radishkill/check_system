#include <stdio.h>
#include <string.h>

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "CKCameraInterface.h"
#include "camera_manager.h"
#include "lcd.h"
#include "mutils.h"

using namespace std;
using check_system::CameraManager;
using check_system::Lcd;
namespace po = boost::program_options;

class GlobalArg {
 public:
  CameraManager *camera;
  Lcd *lcd;
  int exposion_time;
  int laser_current;
  int resolution_index;
  int roi_x, roi_y, roi_w, roi_h;
  bool no_button_flag;
  bool no_laser_flag;
  bool no_lcd;
  bool save_lcd;
  int lcd_width;
  int lcd_height;
  int lcd_wh;

  int camera_gamma;
  int camera_contrast;
  int camera_sharpness;

  int camera_lut_mode;
  int stable_flag;
  int n1;
  int n2;
  int seed;
  std::vector<int> rgb;
  std::string temp;
  std::string lcd_pic_addr;

  std::string camera_config_file_addr;
  std::string mid_save_addr;

 private:
};

GlobalArg *global_arg;

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("no-button", "")("no-laser", "");
  desc.add_options()(
      "no-lcd", po::bool_switch(&global_arg->no_lcd)->default_value(false), "");
  desc.add_options()("no-led", "");
  desc.add_options()(
      "mid-save",
      po::value<std::string>(&global_arg->mid_save_addr)->default_value(""),
      "");
  desc.add_options()(
      "save-lcd", po::bool_switch(&global_arg->save_lcd)->default_value(false),
      "");
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
      "exposion-time,e",
      po::value<int>(&global_arg->exposion_time)->default_value(-1), "us");
  desc.add_options()(
      "laser-current",
      po::value<int>(&global_arg->laser_current)->default_value(-1), "uA");
  desc.add_options()(
      "lcd-wh", po::value<int>(&global_arg->lcd_wh)->default_value(-1), "");
  desc.add_options()(
      "camera-gamma",
      po::value<int>(&global_arg->camera_gamma)->default_value(-1), "");
  desc.add_options()(
      "camera-contrast",
      po::value<int>(&global_arg->camera_contrast)->default_value(-1), "");
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
  desc.add_options()("n1", po::value<int>(&global_arg->n1)->default_value(1),
                     "");
  desc.add_options()("n2", po::value<int>(&global_arg->n2)->default_value(1),
                     "");
  desc.add_options()(
      "temp,T", po::value<std::string>(&global_arg->temp)->default_value(""),
      "");
  desc.add_options()("seed,S",
                     po::value<int>(&global_arg->seed)->default_value(-1), "");
  desc.add_options()(
      "rgb", po::value<std::vector<int>>(&global_arg->rgb)->multitoken(), "");
  desc.add_options()(
      "lcdaddr,A",
      po::value<std::string>(&global_arg->lcd_pic_addr)->default_value(""), "");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file("./config/camera.cfg", desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int ret = 0;
  global_arg = new GlobalArg();
  InitCmdLine(argc, argv);
  global_arg->camera = new CameraManager();
  if (global_arg->camera->IsOpen() == -1) {
    return 0;
  }
  if (!global_arg->camera_config_file_addr.empty()) {
    global_arg->camera->ReadParameterFromFile(
        global_arg->camera_config_file_addr.c_str());
  }
  global_arg->camera->ShowCameraBaseConfig();

  //设置lut模式
  global_arg->camera_lut_mode == -1
      ?: global_arg->camera->SetLutMode(global_arg->camera_lut_mode);
  //设置分辨率
  global_arg->resolution_index == -1
      ?: global_arg->camera->SetResolution(global_arg->resolution_index);
  //设置曝光时间
  global_arg->exposion_time == -1
      ?: global_arg->camera->SetExposureTimeAndAnalogGain(global_arg->exposion_time, -1);

  //设置兴趣区域
  (global_arg->roi_x == -1 || global_arg->roi_y == -1 ||
   global_arg->roi_w == -1 || global_arg->roi_h == -1)
      ?: global_arg->camera->SetRoi(global_arg->roi_x, global_arg->roi_y,
                                    global_arg->roi_w, global_arg->roi_h);

  global_arg->camera_contrast == -1
      ?: global_arg->camera->SetContrast(global_arg->camera_contrast);

  global_arg->camera_gamma == -1
      ?: global_arg->camera->SetGamma(global_arg->camera_gamma);

  global_arg->camera_sharpness == -1
      ?: global_arg->camera->SetSharpness(global_arg->camera_sharpness);

  global_arg->camera->Play();

  if (!global_arg->no_lcd) {
    global_arg->lcd = new Lcd("/dev/fb0");
    if (global_arg->lcd_wh != -1) {
      global_arg->lcd->SetRect(global_arg->lcd_wh, global_arg->lcd_wh);
    }
  }

  int n = global_arg->n1;
  int n2 = global_arg->n2;
  auto begin_tick = std::chrono::steady_clock::now();
  if (global_arg->seed == -1)
    std::srand(time(0));
  else
    std::srand(global_arg->seed);
  for (int i = 0; i < n; i++) {
    if (global_arg->no_lcd) {
    } else if (global_arg->rgb.size() == 3) {
      unsigned char color[4] = {(unsigned char)global_arg->rgb[2],
                                (unsigned char)global_arg->rgb[1],
                                (unsigned char)global_arg->rgb[0], 0xff};
      std::cout << global_arg->rgb[0] << " " << global_arg->rgb[1] << " "
                << global_arg->rgb[2] << std::endl;
      global_arg->lcd->ShowByColor(color[0]);
    } else if (!global_arg->lcd_pic_addr.empty()) {
      cv::Mat lcd_pic =
          cv::imread(global_arg->lcd_pic_addr, cv::IMREAD_UNCHANGED);
      global_arg->lcd->ShowByMat(lcd_pic);
    } else {
      global_arg->lcd->ShowBySeed(std::rand());
    }
    cv::Mat pic1;
    if (!global_arg->no_lcd) {
    pic1 =
        cv::Mat(global_arg->lcd->GetFbHeight(), global_arg->lcd->GetFbWidth(),
                CV_8UC4, global_arg->lcd->GetFrameBuffer());
    }
    Utils::MSleep(1000);
    std::cout << "-----------------" << std::endl;
    for (int j = 0; j < n2; j++) {
      ret = global_arg->camera->GetPic();
      if (ret == -1) continue;
      global_arg->camera->CheckPic(30, 80);
      if (!global_arg->mid_save_addr.empty()) {
        cv::imwrite(global_arg->mid_save_addr + std::string("camera") +
                        global_arg->temp + std::to_string(i) +
                        std::string("_") + std::to_string(j) + ".bmp",
                    global_arg->camera->GetPicMat());
        if (!global_arg->no_lcd && global_arg->save_lcd)
          cv::imwrite(global_arg->mid_save_addr + std::string("lcd") +
                          global_arg->temp + std::to_string(i) +
                          std::string("_") + std::to_string(j) + ".bmp",
                      pic1);
      }
      Utils::MSleep(200);
    }
  }

  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "sum time:"
            << (std::chrono::duration_cast<std::chrono::milliseconds>(
                    end_tick - begin_tick)
                    .count() /
                n)
            << "ms" << std::endl;
  global_arg->camera->Uninit();
  return 0;
}
