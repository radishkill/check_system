#include "authpic.h"

#include "gabor_im.h"
#include "gabor_im_emxAPI.h"
#include "gabor_im_initialize.h"
#include "gabor_im_terminate.h"
#include "gabor_im_types.h"
#include "imtrans_my.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"

emxArray_uint8_T *Mat2Emx_U8(Mat &srcImage) {
  int idx0;
  int idx1;
  emxArray_uint8_T *dstImage;
  // Set the size of the array.
  // Change this size to the value that the application requires.
  dstImage = emxCreate_uint8_T(srcImage.cols, srcImage.rows);
  // memcpy(result, Io.data, result->allocatedSize * sizeof(uchar));

  // Loop over the array to initialize each element.
  for (idx0 = 0; idx0 < dstImage->size[0]; idx0++) {
    for (idx1 = 0; idx1 < dstImage->size[1]; idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      dstImage->data[idx0 + dstImage->size[0] * idx1] =
          srcImage.data[idx0 + dstImage->size[0] * idx1];
    }
  }
  return dstImage;
}
Mat Emx2Mat_U8c(const emxArray_uint8_T *srcImage) {
  Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_8U,
                         srcImage->data);
  return dstImage;
}
Mat Emx2Mat_U8(const emxArray_real_T *srcImage) {
  Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_64F,
                         srcImage->data);
  return dstImage;
}
Mat Emx2Mat_bool(const emxArray_boolean_T *srcImage) {
  Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_8U,
                         srcImage->data);
  return dstImage;
}

static double argInit_real_T() { return 0.0; }

//
// Arguments    : void
// Return Type  : unsigned char
//
static unsigned char argInit_uint8_T() { return 0U; }

//
// Arguments    : void
// Return Type  : emxArray_uint8_T *
//
static emxArray_uint8_T *c_argInit_UnboundedxUnbounded_u() {
  emxArray_uint8_T *result;
  int idx0;
  int idx1;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result = emxCreate_uint8_T(2, 2);

  // Loop over the array to initialize each element.
  for (idx0 = 0; idx0 < result->size[0U]; idx0++) {
    for (idx1 = 0; idx1 < result->size[1U]; idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result->data[idx0 + result->size[0] * idx1] = argInit_uint8_T();
    }
  }

  return result;
}

// hamming
double hamming(Mat input1, Mat input2) {
  double diff = 0;
  for (int i = 0; i < input2.rows && i < input1.rows; i++) {
    uchar *data1 = input1.ptr<uchar>(i);
    uchar *data2 = input2.ptr<uchar>(i);
    for (int j = 0; j < input2.cols && j < input1.cols; j++) {
      if (data1[j] != data2[j]) {
        diff++;
      }
    }
  }
  // cout << "pdist=" << diff << endl;
  diff = diff / input1.cols / input1.rows;
  return diff;
}

class MyLoopBody : public ParallelLoopBody {
 public:
  MyLoopBody(emxArray_uint8_T **image, emxArray_real_T **Gimage_im,
             emxArray_boolean_T **BW_im, emxArray_boolean_T **K, Mat *Gim_mat,
             Mat *bw_im) {
    image_ = image;
    Gimage_im_ = Gimage_im;
    K_ = K;
    BW_im_ = BW_im;
    Gim_mat_ = Gim_mat;
    bw_im_ = bw_im;
  };
  virtual void operator()(const Range &range) const {
    for (int colIdx = range.start; colIdx < range.end; ++colIdx) {
      gabor_im(image_[colIdx], kWaveLength, 45, Gimage_im_[colIdx],
               BW_im_[colIdx], K_[colIdx]);

      Gim_mat_[colIdx] = Emx2Mat_U8(Gimage_im_[colIdx]);
      //阈值
      threshold(Gim_mat_[colIdx], bw_im_[colIdx], 0, 255,
                THRESH_BINARY_INV);  // INV_THRESH_BINARY

      bw_im_[colIdx].convertTo(bw_im_[colIdx], CV_8U, 1, 0);  // 64f->8u
    }
  }

