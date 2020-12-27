#!/bin/bash

cd `dirname $0`

echo "off vga"
echo off > /sys/class/drm/card0-HDMI-A-1/status
#sleep 1
#echo on > /sys/class/drm/card0-HDMI-A-1/status
#sleep 1
echo "open vga"
echo on > /sys/class/drm/card0-VGA-1/status


if [ ! -d "/sys/class/gpio/gpio223" ];then
  echo -e -n 223 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio139" ];then
  echo -e -n 139 > /sys/class/gpio/export
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
echo -e -n out > /sys/class/gpio/gpio139/direction 
echo -e -n out > /sys/class/gpio/gpio255/direction 
echo -e -n out > /sys/class/gpio/gpio254/direction 
#rs485 ctl
echo -e -n out > /sys/class/gpio/gpio164/direction


echo -e -n in > /sys/class/gpio/gpio257/direction 
echo -e -n in > /sys/class/gpio/gpio252/direction 
echo -e -n in > /sys/class/gpio/gpio218/direction 
echo -e -n in > /sys/class/gpio/gpio251/direction 
echo -e -n both > /sys/class/gpio/gpio257/edge 
echo -e -n both > /sys/class/gpio/gpio252/edge
echo -e -n both > /sys/class/gpio/gpio218/edge
echo -e -n both > /sys/class/gpio/gpio251/edge

echo -e -n 0 > /sys/class/gpio/gpio164/value


if test $1 == "1"
then
  ./CalcTwoPic_arm --pic1=../res/pic1.bmp --pic2=../res/pic2.bmp --n=2
fi
if test $1 == "2"
then
  ./OnlyAuth_arm --no-laser \
    --mid-save=./mid_save \
    --ROI-x=0 --ROI-y=0 --ROI-w=320 --ROI-h=240 \
    --exposion-time=10000
fi
