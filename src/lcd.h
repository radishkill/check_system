#ifndef LCD_H
#define LCD_H

#include <string>

namespace check_system {
class Lcd {
 public:
  Lcd(const char* device_name);
  int ShowBySeed(int seed);
 private:
  std::string device_name_;
};
}



#endif // LCD_H
