#include "camera_manager.h"

namespace check_system {

CameraManager::CameraManager() {
  pRBGBuffer_ = nullptr;
  dwRGBBufSize_ = 0;
  CameraSdkStatus ret;
  int camera_nums;
  if((ret = CameraEnumerateDevice(&camera_nums)) != CAMERA_STATUS_SUCCESS || camera_nums <= 0)
    return;
  //  GetDeviceList();
  if (device_list_.empty()) {
    perror("none camera device!!!");
    return;
  }
  if((ret = CameraInitEx(&hCamera_, camera_nums - 1, -1, -1)) != CAMERA_STATUS_SUCCESS)
    return;
//  if((ret = CameraDisplayInit(hCamera_, hwnd)) != CAMERA_STATUS_SUCCESS) {
//    CameraUnInit(hCamera_);
//    return;
//  }
  CameraGetOutImageSize(hCamera_, &dwWidth_, &dwHeight_);
  CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_BGR8);
  is_open_flag_ = 1 ;
}

const std::vector<std::string>& CameraManager::GetDeviceList() {
  int cameraNum = 0;
  CameraEnumerateDevice(&cameraNum);
  for (int i = 0; i < cameraNum; i++) {
    tDevEnumInfo devAllInfo;
    CameraSdkStatus status = CameraGetEnumIndexInfo(i, &devAllInfo);
    if (status != CAMERA_STATUS_SUCCESS)
      continue;
    device_list_.push_back(devAllInfo.DevAttribute.Name);
  }
  return device_list_;
}

int CameraManager::GetPic() {
  CameraSdkStatus status;
  HANDLE hBuf;
  BYTE* pbyBuffer;
  UINT disFrameCnt = 0;
//  FrameStatistic curFs;
//  FrameStatistic lastFS;
  stImageInfo imageInfo;

  //打开相机
  CameraPlay(hCamera_);
  // 获取统计信息来统计帧率
//  CameraGetFrameStatistic(hCamera_, &lastFS);
  // 获取raw image data 1000ms超时时间
  status = CameraGetRawImageBuffer(hCamera_, &hBuf, 1000);
  if (status != CAMERA_STATUS_SUCCESS) {
    perror("error");
    return -1;
  }
  // 获取图像帧信息
  pbyBuffer = CameraGetImageInfo(hCamera_, hBuf, &imageInfo);
  // 申请RGB image buffer内存
  if (pRBGBuffer_ == NULL || imageInfo.iWidth * imageInfo.iHeight * 4 > dwRGBBufSize_) {
      if (pRBGBuffer_)
          delete []pRBGBuffer_;
      dwRGBBufSize_ = imageInfo.iWidth * imageInfo.iHeight * 4;
      pRBGBuffer_ = new BYTE[dwRGBBufSize_];
  }
  //处理原始图像
  status = CameraGetOutImageBuffer(hCamera_, &imageInfo, pbyBuffer, pRBGBuffer_);
  CameraReleaseFrameHandle(hCamera_, hBuf);
  if (status == CAMERA_STATUS_SUCCESS) {
      // 叠加十字线等
      CameraImageOverlay(hCamera_, pRBGBuffer_, &imageInfo);
      disFrameCnt++;
  } else {
    perror("error");
    CameraPause(hCamera_);
    return -1;
  }
  //停止相机
  CameraPause(hCamera_);
  pic_width_ = imageInfo.iWidth;
  pic_height_ = imageInfo.iHeight;
  return 0;
}

char *CameraManager::GetRBGBuffer() {
  return (char*)pRBGBuffer_;
}

int CameraManager::CheckPic(int threshold) {
  int average_data = 0;
  if (pRBGBuffer_ == nullptr)
    return -1;
  average_data = pRBGBuffer_[0];

  for (unsigned int i = 1; i < pic_width_*pic_height_*4; i++) {
    average_data += pRBGBuffer_[i];
    average_data /= 2;
  }
  if (average_data <= threshold) {
    return 0;
  }
  return -1;
}
}
