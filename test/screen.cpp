#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <random>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *frameBuffer = 0;
 
//打印fb驱动中fix结构信息，注：在fb驱动加载后，fix结构不可被修改。
void printFixedInfo ()
{
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
           finfo.id, finfo.smem_start, finfo.smem_len, finfo.type,
           finfo.type_aux, finfo.visual, finfo.xpanstep, finfo.ypanstep,
           finfo.ywrapstep, finfo.line_length, finfo.mmio_start,
           finfo.mmio_len, finfo.accel);
}
 
//打印fb驱动中var结构信息，注：fb驱动加载后，var结构可根据实际需要被重置
void printVariableInfo ()
{
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
           vinfo.xres, vinfo.yres, vinfo.xres_virtual, vinfo.yres_virtual,
           vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel,
           vinfo.grayscale, vinfo.red.offset, vinfo.red.length,
            vinfo.red.msb_right,vinfo.green.offset, vinfo.green.length,
           vinfo.green.msb_right, vinfo.blue.offset, vinfo.blue.length,
           vinfo.blue.msb_right, vinfo.transp.offset, vinfo.transp.length,
           vinfo.transp.msb_right, vinfo.nonstd, vinfo.activate,
           vinfo.height, vinfo.width, vinfo.accel_flags, vinfo.pixclock,
           vinfo.left_margin, vinfo.right_margin, vinfo.upper_margin,
           vinfo.lower_margin, vinfo.hsync_len, vinfo.vsync_len,
           vinfo.sync, vinfo.vmode);
}
//画一条直线
void drawline_rgb16 (int x0,int y0, int width,int height, int color,int flag0)
{
   const int bytesPerPixel = 2;//因为是rgb16，用16位来描述色深，所以2个字节
   const int stride = finfo.line_length / bytesPerPixel;//一行有多少个点
   const int red = (color & 0xff0000) >> (16 + 3);//下面是颜色的操作，我目前还没弄明白
   const int green = (color & 0xff00) >> (8 + 2);
   const int blue = (color & 0xff) >> 3;
   const short color16 = blue | (green << 5) | (red << (5 +6));
   int flag=flag0;//这里我为了图个方便就用一个flag来区分是画横线还是竖线，0表示横线，1表示竖线。


   short *dest = (short *) (frameBuffer)+ (y0 + vinfo.yoffset) * stride + (x0 +vinfo.xoffset);//这个就是我们画点的起始位置，其+stride就是换行（这个是我个人通过代码测试得出来的结论）

   int x=0,y=0;
   if(flag==0)
   {
   for (x = 0; x < width; ++x)//width就是我们x方向的终点
       {
           dest[x] = color16;
       }
   }
   else if(flag==1)
   {
       for(y=0;y<height;y++)//height就是我们y方向的终点
       {
           dest[x]=color16;//这里x始终为0，和下面一句结合起来就是每一行就画一个点，一共画height行，不就是一条竖线了么，这里我还思考了很久。

           dest +=stride;
       }
   }
}
//画大小为width*height的同色矩阵，5reds+6greens+5blues
void drawRect_rgb16 (int x0, int y0, int width,int height, int color)
{
   const int bytesPerPixel = 2;
   const int stride = finfo.line_length / bytesPerPixel;
   const int red = (color & 0xff0000) >> (16 + 3);
   const int green = (color & 0xff00) >> (8 + 2);
   const int blue = (color & 0xff) >> 3;
   const short color16 = blue | (green << 5) | (red << (5 +6));

   short *dest = (short *) (frameBuffer)+ (y0 + vinfo.yoffset) * stride + (x0 +vinfo.xoffset);

   int x, y;
   for (y = 0; y < height; ++y)
    {
       for (x = 0; x < width; ++x)
       {
           dest[x] = color16;
       }
       dest += stride;
    }
}
int ShowBySeed(int seed) {
   int x0 = 0;
   int y0 = 0;
   const int bytesPerPixel = 2;
   int color = 0xff00ff00;
   const int stride = finfo.line_length / bytesPerPixel;
  int x, y;
  short *dest = (short *) (frameBuffer)+ (y0 + vinfo.yoffset) * stride + (x0 +vinfo.xoffset);
  std::srand(seed);
  char s;
  for (y = 0; y < vinfo.yres; y += 10) { 
      for (x = 0; x < vinfo.xres; x++) {
           if (x%100 == 0) {
              s = std::rand()%0x100;
           }
           for (int w = 0; w < 10; w++) {
              if (y+w > vinfo.yres) {
                 break;
              }
            *(dest + (y+w)*stride + x*4) = s;
            *(dest+ (y+w)*stride + x*4 + 1) = s;
            *(dest+ (y+w)*stride + x*4 + 2) = s;

            *(dest+ (y+w)*stride + x*4 + 3) = 0;
            }
     }
  }
  return 0;
}
int main (int argc, char **argv)
{
   const char *devfile = "/dev/fb0";
   long int screensize = 0;
   int fbFd = 0;



   /* Open the file for reading and writing */
   fbFd = open (devfile, O_RDWR);
   if (fbFd == -1)
    {
       perror ("Error: cannot open framebuffer device");
       exit (1);
    }

   //获取finfo信息并显示
   if (ioctl (fbFd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
       perror ("Error reading fixed information");
       exit (2);
    }
    printFixedInfo ();
   //获取vinfo信息并显示drawRect_rgb16
   if (ioctl (fbFd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
       perror ("Error reading variable information");
       exit (3);
    }
    //  vinfo.xres = 800;
    //  vinfo.yres = 600;
   //  vinfo.bits_per_pixel = 32;
    // if (ioctl (fbFd, FBIOPUT_VSCREENINFO, &vinfo) == -1)
    // {
    //    perror ("Error reading variable information");
    //    exit (3);
    // }
    printVariableInfo ();

   /* Figure out the size of the screen in bytes */
   screensize = finfo.smem_len;//fb的缓存长度

   /* Map the device to memory */
    frameBuffer =(char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbFd, 0);
    if (frameBuffer == MAP_FAILED)
       {
           perror ("Error: Failed to map framebuffer device to memory");
           exit (4);
       }

      //drawRect_rgb16 (vinfo.xres *3 / 8, vinfo.yres * 3 / 8,vinfo.xres / 4, vinfo.yres / 4,0xff00ff00);//实现画矩形

       //drawline_rgb16(0,0,vinfo.xres,vinfo.yres,0xffff0000,0);

       //drawline_rgb16(260,10,100,280,0xff00ff00,1);//可以画出一个交叉的十字，坐标都是自己设的。
       auto begin_tick = std::chrono::steady_clock::now();
       std::srand(std::time(nullptr));
       ShowBySeed(std::rand());
        auto end_tick = std::chrono::steady_clock::now();
        std::cout << "elapsed time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
       printf (" Done.\n");

       munmap (frameBuffer, screensize);   //解除内存映射，与mmap对应

       close (fbFd);
       return 0;
}
