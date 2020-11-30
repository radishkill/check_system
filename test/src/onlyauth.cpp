#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

#include "usart.h"
#include "laser.h"
#include "camera_manager.h"
#include "utils.h"
#include "key_file.h"
#include "lcd.h"
#include "eventmanager.h"
#include "led.h"
#include "constant.h"
#include "authpic.h"

namespace po = boost::program_options;
using check_system::CameraManager;
using check_system::EventManager;
using check_system::KeyFile;
using check_system::Laser;
using check_system::Lcd;
using check_system::LedController;

bool no_button_flag = false;
bool no_laser_flag = false;
bool no_lcd_flag = false;

class GlobalArg
{
public:
  Laser *laser;
  CameraManager *camera;
  KeyFile *key_file;
  Lcd *lcd;
  EventManager *em;
  LedController *led;

private:
};
GlobalArg *arg;

bool is_running = false;

void InitCmdLine(int argc, char **argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()("help", "")("no-button", "")("no-laser", "")("no-lcd", "");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    exit(1);
  }
  if (vm.count("no-button"))
  {
    no_button_flag = true;
  }
  if (vm.count("no-laser"))
  {
    no_laser_flag = true;
  }
  if (vm.count("no-lcd"))
  {
    no_lcd_flag = true;
  }
}
std::vector<int> empty_pair_list_;
std::vector<int> available_pair_list_;

//插入检测算法
int CheckKeyInsert()
{
  std::srand(std::time(nullptr));
  //  return (int)(std::rand()*48271ll%2147483647);
  int seed = std::rand();
  if (arg->lcd)
    arg->lcd->ShowBySeed(seed);
  arg->camera->GetOnePic();
  return arg->camera->CheckPic(100, 200);
}
int CheckPairStore(int id)
{
  //清空empty_pairs
  std::vector<int>().swap(empty_pair_list_);
  std::vector<int>().swap(available_pair_list_);

  for (int i = 0; i < 1000; i++)
  {
    if (arg->key_file->IsSeedAvailable(id, i))
    {
      available_pair_list_.push_back(i);
    }
    else
    {
      empty_pair_list_.push_back(i);
    }
  }
  return 0;
}

int CheckKey(int key_id)
{
  CheckPairStore(key_id);
  if (available_pair_list_.empty())
  {
    std::cout << "available pair is empty keyid = " << key_id << std::endl;
    return -1;
  }
  int n = 10;
  int available_num = available_pair_list_.size();
  double result;
  while (n--)
  {
    int index = std::rand() % available_pair_list_.size();
    int seed_index = available_pair_list_[index];
    //如果随机到的是已经被删除了的
    if (seed_index == -1)
    {
      int i = index;
      while (i++)
      {

        if (i == available_pair_list_.size())
          i = 0;
        //找到了一个没有被删除的钥匙
        if (available_pair_list_[i] != -1)
        {
          index = i;
          seed_index = available_pair_list_[index];
          break;
        }
        //钥匙已经被删完了
        if (i == index)
        {
          std::cout << "available pair is empty keyid = " << key_id << std::endl;
          return 0;
        }
      }
    }
    int seed = arg->key_file->GetSeed(key_id, seed_index);
    if (seed == -1)
    {
      std::cout << "fatal error seed = -1" << std::endl;
      return -1;
    }
    if (arg->lcd)
      arg->lcd->ShowBySeed(seed);
    arg->key_file->ReadPicAsBmp(key_id, seed_index);
    arg->camera->GetOnePic();
    cv::Mat pic1 = arg->key_file->GetMatImage();

    //将TEMP与Pic进行运算，得出结果值和阈值T进行比较
    result = AuthPic(pic1, arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);

    //删除本次循环使用的Seed文件及其对应的Pic文件
    std::cout << "delete old pair index = " << seed_index << std::endl;
    arg->key_file->DeleteSeed(key_id, seed_index);
    arg->key_file->DeletePic(key_id, seed_index);
    available_pair_list_[index] = -1;

    //值越小说明两张图片越相似
    if (result <= check_system::kAuthThreshold)
    {
      std::cout << "generate new a pair index = " << seed_index << std::endl;
      //认证通过
      //然后重新生成新的激励对
      int rand_seed = std::rand();
      if (arg->lcd)
        arg->lcd->ShowBySeed(rand_seed);
      arg->camera->GetOnePic();

      arg->key_file->CopyPicToBuffer(arg->camera->GetPicBuffer(), CAMERA_WIDTH, CAMERA_HEIGHT);
      arg->key_file->SavePicAndSeed(key_id, seed_index, rand_seed);
      return 1;
    }
    available_num--;
    //用完了库里面所有的钥匙
    if (available_num == 0)
    {
      break;
    }
  }
  return 0;
}

