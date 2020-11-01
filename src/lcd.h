#ifndef LCD_H
#define LCD_H

#include <string>
#include <linux/fb.h>

namespace check_system {


/*
 *
 * width*height -> 800*600*3
 *
 */
class Lcd {
 public:
  Lcd(const char* device_file);
  int Open(const char* device_file);
  int ShowBySeed(int seed);
  bool IsOpen() const;
  int Close();
 private:
  void PrintFixedInfo();
  void PrintVariableInfo();
  const int width_;
  const int height_;
  const int per_pixel_;
  int fd_;
  struct fb_var_screeninfo var_info_;
  struct fb_fix_screeninfo fix_info_;
  char* frame_buffer_;
  std::string device_name_;
};
}



#endif // LCD_H
