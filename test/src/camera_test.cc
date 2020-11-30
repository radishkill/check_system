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
  
  camera = new CameraManager(0);
  if (!camera->IsOpen()) {
    return 0;
  }
  camera->SetExposureTime(4000);
  camera->Play();
  camera->GetPic();
  SavePic("test.bmp", camera->GetPicBuffer(), camera->GetWidth(), camera->GetHeight());
  camera->Uninit();
  return 0;
}
