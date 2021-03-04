#include "authpic.h"

#include <algorithm>  //输出vector所有值
#include <iostream>
#include <iterator>
#include <numeric>  //计算方差和均值
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <vector>

// using namespace cv;
// using namespace cv::xfeatures2d;

namespace check_system {

emxArray_real_T *AuthPic::Gimage_im[3];
emxArray_boolean_T *AuthPic::BW_im[3];
emxArray_boolean_T *AuthPic::K[3];
emxArray_uint8_T *AuthPic::image[3];
cv::Mat AuthPic::bw_im[3];
cv::Mat AuthPic::Gim_mat[3];
double *AuthPic::wave_length_ = nullptr;

emxArray_uint8_T *Mat2Emx_U8(cv::Mat &srcImage) {
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
cv::Mat Emx2Mat_U8c(const emxArray_uint8_T *srcImage) {
  cv::Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0],
                             CV_8U, srcImage->data);
  return dstImage;
}
cv::Mat Emx2Mat_U8(const emxArray_real_T *srcImage) {
  cv::Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0],
                             CV_64F, srcImage->data);
  return dstImage;
}
cv::Mat Emx2Mat_bool(const emxArray_boolean_T *srcImage) {
  cv::Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0],
                             CV_8U, srcImage->data);
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
double hamming(cv::Mat input1, cv::Mat input2) {
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

int TransformPic(cv::Mat img1, cv::Mat img2, cv::Mat rI2) {
  cv::Mat rI1;

  if (img1.empty() || img2.empty()) {
    std::cout << "Could not open or find the image!\n" << std::endl;
    return -1;
  }

  //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
  int minHessian = 800;
  // int minHessian = 5;
  cv::Ptr<cv::xfeatures2d::SURF> detector =
      cv::xfeatures2d::SURF::create(minHessian);
  std::vector<cv::KeyPoint> keypoints1, keypoints2;
  cv::Mat descriptors1, descriptors2;
  detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
  detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

  std::cout << "key1 key2  " << keypoints1.size() << "   " << keypoints2.size() <<std::endl;
  if (keypoints1.size() == 0 || keypoints2.size() == 0) {
    return -1;
  }

  //-- Step 2: Matching descriptor vectors with a brute force matcher
  // Since SURF is a floating-point descriptor NORM_L2 is used
  /*Ptr<DescriptorMatcher> matcher =
  DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE); std::vector< DMatch
  > matches; matcher->match(descriptors1, descriptors2, matches);*/

  /*KNN匹配方法*/
  cv::Ptr<cv::DescriptorMatcher> matcher =
      cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
  std::vector<std::vector<cv::DMatch> > knn_matches;
  matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
  // cout << "KNN_matches; " << knn_matches.size() << endl;
  /*对应matlab中的MaxRatio*/
  const float ratio_thresh = 0.6f;
  std::vector<cv::DMatch> good_matches;
  for (size_t i = 0; i < knn_matches.size(); i++) {
    if (knn_matches[i][0].distance <
        ratio_thresh * knn_matches[i][1].distance) {
      good_matches.push_back(knn_matches[i][0]);
    }
  }
  // cout << "ratio_thresh good_matches; " << good_matches.size() << endl;
  /*对应matlab中的MatchThreshold*/
  // sort函数对数据进行升序排列
  //筛选匹配点，根据match里面特征对的距离从小到大排序
  //筛选出最优的knn_matches.size() * MatchThreshold个匹配点
  float MatchThreshold = 0.1;
  sort(knn_matches.begin(), knn_matches.end());
  float matche_distant =
      knn_matches[floor(knn_matches.size() * MatchThreshold)][0]
          .distance;  //位于knn_matches.size() * MatchThreshold处的distance
  std::vector<cv::DMatch> good_good_matches;
  for (int i = 0; i < good_matches.size() * MatchThreshold; i++) {
    if (good_matches[i].distance < matche_distant)
      good_good_matches.push_back(good_matches[i]);
  }
  // cout << "MatchThreshold good good_matches; " << good_good_matches.size() <<
  // endl;

  //-- Draw matches
  std::vector<cv::Point2f> keypoints01;
  std::vector<cv::Point2f> keypoints02;
  for (size_t i = 0; i < good_good_matches.size(); i++) {
    //-- Get the keypoints from the good matches
    keypoints01.push_back(keypoints1[good_good_matches[i].queryIdx].pt);
    keypoints02.push_back(keypoints2[good_good_matches[i].trainIdx].pt);
    circle(img1, keypoints01[i], 10, cv::Scalar(0, 255, 0));
  }
  // cout << "  1 的特征点的坐标" << keypoints01 << endl;
  // cout << "  2 的特征点的坐标" << keypoints02 << endl;

  cv::Mat img_matches;

  drawMatches(img1, keypoints1, img2, keypoints2, good_good_matches,
              img_matches, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255));
  bitwise_not(img1, img1);
  //    imshow("img1", img1);

  //-- Localize the object
  /*得到成对的特征点的坐标，因为特征点1 和2  的数量不相同直接使用 .pt
   * 的话，得到的可能不是成对的两个点*/
  std::vector<cv::Point2f> obj;
  std::vector<cv::Point2f> scene;

  for (size_t i = 0; i < good_good_matches.size(); i++) {
    //-- Get the keypoints from the good matches
    obj.push_back(keypoints1[good_good_matches[i].queryIdx].pt);
    scene.push_back(keypoints2[good_good_matches[i].trainIdx].pt);
  }

  // cout << "筛选之前的数量" << good_good_matches.size() << endl;

  // TODO
  std::vector<cv::Point2f> x1;
  std::vector<cv::Point2f>::iterator i;
  std::vector<cv::Point2f>::iterator j;
  std::vector<float> distant;  //半径过大异常

  int ii = 0;
  int test_count = 0;  //距离大于阈值的个数  测试删除部分是否正确
  for (i = obj.begin(), j = scene.begin();
       // ii<786;
       (i != obj.end()) && (j != scene.end()); ii++, i++, j++) {
    distant.push_back(powf(((*i).x - (*j).x), 2) + powf(((*i).y - (*j).y), 2));
    if (distant[ii] > 2000) {
      test_count++;
    }
  }

  /*DMatch的distance（在0-1之间）  和手动计算的distance*/
  /*for (int a = 0; a < 768; a++) {
      cout<<matches[a].distance<<endl;
  }
  cout << "---------------------";
  std::copy(distant.begin(), distant.end(), std::ostream_iterator<int>(cout,
  "\n"));*/

  for (
      int a = 0;
      a <
      distant
          .size();) {  /// distant.size()
                       /// 是变化的，值为最开始的size-count（删除了多少个）,因为做了删除之后值会变小
    if (distant[a] > 2000) {
      // cout << " 删除此点 位置 距离" << obj[a] << "  " << scene[a] << " " <<
      // distant[a] << endl;
      obj.begin() + a = obj.erase(obj.begin() + a);  //删除
      scene.begin() + a = scene.erase(scene.begin() + a);
      distant.begin() + a = distant.erase(distant.begin() + a);

    } else {
      a++;
    }
  }
  // cout << "删除半径过大的 " << obj.size() << "  " << distant.size() << endl;

  /*经过测试，删除成功*/
  /*cout << endl << "删除之后的 理论值" << matches.size() -test_count << endl;
  cout << "实际值 "<<obj.size()<<"  "<<distant.size()<<endl;*/

  /*消除半径异常的*/
  /*  tip  在matlab 代码中没有判断删除完半径过大 是否为空*/
  for (int aa = 0; aa < 3; aa++) {
    if (distant.size() != 0) {
      float sum = std::accumulate(distant.begin(), distant.end(), 0.0);
      float mean = sum / distant.size();  //均值
      float accum = 0.0;
      std::for_each(distant.begin(), distant.end(),
                    [&](const double d) { accum += (d - mean) * (d - mean); });

      double stdev = sqrt(accum / (distant.size() - 1));  //方差  标准差

      // cout << "avg " << mean << "std  " << stdev << endl;
      for (int a = 0; a < distant.size();) {
        if ((distant[a] > (mean + 3 * stdev)) ||
            (distant[a] < (mean - 3 * stdev))) {
          // cout << " 删除此点 位置 距离" << obj[a] << "  " << scene[a] << " "
          // << distant[a] << endl;
          obj.begin() + a = obj.erase(obj.begin() + a);  //删除
          scene.begin() + a = scene.erase(scene.begin() + a);
          distant.begin() + a = distant.erase(distant.begin() + a);
        } else {
          a++;
        }
      }

    } else
      continue;
  }
  // cout << "/*消除半径异常的*/ " << obj.size() << "  " << distant.size() <<
  // endl;

  /*消除方向异常的*/
  std::vector<float> x_div_y_of_delt_distant;  //方向异常
  /*x_div_y_of_delt_distant 不能和distance在上面的 for 中push
  因为上面做了删除操作，keypoints长度变小，
  x_div_y_of_delt_distant如果在上面初始化，长度会比做了半径删除操作之后的keypoints的长度长，溢出*/
  for (i = obj.begin(), j = scene.begin();
       (i != obj.end()) && (j != scene.end()); i++, j++) {
    x_div_y_of_delt_distant.push_back(cv::Point2f(i - j).x /
                                      cv::Point2f(i - j).y);
  }

  // cout << "删除方向异常前 " << x_div_y_of_delt_distant.size();
  for (int aa = 0; aa < 3; aa++) {
    float sum = std::accumulate(x_div_y_of_delt_distant.begin(),
                                x_div_y_of_delt_distant.end(), 0.0);
    float mean = sum / x_div_y_of_delt_distant.size();  //均值
    float accum = 0.0;
    std::for_each(x_div_y_of_delt_distant.begin(),
                  x_div_y_of_delt_distant.end(),
                  [&](const double d) { accum += (d - mean) * (d - mean); });

    double stdev =
        sqrt(accum / (x_div_y_of_delt_distant.size() - 1));  //方差  标准差

    // cout << "avg " << mean << "std  " << stdev << endl;
    // int b=0;
    if (x_div_y_of_delt_distant.size() != 0) {
      for (int a = 0; a < x_div_y_of_delt_distant.size();) {
        if ((x_div_y_of_delt_distant[a] > (mean + 3 * stdev)) ||
            (x_div_y_of_delt_distant[a] < (mean - 3 * stdev))) {
          // cout << " 删除此点 位置 方向" << obj[a] << "  " << scene[a] << " "
          // << x_div_y_of_delt_distant[a] << endl;
          obj.begin() + a = obj.erase(obj.begin() + a);  //删除
          scene.begin() + a = scene.erase(scene.begin() + a);
          x_div_y_of_delt_distant.begin() + a = x_div_y_of_delt_distant.erase(
              x_div_y_of_delt_distant.begin() + a);
          // cout <<" x "<< b<<endl;
        } else {
          a++;
          // cout << " y " << b << endl;
        }
        // b++;
      }
    } else
      continue;
  }
  // cout << "/*消除方向异常的*/ " << x_div_y_of_delt_distant.size() << "   " <<
  // obj.size() << endl;

  cv::Rect rect =
      cv::Rect(cv::Point(50, 50), cv::Point(img2.cols - 50, img2.rows - 50));

  if (obj.size() != 0) {
    if (obj.size() >= 8) {
      /*投射变换，旋转*/

      /*变换核 需要变换的，参考的 （类型：特征点的坐标）*/
      cv::Mat HH = findHomography(scene, obj, cv::RANSAC);

      /*做变换的图片，输出，变换核，大小*/
      cv::warpPerspective(img2, rI2, HH, img2.size(), cv::INTER_LINEAR,
                          cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

    }

    else if ((obj.size() < 8) && (obj.size() > 0)) {
      cv::Point2f dx;
      if (obj.size() == 1) {
        dx =
            cv::Point2f(floor((obj[0] - obj[0]).x), floor((obj[0] - obj[0]).y));
      } else {
        /*求均值*/
        float avgx = 0;
        float sumx = 0;
        float avgy = 0;
        float sumy = 0;
        for (int aa = 0; aa < obj.size(); aa++) {
          sumx += (obj[aa] - scene[aa]).x;
          sumy += (obj[aa] - scene[aa]).y;
        }
        dx = cv::Point2f(floor(sumx / obj.size()), floor(sumy / obj.size()));
      }

      /*平移变换*/
      cv::Size dst_sz = img2.size();

      //定义平移矩阵
      cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);

      t_mat.at<float>(0, 0) = 1;
      t_mat.at<float>(0, 2) = dx.x;  //水平平移量
      t_mat.at<float>(1, 1) = 1;
      t_mat.at<float>(1, 2) = dx.y;  //竖直平移量

      //根据平移矩阵进行仿射变换
      cv::warpAffine(img2, rI2, t_mat, dst_sz);
      rI2 = cv::Mat(rI2, rect);

    }

    else {
      rI2 = cv::Mat(img2, rect);
    }
  } else {
    rI2 = cv::Mat(img2, rect);
  }

  rI1 = cv::Mat(img1, rect);

  ////-- Show detected matches
  //    imshow("Matches", img_matches);
  //    imshow("rI2", rI2);
  //    waitKey();
  return 0;
}

