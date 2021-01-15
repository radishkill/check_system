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

namespace check_system {

using namespace cv;
using namespace std;

StateMachine::StateMachine() : is_running_(false) {}

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
        if (global_arg->host->IsOpen()) {
          global_arg->host->CheckStatus();
        }
        std::cout << "Self Test Success!!!" << std::endl;

      } else {
        std::cout << "Self Test Fault!!!" << std::endl;
      }
      break;
    }
    case kRegister: {
      std::cout << "Start Run Register" << std::endl;
      if (global_arg->is_fault) {
        std::cout << "system fault" << std::endl;
        // break;
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
      if (ret < 0) {
        std::cout << "Register Fault!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->RegisterFail();
        if (global_arg->host->IsOpen()) global_arg->host->RegisterSuccess();

        //失败打灯:红灯开,3个绿灯关
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(1);

      } else if (ret == 0) {
        std::cout << "Register Success!!!" << std::endl;
        if (global_arg->host->IsOpen()) global_arg->host->RegisterFail();
        if (global_arg->host->IsOpen()) global_arg->host->RegisterSuccess();

        //成功打灯:红灯关,3个绿灯开
        global_arg->led->CmosLed(1);
        global_arg->led->LaserLed(1);
        global_arg->led->LcdLed(1);
        global_arg->led->ErrorLed(0);
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
        //        break;
      }

      auto begin_tick = std::chrono::steady_clock::now();
      global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();

      global_arg->led->CloseBlink();

      //全灯OFF
      global_arg->led->CmosLed(0);
      global_arg->led->LaserLed(0);
      global_arg->led->LcdLed(0);
      global_arg->led->ErrorLed(0);

      ret = Authentication();
      if (ret < 0) {
        std::cout << "Auth Fault!!! \n";
        //认证失败

        if (global_arg->host->IsOpen()) global_arg->host->AuthFail();
        if (global_arg->host->IsOpen()) global_arg->host->AuthSuccess();

        //失败打灯:红灯开,3个绿灯关
        global_arg->led->CmosLed(0);
        global_arg->led->LaserLed(0);
        global_arg->led->LcdLed(0);
        global_arg->led->ErrorLed(1);

      } else if (ret == 0) {
        std::cout << "Auth Success!!! \n";
        if (global_arg->host->IsOpen()) global_arg->host->AuthFail();
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
    case kOther: {
      std::cout << "Start Other\n";
      int random_seed = GenerateRandomSeed();
      std::cout << "seed:" << random_seed << std::endl;
      global_arg->lcd->ShowBySeed(random_seed);
      global_arg->camera->GetPic();
      cv::Mat pic1 = global_arg->camera->GetPicMat().clone();
      sleep(10);
      global_arg->lcd->ShowBySeed(1);
      global_arg->lcd->ShowBySeed(random_seed);

      global_arg->camera->GetPic();
      cv::Mat pic2 = global_arg->camera->GetPicMat().clone();

      cv::imwrite("./pic1.bmp", pic1);
      cv::imwrite("./pic2.bmp", pic2);
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
    std::cout << "camera not open" << std::endl;
    return -1;
  }
  if (!global_arg->lcd->IsOpen()) {
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
    laser_err = false;
  }

  int random_seed = GenerateRandomSeed();
  if (global_arg->lcd->IsOpen()) {
    std::cout << "random_seed:" << random_seed << std::endl;
    global_arg->lcd->ShowBySeed(random_seed);
  } else {
    lcd_err = true;
  }
  sleep(3);
  camera_err = global_arg->camera->GetPic() == 0 ? false : true;
  cv::Mat pic = global_arg->camera->GetPicMat();
  cv::imwrite("./randpic.bmp", pic);

  lcd_err = global_arg->camera->CheckPic(30, 80) == 0 ? false : true;

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

    global_arg->is_fault = 1;
    std::cout << "test check fault:"
              << " laser : " << laser_err << " lcd : " << lcd_err
              << " cmos : " << camera_err << " error : " << global_arg->is_fault
              << std::endl;
    return -1;
  }

  global_arg->led->CloseBlink();
  //正确ok
  for (int i = 0; i < 3; i++) {
    global_arg->led->LaserLed(0);
    global_arg->led->CmosLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);
    global_arg->led->LaserLed(1);
    global_arg->led->CmosLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);
  }
  global_arg->led->LaserLed(1);
  global_arg->led->CmosLed(1);
  global_arg->led->LcdLed(1);
  global_arg->led->ErrorLed(0);
  global_arg->is_fault = 0;
  return 0;
}

