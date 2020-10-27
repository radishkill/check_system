#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


class StateMachine {
 public:
  StateMachine();
  int SelfTest();
  int Register();
  int Authentication();
  //生成随机的seed
  int GenerateRandomSeed();
  int FindKey();

  int CheckKey();     //插入检测算法
  int Collection();   //采集算法
  int CheckAdminKey();
 private:
};

#endif // STATE_MACHINE_H
