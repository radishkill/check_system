#include <boost/program_options.hpp>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "authpic.h"
#include "mutils.h"

namespace po = boost::program_options;

using check_system::AuthPic;

int n = 1;
std::vector<std::string> pic_addr;
int mode;

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("mode", po::value<int>(&mode)->default_value(0), "");
  desc.add_options()(
      "pic,p", po::value<std::vector<std::string>>(&pic_addr)->multitoken(),
      "picture addr");
  desc.add_options()("n", po::value<int>(&n), "loop num");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
}

int main(int argc, char **argv) {
  InitCmdLine(argc, argv);
  cv::Mat pic1, pic2;
  AuthPic::InitAuth();
  
  if (mode == 1) {
    int n1 = 10;
    int n2 = 5;
    for (int i = 1; i < 33; i++) {
      for (int j = i + 1; j < 34; j++) {

        std::string picture_addr1 = std::to_string(i) + ".bmp";
        std::string picture_addr2 = std::to_string(j) + ".bmp";
        std::cout << picture_addr1 << " " << picture_addr2 << std::endl;
        pic1 = cv::imread(std::string("./mid_save1/") + picture_addr1,
                          cv::IMREAD_UNCHANGED);
        pic2 = cv::imread(std::string("./mid_save1/") + picture_addr2,
                          cv::IMREAD_UNCHANGED);
        AuthPic::DoAuthPic(pic1, pic2, check_system::kAuthThreshold);
      }
    }
    return 0;
  } else if (mode == 2) {
    for (int k = 0; k < 3; k++)
      for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
          std::string picture_addr1 =
              std::to_string(k) + std::to_string(i) + std::string("_camera") +
              std::to_string(0) + std::string("_") + std::to_string(0) + ".bmp";
          std::string picture_addr2 =
              std::to_string(k) + std::to_string(j) + std::string("_camera") +
              std::to_string(0) + std::string("_") + std::to_string(0) + ".bmp";
          std::cout << picture_addr1 << " " << picture_addr2 << std::endl;
          pic1 = cv::imread(std::string("./mid_save1/") + picture_addr1,
                            cv::IMREAD_UNCHANGED);
          pic2 = cv::imread(std::string("./mid_save1/") + picture_addr2,
                            cv::IMREAD_UNCHANGED);
          AuthPic::DoAuthPic(pic1, pic2,check_system::kAuthThreshold);
        }
      }
    return 0;
  }

  if (pic_addr.size() < 2) return 0;
  n = 10;
  

  auto begin_tick = std::chrono::steady_clock::now();
  
  for (int i =0 ; i < n; i++) {
    pic1 = cv::imread(pic_addr[0], cv::IMREAD_UNCHANGED);
    pic2 = cv::imread(pic_addr[1], cv::IMREAD_UNCHANGED);
    AuthPic::DoAuthPic(pic1, pic2, check_system::kAuthThreshold);
  }
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "test " << n << " times\n";
  std::cout << "avg : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                       .count()/n
            << " ms" << std::endl;

  return 0;
}
