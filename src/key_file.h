#ifndef KEY_FILE_H
#define KEY_FILE_H

#include <string>
#include <list>

#include <opencv2/opencv.hpp>

#include "camera_manager.h"

namespace check_system {
/*
 * 此类用来操作Puf File数据文件夹
 * 从Puf File数据文件夹 增加 删除 查改数据都要从此类进入
 */
class KeyFile {
 public:
  /*
   * 用来存放Puf数据的数据结构
   */
  struct Puf {
    int id;
    std::string adress;
  };
  /*
   * 激励响应对
   */
  struct RelevantPair {
    std::string adress_seed;
    std::string adress_pic;
  };

  KeyFile(const char* base_path);
  //向PufData文件夹追加一个新的文件
  int AppendPufFile();
  int FindEmptyKeyDir();
  int GetSeed(int id,int index);
  int IsSeedAvailable(int id, int index);
  int IsPicAvailable(int id, int index);
  int CheckKeyDirAvailable(int id);
  int ReadPicAsBmp(int id, int index);
  int SetMatImage(cv::Mat m) {image_ = m;};
  cv::Mat GetMatImage() {return image_;};
  int CopyPicToBuffer(char* pic, int width, int height);
  int SavePic(int id, int index, cv::Mat pic);
  int SaveSeed(int id,int index,int seed);
  int SavePicAndSeed(int key_id, int index, cv::Mat pic, int seed);
  int DeletePic(int id,int index);
  int DeleteSeed(int id,int index);
  int DeletePicAndSeed(int id, int index);
  int DeleteAllExceptAdmin();
  bool Is_Open()const;

 private:
  //base_path应该指向PUFData文件夹的内部,地址最后不包括/
  std::string base_path_;
  cv::Mat image_;
  int error = 0 ;
};
}

#endif // KEY_FILE_H
