#include "camera_manager.h"

#include <chrono>
#include <iostream>

#include "mutils.h"

namespace check_system {

CameraManager::CameraManager()
    : is_open_flag_(0), roi_x_(-1), roi_y_(-1), roi_w_(-1), roi_h_(-1) {
  pbuffer_ = nullptr;
  dwRGBBufSize_ = 0;
  camera_nums_ = 0;
  CameraSdkStatus status;
  status = CameraEnumerateDevice(&camera_nums_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "none camera device" << std::endl;
    is_open_flag_ = -1;
    return;
  }
  std::cout << "enumerate camera num " << camera_nums_ << std::endl;
  ShowDeviceList(camera_nums_);
  //应该只有一个摄像头
  // status = CameraInitEx(&hCamera_, camera_nums_ - 1, -1, -1);
  status = CameraInit(&hCamera_, camera_nums_ - 1);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Camera init failed" << status << std::endl;
    is_open_flag_ = -1;
    return;
  }
  is_open_flag_ = 1;
}

int CameraManager::InitCameraByDefault() {
  CameraSdkStatus status;
  // status = CameraLoadParameter(hCamera_, PARAMETER_TEAM_A);
  status = CameraSetTriggerMode(hCamera_, 1);  // soft trigger
  status = CameraSetFrameSpeed(
      hCamera_, 0);  // low speed when use high speed have some problem
  status = CameraSetCorrectDeadPixel(hCamera_, FALSE);
  // status = CameraSetAeState(hCamera_, TRUE);
  // sleep(2);
  status = CameraSetAeState(hCamera_, FALSE);  // by hand
  status = CameraSetIspOutFormat(hCamera_, CAMERA_MEDIA_TYPE_MONO8);
  status = CameraSetTriggerDelayTime(hCamera_, 0);
  status = CameraSetTriggerFrameCount(hCamera_, 1);
  status = CameraSetOnceWB(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    printf("Camera init failed\n");
    is_open_flag_ = -1;
  }
  is_open_flag_ = 1;
  return 0;
}

int CameraManager::ShowDeviceList(int n) {
  for (int i = 0; i < n; i++) {
    tDevEnumInfo devAllInfo;
    CameraSdkStatus status = CameraGetEnumIndexInfo(i, &devAllInfo);
    if (status != CAMERA_STATUS_SUCCESS) continue;
    std::cout << i << ":" << devAllInfo.DevAttribute.acProductName << " "
              << devAllInfo.DevAttribute.acSensorType << std::endl;
  }
  return 0;
}

int CameraManager::ReadParameterFromFile(const char* file) {
  assert(file != nullptr);
  CameraSdkStatus status;
  status = CameraReadParameterFromFile(hCamera_, file);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Read Parameter From " << file << " Fault status=" << status
              << std::endl;
    return -1;
  }
  std::cout << "Success Read Parameter From " << file << std::endl;
  return 0;
}
int CameraManager::SaveParameterToFile(const char* file) {
  assert(file != nullptr);
  CameraSdkStatus status;
  status = CameraSaveParameterToFile(hCamera_, file);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Save Parameter To " << file << " Fault status=" << status
              << std::endl;
    return -1;
  }
  std::cout << "Success Save Parameter To " << file << std::endl;
  return 0;
}
int CameraManager::SetLutMode(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetLutMode(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Lut Mode Fault status=" << status << std::endl;
    return -1;
  }
  CameraGetLutMode(hCamera_, &value);
  std::cout << "actual Lut Mode:" << value << std::endl;
  return 0;
}

int CameraManager::SetExposureTimeAndAnalogGain(double value, int analog_gain) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  if (analog_gain != -1) {
    status = CameraSetAnalogGain(hCamera_, (unsigned int)analog_gain);
    if (status != CAMERA_STATUS_SUCCESS) {
      std::cout << "Set Analog Gain Fault status=" << status << std::endl;
      return -1;
    }
  }
  status = CameraSetExposureTime(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Exposure Time Fault status=" << status << std::endl;
    return -1;
  }
  unsigned int gain;
  CameraGetExposureTime(hCamera_, &exposion_time_);
  CameraGetAnalogGain(hCamera_, &gain);
  std::cout << "actual exposure time:" << exposion_time_ << "us\n";
  std::cout << "actual analog gain:" << gain << std::endl;
  return 0;
}

int CameraManager::SetResolution(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetResolution(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Resolution fault status=" << status << std::endl;
    return -1;
  }
  CameraGetOutImageSize(hCamera_, &dwWidth_, &dwHeight_);
  std::cout << "current resolution : " << dwWidth_ << "x" << dwHeight_
            << std::endl;
  CameraGetResolution(hCamera_, &resolution_index_);
  std::cout << "resolution id=" << resolution_index_ << std::endl;
  return 0;
}
int CameraManager::SetGamma(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetGamma(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Gamma fault status=" << status << std::endl;
    return -1;
  }
  CameraGetGamma(hCamera_, &value);
  std::cout << "current Gamma : " << value << std::endl;
  return 0;
}
int CameraManager::SetContrast(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetContrast(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Contrast fault status=" << status << std::endl;
    return -1;
  }
  CameraGetContrast(hCamera_, &value);
  std::cout << "current Contrast : " << value << std::endl;
  return 0;
}

int CameraManager::SetSaturation(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetSaturation(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Saturation fault status=" << status << std::endl;
    return -1;
  }
  CameraGetSaturation(hCamera_, &value);
  std::cout << "current Saturation : " << value << std::endl;
  return 0;
}

