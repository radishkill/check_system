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
 private:
};

#endif // STATE_MACHINE_H
