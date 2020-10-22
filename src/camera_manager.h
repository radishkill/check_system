#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <string>

#include "CKCameraInterface.h"


class CameraManager {
 public:
  CameraManager();
  const std::vector<std::string>& GetDeviceList();
  int GetPic();
 private:
  std::vector<std::string> device_list_;
  HANDLE hCamera_;
  DWORD dwWidth_;
  DWORD dwHeight_;
  BYTE* pRBGBuffer = NULL;
  DWORD dwRGBBufSize = 0;
};

#endif // CAMERAMANAGER_H
