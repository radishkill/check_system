#ifndef LCD_H
#define LCD_H

#include <string>

namespace check_system {


/*
 *
 * width*height -> 800*600*3
 *
 */
class Lcd {
 public:
  Lcd(const char* device_name);
  int ShowBySeed(int seed);
 private:
  int fd_;
  std::string device_name_;
};
}



#endif // LCD_H
