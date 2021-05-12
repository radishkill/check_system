## checksystem

### 文件介绍

下列文件为存放在此文档同级目录下

* env: 存放编译环境和运行环境的文件夹
  * 90-ckusb.rules: cmos相关配置文件，需要存放到嵌入式设备的 /etc/udev/rules.d 文件夹下
  * arm.tar.gz: opencv+boost的编译和运行环境,需要解压到嵌入式设备的 /usr/local 文件夹下,必须将内部相应文件夹内容放至目标相应文件夹
  * libCKCameraSDK.so: cmos的动态运行库,需要存放到嵌入式设备的 /usr/lib 文件夹下
  * libstdc++.so.6.0.22: c++动态运行库,需要复制到嵌入式设备的 /usr/lib/arm-linux-gnueabihf 文件夹下,并替换原来的libstdc++.so.6链接指向此文件
```
  rm libstdc++.so.6
  ln -s libstdc++.so.6.0.22 libstdc++.so.6
```
* checksystem.service: 程序的服务配置文件,需要复制到嵌入式设备的 /etc/systemd/system 文件下
* run.sh: 程序启动脚步
* CheckSystem: 主程序
* config： 存放配置文件的文件夹
  * base.cfg： 存放基本配置，可修改
  * camera_cfg.bin: 相机配置文件
  * U3BM200C-SV_Cfg_A.bin: 相机配置文件
* test: 放测试程序代码和运行文件的文件夹
* src: 放主程序代码的文件夹
* doc: 相关文档


### 编译教程

1. 如果没有build文件夹,就在此文档同级目录创建build文件夹
```
mkdir build
```
2. 进入build文件夹
```
cd build
```
3. 编译程序
```
make
```
4. 编译完成后会在此文档同级文件夹下生成CheckSystem主程序  

### 安装教程

1. 先编译完成并生成CheckSystem主程序文件
2. 然后启动minicom程序，连接调试串口线至电脑，上电
3. 此时minicom程序内便能连接到设备的控制系统，并等待系统设备完全启动
4. 然后使用lrz程序，复制相关文件设备的/root/checksystem文件夹下，lrz复制文件教程见<https://blog.csdn.net/xiao628945/article/details/8259063>
5. 启动文件存放目录见前面的文件介绍目录，大文件需要使用内存卡进行传输
6. 最后设置程序自启动
```
systemctl enable checksystem
```

