#!/bin/bash

cd `dirname $0`

echo "open vga"
echo off > /sys/class/drm/card0-HDMI-A-1/status
#sleep 1
#echo on > /sys/class/drm/card0-HDMI-A-1/status
#sleep 1
echo on > /sys/class/drm/card0-VGA-1/status
echo "open vga success"


if [ ! -d "/sys/class/gpio/gpio223" ];then
  echo -e -n 223 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio138" ];then
  echo -e -n 138 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio255" ];then
  echo -e -n 255 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio254" ];then
  echo -e -n 254 > /sys/class/gpio/export
fi

#button
if [ ! -d "/sys/class/gpio/gpio257" ];then
  echo -e -n 257 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio252" ];then
  echo -e -n 252 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio218" ];then
  echo -e -n 218 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio251" ];then
  echo -e -n 251 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio164" ];then
  echo -e -n 164 > /sys/class/gpio/export
fi

echo -e -n out > /sys/class/gpio/gpio223/direction 
echo -e -n out > /sys/class/gpio/gpio138/direction 


if test $1 == "1"
then
  ./CalcTwoPic_arm --pic1=../res/pic1.bmp --pic2=../res/pic2.bmp --n=2
fi
if test $1 == "2"
then
  ./OnlyAuth_arm --no-button --no-laser --no-lcd \
    --mid-save=./mid_save \
    --ROI-x=10 --ROI-y=10 --ROI-w=50 --ROI-h=50 \
    --exposion-time=2000 \
    --laser-current=300 
fi
