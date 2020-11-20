#include "global_arg.h"

namespace check_system {

GlobalArg* GlobalArg::global_arg_ = nullptr;

GlobalArg *GlobalArg::GetInstance() {
  if (global_arg_ == nullptr) {
    global_arg_ = new GlobalArg();
  }
  return global_arg_;
}

GlobalArg::GlobalArg() {
  interrupt_flag = 0;
  hsk_flag = 0;
  is_fault = 0;
}
}
