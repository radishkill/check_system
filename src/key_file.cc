#include "key_file.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>

#include "mutils.h"

namespace check_system {
/*
 * 这里面的步骤应该是
 * 1. 检查base_path是否存在  不存在就报错返回 base_path direcotry
 * 2. 检查PUF00 是否存在  不存在就报错返回
 * 3. 检查PUF00中是否包含1000个激励相应对 没有就报错返回 （可以跳过）
 */
KeyFile::KeyFile(const char* base_path) : image_() {
  base_path_ = base_path;
  if (access(base_path, 0) != 0) {
    error = 1;
    std::cout << base_path << " does not exist" << std::endl;
    return;
  }
  std::string admin_path = base_path_ + "/PUF00";
  if (access(admin_path.c_str(), 0) != 0) {
    error = 1;
    std::cout << admin_path << " does not exist" << std::endl;
    return;
  }
  std::string admin_seed_path = admin_path + "/PUF00_Seed";
  if (access(admin_seed_path.c_str(), 0) != 0) {
    error = 1;
    std::cout << admin_seed_path << "does not exist" << std::endl;
    return;
  }
  std::string admin_pic_path = admin_path + "/PUF00_Pic";
  if (access(admin_pic_path.c_str(), 0) != 0) {
    error = 1;
    std::cout << admin_pic_path << "does not exist" << std::endl;
    return;
  }

  //将所有的文件夹填充好
  for (int i = 1; i < 100; i++) {
    int ret = CheckKeyDirAvailable(i);
    if (ret != 0) {
      std::string key_dir_path =
          base_path_ + std::string("/PUF" + Utils::DecToStr(i, 2));
      std::string key_seed_path =
          key_dir_path + std::string("/PUF" + Utils::DecToStr(i, 2)) + "_Seed";
      std::string key_pic_path =
          key_dir_path + std::string("/PUF" + Utils::DecToStr(i, 2)) + "_Pic";

      mkdir(key_dir_path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      //创建PUF_Pic和PUF_Seed
      mkdir(key_seed_path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      mkdir(key_pic_path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      std::cout << "make key dir " << i << std::endl;
    }
  }
}

int KeyFile::AppendPufFile() {
  int i = 0;
  while (i < 100) {
    std::string addpuf_path = std::string("/PUF" + Utils::DecToStr(i, 2));
    if (access((base_path_ + addpuf_path).c_str(), 0) != 0) {
      std::string addpufseed_path =
          base_path_ + addpuf_path + addpuf_path + "_Seed";
      std::string addpufpic_path =
          base_path_ + addpuf_path + addpuf_path + "_Pic";

      mkdir((base_path_ + addpuf_path).c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      //创建PUF_Pic和PUF_Seed
      mkdir(addpufseed_path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      mkdir(addpufpic_path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
      std::cout << addpuf_path.c_str() << "mkdir seccess" << std::endl;
      break;
    }
    i++;
  }
  if (i >= 100) {
    i = -1;
  }
  return i;
}

int KeyFile::FindEmptyKeyDir() {
  int i, j;
  for (i = 1; i < 100; i++) {
    for (j = 0; j < 1000; j++) {
      if (IsSeedAvailable(i, j)) break;
    }
    if (j == 1000) {
      return i;
    }
  }
  return -1;
}
//获得Seed内容
int KeyFile::GetSeed(int id, int index) {
  std::ifstream ifs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ifs.open(base_path_ + puf_file_name + puf_file_name + "_Seed" +
           puf_file_name + "_Seed" + Utils::DecToStr(index, 4));
  if (!ifs.is_open()) {
    std::cout << "open seed file "
              << "PUF" << Utils::DecToStr(id, 2) << " "
              << Utils::DecToStr(index, 4) << " wrong!!!" << std::endl;
    return -1;
  }
  int seed = 0;
  ifs >> seed;
  if (seed == 0) {
    std::cout << "read seed file "
              << "PUF" << Utils::DecToStr(id, 2) << " "
              << Utils::DecToStr(index, 4) << " wrong!!!" << std::endl;
  }
  ifs.close();
  return seed;
}
//可用的seed
int KeyFile::IsSeedAvailable(int id, int index) {
  std::ifstream ifs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ifs.open(base_path_ + puf_file_name + puf_file_name + "_Seed" +
           puf_file_name + "_Seed" + Utils::DecToStr(index, 4));
  if (!ifs.is_open()) {
    return 0;
  }
  ifs.close();
  return 1;
}
int KeyFile::IsPicAvailable(int id, int index) {
  std::ifstream ifs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ifs.open(base_path_ + puf_file_name + puf_file_name + "_Pic" + puf_file_name +
           "_Pic" + Utils::DecToStr(index, 4) + ".bmp");
  if (!ifs.is_open()) {
    return 0;
  }
  ifs.close();
  return 1;
}

int KeyFile::CheckKeyDirAvailable(int id) {
  std::string key_dir_path =
      base_path_ + std::string("/PUF" + Utils::DecToStr(id, 2));
  if (access(key_dir_path.c_str(), 0) != 0) {
    return -1;
  }
  std::string key_seed_path =
      key_dir_path + std::string("/PUF" + Utils::DecToStr(id, 2)) + "_Seed";
  if (access(key_seed_path.c_str(), 0) != 0) {
    return -1;
  }
  std::string key_pic_path =
      key_dir_path + std::string("/PUF" + Utils::DecToStr(id, 2)) + "_Pic";
  if (access(key_pic_path.c_str(), 0) != 0) {
    return -1;
  }
  return 0;
}

cv::Mat KeyFile::ReadPic(int id, int index) {
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  image_ = cv::imread(base_path_ + puf_file_name + puf_file_name + "_Pic" +
                          puf_file_name + "_Pic" + Utils::DecToStr(index, 4) +
                          ".bmp",
                      cv::IMREAD_UNCHANGED);
  if (!image_.data) {
    std::cout << "read pic file "
              << "PUF" << Utils::DecToStr(id, 2) << " "
              << Utils::DecToStr(index, 4) << " wrong!!!" << std::endl;
    return image_;
  }
  return image_;
}
//复制图片到文件图片缓冲区
int KeyFile::CopyPicToBuffer(char* pic, int width, int height) {
  image_ = cv::Mat(height, width, CV_8UC1);
  std::memcpy(image_.data, pic, width * height);
  return 0;
}

int KeyFile::SavePic(int id, int index, cv::Mat pic) {
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  cv::imwrite(base_path_ + puf_file_name + puf_file_name + "_Pic" +
                  puf_file_name + "_Pic" + Utils::DecToStr(index, 4) + ".bmp",
              pic);
  return 0;
}
//保存seed
int KeyFile::SaveSeed(int id, int index, int seed) {
  std::ofstream ofs;
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  ofs.open(base_path_ + puf_file_name + puf_file_name + "_Seed" +
           puf_file_name + "_Seed" + Utils::DecToStr(index, 4));
  if (!ofs.is_open()) {
    std::cout << "open seed file "
              << "PUF" << Utils::DecToStr(id, 2) << " wrong!!!" << std::endl;
    return 0;
  }
  ofs << seed;
  ofs.close();
  return 0;
}

int KeyFile::SavePicAndSeed(int key_id, int index, cv::Mat pic, int seed) {
  SavePic(key_id, index, pic);
  SaveSeed(key_id, index, seed);
  return 0;
}
//删除pic
int KeyFile::DeletePic(int id, int index) {
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  std::string deletepic = base_path_ + puf_file_name + puf_file_name + "_Pic" +
                          puf_file_name + "_Pic" + Utils::DecToStr(index, 4) +
                          ".bmp";
  std::cout << "remove " << deletepic << std::endl;
  remove(deletepic.c_str());
  return 0;
}
//删除seed
int KeyFile::DeleteSeed(int id, int index) {
  std::string puf_file_name = std::string("/PUF" + Utils::DecToStr(id, 2));
  std::string deleteseed = base_path_ + puf_file_name + puf_file_name +
                           "_Seed" + puf_file_name + "_Seed" +
                           Utils::DecToStr(index, 4);
  std::cout << "remove " << deleteseed << std::endl;
  remove(deleteseed.c_str());
  return 0;
}
int KeyFile::DeletePicAndSeed(int id, int index) {
  DeletePic(id, index);
  DeleteSeed(id, index);
  return 0;
}

int KeyFile::DeleteAllExceptAdmin() {
  const int max_key = 100;
  const int max_seed = 1000;
  for (int i = 1; i < max_key; i++) {
    for (int j = 0; j < max_seed; j++) {
      if (IsSeedAvailable(i, j)) {
        DeleteSeed(i, j);
      }
      if (IsPicAvailable(i, j)) {
        DeletePic(i, j);
      }
    }
  }
  return 0;
}
int KeyFile::DeleteAdmin() {
  const int max_seed = 1000;
  int i = 0;
  for (int j = 0; j < max_seed; j++) {
    if (IsSeedAvailable(i, j)) {
      DeleteSeed(i, j);
    }
    if (IsPicAvailable(i, j)) {
      DeletePic(i, j);
    }
  }
  return 0;
}
int KeyFile::DeleteAll() {
  const int max_key = 100;
  const int max_seed = 1000;
  for (int i = 0; i < max_key; i++) {
    for (int j = 0; j < max_seed; j++) {
      if (IsSeedAvailable(i, j)) {
        DeleteSeed(i, j);
      }
      if (IsPicAvailable(i, j)) {
        DeletePic(i, j);
      }
    }
  }
  return 0;
}

bool KeyFile::Is_Open() const {
  if (error == 1) return false;
  return true;
}
}  // namespace check_system
