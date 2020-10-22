#include "camera_manager.h"



CameraManager::CameraManager() {
  GetDeviceList();
  if (device_list_.empty()) {
    perror("none camera device!!!");
    return;
  }
  CameraSdkStatus ret = CameraInit(&hCamera_, 0);
  if (ret != CAMERA_STATUS_SUCCESS) {
      perror("open camer error\n");
      hCamera_ = NULL;
      return;
  }
  CameraGetOutImageSize(hCamera_, &dwWidth_, &dwHeight_);
  CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_BGR8);
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
  FrameStatistic curFS, lastFS;
  stImageInfo imageInfo;

  //打开相机
  CameraPlay(hCamera_);
  // 获取统计信息来统计帧率
  CameraGetFrameStatistic(hCamera_, &lastFS);
  // 获取raw image data 1000ms超时时间
  status = CameraGetRawImageBuffer(hCamera_, &hBuf, 1000);
  if (status != CAMERA_STATUS_SUCCESS) {
    perror("error");
    return -1;
  }
  // 获取图像帧信息
  pbyBuffer = CameraGetImageInfo(hCamera_, hBuf, &imageInfo);
  // 申请RGB image buffer内存
  if (pRBGBuffer == NULL || imageInfo.iWidth * imageInfo.iHeight * 4 > dwRGBBufSize) {
      if (pRBGBuffer)
          delete []pRBGBuffer;
      dwRGBBufSize = imageInfo.iWidth * imageInfo.iHeight * 4;
      pRBGBuffer = new BYTE[dwRGBBufSize];
  }
  //处理原始图像
  status = CameraGetOutImageBuffer(hCamera_, &imageInfo, pbyBuffer, pRBGBuffer);
  CameraReleaseFrameHandle(hCamera_, hBuf);
  if (status == CAMERA_STATUS_SUCCESS) {
      // 叠加十字线等
      CameraImageOverlay(hCamera_, pRBGBuffer, &imageInfo);
      disFrameCnt++;
  } else {
    perror("error");
    CameraPause(hCamera_);
    return -1;
  }
  //停止相机
  CameraPause(hCamera_);
  return 0;
}
