#include "state_machine.h"

#include <cstdlib>
#include <ctime>
#include <cassert>

#include "global_arg.h"
#include "laser.h"
#include "utils.h"
#include "camera_manager.h"
#include "key_file.h"
#include "lcd.h"


StateMachine::StateMachine() {

}
/*
 * 此过程中但凡有一个步骤出错都应该退出
 *
 */
int StateMachine::SelfTest() {
  int ret;
  GlobalArg* arg = GlobalArg::GetInstance();
  ret = arg->laser->SendCloseCmd();
  Utils::MSleep(3000);
  ret = arg->laser->SendOpenCmd();
  Utils::MSleep(3000);
  //此次还需要在LCD上显示随机的SEED图
  //Seed生成方法还不知道

  ret = arg->camera->GetPic();
  ret = arg->camera->CheckPic();
  ret = arg->laser->SendCloseCmd();
  return 0;
}

int StateMachine::Register() {
  GlobalArg* arg = GlobalArg::GetInstance();
  if (!arg->laser->IsOpen()) {
    return -1;
  }
  arg->laser->SendOpenCmd();
  Utils::MSleep(10000);
  arg->laser->SendCheckCmd();

  return 0;
}

int StateMachine::Authentication() {
  return 0;
}

int StateMachine::GenerateRandomSeed() {
  std::srand(std::time(nullptr));
  return std::rand()%100000;
}

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
    int seed_index = std::rand()%1000;
    int seed = arg->key_file->GetSeed(i, seed_index);
    arg->lcd->ShowBySeed(seed);
    arg->key_file->GetPic(i, seed_index);
    arg->camera->GetPic();
    temp_pic = arg->camera->GetRBGBuffer();
    pic = arg->key_file->GetPicBuffer();

    //运算temp_pic 与 pic 得到结果

    i++;
  }
  return i;
}