class MyLoopBody : public cv::ParallelLoopBody {
 public:
  MyLoopBody(emxArray_uint8_T **image, emxArray_real_T **Gimage_im,
             emxArray_boolean_T **BW_im, emxArray_boolean_T **K,
             cv::Mat *Gim_mat, cv::Mat *bw_im) {
    image_ = image;
    Gimage_im_ = Gimage_im;
    K_ = K;
    BW_im_ = BW_im;
    Gim_mat_ = Gim_mat;
    bw_im_ = bw_im;
  };
  virtual void operator()(const cv::Range &range) const {
    for (int colIdx = range.start; colIdx < range.end; ++colIdx) {
      gabor_im(image_[colIdx], *AuthPic::wave_length_, 45, Gimage_im_[colIdx],
               BW_im_[colIdx], K_[colIdx]);

      Gim_mat_[colIdx] = Emx2Mat_U8(Gimage_im_[colIdx]);
      //阈值
      cv::threshold(Gim_mat_[colIdx], bw_im_[colIdx], 0, 255,
                    cv::THRESH_BINARY_INV);  // INV_THRESH_BINARY

      bw_im_[colIdx].convertTo(bw_im_[colIdx], CV_8U, 1, 0);  // 64f->8u
    }
  }

 private:
  emxArray_real_T **Gimage_im_;
  emxArray_boolean_T **BW_im_;
  emxArray_boolean_T **K_;
  emxArray_uint8_T **image_;
  cv::Mat *Gim_mat_;
  cv::Mat *bw_im_;
};
int AuthPic::InitAuth(double* wave_length) {
  // Initialize the application.
  // 其实就是打开OpenMp并行
  gabor_im_initialize();
  for (int i = 0; i < 3; i++) {
    emxInitArray_real_T(&Gimage_im[i], 2);
    emxInitArray_boolean_T(&BW_im[i], 2);
    emxInitArray_boolean_T(&K[i], 2);
  }
  wave_length_ = wave_length; 
  return 0;
}
int AuthPic::DestroyAuth() {
  for (int i = 0; i < 3; i++) {
    emxDestroyArray_boolean_T(K[i]);
    emxDestroyArray_boolean_T(BW_im[i]);
    emxDestroyArray_real_T(Gimage_im[i]);
    if (image[i]) emxDestroyArray_uint8_T(image[i]);
  }
  // Terminate the application.
  gabor_im_terminate();
  return 0;
}

