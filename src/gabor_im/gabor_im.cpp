//
// File: gabor_im.cpp
//
// MATLAB Coder version            : 4.1
// C/C++ source code generated on  : 15-Oct-2020 16:25:25
//

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "gabor_im_emxutil.h"
#include "imgaborfilt.h"

// Function Definitions

//
// image = imresize(im2double(image), [360 480]);
// Arguments    : const emxArray_uint8_T *image
//                double wavelength
//                double angle
//                emxArray_real_T *Gimage_im
//                emxArray_boolean_T *BW_im
//                emxArray_boolean_T *K
// Return Type  : void
//
void gabor_im(const emxArray_uint8_T *image, double wavelength, double angle,
              emxArray_real_T *Gimage_im, emxArray_boolean_T *BW_im,
              emxArray_boolean_T *K)
{
  emxArray_real_T *b_image;
  int i0;
  int loop_ub;
  emxArray_real_T *M;
  emxInit_real_T(&b_image, 2);
  i0 = b_image->size[0] * b_image->size[1];
  b_image->size[0] = image->size[0];
  b_image->size[1] = image->size[1];
  emxEnsureCapacity_real_T(b_image, i0);
  loop_ub = image->size[0] * image->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    b_image->data[i0] = image->data[i0];
  }

  emxInit_real_T(&M, 2);
  applyGaborFilterFFT(b_image, wavelength, angle, wavelength *
                      0.58870501125773733 / 3.1415926535897931 * 3.0, wavelength
                      / 0.5 * 0.58870501125773733 / 3.1415926535897931 * 3.0, M,
                      Gimage_im);

  //      BW_im=im2bw(Gimage_im,0);       %对RGB处理
  i0 = BW_im->size[0] * BW_im->size[1];
  BW_im->size[0] = Gimage_im->size[0];
  BW_im->size[1] = Gimage_im->size[1];
  emxEnsureCapacity_boolean_T(BW_im, i0);
  loop_ub = Gimage_im->size[0] * Gimage_im->size[1];
  emxFree_real_T(&b_image);
  emxFree_real_T(&M);
  for (i0 = 0; i0 < loop_ub; i0++) {
    BW_im->data[i0] = (Gimage_im->data[i0] > 0.0);
  }

  // 全局阈值对图像I进行二值化
  loop_ub = BW_im->size[0] * BW_im->size[1];
  i0 = K->size[0] * K->size[1];
  K->size[0] = 1;
  K->size[1] = loop_ub;
  emxEnsureCapacity_boolean_T(K, i0);
  for (i0 = 0; i0 < loop_ub; i0++) {
    K->data[i0] = BW_im->data[i0];
  }
}

//
// File trailer for gabor_im.cpp
//
// [EOF]
//