//注册
int StateMachine::Register() {
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

  if (global_arg->sm->CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

  auto begin_tick = std::chrono::steady_clock::now();
  //实际为之前的管理检查算法 用于检查此卡是否为管理员
  ret = global_arg->sm->CheckKey(0);
  if (ret == 0) {
    std::cout << "is not admin key insert" << std::endl;
    //非管理员key插入
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    for (int i = 0; i < 20; i++) {
      global_arg->led->ErrorLed(0);
      Utils::MSleep(200);
      global_arg->led->ErrorLed(1);
      Utils::MSleep(200);
    }
    return -1;
  } else if (ret == -1) {
    std::cout << "check key run fault" << std::endl;
    //无admin key 错误
    return -1;
  }
  std::cout << "admin key insert" << std::endl;
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "check admin key elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;

  //管理员key插入
  //闪烁 并给用户插入新卡的时间 10s
  std::cout << "wait 10s\n";
  global_arg->led->ErrorLed(0);
  for (int i = 0; i < 10; i++) {
    global_arg->led->CmosLed(0);
    global_arg->led->LaserLed(0);
    global_arg->led->LcdLed(0);
    Utils::MSleep(500);
    global_arg->led->CmosLed(1);
    global_arg->led->LaserLed(1);
    global_arg->led->LcdLed(1);
    Utils::MSleep(500);

    //中断
    if (global_arg->interrupt_flag) {
      global_arg->interrupt_flag = 0;
      return -1;
    }
  }

  //确认激光器是打开状态
  global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();

  if (global_arg->sm->CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    return -1;
  }
  std::cout << "key insert" << std::endl;

  begin_tick = std::chrono::steady_clock::now();
  int key_id = global_arg->sm->FindKey();
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
  }

  global_arg->sm->CheckPairStore(key_id);

  //中断返回复位状态
  if (global_arg->interrupt_flag) {
    global_arg->interrupt_flag = 0;
    return -1;
  }
  begin_tick = std::chrono::steady_clock::now();
  //连续拍100张照片
  int n = 100;
  for (unsigned int i = 0; i < empty_pair_list_.size() && i < n; i++) {
    GlobalArg* global_arg = GlobalArg::GetInstance();
    int seed = global_arg->sm->GenerateRandomSeed();
    global_arg->lcd->ShowBySeed(seed);
    ret = global_arg->camera->GetPic();
    if (ret == -1) continue;

    //复制
    global_arg->key_file->SetMatImage(global_arg->camera->GetPicMat());

    //保存激励对
    global_arg->key_file->SavePicAndSeed(key_id, empty_pair_list_[i], seed);

    //中断返回复位状态
    if (global_arg->interrupt_flag == 1) {
      goto status_fault;
    }
    //确认激光器是打开状态 多次拍照可能会超过30s
    global_arg->laser == nullptr ?: global_arg->laser->ForceOpen();
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "add " << n << "pair"
            << "elapsed time :"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;
  return 0;
status_fault:
  return -1;
}
//认证
int StateMachine::Authentication() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret = 0;

  assert(global_arg->led != nullptr);
  assert(global_arg->lcd != nullptr);
  // assert(global_arg->laser != nullptr);
  assert(global_arg->camera != nullptr);

  if (!global_arg->camera->IsOpen()) {
    return -1;
  }

  if (global_arg->sm->CheckKeyInsert() == -1) {
    std::cout << "no key insert" << std::endl;
    //认证失败
    return -1;
  }
  std::cout << "key insert" << std::endl;
  auto begin_tick = std::chrono::steady_clock::now();
  int key_id = global_arg->sm->FindKey();
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
  //  //认证成功了,绿灯1 2 3 ON
  //  global_arg->led->LaserLed(1);
  //  global_arg->led->LcdLed(1);
  //  global_arg->led->CmosLed(1);
  //  global_arg->led->ErrorLed(0);

  std::cout << "auth success!!!" << std::endl;
  return 0;
}

//随机生成seed a number range from 0 to RAND_MAX
int StateMachine::GenerateRandomSeed() {
  std::srand(std::time(nullptr));
  //  return (int)(std::rand()*48271ll%2147483647);
  return std::rand();
}

int StateMachine::Other(int s) {}

