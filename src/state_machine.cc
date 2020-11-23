#include "state_machine.h"

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <omp.h>
#include <chrono>

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
  int n;
  int ret = 0;
  if (is_running_) {
    std::cout << "system is running" << std::endl;
    return -1;
  }
  std::lock_guard<std::mutex> l(mutex_);
  is_running_ = true;
  arg->interrupt_flag = 0;
  switch (state) {
    case kSelfTest: {
      std::cout << "Run Self Test" << std::endl;
      ret = SelfTest();
      if (ret == 0 && arg->host->IsOpen()) {
        arg->host->CheckStatus();
      }
      break;
    }
    case kRegister: {
      std::cout << "Run Register" << std::endl;
      if (arg->is_fault) {
        std::cout << "system fault" << std::endl;
        //break;
      }

      auto begin_tick = std::chrono::steady_clock::now();

      arg->led->CloseBlink();

      //全灯OFF
      arg->led->CmosLed(0);
      arg->led->LaserLed(0);
      arg->led->LcdLed(0);
      arg->led->ErrorLed(0);

      n = 10;
      while (n--) {
        ret = arg->laser->SendOpenCmd();
        if (ret == 0)
          break;
        Utils::MSleep(2000);
      }

      //注册模块
      ret = Register();
      if (ret < 0) {
        if (arg->host->IsOpen())
          arg->host->RegisterFail();

        //失败打灯:红灯开,3个绿灯关
        arg->led->CmosLed(0);
        arg->led->LaserLed(0);
        arg->led->LcdLed(0);
        arg->led->ErrorLed(1);

      } else if (ret == 0) {
        if (arg->host->IsOpen())
          arg->host->RegisterSuccess();

        //成功打灯:红灯关,3个绿灯开
        arg->led->CmosLed(1);
        arg->led->LaserLed(1);
        arg->led->LcdLed(1);
        arg->led->ErrorLed(0);
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "Register total elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;

      break;
    }
    case kAuth: {
      std::cout << "Run Auth" << std::endl;
      if (arg->is_fault) {
        std::cout << "system fault" << std::endl;
//        break;
      }

      auto begin_tick = std::chrono::steady_clock::now();

      n = 10;
      while (n--) {
        ret = arg->laser->SendOpenCmd();
        if (ret == 0)
          break;
        Utils::MSleep(2000);
      }

      arg->led->CloseBlink();

      //全灯OFF
      arg->led->CmosLed(0);
      arg->led->LaserLed(0);
      arg->led->LcdLed(0);
      arg->led->ErrorLed(0);


      ret = Authentication();
      if (ret < 0) {
        if (arg->host->IsOpen())
          arg->host->AuthFail();

        //失败打灯:红灯开,3个绿灯关
        arg->led->CmosLed(0);
        arg->led->LaserLed(0);
        arg->led->LcdLed(0);
        arg->led->ErrorLed(1);

      } else if (ret == 0) {
        if (arg->host->IsOpen())
          arg->host->AuthSuccess();

        //成功打灯:红灯关,3个绿灯开
        arg->led->CmosLed(1);
        arg->led->LaserLed(1);
        arg->led->LcdLed(1);
        arg->led->ErrorLed(0);
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "auth total elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;

      break;
    }
    default: {
    }
  }
  arg->interrupt_flag = 0;
  is_running_ = false;
  return ret;
}
/*
 * 此过程中但凡有一个步骤出错都应该退出
 *
 */
int StateMachine::SelfTest() {
  int laser_flag;
  int camera_flag;
  int lcd_flag;

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
  int n = 0;
  n = 12;
  while (n--) {
    laser_flag = arg->laser->SendCheckCmd();
    if (laser_flag == 0)
      break;
    Utils::MSleep(1000);
  }

  n = 10;
  while (n--) {
    //设置温度
    laser_flag = arg->laser->SetTemperature(20);
    if (laser_flag == 0)
      break;
    Utils::MSleep(2000);
  }
  n = 5;
  while (n--) {
    //电流
    laser_flag = arg->laser->SetCurrent(3000);
    if (laser_flag == 0)
      break;
    Utils::MSleep(2000);
  }
  n = 5;
  while (n--) {
    //设置最大电流
    laser_flag = arg->laser->SetMaxCurrent(5000);
    if (laser_flag == 0)
      break;
    Utils::MSleep(2000);
  }

  n = 20;
  while (n--) {
    //这里面最长超时时间为5s
    laser_flag = arg->laser->SendOpenCmd();
    //启动成功
    if (laser_flag == 0)
      break;
    Utils::MSleep(2000);
  }

  int random_seed = GenerateRandomSeed();

  if (arg->lcd->IsOpen()) {
    arg->lcd->ShowBySeed(random_seed);
  } else {
    lcd_flag = -1;
  }

  camera_flag = arg->camera->GetOnePic();

  camera_flag = arg->camera->CheckPic(100, 200);
  if (camera_flag == -1) {
    std::cout << "check pic : pic wrong" << std::endl;
  }

  //  n = 20;
  //  while (n--) {
  //    ret1 = arg->laser->SendCloseCmd();
  //    if (ret1 == 0)
  //      break;
  //    Utils::MSleep(2000);
  //  }

  //逻辑与,输出LED灯的状态
  //错误
  if ((laser_flag != 0) || (camera_flag != 0) || (camera_flag != 0)) {

    if (laser_flag != 0) {
      arg->led->laser_blink_ = 200;
    }
    if (lcd_flag != 0) {
      arg->led->lcd_blink_ = 200;
    }
    if (camera_flag != 0) {
      arg->led->cmos_blink_ = 200;
    }
    arg->led->error_blink_ = 200;

    arg->is_fault = 1;
    std::cout << "test check fault:"
              << " laser : " << laser_flag
              << " lcd : " << lcd_flag
              << " cmos : " << camera_flag
              << " error : " << arg->is_fault;
    return -1;
  }

  arg->led->CloseBlink();
  //正确ok
  for (int i = 0 ; i < 3; i++ ) {
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
  arg->led->ErrorLed(0);
  arg->is_fault = 0;
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

  if (arg->sm->CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

//  ret = arg->sm->CheckAdminKey();

  auto begin_tick = std::chrono::steady_clock::now();
  ret = arg->sm->CheckKey(0);
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "check admin key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
  if(ret == 0) {
    std::cout << "is not admin key insert" << std::endl;
    //非管理员key插入
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    for(int i = 0 ; i < 20 ; i++ ){
      arg->led->ErrorLed(0);
      Utils::MSleep(200);
      arg->led->ErrorLed(1);
      Utils::MSleep(200);
    }
    return -1;
  } else if (ret == -1) {
    //无admin key 错误
    return -1;
  }

  //管理员key插入
  //闪烁 并给用户插入新卡的时间 10s
  std::cout << "wait 10s\n";
  arg->led->ErrorLed(0);
  for(int i = 0 ; i < 10 ; i++ ) {
    arg->led->CmosLed(0);
    arg->led->LaserLed(0);
    arg->led->LcdLed(0);
    Utils::MSleep(500);
    arg->led->CmosLed(1);
    arg->led->LaserLed(1);
    arg->led->LcdLed(1);
    Utils::MSleep(500);

    //中断
    if (arg->interrupt_flag) {
      arg->interrupt_flag = 0;
      return -1;
    }
  }

  //确认激光器是打开状态
  int n = 10;
  while (n--) {
    ret = arg->laser->SendOpenCmd();
    if (ret == 0)
      break;
    Utils::MSleep(2000);
  }

  if (arg->sm->CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }

  begin_tick = std::chrono::steady_clock::now();
  int key_id = arg->sm->FindKey();
  end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
  if (key_id == -1) {
    //被中断
    if (arg->interrupt_flag) {
      arg->interrupt_flag = 0;
      std::cout << "interrupted\n";
      return -1;
    }
    //没找到库 并新建
    int key_id = arg->key_file->FindEmptyKeyDir();
    if (key_id == -1) {
      std::cout << "store full" << std::endl;
      //库满
      return -1;
    }
    std::cout << "append key store in " << key_id << std::endl;
  }

  arg->sm->CheckPairStore(key_id);


  //中断返回复位状态
  if (arg->interrupt_flag) {
    arg->interrupt_flag = 0;
    return -1;
  }
  begin_tick = std::chrono::steady_clock::now();
  //连续拍100张照片
  n = 10;
  for (unsigned int i = 0; i < empty_pair_list_.size() && i < n; i++) {
    GlobalArg* arg = GlobalArg::GetInstance();
    int seed = arg->sm->GenerateRandomSeed();
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetOnePic();

    //复制
    arg->key_file->CopyPicToBuffer(arg->camera->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);

    //保存激励对
    arg->key_file->SavePicAndSeed(key_id, empty_pair_list_[i], seed);

    //中断返回复位状态
    if (arg->interrupt_flag == 1) {
      goto status_fault;
    }
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "get pic elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
  std::cout << "register sucess !!!!" << std::endl;
  return 0;
status_fault:
  return -1;
}
//认证
int StateMachine::Authentication() {
  GlobalArg* arg = GlobalArg::GetInstance();
  int ret = 0;

  assert(arg->led != nullptr);
  assert(arg->lcd != nullptr);
  assert(arg->laser != nullptr);
  assert(arg->camera != nullptr);

  if (!arg->camera->IsOpen()) {
    return -1;
  }

  if (arg->sm->CheckKeyInsert() == -1) {
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
  ret = CheckKey(key_id);
  //认证失败 或者发生错误
  if (ret == 0 || ret == -1) {
    //红灯 ON没找到成功的对象
    arg->led->ErrorLed(1);
    return -1;
  }
//  //认证成功了,绿灯1 2 3 ON
//  arg->led->LaserLed(1);
//  arg->led->LcdLed(1);
//  arg->led->CmosLed(1);
//  arg->led->ErrorLed(0);

  std::cout << "auth success!!!" << std::endl;
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

  std::srand(std::time(nullptr));
  int i = 0;
  double result;
  while (i < 100) {
    auto begin_tick = std::chrono::steady_clock::now();
    if (arg->interrupt_flag) {
      return -1;
    }
    CheckPairStore(i);
    if (available_pair_list_.empty())
      continue;
    int seed_index = std::rand()%available_pair_list_.size();
    int seed = arg->key_file->GetSeed(i, available_pair_list_[seed_index]);
    arg->lcd->ShowBySeed(seed);
    arg->key_file->ReadPicAsBmp(i, seed_index);
    arg->camera->GetOnePic();

    //运算temp_pic 与 pic 得到结果
    result = AuthPic(arg->key_file->GetMatImage(), arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "auth pic elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
    if (result <= kAuthThreshold) {
      //找到相应的库
      break;
    }
    i++;
  }
  if (i >= 100)
    i = -1;

  //中断返回复位状态
  if(arg->interrupt_flag == 1) {
    return -1;
  }
  return i;
}
//插入检测算法
int StateMachine::CheckKeyInsert()
{
  GlobalArg* arg = GlobalArg::GetInstance();
  int seed = arg->sm->GenerateRandomSeed();
  std::cout << "rand seed : " << seed << std::endl;
  arg->lcd->ShowBySeed(seed);
  arg->camera->GetOnePic();
  return arg->camera->CheckPic(100, 200);
}

int StateMachine::CheckKey(int key_id) {
  GlobalArg* arg = GlobalArg::GetInstance();
  CheckPairStore(key_id);
  if (available_pair_list_.empty()) {
    std::cout << "available pair is empty keyid = " << key_id << endl;
    return -1;
  }
  int n = 10;
  int available_num = available_pair_list_.size();
  double result;
  while (n--) {
    int index = std::rand()%available_pair_list_.size();
    int seed_index = available_pair_list_[index];
    //如果随机到的是已经被删除了的
    if (seed_index == -1) {
      int i = index;
      while (i++) {

        if (i == available_pair_list_.size())
          i = 0;
        //找到了一个没有被删除的钥匙
        if (available_pair_list_[i] != -1) {
          index = i;
          seed_index = available_pair_list_[index];
          break;
        }
        //钥匙已经被删完了
        if (i == index) {
          std::cout << "available pair is empty keyid = " << key_id << endl;
          return 0;
        }
      }
    }
    int seed = arg->key_file->GetSeed(key_id, seed_index);
    if (seed == -1) {
      std::cout << "fatal error seed = -1" << std::endl;
      return -1;
    }

    arg->key_file->ReadPicAsBmp(key_id, seed_index);
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetOnePic();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(arg->key_file->GetMatImage(), arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);

    //删除本次循环使用的Seed文件及其对应的Pic文件
    arg->key_file->DeleteSeed(key_id,seed_index);
    arg->key_file->DeletePic(key_id,seed_index);
    available_pair_list_[seed_index] = -1;

    //值越小说明两张图片越相似
    if (result < kAuthThreshold) {
      return 1;
    }
    //被中断了
    if (arg->interrupt_flag == 1) {
        arg->interrupt_flag = 0;
        return -1;
    }

    available_num--;
    //用完了库里面所有的钥匙
    if (available_num == 0) {
      break;
    }
  }
  return 0;
}
//管理员KEY检测算法
int StateMachine::CheckAdminKey() {
  int n = 10;

  GlobalArg* arg = GlobalArg::GetInstance();
  CheckPairStore(0);
  //如果库钥匙为空
  if (available_pair_list_.empty()) {
    std::cout << "admin key store is empty!!!" << std::endl;
    return -1;
  }
  std::srand(std::time(nullptr));
  double result;
  int available_num = available_pair_list_.size();
  while (n--) {
    int index = std::rand()%available_pair_list_.size();
    int seed_index = available_pair_list_[index];

    //如果随机到的是已经被删除了的
    if (seed_index == -1) {
      int i = index;
      while (i++) {

        if (i == available_pair_list_.size())
          i = 0;
        //找到了一个没有被删除的钥匙
        if (available_pair_list_[i] != -1) {
          index = i;
          seed_index = available_pair_list_[index];
          break;
        }
        if (i == index)
          goto auth_fault;
      }
    }

    int seed = arg->key_file->GetSeed(0, seed_index);
    arg->key_file->ReadPicAsBmp(0, index);
    arg->lcd->ShowBySeed(seed);
    arg->camera->GetOnePic();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(arg->key_file->GetMatImage(), arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);
    if (result < kAuthThreshold) {
      //结果匹配
      //.....
      return 1;
    }

    arg->key_file->DeletePic(0, index);
    arg->key_file->DeleteSeed(0, index);
    available_pair_list_[index] = -1;

    int rand_seed = arg->sm->GenerateRandomSeed();
    arg->lcd->ShowBySeed(rand_seed);

    arg->camera->GetOnePic();

    arg->key_file->CopyPicToBuffer(arg->camera->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
    arg->key_file->SavePicAndSeed(0, index, rand_seed);
  }
  //非管理员key插入
  return 0;
  auth_fault:
  return -1;
}
//库遍历算法
int StateMachine::CheckPairStore(int id) {
  GlobalArg* arg = GlobalArg::GetInstance();
  //清空empty_pairs
  std::vector<int>().swap(empty_pair_list_);
  std::vector<int>().swap(available_pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (arg->key_file->IsSeedAvailable(id, i)) {
      available_pair_list_.push_back(i);
    } else {
      empty_pair_list_.push_back(i);
    }
  }
  return 0;
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
  for (int i = 0; i < input2.rows && i < input1.rows; i++)
  {
    uchar* data1 = input1.ptr<uchar>(i);
    uchar* data2 = input2.ptr<uchar>(i);
    for (int j = 0; j < input2.cols && j < input1.cols; j++) {
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

int StateMachine::AuthPic(cv::Mat& speckle_database, char *auth_pic, int h2, int w2) {
  std::cout << "auth pic\n";
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

//  Mat speckle_database(h1, w1, CV_8UC1);
  Mat speckle_auth(h2, w2, CV_8UC1);
//  std::memcpy(speckle_database.data, database_pic, h1 * w1);
  std::memcpy(speckle_auth.data, auth_pic, h2 * w2);

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

  std::cout << "allocatedSize " << image->allocatedSize << std::endl;
  std::cout << "numDimensions " << image->numDimensions << std::endl;
  std::cout << "image1 size : " << image->size[0] << " " << image->size[1] <<std::endl;
  std::cout << "image2 size : " << image2->size[0] << " " << image2->size[1] <<std::endl;

  auto begin_tick = std::chrono::steady_clock::now();

  //   'gabor_im'.
  gabor_im(image, 8, 45, Gimage_im, BW_im, K);
  gabor_im(image2, 8, 45, Gimage_im2, BW_im2, K2);

  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "hanming forward : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << " ms" << std::endl;

  Gim_mat = Emx2Mat_U8(Gimage_im);
  Gim_mat2 = Emx2Mat_U8(Gimage_im2);

  threshold(Gim_mat, bw_im, 0, 255,THRESH_BINARY_INV);//INV_THRESH_BINARY
  threshold(Gim_mat2, bw_im2, 0, 255, THRESH_BINARY_INV);//INV_THRESH_BINARY

  bw_im.convertTo(bw_im, CV_8U, 1, 0);        //64f->8u
  bw_im2.convertTo(bw_im2, CV_8U, 1, 0);

  double FHD = hamming(bw_im, bw_im2);

//  namedWindow("Display bw_im", WINDOW_AUTOSIZE); // Create a window for display.
//  imshow("Display bw_im", bw_im);
//  namedWindow("Display bw_im2", WINDOW_AUTOSIZE); // Create a window for display.
//  imshow("Display bw_im2", bw_im2);

//  double FHD2 = 0;
  begin_tick = std::chrono::steady_clock::now();
  if (FHD >= 0.1 && FHD <= 0.25) {
//  if (FHD >= 0.01 && FHD <= 0.5) {
    TransformPic(speckle_database, speckle_auth, speckle_auth);
    image3 = Mat2Emx_U8(speckle_auth);
    gabor_im(image3, 8, 45, Gimage_im3, BW_im3, K3);
    Gim_mat3 = Emx2Mat_U8(Gimage_im3);
    threshold(Gim_mat3, bw_im3, 0, 255, THRESH_BINARY_INV);
    bw_im3.convertTo(bw_im3, CV_8U, 1, 0);
    int FHD2 = hamming(bw_im, bw_im3);
    cout << "FHD2=" << FHD2 << endl;
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "hanming FHD2 : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << " ms" << std::endl;
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
