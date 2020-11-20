#!/bin/bash

//run vga
echo off > /sys/devices/platform/display/drm/card0/card-HDMI-A-1/status
sleep 1
echo on > /sys/devices/platform/display/drm/card0/card-HDMI-A-1/status
sleep 1
echo off > /sys/devices/platform/display/drm/card0/card-HDMI-A-1/status