//库定位算法 判断一枚key是否已经建立过数据库了
int StateMachine::FindKey() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret = 0;
  assert(global_arg->camera != nullptr);
  assert(global_arg->key_file != nullptr);
  assert(global_arg->lcd != nullptr);

  std::srand(std::time(nullptr));
  int i = -1;
  double result;
  std::cout << "find key start " << std::endl;
  while (i++ < 100) {
    auto begin_tick = std::chrono::steady_clock::now();
    if (global_arg->interrupt_flag == 1) {
      return -1;
    }
    CheckPairStore(i);

    if (available_pair_list_.empty()) continue;
    std::cout << "key id = " << i << std::endl;
    std::cout << "available pair list = " << available_pair_list_.size()
              << std::endl;
    std::cout << "empty pair list = " << empty_pair_list_.size() << std::endl;
    int seed_index = std::rand() % available_pair_list_.size();
    int seed =
        global_arg->key_file->GetSeed(i, available_pair_list_[seed_index]);

    std::cout << "seed number:" << seed << std::endl;
    global_arg->lcd->ShowBySeed(seed);
    ret = global_arg->key_file->ReadPicAsBmp(i, seed_index);
    if (ret == -1) continue;
    int ret = global_arg->camera->GetPic();
    if (ret == -1) continue;
    cv::Mat pic1 = global_arg->key_file->GetMatImage();
    cv::Mat pic2 = global_arg->camera->GetPicMat();
    cv::imwrite("./a.bmp", pic1);
    cv::imwrite("./b.bmp", pic2);
    //验证两张图片
    result = AuthPic(pic1, pic2);
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "auth pic elapsed time :"
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_tick - begin_tick)
                     .count()
              << "ms" << std::endl;

    if (result <= kAuthThreshold) {
      std::cout << "auth pic ok" << std::endl;
      //找到相应的库
      break;
    } else {
      std::cout << "auth pic fault" << std::endl;
    }
  }
  if (i >= 100) i = -1;

  //中断返回复位状态
  if (global_arg->interrupt_flag == 1) {
    return -1;
  }
  return i;
}
//插入检测算法
int StateMachine::CheckKeyInsert() {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int seed = global_arg->sm->GenerateRandomSeed();
  global_arg->lcd->ShowBySeed(seed);
  global_arg->camera->GetPic();
  return global_arg->camera->CheckPic(30, 80);
}

int StateMachine::CheckKey(int key_id) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  int ret;
  CheckPairStore(key_id);
  if (available_pair_list_.empty()) {
    std::cout << "available pair is empty keyid = " << key_id << endl;
    return -1;
  }
  int n = 10;
  int available_num = available_pair_list_.size();
  double result;
  while (n--) {
    int index = std::rand() % available_pair_list_.size();
    int seed_index = available_pair_list_[index];
    //如果随机到的是已经被删除了的
    if (seed_index == -1) {
      int i = index;
      while (i++) {
        if (i == available_pair_list_.size()) i = 0;
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
    int seed = global_arg->key_file->GetSeed(key_id, seed_index);
    if (seed == -1) {
      std::cout << "fatal error seed = -1" << std::endl;
      return -1;
    }

    global_arg->lcd->ShowBySeed(seed);
    ret = global_arg->key_file->ReadPicAsBmp(key_id, seed_index);
    if (ret == -1) {
      continue;
    }
    global_arg->camera->GetPic();

    cv::Mat pic1 = global_arg->key_file->GetMatImage();
    cv::Mat pic2 = global_arg->camera->GetPicMat();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(pic1, pic2);

    //值越小说明两张图片越相似
    if (result <= kAuthThreshold) {
      //删除本次循环使用的Seed文件及其对应的Pic文件
      std::cout << "delete old pair index = " << seed_index << std::endl;
      global_arg->key_file->DeleteSeed(key_id, seed_index);
      global_arg->key_file->DeletePic(key_id, seed_index);
      available_pair_list_[index] = -1;

      std::cout << "generate new a pair index = " << seed_index << std::endl;
      //认证通过
      //然后重新生成新的激励对
      int rand_seed = global_arg->sm->GenerateRandomSeed();
      global_arg->lcd->ShowBySeed(rand_seed);
      global_arg->camera->GetPic();

      global_arg->key_file->SetMatImage(global_arg->camera->GetPicMat());
      global_arg->key_file->SavePicAndSeed(key_id, seed_index, rand_seed);
      return 1;
    }
    //被中断了
    if (global_arg->interrupt_flag == 1) {
      global_arg->interrupt_flag = 0;
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

//库遍历算法
int StateMachine::CheckPairStore(int id) {
  GlobalArg* global_arg = GlobalArg::GetInstance();
  //清空empty_pairs
  std::vector<int>().swap(empty_pair_list_);
  std::vector<int>().swap(available_pair_list_);

  for (int i = 0; i < 1000; i++) {
    if (global_arg->key_file->IsSeedAvailable(id, i)) {
      available_pair_list_.push_back(i);
    } else {
      empty_pair_list_.push_back(i);
    }
  }
  return 0;
}
}  // namespace check_system
