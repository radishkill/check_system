

// Include Files
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "main.h"
#include "gabor_im_terminate.h"
#include "gabor_im_emxAPI.h"
#include "gabor_im_initialize.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../../../imtrans_my.h"

using namespace cv;
using namespace std;


// Function Declarations

static double argInit_real_T();
static unsigned char argInit_uint8_T();
static emxArray_uint8_T *c_argInit_UnboundedxUnbounded_u();

// Function Definitions



emxArray_uint8_T* Mat2Emx_U8(Mat& srcImage)
{
	int idx0;
	int idx1;
	emxArray_uint8_T* dstImage;
	// Set the size of the array.
	// Change this size to the value that the application requires.
	dstImage = emxCreate_uint8_T(srcImage.cols, srcImage.rows);  //这里的行列是反向的
	//memcpy(result, Io.data, result->allocatedSize * sizeof(uchar));

	// Loop over the array to initialize each element.
	for (idx0 = 0; idx0 < dstImage->size[0U]; idx0++) {
		for (idx1 = 0; idx1 < dstImage->size[1U]; idx1++) {
			// Set the value of the array element.
			// Change this value to the value that the application requires.
			dstImage->data[idx0 + dstImage->size[0] * idx1] = srcImage.data[idx0 + dstImage->size[0] * idx1];
		}

	}
	return dstImage;
}
Mat Emx2Mat_U8c(const emxArray_uint8_T* srcImage)
{

	Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_8U, srcImage->data);
	return dstImage;

}
Mat Emx2Mat_U8(const emxArray_real_T* srcImage)
{

	Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_64F, srcImage->data);
	return dstImage;

}
Mat Emx2Mat_bool(const emxArray_boolean_T* srcImage)
{

	Mat dstImage = cv::Mat((int)srcImage->size[1], (int)srcImage->size[0], CV_8U, srcImage->data);
	return dstImage;

}



static double argInit_real_T()
{
  return 0.0;
}

//
// Arguments    : void
// Return Type  : unsigned char
//
static unsigned char argInit_uint8_T()
{
  return 0U;
}

//
// Arguments    : void
// Return Type  : emxArray_uint8_T *
//
static emxArray_uint8_T *c_argInit_UnboundedxUnbounded_u()
{
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



//hamming
double hamming(Mat input1, Mat input2)
{
	double diff = 0;
	int count = 0;
	for (int i = 0; i < input2.rows; i++)
	{
		uchar* data1 = input1.ptr<uchar>(i);
		uchar* data2 = input2.ptr<uchar>(i);
		for (int j = 0; j < input2.cols; j++)
		{
			if (data1[j] != data2[j])
			{
				diff++;
			}
		}
	}
	//cout << "pdist=" << diff << endl;
	diff = diff / input1.cols / input1.rows;
	cout << "FHD=" << diff << endl;
	return diff;
}

//
// Arguments    : int argc
//                const char * const argv[]
// Return Type  : int
//
int main(int, const char * const [])
{
  // Initialize the application.
  gabor_im_initialize();

  emxArray_real_T* Gimage_im;
  emxArray_boolean_T* BW_im;
  emxArray_boolean_T* K;
  emxArray_uint8_T* image;
  emxArray_real_T* Gimage_im2;
  emxArray_boolean_T* BW_im2;
  emxArray_boolean_T* K2;
  emxArray_uint8_T* image2;
  emxArray_real_T* Gimage_im3;
  emxArray_boolean_T* BW_im3;
  emxArray_boolean_T* K3;
  emxArray_uint8_T* image3;
  


  emxInitArray_real_T(&Gimage_im, 2);
  emxInitArray_boolean_T(&BW_im, 2);
  emxInitArray_boolean_T(&K, 2);
  emxInitArray_real_T(&Gimage_im2, 2);
  emxInitArray_boolean_T(&BW_im2, 2);
  emxInitArray_boolean_T(&K2, 2);
  emxInitArray_real_T(&Gimage_im3, 2);
  emxInitArray_boolean_T(&BW_im3, 2);
  emxInitArray_boolean_T(&K3, 2);

  // Initialize function input argument 'image'. 
  Mat speckle_database = imread("13.bmp",CV_8U);
  Mat speckle_auth = imread("14.bmp", CV_8U);
  Mat ROI = speckle_database;//= speckle_database(Rect(50,50, speckle_database.cols-50, speckle_database.rows-50));
  Mat ROI2 = speckle_auth;//= speckle_auth(Rect(50, 50, speckle_auth.cols - 50, speckle_auth.rows - 50));
  Mat bw_im;                                //相位二值化
  Mat bw_im2;
  Mat bw_im3;
  Mat Gim_mat;
  Mat Gim_mat2;
  Mat Gim_mat3;
  //Mat image11= imread("11.bmp", CV_8U);
  //Mat image22 = imread("22.bmp", CV_8U);

  image = Mat2Emx_U8(ROI);
  image2= Mat2Emx_U8(ROI2);

  //int a = image->size[0];
  //int b = image->size[1];
  //namedWindow("Display imageS", WINDOW_NORMAL); // Create a window for display.
  //imshow("Display imageS", imageS);
  //namedWindow("Display imageS1", WINDOW_NORMAL); // Create a window for display.
  //imshow("Display imageS1", Emx2Mat_U8c(image));
  
  

  
  //   'gabor_im'.
  gabor_im(image, 8, 45, Gimage_im, BW_im, K);
  gabor_im(image2,8,45, Gimage_im2, BW_im2, K2);

   Gim_mat = Emx2Mat_U8(Gimage_im);
   Gim_mat2 = Emx2Mat_U8(Gimage_im2);

  threshold(Gim_mat, bw_im, 0, 255,THRESH_BINARY_INV);//INV_THRESH_BINARY
  threshold(Gim_mat2, bw_im2, 0, 255, THRESH_BINARY_INV);//INV_THRESH_BINARY

  bw_im.convertTo(bw_im, CV_8U, 1, 0);        //64f->8u
  bw_im2.convertTo(bw_im2, CV_8U, 1, 0);
  double FHD=hamming(bw_im, bw_im2);
 

  namedWindow("Display bw_im", WINDOW_AUTOSIZE); // Create a window for display.
  imshow("Display bw_im", bw_im);
  namedWindow("Display bw_im2", WINDOW_AUTOSIZE); // Create a window for display.
  imshow("Display bw_im2", bw_im2);



  if (FHD >= 0.1 && FHD <= 0.25)
  {
	 
	  transform_my(speckle_database, speckle_auth, speckle_auth);
	  image3 = Mat2Emx_U8(speckle_auth);
	  gabor_im(image3, 8, 45, Gimage_im3, BW_im3, K3);
	  Gim_mat3 = Emx2Mat_U8(Gimage_im3);
	  threshold(Gim_mat3, bw_im3, 0, 255, THRESH_BINARY_INV);
	  imshow("bw_im3", bw_im3);
	  bw_im3.convertTo(bw_im3, CV_8U, 1, 0);
	  double FHD2 = hamming(bw_im, bw_im3);
	  cout << "FHD=" << FHD2 << endl;
  }








  
   

 
  waitKey(0);
  emxDestroyArray_boolean_T(K);
  emxDestroyArray_boolean_T(BW_im);
  emxDestroyArray_real_T(Gimage_im);
  emxDestroyArray_uint8_T(image);
  emxDestroyArray_boolean_T(K2);
  emxDestroyArray_boolean_T(BW_im2);
  emxDestroyArray_real_T(Gimage_im2);
  emxDestroyArray_uint8_T(image2);

  // Terminate the application.
  gabor_im_terminate();
  return 0;
}


