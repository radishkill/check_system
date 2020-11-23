#!/bin/bash
cd `dirname $0`

echo "open vga"
echo off > /sys/class/drm/card0/card0-HDMI-A-1/status
sleep 1
echo on > /sys/class/drm/card0/card0-HDMI-A-1/status
sleep 1
echo off > /sys/class/drm/card0/card0-HDMI-A-1/status
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


if [ ! -d "/sys/class/gpio/gpio107" ];then
  echo -e -n 107 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio171" ];then
  echo -e -n 171 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio98" ];then
  echo -e -n 98 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio165" ];then
  echo -e -n 165 > /sys/class/gpio/export
fi

if [ ! -d "/sys/class/gpio/gpio164" ];then
  echo -e -n 164 > /sys/class/gpio/export
fi

echo -e -n out > /sys/class/gpio/gpio223/direction 
echo -e -n out > /sys/class/gpio/gpio138/direction 
echo -e -n out > /sys/class/gpio/gpio255/direction 
echo -e -n out > /sys/class/gpio/gpio254/direction 
#rs485 ctl
echo -e -n out > /sys/class/gpio/gpio164/direction


echo -e -n in > /sys/class/gpio/gpio107/direction 
echo -e -n in > /sys/class/gpio/gpio171/direction 
echo -e -n in > /sys/class/gpio/gpio98/direction 
echo -e -n in > /sys/class/gpio/gpio165/direction 


echo -e -n both > /sys/class/gpio/gpio107/edge 
echo -e -n both > /sys/class/gpio/gpio171/edge
echo -e -n both > /sys/class/gpio/gpio98/edge
echo -e -n both > /sys/class/gpio/gpio165/edge


echo -e -n 0 > /sys/class/gpio/gpio223/value
echo -e -n 0 > /sys/class/gpio/gpio138/value
echo -e -n 0 > /sys/class/gpio/gpio255/value
echo -e -n 0 > /sys/class/gpio/gpio254/value
echo -e -n 0 > /sys/class/gpio/gpio164/value

export LD_LIBRARY_PATH=/usr/local/lib/opencv2:$LD_LIBRARY_PATH

./CheckSystem
