linux下相机配置步骤:
1. 拷贝90-ckusb.rules到udev脚本目录：
  cp 90-ckusb.rules /etc/udev/rules.d
2. 插入相机，运行tools目录下的相应cpu架构目录下的camera_test程序
  如果程序可以正常打开并输出相机的图像的分辩率大小，说明已经成功获取到了图像。如果打开camera_test有依赖错误和其它错误，
  请确定是否选择正确了cpu架构目录下的camera_test程序。


