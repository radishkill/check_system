#include "state_machine.h"

#include <omp.h>

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

#include "authpic.h"
#include "camera_manager.h"
#include "global_arg.h"
#include "hostcontroller.h"
#include "key_file.h"
#include "laser.h"
#include "lcd.h"
#include "led.h"
#include "mutils.h"
#include "fingerprint.h"

namespace check_system {

using namespace cv;
using namespace std;

StateMachine::StateMachine()
    : is_running_(false), find_key_flag_(false), max_key_id_(0) {}

int StateMachine::RunMachine(StateMachine::MachineState state) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int n;
  int ret = 0;
  if (is_running_) {
    std::cout << "system is busy" << std::endl;
    return -1;
  }
  std::lock_guard<std::mutex> l(mutex_);
  is_running_ = true;
  global_arg->interrupt_flag = 0;
  switch (state) {
    case kSelfTest: {
      std::cout << "Start Run Self Test" << std::endl;

      global_arg->led->CloseBlink();
      //全灯OFF
      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      ret = SelfTest();
      if (ret == 0) {
        std::cout << "Self Test Success!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->CheckStatus();

        global_arg->is_fault = false;
      } else {
        std::cout << "Self Test Fault!!!" << std::endl;
        global_arg->is_fault = true;
      }
      break;
    }
    case kRegister: {
      std::cout << "Start Run Register" << std::endl;
      if (global_arg->is_fault) {
        std::cout << "system fault" << std::endl;
        break;
      }

      auto begin_tick = std::chrono::steady_clock::now();

      global_arg->led->CloseBlink();

      //全灯OFF
      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();

      //注册模块
      ret = Register();

      if (ret == 0) {
        //成功打灯:红灯关,3个绿灯开
        global_arg->led->CmosLed(1);
        global_arg->led->LaserLed(1);
        global_arg->led->LcdLed(1);
        global_arg->led->ErrorLed(0);
      } else if (ret == -1) {
        //非管理员key插入
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        for (int i = 0; i < 20; i++) {
          global_arg->led->ErrorLed(1);
          Utils::MSleep(200);
          global_arg->led->ErrorLed(0);
          Utils::MSleep(200);
        }
      } else if (ret == -2) {
        //失败打灯:红灯开,3个绿灯关
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(1);
      }
      if (ret < 0) {
        std::cout << "Register Fault!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->RegisterFail();

      } else if (ret == 0) {
        std::cout << "Register Success!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->RegisterSuccess();
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "Register total elapsed time :"
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_tick - begin_tick)
                       .count()
                << "ms" << std::endl;

      break;
    }
    case kAuth: {
      std::cout << "Start Run Auth" << std::endl;
      if (global_arg->is_fault) {
        std::cout << "system fault" << std::endl;
        break;
      }

      auto begin_tick = std::chrono::steady_clock::now();
      global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();

      global_arg->led->CloseBlink();

      //全灯OFF
      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      ret = AuthState();
      if (ret < 0) {
        std::cout << "Auth Fault!!! \n";
        //认证失败

        if (global_arg->host->IsOpen()) global_arg->host->AuthFail();

        //失败打灯:红灯开,3个绿灯关
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(1);

      } else if (ret == 0) {
        std::cout << "Auth Success!!! \n";
        //认证成功
        if (global_arg->host->IsOpen()) global_arg->host->AuthSuccess();

        //成功打灯:红灯关,3个绿灯开
        global_arg->led->CmosLed(1);
        global_arg->led->LaserLed(1);
        global_arg->led->LcdLed(1);
        global_arg->led->ErrorLed(0);
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "Auth total elapsed time :"
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_tick - begin_tick)
                       .count()
                << "ms" << std::endl;

      break;
    }

      //
    case kSystemInit: {
      std::cout << "Start Run SystemInit" << std::endl;
      if (global_arg->is_fault) {
        std::cout << "system fault" << std::endl;
        break;
      }
      auto begin_tick = std::chrono::steady_clock::now();


      //全灯OFF
      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      ret = SystemInit();
      if (ret == 0) {
        //成功打灯:全灯闪
        for (int i = 0; i < 3; i++) {
          global_arg->led->CmosLed(1);
          global_arg->led->LaserLed(1);
          global_arg->led->LcdLed(1);
          global_arg->led->ErrorLed(1);
          Utils::MSleep(500);
          global_arg->led->CmosLed(0);
          global_arg->led->LaserLed(0);
          global_arg->led->LcdLed(0);
          global_arg->led->ErrorLed(0);
          Utils::MSleep(500);
        }

      } else if (ret == -1) {
        //失败打灯:红灯开,3个绿灯关
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(1);
      } else if (ret == -2) {
        //因为管理员而导致的错误
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        for (int i = 0; i < 20; i++) {
          global_arg->led->ErrorLed(0);
          Utils::MSleep(200);
          global_arg->led->ErrorLed(1);
          Utils::MSleep(200);
        }
      }
      if (ret < 0) {
        std::cout << "SystemInit Fault!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->InitializeFail();
      } else if (ret == 0) {
        std::cout << "SystemInit Success!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->InitializeSuccess();
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "SystemInit total elapsed time :"
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_tick - begin_tick)
                       .count()
                << "ms" << std::endl;
      break;
    }
    case kOther: {
      std::cout << "Start Other\n";

      //全灯OFF

      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      Other(0);

      std::cout << "end!!\n";
      //如果注册成功
      for (int i = 0; i < 10; i++) {
        global_arg->led->CmosLed(1);
        global_arg->led->LaserLed(1);
        global_arg->led->LcdLed(1);
        global_arg->led->ErrorLed(1);
        Utils::MSleep(200);
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(0);
        Utils::MSleep(200);
      }

      break;
    }
    default: {
    }
  }
  global_arg->interrupt_flag = 0;
  is_running_ = false;
  return ret;
}
/*
 * 此过程中但凡有一个步骤出错都应该退出
 *
 */
