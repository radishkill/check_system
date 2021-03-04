#ifndef AUTHPIC_H
#define AUTHPIC_H

#include <opencv2/opencv.hpp>

#include "gabor_im.h"
#include "gabor_im_emxAPI.h"
#include "gabor_im_initialize.h"
#include "gabor_im_terminate.h"
#include "gabor_im_types.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"

#include "constant.h"

namespace check_system {

class AuthPic {
 public:
  static emxArray_real_T *Gimage_im[3];
  static emxArray_boolean_T *BW_im[3];
  static emxArray_boolean_T *K[3];
  static emxArray_uint8_T *image[3];
  static cv::Mat bw_im[3];
  static cv::Mat Gim_mat[3];
  static double* wave_length_;

  static int InitAuth(double* wave_length);
  static int DestroyAuth();

  static double DoAuthPic(cv::Mat speckle_database, cv::Mat speckle_auth, double auth_threshold);

 private:
};
}  // namespace check_system

#endif