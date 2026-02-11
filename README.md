# hik_camera_ros2_driver

## 0. 前言

借鉴[北极熊战队开发的海康工业相机ROS2驱动程序](https://github.com/SMBU-PolarBear-Robotics-Team/hik_camera_ros2_driver.git)，在此基础上进行部分改动。改动如下：

- 更改MVS依赖方式，现在需要**在本机下载MVS**使用，而不是将头文件与库文件一并放入功能包，后文将详述如何处理
- 增加gige相机驱动，可以在yaml文件中进行选择，同时可以打开固定ip或usb设备序列号
- 删除``ADC Bit Depth``参数的修改，实测发现大部分海康相机似乎不支持该参数的修改
- 将MVS官方文档复制在``doc``文件夹下，方便后续的同学进行二次开发

## 1. ROS2相关API介绍

The package includes the `hik_camera_node`, which manages the camera and publishes image data along with camera information to ROS 2 topics.

### Subscribed Topics

None.

### Published Topics

- `<camera_topic>` (sensor_msgs/msg/Image)
  - The image data captured by the Hikvision camera.

- `<camera_topic>/camera_info` (sensor_msgs/msg/CameraInfo)
  - Camera calibration information.

### Parameters

- `exposure_time` (double, default: `5000`)
  - The camera exposure time in microseconds.

- `gain` (double, default: `camera`)
  - The gain setting for the camera.

- `acquisition_frame_rate` (double, default: `165`)
  - The acquisition frame rate in hz for the camera.

- `pixel_format` (string, default: `RGB8Packed`)
  - The pixel format for the image data. Supported values: `Mono8`, `Mono10`, `Mono12`, `RGB8Packed`, `BGR8Packed`, `YUV422_YUYV_Packed`, `YUV422Packed`, `BayerRG8`, `BayerRG10`, `BayerRG10Packed`, `BayerRG12`, `BayerRG12Packed`.

- `use_sensor_data_qos` (bool, default: true)
  - Whether to use the `sensor_data` QoS profile for image topic publication.

- `camera_name` (string, default: `camera`)
  - The name of the camera for identification purposes.

- `frame_id` (string, default: `<camera_name>_optical_frame`)
  - The frame_id assigned to the published image data.

- `camera_topic` (string, default: `<camera_name>/image`)
  - The topic name for publishing image and info data.

- `camera_info_url` (string, default: `package://hik_camera_ros2_driver/config/camera_info.yaml`)
  - The URL for the camera calibration information file.

## 2. 使用

### 2.1 安装

#### 2.1.1 安装MVS

[MVS下载中心](https://www.hikrobotics.com/cn/machinevision/service/download/?module=0)

下载linux版本的MVS并安装其中的deb包，**注意与自己使用的cpu架构对应**

MVS会安装在``/opt``目录下， 然后运行下述命令来删除海康的libusb库以防止污染环境

```bash
sudo rm -rf /opt/MVS/lib/64/libusb-1.0.so.0
```

> 可能以后新版本的MVS自带的usb驱动不叫这个名字，记得自己看一下``/opt/MVS/lib/64/``的内容

#### 2.1.1 克隆该仓库

假设你的工作空间为``ros_ws``且位于主文件夹下。

```bash
mkdir -p ~/ros_ws/src && cd ~/ros_ws/src
```

```bash
git clone https://github.com/LemperorD/hik_camera_ros2_driver.git
```

### 2.2 编译

```bash
cd ~/ros_ws
```

```bash
rosdep install -r --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y
```

```bash
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release
```

### 2.3 运行

```bash
ros2 launch hik_camera_ros2_driver hik_camera_launch.py
```
