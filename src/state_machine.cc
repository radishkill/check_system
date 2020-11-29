#include "state_machine.h"

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <omp.h>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "global_arg.h"
#include "laser.h"
#include "utils.h"
#include "camera_manager.h"
#include "key_file.h"
#include "lcd.h"
#include "key_file.h"
#include "led.h"
#include "hostcontroller.h"
#include "authpic.h"

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
    std::cout << "system is busy" << std::endl;
    return -1;
  }
  std::lock_guard<std::mutex> l(mutex_);
  is_running_ = true;
  arg->interrupt_flag = 0;
  switch (state) {
    case kSelfTest: {
      std::cout << "Start Run Self Test" << std::endl;

      arg->led->CloseBlink();
      //全灯OFF
      arg->led->CmosLed(0);
      arg->led->LaserLed(0);
      arg->led->LcdLed(0);
      arg->led->ErrorLed(0);

      ret = SelfTest();
      if (ret == 0) {
        if (arg->host->IsOpen()) {
          arg->host->CheckStatus();
        }
        std::cout << "Self Test Success!!!" << std::endl;
      } else {
        std::cout << "Self Test Fault!!!" << std::endl;
      }
      break;
    }
    case kRegister: {
      std::cout << "Start Run Register" << std::endl;
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
        std::cout << "Register Fault!!!" << std::endl;
        if (arg->host->IsOpen())
          arg->host->RegisterFail();

        //失败打灯:红灯开,3个绿灯关
        arg->led->CmosLed(0);
        arg->led->LaserLed(0);
        arg->led->LcdLed(0);
        arg->led->ErrorLed(1);

      } else if (ret == 0) {
        std::cout << "Register Success!!!" << std::endl;
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
      std::cout << "Start Run Auth" << std::endl;
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
        std::cout << "Auth Fault!!! \n";
        //认证失败
        if (arg->host->IsOpen())
          arg->host->AuthFail();

        //失败打灯:红灯开,3个绿灯关
        arg->led->CmosLed(0);
        arg->led->LaserLed(0);
        arg->led->LcdLed(0);
        arg->led->ErrorLed(1);

      } else if (ret == 0) {
        std::cout << "Auth Success!!! \n";
        //认证成功
        if (arg->host->IsOpen())
          arg->host->AuthSuccess();

        //成功打灯:红灯关,3个绿灯开
        arg->led->CmosLed(1);
        arg->led->LaserLed(1);
        arg->led->LcdLed(1);
        arg->led->ErrorLed(0);
      }
      auto end_tick = std::chrono::steady_clock::now();
      std::cout << "Auth total elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;

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
  int laser_flag = 0;
  int camera_flag = 0;
  int lcd_flag = 0;

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
    //这里面最长超时时间为1s
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
              << " error : " << arg->is_fault
              << std::endl;
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


  auto begin_tick = std::chrono::steady_clock::now();
  //实际为之前的管理检查算法 用于检查此卡是否为管理员
  ret = arg->sm->CheckKey(0);
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
    std::cout << "check key run fault" << std::endl;
    //无admin key 错误
    return -1;
  }
  std::cout << "admin key insert" << std::endl;
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "check admin key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;


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
  std::cout << "key insert" << std::endl;

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
    key_id = arg->key_file->FindEmptyKeyDir();
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
  n = 100;
  for (unsigned int i = 0; i < empty_pair_list_.size() && i < n; i++) {
    GlobalArg* arg = GlobalArg::GetInstance();
    int seed = arg->sm->GenerateRandomSeed();
    arg->lcd->ShowBySeed(seed);
    ret = arg->camera->GetOnePic();
    if (ret == -1)
      continue;

    //复制
    arg->key_file->CopyPicToBuffer(arg->camera->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);

    //保存激励对
    arg->key_file->SavePicAndSeed(key_id, empty_pair_list_[i], seed);

    //中断返回复位状态
    if (arg->interrupt_flag == 1) {
      goto status_fault;
    }
    //确认激光器是打开状态 多次拍照可能会超过30s
    int n = 10;
    while (n--) {
      ret = arg->laser->SendOpenCmd();
      if (ret == 0)
        break;
      Utils::MSleep(2000);
    }
  }
  end_tick = std::chrono::steady_clock::now();
  std::cout << "add " << n << "pair" << "elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
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
    std::cout << "no key insert" << std::endl;
    //认证失败
    return -1;
  }
  std::cout << "key insert" << std::endl;
  auto begin_tick = std::chrono::steady_clock::now();
  int key_id = arg->sm->FindKey();
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
  if (key_id == -1) {
    std::cout << "can't find key" << std::endl;
    //认证失败
    return -1;
  }

  begin_tick = std::chrono::steady_clock::now();
  ret = CheckKey(key_id);
  end_tick = std::chrono::steady_clock::now();
  std::cout << "check key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;

  //认证失败 或者发生错误check key
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
  int i = -1;
  double result;
  std::cout << "find key start " << std::endl;
  while (i++ < 100) {
    auto begin_tick = std::chrono::steady_clock::now();
    if (arg->interrupt_flag == 1) {
      return -1;
    }
    CheckPairStore(i);

    if (available_pair_list_.empty())
      continue;
    std::cout << "key id = " << i << std::endl;
    std::cout << "available pair list = " << available_pair_list_.size() << std::endl;
    std::cout << "empty pair list = " << empty_pair_list_.size() << std::endl;
    int seed_index = std::rand()%available_pair_list_.size();
    int seed = arg->key_file->GetSeed(i, available_pair_list_[seed_index]);

    arg->lcd->ShowBySeed(seed);
    arg->key_file->ReadPicAsBmp(i, seed_index);
    int ret = arg->camera->GetOnePic();
    if (ret == -1)
      continue;
    cv::Mat pic1 = arg->key_file->GetMatImage();
    //验证两张图片
    result = AuthPic(pic1, arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "auth pic elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
    if (result <= kAuthThreshold) {
      std::cout << "auth pic ok" << std::endl;
      //找到相应的库
      break;
    } else {
      std::cout << "auth pic fault" << std::endl;
    }
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

    arg->lcd->ShowBySeed(seed);
    arg->key_file->ReadPicAsBmp(key_id, seed_index);
    arg->camera->GetOnePic();

    cv::Mat pic1 = arg->key_file->GetMatImage();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(pic1, arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);

    //删除本次循环使用的Seed文件及其对应的Pic文件
    std::cout << "delete old pair index = " << seed_index << std::endl;
    arg->key_file->DeleteSeed(key_id, seed_index);
    arg->key_file->DeletePic(key_id, seed_index);
    available_pair_list_[index] = -1;


    //值越小说明两张图片越相似
    if (result <= kAuthThreshold) {
      std::cout << "generate new a pair index = " << seed_index << std::endl;
      //认证通过
      //然后重新生成新的激励对
      int rand_seed = arg->sm->GenerateRandomSeed();
      arg->lcd->ShowBySeed(rand_seed);
      arg->camera->GetOnePic();

      arg->key_file->CopyPicToBuffer(arg->camera->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
      arg->key_file->SavePicAndSeed(key_id, seed_index, rand_seed);
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
}
