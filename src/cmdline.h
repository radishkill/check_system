#pragma once

#include <iostream>
#include <boost/program_options.hpp>

#include "./global_arg.h"
namespace check_system {
namespace po = boost::program_options;

void InitCmdLine(int argc, char** argv) {
  GlobalArg* arg = GlobalArg::GetInstance();
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("no-button", "")("no-laser", "")("no-lcd", "");
  desc.add_options()("mid-save", po::value<std::string>(&arg->mid_save_addr),
                     "");
  desc.add_options()("resolution-index",
                     po::value<int>(&arg->resolution_index)->default_value(-1),
                     "4 8 15 19 -1=not set");
  desc.add_options()("ROI-x", po::value<int>(&arg->roi_x)->default_value(-1),
                     "");
  desc.add_options()("ROI-y", po::value<int>(&arg->roi_y)->default_value(-1),
                     "");
  desc.add_options()("ROI-w", po::value<int>(&arg->roi_w)->default_value(-1),
                     "");
  desc.add_options()("ROI-h", po::value<int>(&arg->roi_h)->default_value(-1),
                     "");
  desc.add_options()("exposion-time",
                     po::value<int>(&arg->exposion_time)->default_value(-1),
                     "us");
  desc.add_options()("laser-current",
                     po::value<int>(&arg->laser_current)->default_value(-1),
                     "uA");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
  if (vm.count("no-button")) {
    no_button_flag = true;
  }
  if (vm.count("no-laser")) {
    no_laser_flag = true;
  }
  if (vm.count("no-lcd")) {
    no_lcd_flag = true;
  }
}
}  // namespace check_system