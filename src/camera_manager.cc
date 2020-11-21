#include "camera_manager.h"

#include <iostream>
#include <chrono>

namespace check_system {

CameraManager::CameraManager()
    : is_open_flag_(0) {
  pRBGBuffer_ = nullptr;
  dwRGBBufSize_ = 0;
  camera_nums_ = 0;
  CameraSdkStatus status;
  if((status = CameraEnumerateDevice(&camera_nums_)) != CAMERA_STATUS_SUCCESS) {
    perror("none camera device");
    return;
  }
  std::cout << "enumerate camera num " << camera_nums_ << std::endl;

  //应该只有一个摄像头
  status = CameraInitEx(&hCamera_, camera_nums_-1, -1, -1);
  if(status != CAMERA_STATUS_SUCCESS) {
    printf("Camera init failed\n");
    return;
  }
  status = CameraSetTriggerMode(hCamera_, 1);  //soft trigger
  status = CameraSetFrameSpeed(hCamera_, 1);
  status = CameraSetAeState(hCamera_, FALSE);
  status = CameraSetExposureTime(hCamera_, 3000);
//  status = CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_RGB8);
  status = CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_MONO8);
  status = CameraSetTriggerDelayTime(hCamera_, 0);
  status = CameraSetTriggerFrameCount(hCamera_, 1);
  //status = CameraSetResolution(hCamera_, IMAGEOUT_MODE_1280X720);
  SetResolution(IMAGEOUT_MODE_1280X720);

  Play();

  is_open_flag_ = 1;
}

const std::vector<std::string>& CameraManager::GetDeviceList() {
  int cameraNum = 0;
  for (int i = 0; i < cameraNum; i++) {
    tDevEnumInfo devAllInfo;
    CameraSdkStatus status = CameraGetEnumIndexInfo(i, &devAllInfo);
    if (status != CAMERA_STATUS_SUCCESS)
      continue;
    device_list_.push_back(devAllInfo.DevAttribute.Name);
    std::cout << devAllInfo.DevAttribute.Name << std::endl;
  }
  return device_list_;
}

int CameraManager::SetResolution(INT iResolutionIndex) {
  CameraSdkStatus status;
  status = CameraSetResolution(hCamera_, iResolutionIndex);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Resolution fault!!!" << std::endl;
    return -1;
  }
  CameraGetOutImageSize(hCamera_, &dwWidth_, &dwHeight_);
  std::cout << "current resolution : " << dwWidth_ << "x" << dwHeight_ << std::endl;
  return 0;
}

int CameraManager::Play() {
  CameraSdkStatus status = CameraPlay(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Play Fault!!!" << std::endl;
    return -1;
  }
  return 0;
}

int CameraManager::Pause() {
  CameraSdkStatus status = CameraPause(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Play Fault!!!" << std::endl;
    return -1;
  }
  return 0;
}

int CameraManager::GetOnePic() {
  int ret = 0;
  auto begin_tick = std::chrono::steady_clock::now();
  //打开相机
//  Play();

  ret = GetPic();

//  Pause();
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "soft trigger to get image duration " << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << " ms" << std::endl;
  return ret;
}

int CameraManager::GetPic()
{
  CameraSdkStatus status;
  stImageInfo imageInfo;


  auto begin_tick = std::chrono::steady_clock::now();
  status = CameraSoftTrigger(hCamera_);
  if(status != CAMERA_STATUS_SUCCESS) {
    std::cout << "soft trigger failed" << std::endl;
    return -1;
  }

  stImageInfo imgInfo;
  pRBGBuffer_ = CameraGetImageBufferEx(hCamera_, &imgInfo, 1000);
  if (pRBGBuffer_ == NULL) {
    std::cout << "can't get a frame" << std::endl;
  }
  if (imageInfo.iWidth != dwWidth_ && imageInfo.iHeight != dwHeight_) {
    std::cout << "picture resolution inequality" << std::endl;
  }
  std::cout << "get a pic " << imageInfo.iWidth << "x" << imageInfo.iHeight << " " << imageInfo.TotalBytes << std::endl;

  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "soft trigger to get image duration " << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << " ms" << std::endl;

  return 0;
}

char *CameraManager::GetRBGBuffer() {
  return (char*)pRBGBuffer_;
}

int CameraManager::CheckPic(int threshold_low, int threshold_high) {
  int average_data = 0;
  if (pRBGBuffer_ == nullptr)
    return -1;
  average_data = pRBGBuffer_[0];

  for (unsigned int i = 1; i < dwWidth_*dwHeight_; i++) {
    average_data += pRBGBuffer_[i];
    average_data /= 2;
  }
  std::cout << "check pic average value : " << average_data << std::endl;
  if (average_data <= threshold_high && average_data >= threshold_low) {
    return 0;
  }
  return -1;
}
}
