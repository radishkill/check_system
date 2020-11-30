#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <string>

#include "CKCameraInterface.h"

namespace check_system {

#define CAMERA_WIDTH 320
#define CAMERA_HEIGHT 240

//#define CAMERA_WIDTH 1280
//#define CAMERA_HEIGHT 720


class CameraManager {
 public:
  CameraManager(int auto_flag);
  int InitCamera(int auto_flag);

  const std::vector<std::string>& GetDeviceList();
  int SetExposureTime(double time);
  int SetResolution(INT iResolutionIndex);
  int Play();
  int Pause();
  int GetOnePic();
  int GetPic();
  char* GetPicBuffer();
  DWORD GetWidth() {return dwWidth_;};
  DWORD GetHeight() {return dwHeight_;};
  int Reboot();
  int CheckPic(int threshold_low, int threshold_high);
  bool IsOpen() {return is_open_flag_ == 1;};
  int is_open_flag_;
 private:
  std::vector<std::string> device_list_;
  stImageInfo image_info_;
  HANDLE hCamera_;
  DWORD dwWidth_;
  DWORD dwHeight_;
  BYTE* pbuffer_;
  DWORD dwRGBBufSize_;
  double exposion_time_;
  int resolution_index_;
  int camera_nums_;
};
}



#endif // CAMERAMANAGER_H