//库定位算法 判断一枚key是否已经建立过数据库了
int FindKey()
{
  std::srand(std::time(nullptr));
  int i = -1;
  double result;
  std::cout << "find key start " << std::endl;
  while (i++ < 100)
  {
    auto begin_tick = std::chrono::steady_clock::now();

    CheckPairStore(i);

    if (available_pair_list_.empty())
      continue;
    std::cout << "key id = " << i << std::endl;
    std::cout << "available pair list = " << available_pair_list_.size() << std::endl;
    std::cout << "empty pair list = " << empty_pair_list_.size() << std::endl;
    int seed_index = std::rand() % available_pair_list_.size();
    int seed = arg->key_file->GetSeed(i, available_pair_list_[seed_index]);

    if (arg->lcd)
      arg->lcd->ShowBySeed(seed);
    arg->key_file->ReadPicAsBmp(i, seed_index);
    int ret = arg->camera->GetOnePic();
    if (ret == -1)
      continue;
    cv::Mat pic1 = arg->key_file->GetMatImage();
    //验证两张图片
    result = AuthPic(pic1, arg->camera->GetPicBuffer(), CAMERA_HEIGHT, CAMERA_WIDTH);
    auto end_tick = std::chrono::steady_clock::now();
    std::cout << "auth pic elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
    if (result <= check_system::kAuthThreshold)
    {
      std::cout << "auth pic ok" << std::endl;
      //找到相应的库
      break;
    }
    else
    {
      std::cout << "auth pic fault" << std::endl;
    }
  }
  if (i >= 100)
    i = -1;

  return i;
}

//认证
int Authentication()
{
  int ret = 0;

  assert(arg->camera != nullptr);

  if (!arg->camera->IsOpen())
  {
    return -1;
  }

  if (CheckKeyInsert() == -1)
  {
    std::cout << "no key insert" << std::endl;
    //认证失败
    return -1;
  }
  std::cout << "key insert" << std::endl;
  auto begin_tick = std::chrono::steady_clock::now();
  int key_id = FindKey();
  auto end_tick = std::chrono::steady_clock::now();
  std::cout << "find key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;
  if (key_id == -1)
  {
    std::cout << "can't find key" << std::endl;
    //认证失败
    return -1;
  }

  begin_tick = std::chrono::steady_clock::now();
  ret = CheckKey(key_id);
  end_tick = std::chrono::steady_clock::now();
  std::cout << "check key elapsed time :" << std::chrono::duration_cast<std::chrono::milliseconds>(end_tick - begin_tick).count() << "ms" << std::endl;

  //认证失败 或者发生错误check key
  if (ret == 0 || ret == -1)
  {
    //红灯 ON没找到成功的对象
    arg->led->ErrorLed(1);
    return -1;
  }
  //  //认证成功了,绿灯1 2 3 ON
  //  arg->led->LaserLed(1);
  //  arg->led->LcdLed(1);
  //  arg->led->CmosLed(1);
  //  arg->led->ErrorLed(0);

  std::cout << "auth success!!!" << std::endl;
  return 0;
}

int main(int argc, char **argv)
{
  arg = new GlobalArg();
  InitCmdLine(argc, argv);
  if (!no_button_flag)
  {
    arg->em = new EventManager();
  }
  if (!no_laser_flag)
  {
    arg->laser = new Laser("/dev/ttyS0");
    arg->laser->ForceCheck();
    arg->laser->ForceOpen();
  }
  if (!no_lcd_flag)
  {
    arg->lcd = new Lcd("/dev/fb0");
  }
  arg->camera = new CameraManager(0);
  arg->camera->Play();


  arg->key_file = new KeyFile("../res/PUFData");

  if (arg->em == nullptr)
  {
    //直接运行
    int ret;
    ret = Authentication();
    if (ret != 0)
    {
      if (arg->led)
        arg->led->ErrorLed(1);
    }
    if (arg->laser)
      arg->laser->ForceClose();
    arg->camera->Pause();
    return 0;
  }
  int fd;
  char key;
  std::stringstream ss;
  //认证
  ss << "/sys/class/gpio/gpio"
     << std::to_string(check_system::kAuthButtonNumber)
     << "/value";
  fd = open(ss.str().c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1)
  {
    std::cout << "can not open :" << check_system::kAuthButtonNumber << std::endl;
    return 0;
  }
  read(fd, &key, 1);
  arg->em->ListenFd(fd, EventManager::kEventPri, [fd]() {
    char key;
    lseek(fd, 0, SEEK_SET);
    read(fd, &key, 1);
    std::cout << "button " << check_system::kAuthButtonNumber << " " << key << std::endl;
    if (key == 0x31)
    {
      int ret = Authentication();
      if (ret != 0)
      {
        if (arg->led)
          arg->led->ErrorLed(1);
      }
    }
  });
  ss.str("");
  arg->em->Start(1);
}