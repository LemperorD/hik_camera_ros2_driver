#!/bin/bash

ROOT_PATH=$(cd "$(dirname "$0")";pwd)

export LD_LIBRARY_PATH=${ROOT_PATH}:~/nav2_ws/src/hik_camera_ros2_driver/hikSDK/lib/amd64

if [ "$EUID" -ne 0 ]; then
  echo "not root user" 
else
  echo "Run as root"
  chmod -R 777 ${ROOT_PATH}/Cfg
  chmod -R 777 ${ROOT_PATH}/Temp
fi

exec ${ROOT_PATH}/Firmware_Updater -platform xcb