int StateMachine::SelfTest() {
  bool laser_err = false;
  bool camera_err = false;
  bool lcd_err = false;

  GlobalArg* global_arg = GlobalArg::GetInstance();
  assert(global_arg->led != nullptr);
  assert(global_arg->lcd != nullptr);
  // assert(global_arg->laser != nullptr);
  assert(global_arg->camera != nullptr);

  if (!global_arg->camera->IsOpen()) {
    global_arg->led->cmos_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "camera not open" << std::endl;
    return -1;
  }
  if (!global_arg->finger_print->IsConnect()) {
    global_arg->led->cmos_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "finger print not connect" << std::endl;
    return -1;
  }

  if (!global_arg->lcd->IsOpen()) {
    global_arg->led->lcd_blink_ = 200;
    global_arg->led->error_blink_ = 200;
    std::cout << "lcd not open" << std::endl;
    return -1;
  }

  int n = 0;

  if (global_arg->laser) {
    laser_err = global_arg->laser->ForceCheck() == 0 ? false : true;
    n = 10;
    while (n--) {
      //设置温度
      laser_err = global_arg->laser->SetTemperature(20) == 0 ? false : true;
      if (!laser_err) break;
      Utils::MSleep(2000);
    }
    n = 5;
    while (n--) {
      //电流
      laser_err = global_arg->laser->SetCurrent(3000) == 0 ? false : true;
      if (!laser_err) break;
      Utils::MSleep(2000);
    }
    n = 5;
    while (n--) {
      //设置最大电流

      laser_err = global_arg->laser->SetMaxCurrent(5000) == 0 ? false : true;
      if (!laser_err) break;
      Utils::MSleep(2000);
    }
    //打开激光器
    laser_err = global_arg->laser->ForceOpen() == 0 ? false : true;
  } else {
    //现在这不作为判断激光错误的条件
    laser_err = false;
  }

  int random_seed = GenerateRandomSeed();
  if (global_arg->lcd->IsOpen()) {
    std::cout << "random_seed:" << random_seed << std::endl;
    global_arg->lcd->ShowByColor(255);
    global_arg->led->LcdLed(1);
    Utils::MSleep(200);
    global_arg->led->LcdLed(0);
  } else {
    lcd_err = true;
  }
  Utils::MSleep(500);
  global_arg->led->CmosLed(1);
  Utils::MSleep(200);
  global_arg->led->CmosLed(0);
  camera_err = global_arg->camera->TakePhoto() == 0 ? false : true;

  cv::Mat pic1 = global_arg->camera->GetPicMat().clone();
  cv::imwrite("./roipic.bmp", pic1);

  int ret = Utils::CheckPic(pic1, global_arg->pic_avg_low, global_arg->pic_avg_high);
  if (ret == 1) {
    //太亮 lcd没插入
    lcd_err = true;
  } else if (ret == -1) {
    //太暗 没有激光
    laser_err = true;
  }


  //得到最大的key
  max_key_id_ = global_arg->key_file->GetAvailableMaxKey();
  std::cout << "max key id = " << max_key_id_ << std::endl;


  if (laser_err || camera_err || lcd_err) {
    if (laser_err) {
      global_arg->led->laser_blink_ = 200;
    }
    if (lcd_err) {
      global_arg->led->lcd_blink_ = 200;
    }
    if (camera_err) {
      global_arg->led->cmos_blink_ = 200;
    }
    global_arg->led->error_blink_ = 200;

    std::cout << "test check fault:"
              << " laser : " << laser_err << " lcd : " << lcd_err
              << " cmos : " << camera_err << std::endl;
    return -1;
  }

  global_arg->led->CloseBlink();
  //正确ok
  for (int i = 0; i < 3; i++) {
    global_arg->led->LaserLed(1);
    global_arg->led->CmosLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
    global_arg->led->LaserLed(0);
    global_arg->led->CmosLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);
  }
  global_arg->led->LaserLed(1);
  global_arg->led->CmosLed(1);
  global_arg->led->LcdLed(1);
  global_arg->led->ErrorLed(0);
  return 0;
}

