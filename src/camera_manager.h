#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

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
  cv::Mat GetPicMat();
  cv::Mat GetPicMat(int x, int y, int w, int h);
  int GetWidth() const { return picture_mat.cols; };
  int GetHeight() const { return picture_mat.rows; };
  int Reboot();
  int CheckPic(int threshold_low, int threshold_high);
  void ShowResolutionOption();
  int SetRoi(int x, int y, int w, int h) {
    roi_x_ = x;
    roi_y_ = y;
    roi_w_ = w;
    roi_h_ = h;
  };
  bool IsOpen() const { return is_open_flag_ == 1; };
  int is_open_flag_;
  void Uninit();

 private:
  std::vector<std::string> device_list_;
  stImageInfo image_info_;
  HANDLE hCamera_;
  DWORD dwWidth_;
  DWORD dwHeight_;
  BYTE* pbuffer_;
  cv::Mat picture_mat;
  DWORD dwRGBBufSize_;
  int roi_x_, roi_y_, roi_w_, roi_h_;
  tSdkCameraCapbility cap;
  double exposion_time_;
  int resolution_index_;
  int camera_nums_;
};
}  // namespace check_system

#endif  // CAMERAMANAGER_H
