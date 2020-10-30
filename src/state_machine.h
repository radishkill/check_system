#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <vector>

namespace check_system {

class StateMachine {
 public:
  StateMachine();
  int SelfTest();                   //自检
  int Register();                   //注册
  int Authentication();            //认证
  int GenerateRandomSeed();        //随机生成seed
  int FindKey();                   //库定位算法

  int CheckKey();                  //插入检测算法
  int CheckAdminKey();             //管理员KEY检测算法
  int CheckEmptyPair(int id);      //库遍历算法
  int CheckAvailablePair(int id);  //检查可用激励对

  int AuthPic(char *pic1, int h1, int w1, char *pic2, int h2, int w2);



 private:
  std::vector<int> pair_list_;
};
}

#endif // STATE_MACHINE_H