//注册
int StateMachine::Register() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret;
  int n;

  assert(global_arg->led != nullptr);
  assert(global_arg->lcd != nullptr);

  // assert(global_arg->laser != nullptr);
  assert(global_arg->camera != nullptr);

  if (!global_arg->camera->IsOpen()) {
    std::cout << "The camera can't turned on" << std::endl;
    return -1;
  }
  //wait admin put finger
  std::printf("wait %ds\n", 5);
  global_arg->led->ErrorLed(0);
  for (int i = 0; i < 5; i++) {
    global_arg->led->CmosLed(1);
    global_arg->led->LaserLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);

    //中断
    if (global_arg->interrupt_flag) {
      global_arg->interrupt_flag = 0;
      return -1;
    }
  }
  if (!global_arg->finger_print->RecodeFinger(1)) {
    std::cout << "recode finger fault!\n";
    return -1;
  }
  if (!global_arg->finger_print->CheckFinger()) {
    std::cout << "check admin finger fault!\n";
    return -1;
  } else {
    std::cout << "check admin finger ok!\n";
  }



  if (CheckKeyInsert() != 0) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

  auto begin_tick = std::chrono::steady_clock::now();
  //实际为之前的管理检查算法 用于检查此卡是否为管理员
  ret = CheckKey(0);
  if (ret == 0) {
    std::cout << "is not admin key insert" << std::endl;
    return -2;
  } else if (ret == -1) {
    std::cout << "check key run fault" << std::endl;
    //无admin key 错误
    return -2;
  }
  std::cout << "admin key insert" << std::endl;
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "check admin key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;

  //管理员key插入
  //闪烁 并给用户插入新卡的时间 ns
  std::printf("wait %ds\n", global_arg->waiting_time);
  global_arg->led->ErrorLed(0);
  for (int i = 0; i < global_arg->waiting_time; i++) {
    global_arg->led->CmosLed(1);
    global_arg->led->LaserLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);

    //中断
    if (global_arg->interrupt_flag) {
      global_arg->interrupt_flag = 0;
      return -1;
    }
  }

  //确认激光器是打开状态
  global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();

  if (CheckKeyInsert() != 0) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

  begin_tick = std::chrono::steady_clock::now();
  std::cout << "find key start " << std::endl;
  n = 3;
  int key_id;
  for (int i = 0; i < n; i++) {
    key_id = FindKey();
    if (key_id != -1) break;
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  if (key_id == -1) {
    //被中断
    if (global_arg->interrupt_flag) {
      global_arg->interrupt_flag = 0;
      std::cout << "interrupted\n";
      return -1;
    }
    //没找到库 并新建
    key_id = global_arg->key_file->FindEmptyKeyDir();
    if (key_id == -1) {
      std::cout << "store full" << std::endl;
      //库满
      return -1;
    }
    std::cout << "append key store in " << key_id << std::endl;
    //如果添加的key超过缓存的最大数，则更新缓存.
    if (max_key_id_ < key_id) {
      max_key_id_ = key_id;
    }
  }

  CheckPairStore(key_id);

  //中断返回复位状态
  if (global_arg->interrupt_flag) {
    global_arg->interrupt_flag = 0;
    return -1;
  }
  begin_tick = std::chrono::steady_clock::now();
  //连续拍n张照片
  n = global_arg->num_of_additions;

  for (unsigned int i = 0; i < empty_pair_list_.size() && i < n; i++) {
    int seed = GenerateRandomSeed();
    ShowBySeed(seed);

    ret = TakePhoto();
    if (ret == -1) continue;

    global_arg->key_file->SavePicAndSeed(key_id, empty_pair_list_[i],
                                         global_arg->camera->GetPicMat(), seed);


    //中断返回复位状态
    if (global_arg->interrupt_flag == 1) {
      break;
    }
    //确认激光器是打开状态 多次拍照可能会超过30s
    global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "add " << n << " pair"
            << "elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  return 0;
}
//认证
int StateMachine::AuthState() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret = 0;
  int n;

  assert(global_arg->led != nullptr);
  assert(global_arg->lcd != nullptr);
  // assert(global_arg->laser != nullptr);
  assert(global_arg->camera != nullptr);

  if (!global_arg->camera->IsOpen()) {
    return -1;
  }

  if (CheckKeyInsert() != 0) {
    std::cout << "no key insert" << std::endl;
    //认证失败
    return -1;
  }
  std::cout << "key insert" << std::endl;

  auto begin_tick = std::chrono::steady_clock::now();
  std::cout << "find key start " << std::endl;
  n = 3;
  int key_id;
  for (int i = 0; i < n; i++) {
    key_id = FindKey();
    if (key_id != -1) break;
  }
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;

  if (key_id == -1) {
    std::cout << "can't find key" << std::endl;
    //认证失败
    return -1;
  }

  begin_tick = std::chrono::steady_clock::now();
  ret = CheckKey(key_id);
  end_tick = std::chrono::steady_clock::now();
  std::cout << "check key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;

  //认证失败 或者发生错误check key
  if (ret == 0 || ret == -1) {
    //红灯 ON没找到成功的对象
    global_arg->led->ErrorLed(1);
    return -1;
  }

  std::cout << "auth success!!!" << std::endl;
  return 0;
}

