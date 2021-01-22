#ifndef LCD_H
#define LCD_H

#include <string>
#include <linux/fb.h>

#include <opencv2/opencv.hpp>

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
  int ShowBySeed(unsigned int seed);
  int ShowByMat(cv::Mat pic);
  int ShowByColor(unsigned char color);
  int GetFbWidth() const;
  int GetFbHeight() const;
  void SetRect(int w, int h) {
    rect_width_ = w;
    rect_height_ = h;
  }
  char* GetFrameBuffer() {
    return frame_buffer_;
  }
  bool IsOpen() const;
  int Close();
  int rect_width_;
  int rect_height_;
 private:
  void PrintFixedInfo();
  void PrintVariableInfo();

  int fd_;
  struct fb_var_screeninfo var_info_;
  struct fb_fix_screeninfo fix_info_;
  char* frame_buffer_;
  std::string device_name_;
};
}



#endif // LCD_H