double AuthPic::DoAuthPic(cv::Mat speckle_database, cv::Mat speckle_auth, double auth_threshold) {
  if (!speckle_auth.data || !speckle_auth.data) {
    std::cout << "picture data error!!!!\n";
    return 1;
  }

  double FHD = 0;

  if (!image[0] || image[0]->allocatedSize !=
                       speckle_database.cols * speckle_database.rows) {
    if (image[0]) emxDestroyArray_uint8_T(image[0]);
    image[0] = emxCreate_uint8_T(speckle_database.cols, speckle_database.rows);
  }
  memcpy(image[0]->data, speckle_database.data, image[0]->allocatedSize);

  if (!image[1] ||
      image[1]->allocatedSize == speckle_auth.cols * speckle_auth.rows) {
    if (image[1]) emxDestroyArray_uint8_T(image[1]);
    image[1] = emxCreate_uint8_T(speckle_auth.cols, speckle_auth.rows);
  }
  memcpy(image[1]->data, speckle_auth.data, image[1]->allocatedSize);

  // image[0] = Mat2Emx_U8(speckle_database);
  // image[1] = Mat2Emx_U8(speckle_auth);

  // for (int i = 0; i < 2; i++) {
  //   std::cout << "allocatedSize " << image[i]->allocatedSize << std::endl;
  //   std::cout << "numDimensions " << image[i]->numDimensions << std::endl;
  //   std::cout << "image1 size : " << image[i]->size[0] << " "
  //             << image[i]->size[1] << std::endl;
  // }

  for (int i = 0; i < 2; i++) {
    gabor_im(image[i], *AuthPic::wave_length_, 45, Gimage_im[i], BW_im[i], K[i]);
    Gim_mat[i] = Emx2Mat_U8(Gimage_im[i]);
    //阈值
    threshold(Gim_mat[i], bw_im[i], 0, 255,
              cv::THRESH_BINARY_INV);           // INV_THRESH_BINARY
    bw_im[i].convertTo(bw_im[i], CV_8U, 1, 0);  // 64f->8u
  }

  // parallel_for_(Range(0, 2),
  //               MyLoopBody(image, Gimage_im, BW_im, K, Gim_mat, bw_im));

  FHD = hamming(bw_im[0], bw_im[1]);
  std::cout << "FHD=" << FHD << std::endl;
  //下面做平移复位的操作代码没有用
  if (FHD >= auth_threshold && FHD <= 0.35) {
    int ret = TransformPic(speckle_database, speckle_auth, speckle_auth);
    if (ret != -1) {
      if (!image[1] ||
          image[1]->allocatedSize == speckle_auth.cols * speckle_auth.rows) {
        if (image[1]) emxDestroyArray_uint8_T(image[1]);
        image[1] = emxCreate_uint8_T(speckle_auth.cols, speckle_auth.rows);
      }
      memcpy(image[1]->data, speckle_auth.data, image[1]->allocatedSize);
      // image[2] = Mat2Emx_U8(speckle_auth);

      gabor_im(image[1], *AuthPic::wave_length_, 45, Gimage_im[1], BW_im[1], K[1]);
      Gim_mat[1] = Emx2Mat_U8(Gimage_im[1]);
      threshold(Gim_mat[1], bw_im[1], 0, 255, cv::THRESH_BINARY_INV);
      bw_im[1].convertTo(bw_im[1], CV_8U, 1, 0);
      FHD = hamming(bw_im[0], bw_im[1]);
      std::cout << "  FHD2=" << FHD << std::endl;
    }
  }

  return FHD;
}
}  // namespace check_system