#include "camera_manager.h"

#include <iostream>

namespace check_system {

CameraManager::CameraManager()
    : is_open_flag_(0) {
  pRBGBuffer_ = nullptr;
  dwRGBBufSize_ = 0;
  camera_nums_ = 0;
  CameraSdkStatus ret;
  if((ret = CameraEnumerateDevice(&camera_nums_)) != CAMERA_STATUS_SUCCESS) {
    perror("none camera device");
    return;
  }
  std::cout << "enumerate camera num " << camera_nums_ << std::endl;
  //  GetDeviceList();
  //  if (device_list_.empty()) {
  //    perror("none camera device");
  //    return;
  //  }

  //应该只有一个摄像头
  ret = CameraInitEx(&hCamera_, camera_nums_-1, -1, -1);
  if(ret != CAMERA_STATUS_SUCCESS) {
    printf("Camera init failed\n");
    return;
  }

  SetResolution(IMAGEOUT_MODE_320X240);
  CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_MONO8);

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

  //打开相机
  Play();
  GetPic();
  Pause();
  return 0;
}

int CameraManager::GetPic()
{
  //照片结果为320x240
  CameraSdkStatus status;
  HANDLE hBuf;
  BYTE* pbyBuffer;
  stImageInfo imageInfo;

  // 获取统计信息来统计帧率
//  CameraGetFrameStatistic(hCamera_, &lastFS);
  // 获取raw image data 1000ms超时时间
  status = CameraGetRawImageBuffer(hCamera_, &hBuf, 1000);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "CameraGetRawImageBuffer " << status << std::endl;
    return -1;
  }
  // 获取图像帧信息
  pbyBuffer = CameraGetImageInfo(hCamera_, hBuf, &imageInfo);
  std::cout << imageInfo.iWidth << " " <<  imageInfo.iHeight << " " << imageInfo.TotalBytes << std::endl;
  // 申请RGB image buffer内存
  if (pRBGBuffer_ == NULL || imageInfo.iWidth * imageInfo.iHeight > dwRGBBufSize_) {
      if (pRBGBuffer_)
          delete []pRBGBuffer_;
      dwRGBBufSize_ = imageInfo.iWidth * imageInfo.iHeight;
      pRBGBuffer_ = new BYTE[dwRGBBufSize_];
  }
  //处理原始图像
  status = CameraGetOutImageBuffer(hCamera_, &imageInfo, pbyBuffer, pRBGBuffer_);
  CameraReleaseFrameHandle(hCamera_, hBuf);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "handle picture fault " << status  << std::endl;
    return -1;
  }

  if (imageInfo.iWidth != dwWidth_ && imageInfo.iHeight != dwHeight_) {
    std::cout << "picture resolution inequality" << std::endl;
  }
  std::cout << "get a pic " << imageInfo.iWidth << "x" << imageInfo.iHeight << std::endl;
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
  if (average_data <= threshold_high && average_data >= threshold_low) {
    return 0;
  }
  return -1;
}
}