//随机生成seed a number range from 0 to RAND_MAX
int StateMachine::GenerateRandomSeed() {
  std::srand(std::time(nullptr));
  //  return (int)(std::rand()*48271ll%2147483647);
  return std::rand() % 1000;
}

int StateMachine::ShowBySeed(unsigned int seed) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  global_arg->lcd->ShowBySeed(seed);
  global_arg->led->LcdLed(1);
  if (find_key_flag_) global_arg->led->LaserLed(1);
  Utils::MSleep(200);
  global_arg->led->LcdLed(0);
  if (find_key_flag_) global_arg->led->LaserLed(0);
  //这500ms主要是为了保证coms刷新正常
  // Utils::MSleep(500);
  sleep(1);
  return 0;
}

int StateMachine::TakePhoto() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  global_arg->led->CmosLed(1);
  Utils::MSleep(200);
  global_arg->led->CmosLed(0);
  return global_arg->camera->TakePhoto();
}

int StateMachine::Other(int s) {
  //系统初始化 注册管理员
  GlobalArg* global_arg = GlobalArg::GetInstance();

  std::printf("wait %ds\n", 5);
  global_arg->led->ErrorLed(0);
  for (int i = 0; i < 5; i++) {
    global_arg->led->CmosLed(1);
    global_arg->led->LaserLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);

    //中断
    if (global_arg->interrupt_flag) {
      global_arg->interrupt_flag = 0;
      return -1;
    }
  }

  //注册指纹
  if (!global_arg->finger_print->RecodeFinger(2)) {
    std::cout << "recode finger fault!\n";
    return -1;
  }

  int random_seed = GenerateRandomSeed();
  std::cout << "seed:" << random_seed << std::endl;
  ShowBySeed(random_seed);
  TakePhoto();

  cv::Mat pic = global_arg->camera->GetPicMat().clone();

  CheckPairStore(0);

  for (int i = 0; i < global_arg->num_of_additions; i++) {
    global_arg->key_file->SavePicAndSeed(0, empty_pair_list_[i], pic,
                                         random_seed);
  }
  return 0;
}

