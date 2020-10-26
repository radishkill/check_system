#include "key_file.h"
#include<unistd.h>
#include<fstream>
#include<iostream>
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
  FILE *fp;
  if(access(base_path,0) != 0){
    std::cout << base_path<<" does not exist"<< std::endl;
    return;
  }
  std::string admin_path = base_path_ + "/PUF00";
  if(access(admin_path.c_str(), 0)!=0){
    std::cout << admin_path <<"does not exist" << std::endl;
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
    std::string seed_file = admin_seed_path+"/PUF00_Seed0000";
    std::string pic_file = admin_pic_path+"/PUF00_Pic0000";
  }

   // fp = open("base_path_\\PUF00 ","rt");
   //     if( fp == NULL){
    //    std::cout <<"The specified file does not exist"<< std::endl;
    //    return;
   //  }


}
//添加PUTData文件0-99
int KeyFile::AppendPufFile() {
  int i=0;
  while(1){
    std::string addpuf = Utils::DecToStr(i,2);
    std::string addpuf_path = base_path_ + "/PUF"+addpuf;
    if(access(addpuf_path.c_str(), 0) == 0){
      std::cout << addpuf_path <<"has exist" << std::endl;
      }
    else{
     mkdir(addpuf_path.c_str(),0777);
    }
    i++;
    if(i>1)
      break;
  }
  return 0;
}
//获得Seed内容
int KeyFile::GetSeed(int id,int index){

}
//获得Pic内容
int KeyFile::GetPic(int id, int index){

}
//添加Seed 0-1000文件
int KeyFile::AppendSeedPic()
{
  int i=0;
  while(1){
    std::string addseedpic = Utils::DecToStr(i,4);
    std::string addseed_path = base_path_+ "/PUF00/PUF00_Seed"+"/PUF00_Seed"+addseedpic;
    std::string addpic_path = base_path_+ "/PUF00/PUF00_Pic"+"/PUF00_Pic"+addseedpic;
    if(access(addseed_path.c_str(), 0) != 0)
      std::cout << addseed_path.c_str() <<"has not exist" << std::endl;
    if(access(addpic_path.c_str(), 0) != 0)
      std::cout << addpic_path.c_str() <<"has not exist" << std::endl;

    if(access(addseed_path.c_str(), 0) != 0|access(addpic_path.c_str(), 0) != 0){
      remove(addseed_path.c_str());
      remove(addpic_path.c_str());
      creat(addseed_path.c_str(),0777);
      creat(addpic_path.c_str(),0777);
      }
    else{
     creat(addseed_path.c_str(),0777);
     creat(addpic_path.c_str(),0777);
    }
    i++;
    if(i>2)
      break;
  }
  return 0;
}



