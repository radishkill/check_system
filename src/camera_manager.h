#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <string>

#include "CKCameraInterface.h"

namespace check_system {
class CameraManager {
 public:
  CameraManager();
  const std::vector<std::string>& GetDeviceList();
  int GetPic();
  char* GetRBGBuffer();
  int CheckPic(int threshold);
  unsigned int pic_width_;
  unsigned int pic_height_;
 private:
  std::vector<std::string> device_list_;
  HANDLE hCamera_;
  DWORD dwWidth_;
  DWORD dwHeight_;
  BYTE* pRBGBuffer_;
  DWORD dwRGBBufSize_;
};
}



#endif // CAMERAMANAGER_H
