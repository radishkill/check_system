#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
struct fb_var_screeninfo var_info_;
struct fb_fix_screeninfo fix_info_;
char *frame_buffer_ = 0;

//打印fb驱动中fix结构信息，注：在fb驱动加载后，fix结构不可被修改。
void printFixedInfo() {
  printf(
      "Fixed screen info:\n"
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
      fix_info_.type_aux, fix_info_.visual, fix_info_.xpanstep,
      fix_info_.ypanstep, fix_info_.ywrapstep, fix_info_.line_length,
      fix_info_.mmio_start, fix_info_.mmio_len, fix_info_.accel);
}

//打印fb驱动中var结构信息，注：fb驱动加载后，var结构可根据实际需要被重置
void printVariableInfo() {
  printf(
      "Variable screen info:\n"
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
      var_info_.xres, var_info_.yres, var_info_.xres_virtual,
      var_info_.yres_virtual, var_info_.xoffset, var_info_.yoffset,
      var_info_.bits_per_pixel, var_info_.grayscale, var_info_.red.offset,
      var_info_.red.length, var_info_.red.msb_right, var_info_.green.offset,
      var_info_.green.length, var_info_.green.msb_right, var_info_.blue.offset,
      var_info_.blue.length, var_info_.blue.msb_right, var_info_.transp.offset,
      var_info_.transp.length, var_info_.transp.msb_right, var_info_.nonstd,
      var_info_.activate, var_info_.height, var_info_.width,
      var_info_.accel_flags, var_info_.pixclock, var_info_.left_margin,
      var_info_.right_margin, var_info_.upper_margin, var_info_.lower_margin,
      var_info_.hsync_len, var_info_.vsync_len, var_info_.sync,
      var_info_.vmode);
}
//画一条直线
void drawline_rgb16(int x0, int y0, int width, int height, int color,
                    int flag0) {
  const int bytesPerPixel = 2;  //因为是rgb16，用16位来描述色深，所以2个字节
  const int stride = fix_info_.line_length / bytesPerPixel;  //一行有多少个点
  const int red =
      (color & 0xff0000) >> (16 + 3);  //下面是颜色的操作，我目前还没弄明白
  const int green = (color & 0xff00) >> (8 + 2);
  const int blue = (color & 0xff) >> 3;
  const short color16 = blue | (green << 5) | (red << (5 + 6));
  int flag =
      flag0;  //这里我为了图个方便就用一个flag来区分是画横线还是竖线，0表示横线，1表示竖线。

  short *dest =
      (short *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride +
      (x0 +
       var_info_
           .xoffset);  //这个就是我们画点的起始位置，其+stride就是换行（这个是我个人通过代码测试得出来的结论）

  int x = 0, y = 0;
  if (flag == 0) {
    for (x = 0; x < width; ++x)  // width就是我们x方向的终点
    {
      dest[x] = color16;
    }
  } else if (flag == 1) {
    for (y = 0; y < height; y++)  // height就是我们y方向的终点
    {
      dest[x] =
          color16;  //这里x始终为0，和下面一句结合起来就是每一行就画一个点，一共画height行，不就是一条竖线了么，这里我还思考了很久。

      dest += stride;
    }
  }
}
//画大小为width*height的同色矩阵，5reds+6greens+5blues
void drawRect_rgb16(int x0, int y0, int width, int height, int color) {
  const int bytesPerPixel = 2;
  const int stride = fix_info_.line_length / bytesPerPixel;
  const int red = (color & 0xff0000) >> (16 + 3);
  const int green = (color & 0xff00) >> (8 + 2);
  const int blue = (color & 0xff) >> 3;
  const short color16 = blue | (green << 5) | (red << (5 + 6));

  short *dest = (short *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride +
                (x0 + var_info_.xoffset);

  int x, y;
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      dest[x] = color16;
    }
    dest += stride;
  }
}
int ShowBySeed(int seed) {
  const int width = var_info_.xres;
  const int height = var_info_.yres;
  int x0 = 0;
  int y0 = 0;
  const int bytesPerPixel = 4;
  const int stride = fix_info_.line_length;
  int x, y;
  char *dest = (char *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride +
               (x0 + var_info_.xoffset);
  std::srand(seed);
  char c1, c2, c3;
  int rect_width = 50, rect_height = 50;
  for (y = 0; y < height; y += rect_height) {
    for (x = 0; x < width; x++) {
      if (x % rect_width == 0) {
        c1 = std::rand() % 2;
        if (c1 == 1) c1 = 0xff;
        // c2 = std::rand() % 0x100;
        // c3 = std::rand() % 0x100;
      }
      for (int w = 0; w < rect_height; w++) {
        if (y + w >= height) {
          break;
        }
        *(dest + (y + w) * stride + x * 4) = c1;
        *(dest + (y + w) * stride + x * 4 + 1) = c1;
        *(dest + (y + w) * stride + x * 4 + 2) = c1;

        *(dest + (y + w) * stride + x * 4 + 3) = 0xff;
      }
    }
  }
  return 0;
}
int ShowByMat(cv::Mat pic) {
  const int width = var_info_.xres;
  const int height = var_info_.yres;
  const int bytes_per_pixel = 4;
  const int stride = fix_info_.line_length;
  int x, y;
  int x0 = 0, y0 = 0;
  char *dest = (char *)(frame_buffer_) + (y0 + var_info_.yoffset) * stride +
               (x0 + var_info_.xoffset);
  char *src = (char *)pic.data;
  for (y = 0; y < height && y < pic.rows; y++) {
    int w = width < pic.cols ? width : pic.cols;
    std::memcpy(dest, src, w * bytes_per_pixel);
    dest += stride;
    src += pic.cols * bytes_per_pixel;
  }
  return 0;
}
int ShowByColor(unsigned char color[4]) {
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

int GetFbWidth() { return fix_info_.line_length / 4; }
int GetFbHeight() { return var_info_.yres_virtual; }
int main(int argc, char **argv) {
  const char *devfile = "/dev/fb0";
  long int screensize = 0;
  int fbFd = 0;

  /* Open the file for reading and writing */
  fbFd = open(devfile, O_RDWR);
  if (fbFd == -1) {
    perror("Error: cannot open framebuffer device");
    exit(1);
  }

  //获取finfo信息并显示
  if (ioctl(fbFd, FBIOGET_FSCREENINFO, &fix_info_) == -1) {
    perror("Error reading fixed information");
    exit(2);
  }
  printFixedInfo();
  //获取vinfo信息并显示drawRect_rgb16
  if (ioctl(fbFd, FBIOGET_VSCREENINFO, &var_info_) == -1) {
    perror("Error reading variable information");
    exit(3);
  }
  //  vinfo.xres = 800;
  //  vinfo.yres = 600;
  //  vinfo.bits_per_pixel = 32;
  // if (ioctl (fbFd, FBIOPUT_VSCREENINFO, &vinfo) == -1)
  // {
  //    perror ("Error reading variable information");
  //    exit (3);
  // }
  printVariableInfo();

  /* Figure out the size of the screen in bytes */
  screensize = fix_info_.smem_len;  // fb的缓存长度

  /* Map the device to memory */
  frame_buffer_ =
      (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbFd, 0);
  if (frame_buffer_ == MAP_FAILED) {
    perror("Error: Failed to map framebuffer device to memory");
    exit(4);
  }

  // drawRect_rgb16 (vinfo.xres *3 / 8, vinfo.yres * 3 / 8,vinfo.xres / 4,
  // vinfo.yres / 4,0xff00ff00);//实现画矩形

  // drawline_rgb16(0,0,vinfo.xres,vinfo.yres,0xffff0000,0);

  // drawline_rgb16(260,10,100,280,0xff00ff00,1);//可以画出一个交叉的十字，坐标都是自己设的。
  auto begin_tick = std::chrono::steady_clock::now();
  unsigned char color[4] = {0xff, 0x00, 0x00, 0xff};
  ShowByColor(color);
  // cv::Mat pic = cv::imread("./pic.bmp", cv::IMREAD_UNCHANGED);
  // ShowByMat(pic);

  // std::srand(std::time(nullptr));
  // ShowBySeed(std::rand());
  // cv::Mat pic = cv::Mat(GetFbHeight(), GetFbWidth(), CV_8UC4, frameBuffer);
  // cv::imwrite("./pic.bmp", pic);
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "elapsed time:"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick -
                                                                     begin_tick)
                   .count()
            << "ms" << std::endl;

  munmap(frame_buffer_, screensize);  //解除内存映射，与mmap对应

  close(fbFd);
  return 0;
}
