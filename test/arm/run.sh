#!/bin/bash

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
