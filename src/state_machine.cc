#include "state_machine.h"

#include "global_arg.h"
#include "laser.h"
#include "utils.h"
#include "camera_manager.h"


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
