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

namespace check_system {
Lcd::Lcd(const char *device_file)
  : device_name_(device_file), width_(800), height_(600)
    , per_pixel_(24) {
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
  var_info_.xres = width_;
  var_info_.yres = height_;
  var_info_.bits_per_pixel = per_pixel_;
  if (ioctl(fd_, FBIOPUT_VSCREENINFO, &var_info_) == -1) {
    perror("Error reading variable information");
    close(fd_);
    fd_ = -1;
    return -1;
  }
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
//显示seed到LCD 800 * 600
int Lcd::ShowBySeed(int seed) {
  int x, y;
  char* dest = frame_buffer_;
  std::srand(seed);
  for (y = 0; y < height_; y++) {
    for (x = 0; x < width_; x++) {
      *dest = std::rand()%0x100;
      *(dest+1) = std::rand()%0x100;
      *(dest+2) = std::rand()%0x100;
      dest += 3;
    }
  }
  return 0;
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


