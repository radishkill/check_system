#include <cstdlib>
#include <ctime>
#include <cassert>
#include <omp.h>
#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>


#include "utils.h"
#include "authpic.h"

namespace po = boost::program_options;

std::string base_path_320 = "../res/resource320x240/PUFData/";
std::string base_path_1280 = "../res/resource1280x720/PUFData/";
std::string base_path_ = base_path_1280;

int n = 1;
std::string picture_addr1;
std::string picture_addr2;

void InitCmdLine(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "");
  desc.add_options()("pic1", po::value<std::string>(&picture_addr1), "picture first");
  desc.add_options()("pic2", po::value<std::string>(&picture_addr2), "picture second");
  desc.add_options()("n", po::value<int>(&n), "loop num");


  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }
}

cv::Mat ReadPicAsBmp(int id, int index)
{

  // std::string base_path_ = "./res/resource1280x720/PUFData/";
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  cv::Mat image_ = cv::imread(base_path_ + puf_file_name + puf_file_name + "_Pic" 
  + puf_file_name + "_Pic" + Utils::DecToStr(index, 4) + ".bmp", cv::IMREAD_UNCHANGED);
  if (!image_.data)
  {
    std::cout << "read pic file "
              << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
  }
  return image_;
}

int main(int argc, char **argv)
{
  InitCmdLine(argc, argv);
  cv::Mat pic1, pic2;

  auto begin_tick = std::chrono::steady_clock::now();
  for (int i = 0; i < n; i++)
  {
    // pic1 = ReadPicAsBmp(0, 1);
    // pic2 = ReadPicAsBmp(0, 2);
    pic1 = cv::imread(picture_addr1, cv::IMREAD_UNCHANGED);
    pic2 = cv::imread(picture_addr2, cv::IMREAD_UNCHANGED);
    AuthPic(pic1, pic2);
  }
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "test " << n << " times\n";
  std::cout << "avg : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() / n << " ms" << std::endl;

  return 0;
}