 private:
  emxArray_real_T **Gimage_im_;
  emxArray_boolean_T **BW_im_;
  emxArray_boolean_T **K_;
  emxArray_uint8_T **image_;
  Mat *Gim_mat_;
  Mat *bw_im_;
};
emxArray_real_T *Gimage_im[3];
emxArray_boolean_T *BW_im[3];
emxArray_boolean_T *K[3];
emxArray_uint8_T *image[3] = {nullptr, nullptr, nullptr};
  Mat bw_im[3];
  Mat Gim_mat[3];
int InitAuth() {
   // Initialize the application.
  // 其实就是打开OpenMp并行
  gabor_im_initialize();

    for (int i = 0; i < 3; i++) {
    emxInitArray_real_T(&Gimage_im[i], 2);
    emxInitArray_boolean_T(&BW_im[i], 2);
    emxInitArray_boolean_T(&K[i], 2);
  }
}
int DestroyAuth() {
  // Terminate the application.
  gabor_im_terminate();
}

double AuthPic(cv::Mat speckle_database, char *auth_pic, int h2, int w2) {
  Mat speckle_auth(h2, w2, CV_8UC1);
  std::memcpy(speckle_auth.data, auth_pic, h2 * w2);
  double FHD = AuthPic(speckle_database, speckle_auth);
  return FHD;
}



double AuthPic(cv::Mat speckle_database, cv::Mat speckle_auth) {
  if (!speckle_auth.data || speckle_auth.data) {
    std::cout << "picture data error!!!!\n";
    return 1;
  }
  double FHD = 0;

  // Mat ROI = speckle_database;//= speckle_database(Rect(50,50,
  // speckle_database.cols-50, speckle_database.rows-50)); Mat ROI2 =
  // speckle_auth;//= speckle_auth(Rect(50, 50, speckle_auth.cols - 50,
  // speckle_auth.rows - 50));

  image[0] = Mat2Emx_U8(speckle_database);
  image[1] = Mat2Emx_U8(speckle_auth);

  // for (int i = 0; i < 2; i++) {
  //   std::cout << "allocatedSize " << image[i]->allocatedSize << std::endl;
  //   std::cout << "numDimensions " << image[i]->numDimensions << std::endl;
  //   std::cout << "image1 size : " << image[i]->size[0] << " "
  //             << image[i]->size[1] << std::endl;
  // }
  for (int i = 0; i < 2; i++) {
    gabor_im(image[i], kWaveLength, 45, Gimage_im[i], BW_im[i], K[i]);
    Gim_mat[i] = Emx2Mat_U8(Gimage_im[i]);
    //阈值
    threshold(Gim_mat[i], bw_im[i], 0, 255,
              THRESH_BINARY_INV);               // INV_THRESH_BINARY
    bw_im[i].convertTo(bw_im[i], CV_8U, 1, 0);  // 64f->8u
  }

  // parallel_for_(Range(0, 2),
  //               MyLoopBody(image, Gimage_im, BW_im, K, Gim_mat, bw_im));

  FHD = hamming(bw_im[0], bw_im[1]);
  cout << "FHD=" << FHD << endl;
  //下面做平移复位的操作代码没有用
  // if (FHD >= 0.1 && FHD <= 0.25)
  // {
  //   int ret = TransformPic(speckle_database, speckle_auth, speckle_auth);
  //   if (ret != -1) {
  //     image[2] = Mat2Emx_U8(speckle_auth);
  //     gabor_im(image[2], kWaveLength, 45, Gimage_im[2], BW_im[2], K[2]);
  //     Gim_mat[2] = Emx2Mat_U8(Gimage_im[2]);
  //     threshold(Gim_mat[2], bw_im[2], 0, 255, THRESH_BINARY_INV);
  //     bw_im[2].convertTo(bw_im[2], CV_8U, 1, 0);
  //     FHD = hamming(bw_im[0], bw_im[2]);
  //     cout << "FHD2=" << FHD << endl;
  //   }
  // }

  for (int i = 0; i < 3; i++) {
    emxDestroyArray_boolean_T(K[i]);
    emxDestroyArray_boolean_T(BW_im[i]);
    emxDestroyArray_real_T(Gimage_im[i]);
    if (image[i]) emxDestroyArray_uint8_T(image[i]);
  }

  
  return FHD;
}