#include "lcd.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>


#include <iostream>
#include <chrono>

namespace check_system {
Lcd::Lcd(const char *device_file)
  : device_name_(device_file), rect_width_(5), rect_height_(5) {
  fd_ = -1;
  Open(device_file);
}

int Lcd::Open(const char *device_file) {
  fd_ = open(device_file, O_RDWR);
  if (fd_ == -1) {
    perror("can't open framebuffer device");
    return -1;
  }
  if (ioctl(fd_, FBIOGET_FSCREENINFO, &fix_info_) == -1) {
    perror("Error reading fixed information");
    close(fd_);
    fd_ = -1;
    return -1;
  }
  if (ioctl(fd_, FBIOGET_VSCREENINFO, &var_info_) == -1) {
    perror("Error reading variable information");
    close(fd_);
    fd_ = -1;
    return -1;
  }
//  var_info_.xres = width_;
//  var_info_.yres = height_;
//  var_info_.bits_per_pixel = per_pixel_;
//  if (ioctl(fd_, FBIOPUT_VSCREENINFO, &var_info_) == -1) {
//    perror("Error reading variable information");
//    close(fd_);
//    fd_ = -1;
//    return -1;
//  }
  int mem_size = fix_info_.smem_len;
//  frame_buffer_ = new char[800*600];
  frame_buffer_ =(char *) mmap (0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (frame_buffer_ == MAP_FAILED) {
    perror ("Error: Failed to map framebuffer device to memory");
    close(fd_);
    fd_ = -1;
    return -1;
  }
  return 0;
}
int Lcd::ShowBySeed(int seed)
{
   const int width = var_info_.xres;
   const int height = var_info_.yres;
   int x0 = 0;
   int y0 = 0;
   const int bytes_per_pixel = 4;
   const int stride = fix_info_.line_length;
   int x, y;
   char *dest = (char *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride + (x0 + var_info_.xoffset);
   std::srand(seed);
   char c1,c2,c3;
   int rect_width = rect_width_, rect_height = rect_height_;
   for (y = 0; y < height; y += rect_height)
   {
      for (x = 0; x < width; x++)
      {
         if (x % rect_width == 0) {
            c1 = std::rand() % 0x100;
            // if (c1 == 1)
              // c1 = 0xff;
         }
         
         // std::cout << x << " " << y << " " << std::endl;
         for (int w = 0; w < rect_height; w++)
         {
            if (y + w >= height)
            {
               break;
            }
            *(dest + (y + w) * stride + x * bytes_per_pixel) = c1;
            *(dest + (y + w) * stride + x * bytes_per_pixel + 1) = c1;
            *(dest + (y + w) * stride + x * bytes_per_pixel + 2) = c1;

            *(dest + (y + w) * stride + x * bytes_per_pixel + 3) = 0xff;
         }
      }
   }
   return 0;
}
int Lcd::ShowByMat(cv::Mat pic) {
  const int width = var_info_.xres;
  const int height = var_info_.yres;
  const int bytes_per_pixel = 4;
  const int stride = fix_info_.line_length;
  int x, y;
  int x0 = 0, y0 = 0;
  char *dest = (char *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride + (x0 + var_info_.xoffset);
  char *src = (char*)pic.data;
  for (y = 0; y < height && y < pic.rows; y++) {
    int w = width<pic.cols?width:pic.cols;
    std::memcpy(dest, src, w*bytes_per_pixel);
    dest += stride;
    src += pic.cols*bytes_per_pixel;
  }
  return 0;
}
int Lcd::ShowByColor(unsigned char color[4]) {
  const int width = var_info_.xres;
  const int height = var_info_.yres;
  const int bytes_per_pixel = 4;
  const int stride = fix_info_.line_length;
  int x, y;
  int x0 = 0, y0 = 0;
  char *dest = (char *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride +
               (x0 + var_info_.xoffset);
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      std::memcpy(dest + x*bytes_per_pixel, color, bytes_per_pixel);
    }
    dest += stride;
  }
  return 0;
}

int Lcd::GetFbWidth() const {
  return fix_info_.line_length/4;
}
int Lcd::GetFbHeight() const {
  return var_info_.yres_virtual;
}

bool Lcd::IsOpen() const {
  if (fd_ <= 0) {
    return false;
  }
  return true;
}

int Lcd::Close() {
  int mem_size = fix_info_.smem_len;
  munmap(frame_buffer_, mem_size);   //解除内存映射，与mmap对应
  close(fd_);
  return 0;
}

void Lcd::PrintFixedInfo() {
  printf ("Fixed screen info:\n"
                       "\tid: %s\n"
                       "\tsmem_start:0x%lx\n"
                       "\tsmem_len:%d\n"
                       "\ttype:%d\n"
                       "\ttype_aux:%d\n"
                       "\tvisual:%d\n"
                       "\txpanstep:%d\n"
                       "\typanstep:%d\n"
                       "\tywrapstep:%d\n"
                       "\tline_length: %d\n"
                       "\tmmio_start:0x%lx\n"
                       "\tmmio_len:%d\n"
                       "\taccel:%d\n"
          "\n",
          fix_info_.id, fix_info_.smem_start, fix_info_.smem_len, fix_info_.type,
          fix_info_.type_aux, fix_info_.visual, fix_info_.xpanstep, fix_info_.ypanstep,
          fix_info_.ywrapstep, fix_info_.line_length, fix_info_.mmio_start,
          fix_info_.mmio_len, fix_info_.accel);
}

void Lcd::PrintVariableInfo() {
  printf ("Variable screen info:\n"
                       "\txres:%d\n"
                       "\tyres:%d\n"
                       "\txres_virtual:%d\n"
                       "\tyres_virtual:%d\n"
                       "\tyoffset:%d\n"
                       "\txoffset:%d\n"
                       "\tbits_per_pixel:%d\n"
                       "\tgrayscale:%d\n"
                       "\tred: offset:%2d, length: %2d, msb_right: %2d\n"
                       "\tgreen: offset:%2d, length: %2d, msb_right: %2d\n"
                       "\tblue: offset:%2d, length: %2d, msb_right: %2d\n"
                       "\ttransp: offset:%2d, length: %2d, msb_right: %2d\n"
                       "\tnonstd:%d\n"
                       "\tactivate:%d\n"
                       "\theight:%d\n"
                       "\twidth:%d\n"
                       "\taccel_flags:0x%x\n"
                       "\tpixclock:%d\n"
                       "\tleft_margin:%d\n"
                       "\tright_margin: %d\n"
                       "\tupper_margin:%d\n"
                       "\tlower_margin:%d\n"
                       "\thsync_len:%d\n"
                       "\tvsync_len:%d\n"
                       "\tsync:%d\n"
                      "\tvmode:%d\n"
          "\n",
          var_info_.xres, var_info_.yres, var_info_.xres_virtual, var_info_.yres_virtual,
          var_info_.xoffset, var_info_.yoffset, var_info_.bits_per_pixel,
          var_info_.grayscale, var_info_.red.offset, var_info_.red.length,
          var_info_.red.msb_right,var_info_.green.offset, var_info_.green.length,
          var_info_.green.msb_right, var_info_.blue.offset, var_info_.blue.length,
          var_info_.blue.msb_right, var_info_.transp.offset, var_info_.transp.length,
          var_info_.transp.msb_right, var_info_.nonstd, var_info_.activate,
          var_info_.height, var_info_.width, var_info_.accel_flags, var_info_.pixclock,
          var_info_.left_margin, var_info_.right_margin, var_info_.upper_margin,
          var_info_.lower_margin, var_info_.hsync_len, var_info_.vsync_len,
          var_info_.sync, var_info_.vmode);
}
}


