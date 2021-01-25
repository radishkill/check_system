#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <mutex>
#include <vector>

namespace check_system {

class StateMachine {
 public:
  enum MachineState { kSelfTest, kRegister, kSystemInit, kAuth, kOther };
  StateMachine();
  int RunMachine(MachineState state);
  int SelfTest();            //自检
  int Register();            //注册
  int Authentication();      //认证
  int GenerateRandomSeed();  //随机生成seed
  int FindKey();             //库定位算法
  int ShowBySeed(int seed);
  int TakePhoto();
  int Other(int s);

  int CheckKeyInsert();  //插入检测算法
  int CheckKey(int key_id);
  int CheckPairStore(int id);  //库遍历算法
  int SystemInit();            //系统初始化
 private:
  
  std::vector<int> empty_pair_list_;
  std::vector<int> available_pair_list_;
  bool find_key_flag_;
  bool is_running_;
  std::mutex mutex_;
};
}  // namespace check_system

#endif  // STATE_MACHINE_H
