#ifndef KEY_FILE_H
#define KEY_FILE_H

#include <string>
#include <list>
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
  //向PufData文件夹追加新的puf数据
  int AppendPufFile();
  int GetSeed(int id,int index);
  int IsSeedAvailable(int id, int index);
  int GetPic(int id,int index);
  char* GetPicBuffer();
  int CopyPicToBuffer(char* pic, int width, int height);
  int SavePic(int id, int index);
  int SaveSeed(int id,int index,int seed);
  int DeletePic(int id,int index);
  int DeleteSeed(int id,int index);
 private:
  //base_path应该指向PUFData文件夹的内部,地址最后不包括/
  std::string base_path_;
  char pic_buffer_[1080][1920][4];
};


#endif // KEY_FILE_H