//库定位算法 判断一枚key是否已经建立过数据库了
int StateMachine::FindKey() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  find_key_flag_ = true;
  int ret = 0;
  assert(global_arg->camera != nullptr);
  assert(global_arg->key_file != nullptr);
  assert(global_arg->lcd != nullptr);

  //库查询 从库01号钥匙起
  int i = 1;
  double result;

  std::cout << "max key id = " << max_key_id_ << std::endl;
  while (i <= max_key_id_) {
    auto begin_tick = std::chrono::steady_clock::now();
    if (global_arg->interrupt_flag) {
      //运行时，被中断
      break;
    }
    CheckPairStore(i);

    if (available_pair_list_.empty()) {
      i++;
      continue;
    }
    std::cout << "key id = " << i << std::endl;
    std::cout << "available pair list = " << available_pair_list_.size()
              << std::endl;
    std::cout << "empty pair list = " << empty_pair_list_.size() << std::endl;
    int seed_index = std::rand() % available_pair_list_.size();
    int seed =
        global_arg->key_file->GetSeed(i, available_pair_list_[seed_index]);

    std::cout << "seed number:" << seed << std::endl;

    ShowBySeed(seed);

    int ret = TakePhoto();

    //如果拍照错误
    if (ret == -1) {
      i++;
      continue;
    }


    cv::Mat pic1 =
        global_arg->key_file->ReadPic(i, available_pair_list_[seed_index])
            .clone();
    cv::Mat pic2 = global_arg->camera->GetPicMat().clone();

    //验证两张图片
    result = AuthPic::DoAuthPic(pic1.clone(), pic2.clone(), global_arg->auth_threshold);
    auto end_tick = std::chrono::steady_clock::now();

    // std::cout << "auth pic elapsed time :"
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(
    //                  end_tick - begin_tick)
    //                  .count()
    //           << "ms" << std::endl;

    if (result <= global_arg->auth_threshold) {
      std::cout << "find key ok key id = " << i << std::endl;
      //找到相应的库
      break;
    }
    i++;
  }
  //这是没有找到key库的情况
  if (i > max_key_id_) i = -1;

  find_key_flag_ = false;
  //中断返回复位状态
  if (global_arg->interrupt_flag) {
    return -1;
  }
  return i;
}
//插入检测算法
int StateMachine::CheckKeyInsert() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int seed = GenerateRandomSeed();

  ShowBySeed(seed);

  TakePhoto();
  return Utils::CheckPic(global_arg->camera->GetPicMat(), global_arg->pic_avg_low, global_arg->pic_avg_high);
}

