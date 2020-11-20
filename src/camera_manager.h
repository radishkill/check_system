#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <string>

#include "CKCameraInterface.h"

namespace check_system {

#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720

class CameraManager {
 public:
  CameraManager();

  const std::vector<std::string>& GetDeviceList();
  int SetResolution(INT iResolutionIndex);
  int Play();
  int Pause();
  int GetOnePic();
  int GetPic();
  char* GetRBGBuffer();
  int CheckPic(int threshold_low, int threshold_high);
  bool IsOpen() {return is_open_flag_ == 1;};
  int is_open_flag_;
 private:
  std::vector<std::string> device_list_;
  HANDLE hCamera_;
  DWORD dwWidth_;
  DWORD dwHeight_;
  BYTE* pRBGBuffer_;
  DWORD dwRGBBufSize_;
  int camera_nums_;
};
}



#endif // CAMERAMANAGER_H
