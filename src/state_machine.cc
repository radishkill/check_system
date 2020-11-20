#include "state_machine.h"

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <omp.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rtwtypes.h"
#include "gabor_im_types.h"
#include "rt_nonfinite.h"
#include "gabor_im.h"
#include "gabor_im_terminate.h"
#include "gabor_im_emxAPI.h"
#include "gabor_im_initialize.h"
#include "imtrans_my.h"

#include "global_arg.h"
#include "laser.h"
#include "utils.h"
#include "camera_manager.h"
#include "key_file.h"
#include "lcd.h"
#include "key_file.h"
#include "led.h"
#include "hostcontroller.h"

namespace check_system {

using namespace cv;
using namespace std;

StateMachine::StateMachine()
    : is_running_(false) {

}

int StateMachine::RunMachine(StateMachine::MachineState state) {
  GlobalArg* arg = GlobalArg::GetInstance();
  if (is_running_) {
    return -1;
  }
  std::lock_guard<std::mutex> l(mutex_);
  is_running_ = true;
  arg->interrupt_flag = 0;
  int ret = 0;
  switch (state) {
    case kSelfTest: {
      std::cout << "Run Self Test" << std::endl;
      ret = SelfTest();
      break;
    }
    case kRegister: {
      std::cout << "Run Register" << std::endl;
      if (arg->is_fault) {
        std::cout << "system fault" << std::endl;
//        break;
      }
      arg->laser->SendOpenCmd();
      //注册模块
      ret = Register();
      if (ret < 0 && arg->host->IsOpen()) {
        arg->host->RegisterFail();
      } else if (ret == 0 && arg->host->IsOpen()) {
        arg->host->RegisterSuccess();
      }
      arg->laser->SendCloseCmd();
      break;
    }
    case kAuth: {
      std::cout << "Run Auth" << std::endl;
      if (arg->is_fault) {
        std::cout << "system fault" << std::endl;
//        break;
      }
      arg->laser->SendOpenCmd();
      ret = Authentication();
      if (ret < 0 && arg->host->IsOpen()) {
        arg->host->AuthFail();
        //失败打灯:红灯开,3个绿灯关
        arg->led->CmosLed(0);
        arg->led->LaserLed(0);
        arg->led->LcdLed(0);
        arg->led->ErrorLed(1);

      } else if (ret == 0 && arg->host->IsOpen()) {
        arg->host->AuthSuccess();
        //成功打灯:红灯关,3个绿灯开
        arg->led->CmosLed(1);
        arg->led->LaserLed(1);
        arg->led->LcdLed(1);
        arg->led->ErrorLed(0);
      }
      arg->laser->SendCloseCmd();
      break;
    }
    default: {
    }
  }
  is_running_ = false;
  return ret;
}
/*
 * 此过程中但凡有一个步骤出错都应该退出
 *
 */
int StateMachine::SelfTest() {
  int ret0,ret1,ret2,ret3;
  GlobalArg* arg = GlobalArg::GetInstance();
  assert(arg->led != nullptr);
  assert(arg->lcd != nullptr);
  assert(arg->laser != nullptr);
  assert(arg->camera != nullptr);

  if (!arg->camera->IsOpen()) {
    std::cout << "camera not open" << std::endl;
    return -1;
  }
  if (!arg->lcd->IsOpen()) {
    std::cout << "lcd not open" << std::endl;
    return -1;
  }
  if (!arg->laser->IsOpen()) {
    std::cout << "laser not open" << std::endl;
    return -1;
  }

  ret1 = arg->laser->SendCloseCmd();
  Utils::MSleep(3000);

  //设置温度
  ret1 = arg->laser->SetTemperature(20);
  Utils::MSleep(1000);

  //电流
  ret1 = arg->laser->SetCurrent(3000);
  Utils::MSleep(1000);
  //设置最大电流
  ret1 = arg->laser->SetMaxCurrent(5000);
  Utils::MSleep(1000);

  int n = 3;
  while (n--) {
    //这里面最长超时时间为5s
    ret1 = arg->laser->SendOpenCmd();
    //启动成功
    if (ret1 == 0)
      break;
    Utils::MSleep(1000);
  }

  int random_seed = GenerateRandomSeed();

  arg->lcd->ShowBySeed(random_seed);

  ret2 = arg->camera->GetOnePic();

  ret3 = arg->camera->CheckPic(100, 200);
  if (ret3 == -1) {
    std::cout << "check pic : pic wrong" << std::endl;
  }

  ret1 = arg->laser->SendCloseCmd();

  //逻辑与,输出LED灯的状态
  //错误
  if((ret0 != 0)||(ret1 != 0)||(ret2 != 0)||(ret3 != 0)) {
      arg->led->error_blink_=200;
    if((ret0 != 0)||(ret1 != 0)) {
      arg->led->laser_blink_=200;
    }
    if((ret2 != 0)||(ret3 != 0)){
      arg->led->lcd_blink_ =200;
      arg->led->cmos_blink_ =200;
    }
    arg->is_fault = 1;
  } else {
    //正确ok
    arg->led->ErrorLed(0);
    for(int i = 0 ; i < 3; i++ ){
      arg->led->LaserLed(0);
      arg->led->CmosLed(0);
      arg->led->LcdLed(0);
      Utils::MSleep(500);
      arg->led->LaserLed(1);
      arg->led->CmosLed(1);
      arg->led->LcdLed(1);
      Utils::MSleep(500);
    }
    arg->led->LaserLed(1);
    arg->led->CmosLed(1);
    arg->led->LcdLed(1);
    arg->is_fault = 0;
  }
  return 0;
}

//注册
int StateMachine::Register() {
  GlobalArg* arg = GlobalArg::GetInstance();
  int ret;

  assert(arg->led != nullptr);
  assert(arg->lcd != nullptr);
  assert(arg->laser != nullptr);
  assert(arg->camera != nullptr);

  if (!arg->camera->IsOpen()) {
    std::cout << "The camera can't turned on" << std::endl;
    return -1;
  }

  arg->led->lcd_blink_ =0;
  arg->led->cmos_blink_ =0;
  arg->led->laser_blink_=0;
  arg->led->error_blink_=0;
  Utils::MSleep(100);

  //全灯OFF
  arg->led->CmosLed(0);
  arg->led->LaserLed(0);
  arg->led->LcdLed(0);
  arg->led->ErrorLed(0);
  Utils::MSleep(250);
  std::cout << "led off ok" << std::endl;

  if (arg->sm->CheckKey() == -1) {
    //检测到无key插入
    //设置灯显示 并返回到起始点
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    arg->led->ErrorLed(1);
    std::cout << "no key insert" << std::endl;
    return -1;
  }

  ret = arg->sm->CheckAdminKey();
  //非管理员key插入
  if(ret == 0) {
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    for(int i = 0 ; i < 20 ; i++ ){
      arg->led->ErrorLed(0);
      Utils::MSleep(100);
      arg->led->ErrorLed(1);
      Utils::MSleep(100);
    }
    return -1;
  } else {
    //管理员key插入
    arg->led->ErrorLed(0);
    for(int i = 0 ; i < 20 ; i++ ) {
      arg->led->CmosLed(0);
      arg->led->LaserLed(0);
      arg->led->LcdLed(0);
      Utils::MSleep(250);
      arg->led->CmosLed(1);
      arg->led->LaserLed(1);
      arg->led->LcdLed(1);
      Utils::MSleep(250);
    }
  }
  //給用户插入新卡的时间
  int time = 10;
  while (time--) {
    if (arg->interrupt_flag) {
      arg->interrupt_flag=0;
      return -1;
    }
    Utils::MSleep(1000);
  }
  if (arg->sm->CheckKey() == -1) {
    //检测到无key插入
    //设置灯显示 并返回到起始点
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    arg->led->ErrorLed(1);
    return -1;
  }
  int key_id = arg->sm->FindKey();
  if (key_id == -1) {
    //被中断
    if (arg->interrupt_flag) {
      arg->interrupt_flag = 0;
      return -1;
    }

    //没找到库 并新建
    int key_id = arg->key_file->AppendPufFile();
    if (key_id == -1) {
      //库满
      return -1;
    }
  }
  arg->sm->CheckEmptyPair(key_id);

  //中断返回复位状态
  if (arg->interrupt_flag) {
    arg->interrupt_flag=0;
    return -1;
  }

  //连续拍100张照片
  arg->camera->Play();
  for (unsigned int i = 0; i < pair_list_.size() && i < 100; i++) {
    GlobalArg* arg = GlobalArg::GetInstance();
    int seed = arg->sm->GenerateRandomSeed();
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetPic();

    arg->key_file->CopyPicToBuffer(arg->camera->GetRBGBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
    //保存激励对
    arg->key_file->SavePicAndSeed(key_id, pair_list_[i], seed);

    //中断返回复位状态
    if (arg->interrupt_flag == 1) {
      arg->interrupt_flag=0;
      goto status_fault;
    }
  }
  arg->camera->Pause();
  return 0;
  status_fault:
  arg->camera->Pause();
  return -1;
}
//认证
int StateMachine::Authentication() {
  GlobalArg* arg = GlobalArg::GetInstance();

  assert(arg->led != nullptr);
  assert(arg->lcd != nullptr);
  assert(arg->laser != nullptr);
  assert(arg->camera != nullptr);

  if (!arg->camera->IsOpen()) {
    return -1;
  }

  //全灯OFF
  arg->led->CmosLed(0);
  arg->led->LaserLed(0);
  arg->led->LcdLed(0);
  arg->led->ErrorLed(0);
  Utils::MSleep(2 * 1000);
  if (arg->sm->CheckKey() == -1) {
    //检测到无key插入
    //设置灯显示 并返回到起始点
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    arg->led->error_blink_=100;
    //跳转至“认证失败”处理模块
    if (arg->interrupt_flag == 1) {
      arg->interrupt_flag=0;
    }
    return -1;
  }
  int key_id = arg->sm->FindKey();
  if (key_id == -1) {
   //跳转至“认证失败”处理模块
    if (arg->interrupt_flag == 1) {
        arg->interrupt_flag=0;
    }
    return -1;
  }

  int ret = CheckAvailablePair(key_id);
  if (ret == 0) {
    //没有激励对了,直接跳转至“认证失败”处理模块
    if (arg->interrupt_flag == 1) {
        arg->interrupt_flag=0;
    }
    return -1;
  }
  int n = 10;
  while (n--) {
    int index = std::rand()%ret;
    int seed_index = pair_list_[index];
    int seed = arg->key_file->GetSeed(key_id, seed_index);
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetOnePic();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    int result = AuthPic(arg->camera->GetRBGBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT, arg->key_file->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);

    //删除本次循环使用的Seed文件及其对应的Pic文件
    arg->key_file->DeleteSeed(key_id,seed_index);
    arg->key_file->DeletePic(key_id,seed_index);

    //值越小说明两张图片越相似
    if (result < 0.25) {
      //认证成功了,绿灯1 2 3 ON
      arg->led->LaserLed(1);
      arg->led->LcdLed(1);
      arg->led->CmosLed(1);
      break;
    }
  }
  //认证失败
  if (n < 0) {
    //红灯 ON
    arg->led->ErrorLed(1);
    //中断返回复位状态
    if (arg->interrupt_flag == 1) {
      arg->interrupt_flag=0;
      return -1;
      }
  }
  return 0;
}

//随机生成seed a number range from 0 to RAND_MAX
int StateMachine::GenerateRandomSeed() {
  std::srand(std::time(nullptr));
//  return (int)(std::rand()*48271ll%2147483647);
  return std::rand();
}

//库定位算法 判断一枚key是否已经建立过数据库了
int StateMachine::FindKey() {
  GlobalArg* arg = GlobalArg::GetInstance();
  assert(arg->camera != nullptr);
  assert(arg->key_file != nullptr);
  assert(arg->lcd != nullptr);
  char* temp_pic;
  char* pic;

  std::srand(std::time(nullptr));
  int i = 0;
  while (i < 100) {
    if (arg->interrupt_flag) {
      return -1;
    }
    int seed_index = std::rand()%1000;
    int seed = arg->key_file->GetSeed(i, seed_index);
    arg->lcd->ShowBySeed(seed);
    arg->key_file->GetPic(i, seed_index);
    arg->camera->GetOnePic();
    temp_pic = arg->camera->GetRBGBuffer();
    pic = arg->key_file->GetPicBuffer();
    //运算temp_pic 与 pic 得到结果
    int result = AuthPic(pic, CAMERA_WIDTH, CAMERA_HEIGHT, temp_pic, CAMERA_WIDTH, CAMERA_HEIGHT);
    if (result <= 5) {
      //找到相应的库
      break;
    }
    i++;
  }
  if (i >= 100)
    i = -1;

  //中断返回复位状态
  if(arg->interrupt_flag==1){
    arg->interrupt_flag=0;
    return -1;
  }
  return i;
}
//插入检测算法
int StateMachine::CheckKey()
{
  GlobalArg* arg = GlobalArg::GetInstance();
  int seed = arg->sm->GenerateRandomSeed();
  std::cout << "rand seed : " << seed << std::endl;
  arg->lcd->ShowBySeed(seed);
  std::cout << "show seed!!" << std::endl;
  arg->camera->GetOnePic();
  return arg->camera->CheckPic(100, 200);
}
//管理员KEY检测算法
int StateMachine::CheckAdminKey() {
  int n = 10;

  GlobalArg* arg = GlobalArg::GetInstance();
  while (n--) {
    int seed_index = std::rand()%1000;
    int seed = arg->key_file->GetSeed(0, seed_index);
    arg->key_file->GetPic(0, seed_index);
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetOnePic();
    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    double result = AuthPic(arg->key_file->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT, arg->camera->GetRBGBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
    if (result < 0.25) {
      //结果匹配
      //.....
      return 1;
    }

    arg->key_file->DeletePic(0, seed_index);
    arg->key_file->DeleteSeed(0, seed_index);

    int rand_seed = arg->sm->GenerateRandomSeed();
    arg->lcd->ShowBySeed(rand_seed);

    arg->camera->GetOnePic();

    arg->key_file->CopyPicToBuffer(arg->camera->GetRBGBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
    arg->key_file->SavePicAndSeed(0, seed_index, rand_seed);
  }
  //非管理员key插入
  return 0;
}
//库遍历算法
int StateMachine::CheckEmptyPair(int id) {
  GlobalArg* arg = GlobalArg::GetInstance();
  //清空empty_pairs
  std::vector<int>().swap(pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (arg->key_file->IsSeedAvailable(id, i)) continue;
    pair_list_.push_back(i);
  }

  return pair_list_.size();
}
//检查可用激励对
int StateMachine::CheckAvailablePair(int id) {
  GlobalArg* arg = GlobalArg::GetInstance();
  //清空empty_pairs
  std::vector<int>().swap(pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (!arg->key_file->IsSeedAvailable(id, i)) continue;
    pair_list_.push_back(i);
  }
  return pair_list_.size();
}

emxArray_uint8_T* Mat2Emx_U8(Mat& srcImage)
{
  int idx0;
  int idx1;
  emxArray_uint8_T* dstImage;
  // Set the size of the array.
  // Change this size to the value that the application requires.
  dstImage = emxCreate_uint8_T(srcImage.cols, srcImage.rows);  //ÕâÀïµÄÐÐÁÐÊÇ·´ÏòµÄ
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

int StateMachine::AuthPic(char *pic1, int h1, int w1, char *pic2, int h2, int w2) {

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
//  Mat speckle_database = imread("./13.bmp",CV_8U);
//  Mat speckle_auth = imread("./14.bmp", CV_8U);

  Mat speckle_database(h1, w1, CV_8UC1);
  Mat speckle_auth(h2, w2, CV_8UC1);
  std::memcpy(speckle_database.data, pic1, h1*w1);
  std::memcpy(speckle_auth.data, pic2, h2*w2);

  Mat ROI = speckle_database;//= speckle_database(Rect(50,50, speckle_database.cols-50, speckle_database.rows-50));
  Mat ROI2 = speckle_auth;//= speckle_auth(Rect(50, 50, speckle_auth.cols - 50, speckle_auth.rows - 50));
  Mat bw_im;                                //ÏàÎ»¶þÖµ»¯
  Mat bw_im2;
  Mat bw_im3;
  Mat Gim_mat;
  Mat Gim_mat2;
  Mat Gim_mat3;
  //Mat image11= imread("11.bmp", CV_8U);
  //Mat image22 = imread("22.bmp", CV_8U);

  image = Mat2Emx_U8(ROI);
  image2 = Mat2Emx_U8(ROI2);

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


//  namedWindow("Display bw_im", WINDOW_AUTOSIZE); // Create a window for display.
//  imshow("Display bw_im", bw_im);
//  namedWindow("Display bw_im2", WINDOW_AUTOSIZE); // Create a window for display.
//  imshow("Display bw_im2", bw_im2);

//  double FHD2 = 0;
  if (FHD >= 0.1 && FHD <= 0.25) {
    TransformPic(speckle_database, speckle_auth, speckle_auth);
    image3 = Mat2Emx_U8(speckle_auth);
    gabor_im(image3, 8, 45, Gimage_im3, BW_im3, K3);
    Gim_mat3 = Emx2Mat_U8(Gimage_im3);
    threshold(Gim_mat3, bw_im3, 0, 255, THRESH_BINARY_INV);
//    imshow("bw_im3", bw_im3);
    bw_im3.convertTo(bw_im3, CV_8U, 1, 0);
    FHD = hamming(bw_im, bw_im3);
//    cout << "FHD=" << FHD2 << endl;
  }
//  waitKey(0);
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
  return FHD;
}
}
