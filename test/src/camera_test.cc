#include <stdio.h>
#include <string.h>

#include <chrono>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "CKCameraInterface.h"
#include "camera_manager.h"

using namespace std;
using check_system::CameraManager;


CameraManager* camera;

void SavePic(std::string name, char* buffer, int width, int heigh) {
  cv::Mat pic(heigh, width, CV_8UC1, buffer);
  cv::imwrite(name, cv::Mat(pic, cv::Rect(0, 0, pic.rows/2, pic.cols/2)));
}

int main(int argc, char *argv[])
{  
  // camera = new CameraManager(0);
  // if (!camera->IsOpen()) {
  //   return 0;
  // }
  // camera->SetExposureTime(4000);
  // camera->Play();
  // camera->GetPic();
  // SavePic("test.bmp", camera->GetPicBuffer(), camera->GetWidth(), camera->GetHeight());
  // camera->Uninit();
  char* buffer = new char[16];
  for (int i = 0; i < 16; i++) {
    buffer[i] = i;
  }
  cv::Mat pic = cv::Mat(4, 4, CV_8UC1, buffer);
  std::cout << pic.size << " " << pic.total() << std::endl;
  cv::Mat pic2 = cv::Mat(pic, cv::Rect(0, 0, 2, 2));
  buffer[1] = 0;
  pic = pic(cv::Rect(1, 1, 2, 2));
  pic2.ptr<unsigned char>(1)[1] = 8;
  std::cout << pic << std::endl;
  std::cout << pic2 << std::endl;
  // cv::imwrite("./test1.bmp", cv::Mat(pic, cv::Rect(0, 0, pic.rows/2, pic.cols/2)));
  return 0;
}
