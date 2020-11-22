#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <vector>
#include <mutex>
#include <opencv2/opencv.hpp>

namespace check_system {

class StateMachine {
 public:
  enum MachineState {
    kSelfTest,
    kRegister,
    kAuth
  };
  StateMachine();
  int RunMachine(MachineState state);
  int SelfTest();                   //自检
  int Register();                   //注册
  int Authentication();            //认证
  int GenerateRandomSeed();        //随机生成seed
  int FindKey();                   //库定位算法

  int CheckKey();                  //插入检测算法
  int CheckAdminKey();             //管理员KEY检测算法
  int CheckPairStore(int id);      //库遍历算法
  int CheckAvailablePair(int id);  //检查可用激励对

  int AuthPic(cv::Mat& speckle_database, char *pic2, int h2, int w2);

 private:
  std::vector<int> empty_pair_list_;
  std::vector<int> available_pair_list_;
  bool is_running_;
  std::mutex mutex_;
};
}

#endif // STATE_MACHINE_H
