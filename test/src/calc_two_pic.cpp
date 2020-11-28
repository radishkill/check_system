#include <cstdlib>
#include <ctime>
#include <cassert>
#include <omp.h>
#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>


#include "utils.h"
#include "authpic.h"

std::string base_path_320 = "../res/resource320x240/PUFData/";
std::string base_path_1280 = "../res/resource1280x720/PUFData/";
std::string base_path_ = base_path_1280;

cv::Mat ReadPicAsBmp(int id, int index)
{

  // std::string base_path_ = "./res/resource1280x720/PUFData/";
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  cv::Mat image_ = cv::imread(base_path_ + puf_file_name + puf_file_name + "_Pic" + puf_file_name + "_Pic" + Utils::DecToStr(index, 4) + ".bmp");
  if (!image_.data)
  {
    std::cout << "read pic file "
              << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
  }
  return image_;
}

int main(int argc, char **argv)
{
  cv::Mat pic1, pic2;

  if (argc == 2)
  {
    base_path_ = base_path_320;
  }
  int n = 10;
  auto begin_tick = std::chrono::steady_clock::now();
  for (int i = 0; i < n; i++)
  {
    pic1 = ReadPicAsBmp(0, 1);
    pic2 = ReadPicAsBmp(0, 2);

    AuthPic(pic1, pic2);
  }

  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "avg : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() / n << " ms" << std::endl;

  return 0;
}
