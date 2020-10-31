#include "hostcontroller.h"





check_system::HostController::HostController(const char *device_file) {
  Open(device_file);
}

bool check_system::HostController::IsOpen() const {
}

void check_system::HostController::Open(const char *device_file) {
  usart.Open(device_file, 115200, 8, 1, 0, 0);
}
