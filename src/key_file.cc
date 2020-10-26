#include "key_file.h"
#include<unistd.h>
#include<fstream>
#include<iostream>
#include <cstring>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include "utils.h"
/*
 * 这里面的步骤应该是
 * 1. 检查base_path是否存在  不存在就报错返回 base_path direcotry
 * 2. 检查PUF00 是否存在  不存在就报错返回
 * 3. 检查PUF00中是否包含1000个激励相应对 没有就报错返回 （可以跳过）
 */
using namespace std;
KeyFile::KeyFile(const char* base_path) {
  base_path_ = base_path;
  if(access(base_path,0) != 0){
    std::cout << base_path<<" does not exist"<< std::endl;
    return;
  }
  std::string admin_path = base_path_ + "/PUF00";
  if(access(admin_path.c_str(), 0)!=0){
    std::cout << admin_path <<" does not exist" << std::endl;
    return;
  }
  std::string admin_seed_path = admin_path +"/PUF00_Seed";
   if(access(admin_seed_path.c_str(), 0)!=0){
        std::cout << admin_seed_path <<"does not exist" << std::endl;
        return;
   }
   std::string admin_pic_path = admin_path+"/PUF00_Pic";
   if(access(admin_pic_path.c_str(), 0)!=0){
        std::cout << admin_pic_path <<"does not exist" << std::endl;
        return;
   }
  for (int i = 0; i < 1; i++) {
    std::string seed_file = admin_seed_path + Utils::DecToStr(i, 4);
    std::string pic_file = admin_pic_path + Utils::DecToStr(i, 4);
    std::cout << "not found " << seed_file << std::endl;
    std::cout << "not found " << pic_file << std::endl;
  }
}
//添加PUTData文件0-99
int KeyFile::AppendPufFile() {
  int i = 0;
  while(1) {
    std::string addpuf_path = std::string("/PUF" + Utils::DecToStr(i, 2));
    std::string addpufseed_path = base_path_ + addpuf_path + addpuf_path+"_Seed";
    std::string addpufpic_path = base_path_ + addpuf_path + addpuf_path+"_Pic";
    if(access((base_path_+addpuf_path).c_str(), 0) != 0) {
      mkdir((base_path_+addpuf_path).c_str(), 0777);
       //创建PUF_Pic和PUF_Seed
      mkdir(addpufseed_path.c_str(),0777);
      mkdir(addpufpic_path.c_str(),0777);
      std::cout << addpuf_path.c_str() <<  "mkdir seccess"<< std::endl;
    }
    if (i >= 6) {
      break;
    }
    i++;
  }
  return i;
}
//获得Seed内容
int KeyFile::GetSeed(int id, int index){
  std::ifstream ifs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ifs.open(base_path_ + puf_file_name + puf_file_name + "_Seed" + puf_file_name + "_Seed" + Utils::DecToStr(index, 4));
  if (!ifs.is_open()) {
    std::cout << "open seed file " << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
    return 0;
  }
  int seed = 0;
  ifs >> seed;
  if (seed == 0) {
    std::cout << "read seed file " << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
  }
  ifs.close();
  return seed;
}
//获得Pic内容
int KeyFile::GetPic(int id, int index) {
  std::ifstream ifs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ifs.open(base_path_ + puf_file_name + puf_file_name + "_Pic" + puf_file_name + "_Pic" + Utils::DecToStr(index, 4));
  if (!ifs.is_open()) {
    std::cout << "open pic file " << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
    return 0;
  }
  int i = 0;
  int j = 0;
  int ret = 0;
  for (i = 0; i < 1080; i++) {
    for (j = 0; j< 1920; j++) {
      ret = ifs.readsome(pic_buffer_[i][j], 4);
      if (ret != 4 && !ifs.eof()) {
        ifs.sync();
        ifs.readsome(pic_buffer_[i][j] + ret, 4-ret);
        ret += ifs.gcount();
      }
      if (ret  != 4 && ifs.eof()) {
        std::cout << "read pic file " << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
        break;
      }
    }
    if (j != 1920) {
      break;
    }
  }
  ifs.close();
  if (i != 1080) {
    return -1;
  }
  return 0;
}
//得到照片的缓存路径
char *KeyFile::GetPicBuffer() {
  return **pic_buffer_;
}
//获得照片到缓存区
int KeyFile::CopyPicToBuffer(char *pic, int width, int height) {
  for (int i = 0; i < 1080; i++) {
    std::memcpy(*pic_buffer_[i], pic + i*1920*4, 1920*4);
  }
  return 0;
}
//从缓存区保存照片
int KeyFile::SavePic(int id, int index) {
  std::ofstream ofs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ofs.open(base_path_ + puf_file_name + puf_file_name + "_Pic" + puf_file_name + "_Pic" + Utils::DecToStr(index, 4));
  if (!ofs.is_open()) {
    std::cout << "open pic file " << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
    return 0;
  }
  int i = 0;
  int j = 0;
  int ret = 0;
  for (i = 0; i < 1080; i++) {
    for (j = 0; j< 1920; j++) {
      ofs.write(pic_buffer_[i][j], 4);
    }
    ofs.flush();
  }
  ofs.close();
  return 0;
}
////添加Seed 0-1000文件
//int KeyFile::AppendSeedPic()
//{
//  int i=0;
//  while(1){
//    std::string addseedpic = Utils::DecToStr(i,4);
//    std::string addseed_path = base_path_+ "/PUF00/PUF00_Seed"+"/PUF00_Seed"+addseedpic;
//    std::string addpic_path = base_path_+ "/PUF00/PUF00_Pic"+"/PUF00_Pic"+addseedpic;
//    if(access(addseed_path.c_str(), 0) != 0)
//      std::cout << addseed_path.c_str() <<"has not exist" << std::endl;
//    if(access(addpic_path.c_str(), 0) != 0)
//      std::cout << addpic_path.c_str() <<"has not exist" << std::endl;

//    if(access(addseed_path.c_str(), 0) != 0|access(addpic_path.c_str(), 0) != 0){
//      remove(addseed_path.c_str());
//      remove(addpic_path.c_str());
//      creat(addseed_path.c_str(),0777);
//      creat(addpic_path.c_str(),0777);
//      }
//    else{
//     creat(addseed_path.c_str(),0777);
//     creat(addpic_path.c_str(),0777);
//    }
//    i++;
//    if(i>2)
//      break;
//  }
//  return 0;
//}





