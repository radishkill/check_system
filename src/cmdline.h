#pragma once

#include <boost/program_options.hpp>
#include <iostream>

#include "./global_arg.h"
namespace check_system {
namespace po = boost::program_options;

void InitCmdLine(int argc, char** argv) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("no-button", "");
  desc.add_options()(
      "no-laser",
      po::bool_switch(&global_arg->no_laser_flag)->default_value(false), "");
  desc.add_options()(
      "no-lcd", po::bool_switch(&global_arg->no_lcd_flag)->default_value(false),
      "");
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
      po::value<double>(&global_arg->exposion_time)->default_value(-1), "us");
  desc.add_options()(
      "analog-gain",
      po::value<int>(&global_arg->analog_gain)->default_value(-1), "");
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
  desc.add_options()("auth-threshold",
                     po::value<double>(&global_arg->auth_threshold)
                         ->default_value(kAuthThreshold),
                     "auth threshold");
  desc.add_options()(
      "pic-avg-high",
      po::value<int>(&global_arg->pic_avg_high)->default_value(200),
      "pic average threshold high");
  desc.add_options()(
      "pic-avg-low",
      po::value<int>(&global_arg->pic_avg_low)->default_value(20),
      "pic average threshold low");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file("./config/base.cfg", desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
  if (vm.count("no-button")) {
    global_arg->no_button_flag = true;
  }
  if (vm.count("no-led")) {
    global_arg->no_led_flag = true;
  }
}
}  // namespace check_system