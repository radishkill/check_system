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

void SavePic(std::string name, cv::Mat pic) {
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
  for (int i = 0; i < 100; i++) {
    camera->GetPic();
    SavePic(std::string("test") + std::to_string(i) + ".bmp", camera->GetPicMat());
  }
  camera->Uninit();
  return 0;
}
