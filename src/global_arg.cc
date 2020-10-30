#include "global_arg.h"

namespace check_system {

GlobalArg* GlobalArg::global_arg_ = nullptr;

GlobalArg *GlobalArg::GetInstance() {
  if (global_arg_ == nullptr) {
    global_arg_ = new GlobalArg();
  }
  return global_arg_;
}
}