int StateMachine::CheckKey(int key_id) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret;
  CheckPairStore(key_id);
  if (available_pair_list_.empty()) {
    std::cout << "available pair is empty keyid = " << key_id << endl;
    return -1;
  }
  std::srand(time(nullptr));
  //最多检查10个激励对
  int n = available_pair_list_.size() < 10 ? available_pair_list_.size() : 10;
  double result;
  while (n--) {
    int index = std::rand() % available_pair_list_.size();
    int seed_index = available_pair_list_[index];
    //如果随机到的是已经被删除了的
    if (seed_index == -1) {
      int i = index + 1;
      //向一下个查看
      while (1) {
        //如果已经到最后已经，就返回第一个
        if (i == available_pair_list_.size()) i = 0;
        //找到了一个没有被删除的钥匙
        if (available_pair_list_[i] != -1) {
          index = i;
          seed_index = available_pair_list_[i];
          break;
        }
        //钥匙已经被删完了
        if (i == index) {
          std::cout << "available pair is empty keyid = " << key_id << endl;
          return 0;
        }
        i++;
      }
    }
    std::cout << "key id = " << key_id << " " << seed_index << std::endl;
    int seed = global_arg->key_file->GetSeed(key_id, seed_index);
    if (seed == -1) {
      std::cout << "fatal error seed = -1" << std::endl;
      return -1;
    }


    ShowBySeed(seed);

    TakePhoto();

    cv::Mat pic1 = global_arg->key_file->ReadPic(key_id, seed_index).clone();
    cv::Mat pic2 = global_arg->camera->GetPicMat().clone();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic::DoAuthPic(pic1.clone(), pic2.clone(), global_arg->auth_threshold);

    //表示此钥匙已经被使用了一次
    available_pair_list_[index] = -1;

    if (result <= global_arg->auth_threshold) {

    }
    //值越小说明两张图片越相似
    if (result <= global_arg->auth_threshold) {
      //删除本次循环使用的Seed文件及其对应的Pic文件
      std::cout << "delete old pair index = " << seed_index << std::endl;
      global_arg->key_file->DeletePicAndSeed(key_id, seed_index);

      //如果不是管理员
      if (key_id != 0) {
        return 1;
      }
      //如果是管理员就特殊处理
      // if (result > (global_arg->auth_threshold - 0.1)) {
      //   return 1;
      // }

      std::cout << "generate new a pair index = " << seed_index << std::endl;
      //认证通过
      //然后重新生成新的激励对
      int new_seed = GenerateRandomSeed();

      ShowBySeed(new_seed);
      TakePhoto();

      cv::Mat new_pic = global_arg->camera->GetPicMat().clone();

      global_arg->key_file->SavePicAndSeed(key_id, seed_index, new_pic,
                                           new_seed);
      return 1;
    }
    //被中断了
    if (global_arg->interrupt_flag == 1) {
      global_arg->interrupt_flag = 0;
      return -1;
    }
  }

  return 0;
}

//库遍历算法
int StateMachine::CheckPairStore(int id) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  //清空empty_pairs
  std::vector<int>().swap(empty_pair_list_);
  std::vector<int>().swap(available_pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (global_arg->key_file->IsSeedAvailable(id, i) && global_arg->key_file->IsPicAvailable(id, i)) {
      available_pair_list_.push_back(i);
    } else {
      empty_pair_list_.push_back(i);
    }
  }
  return 0;
}

//系统初始化
int StateMachine::SystemInit() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret;

  assert(global_arg->led != nullptr);
  assert(global_arg->lcd != nullptr);
  // assert(global_arg->laser != nullptr);
  assert(global_arg->camera != nullptr);

  if (!global_arg->camera->IsOpen()) {
    std::cout << "The camera can't turned on" << std::endl;
    return -1;
  }

  if (CheckKeyInsert() != 0) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

  auto begin_tick = std::chrono::steady_clock::now();
  //实际为之前的管理检查算法 用于检查此卡是否为管理员
  ret = CheckKey(0);
  if (ret == 0) {
    std::cout << "is not admin key insert" << std::endl;
    //非管理员key插入
    return -2;
  } else if (ret == -1) {
    std::cout << "check key run fault" << std::endl;
    //无admin key 错误
    return -2;
  }
  std::cout << "admin key insert" << std::endl;

  //删除所有除管理员的seed和pic
  global_arg->key_file->DeleteAllExceptAdmin();
  return 0;
}

}  // namespace check_system