int CameraManager::SetSharpness(int value) {
  if (value == -1) return -1;
  CameraSdkStatus status;
  status = CameraSetSharpness(hCamera_, value);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Set Sharpness fault status=" << status << std::endl;
    return -1;
  }
  CameraGetSharpness(hCamera_, &value);
  std::cout << "current Sharpness : " << value << std::endl;
  return 0;
}

int CameraManager::Play() {
  CameraSdkStatus status = CameraPlay(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Play Fault status=" << status << std::endl;
    return -1;
  }
  std::cout << "camera play\n";
  return 0;
}

int CameraManager::Pause() {
  CameraSdkStatus status = CameraPause(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "Pause Fault status=" << status << std::endl;
    return -1;
  }
  return 0;
}

int CameraManager::GetPic() {
  CameraSdkStatus status;
  int ret;

  auto begin_tick = std::chrono::steady_clock::now();
  status = CameraSoftTrigger(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "soft trigger failed : " << status << std::endl;
    //这个时候有可能是照相机突然断开连接了
    //重启一下
    ret = Reboot();
    if (ret == -1) return -1;
    status = CameraSoftTrigger(hCamera_);
    if (status != CAMERA_STATUS_SUCCESS) {
      std::cout << "soft trigger failed : " << status << std::endl;
      return -1;
    }
  }
  pbuffer_ =
      CameraGetImageBufferEx(hCamera_, &image_info_, 10000);  // 10s的超时时间
  if (pbuffer_ == nullptr) {
    std::cout << "can't get a frame picture status=" << std::endl;
    ret = Reboot();
    if (ret == -1) return -1;
    status = CameraSoftTrigger(hCamera_);
    if (status != CAMERA_STATUS_SUCCESS) {
      std::cout << "soft trigger failed : " << status << std::endl;
      return -1;
    }
    pbuffer_ =
      CameraGetImageBufferEx(hCamera_, &image_info_, 10000);  // 10s的超时时间
    if (pbuffer_ == nullptr) {
      std::cout << "can't get a frame picture status=" << std::endl;
      return -1;
    }
  }
  picture_mat_ =
      cv::Mat(image_info_.iHeight, image_info_.iWidth, CV_8UC1, pbuffer_);
  //对图片进行兴趣区域划分
  if (roi_x_ != -1 && roi_y_ != -1 && roi_w_ != -1 && roi_h_ != -1) {
    picture_mat_ = picture_mat_(cv::Rect(roi_x_, roi_y_, roi_w_, roi_h_));
  }
  // std::cout << "photos widthxheight:" << image_info_.iWidth << "x"
  //           << image_info_.iHeight << " total bytes:" <<
  //           image_info_.TotalBytes
  //           << std::endl;
  std::cout << "photos info:" << picture_mat_.size << " " << picture_mat_.total()
            << std::endl;
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "take photos time:"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  dwWidth_ = image_info_.iWidth;
  dwHeight_ = image_info_.iHeight;
  return 0;
}

char* CameraManager::GetPicBuffer() { return (char*)pbuffer_; }
cv::Mat CameraManager::GetPicMat() { return picture_mat_; }
cv::Mat CameraManager::GetPicMat(int x, int y, int w, int h) {
  // std::memcpy(picture_mat.data, pbuffer_, dwHeight_*dwWidth_);
  if (roi_x_ != -1 && roi_y_ != -1 && roi_w_ != -1 && roi_h_ != -1) {
    picture_mat_ = picture_mat_(cv::Rect(x, y, w, h));
  }
  return picture_mat_;
}

int CameraManager::Reboot() {
  CameraSdkStatus status;
  Pause();
  status = CameraUnInit(hCamera_);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "CameraUnInit fault status = " << status << std::endl;
  }
  Utils::MSleep(500);
  if ((status = CameraEnumerateDevice(&camera_nums_)) !=
      CAMERA_STATUS_SUCCESS) {
    std::cout << "none camera device" << std::endl;
    return -1;
  }
  std::cout << "enumerate camera num " << camera_nums_ << std::endl;
  // ShowDeviceList(camera_nums_);

  //应该只有一个摄像头
  // status = CameraInitEx(&hCamera_, camera_nums_ - 1, -1, -1);
  status = CameraInit(&hCamera_, camera_nums_ - 1);
  if (status != CAMERA_STATUS_SUCCESS) {
    printf("Camera init failed\n");
    return -1;
  }
  return Play();
}

void CameraManager::ShowCameraBaseConfig() {
  int mode;
  CameraGetCapability(hCamera_, &cap);
  std::cout << "resolution options" << std::endl;
  for (int i = 0; i < cap.tDeviceCapbility.iImageSizeDesc; i++) {
    std::cout << cap.tDeviceCapbility.pImageSizeDesc[i].iIndex << " "
              << cap.tDeviceCapbility.pImageSizeDesc[i].iWidth << "x"
              << cap.tDeviceCapbility.pImageSizeDesc[i].iHeight << std::endl;
  }
  double exposion_time;
  unsigned int analog_gain;
  CameraGetExposureTime(hCamera_, &exposion_time);
  CameraGetAnalogGain(hCamera_, &analog_gain);
  std::cout << "exposion time = " << exposion_time << "us" << std::endl;
  std::cout << "analog_gain = " << analog_gain << std::endl;
}
void CameraManager::Uninit() {
  CameraSaveParameter(hCamera_, PARAMETER_TEAM_A);
  CameraUnInit(hCamera_);
}
}  // namespace check_system
