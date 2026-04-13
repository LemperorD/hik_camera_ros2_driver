#ifndef HIK_ROS_HPP_
#define HIK_ROS_HPP_

#include <string>
#include <memory>
#include <mutex>
#include <thread>

#include "MvCameraControl.h"
#include "camera_info_manager/camera_info_manager.hpp"
#include "image_transport/image_transport.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/utilities.hpp"
#include "sensor_msgs/msg/imu.hpp"

#include "sync_board/sync_board_macro.hpp"

enum CameraType {
  GIGE_CAMERA = 0, // 使用网口连接相机
  USB_CAMERA // 使用USB连接相机
};

namespace hik_camera_ros2_driver
{

class HikCameraRos2DriverNode : public rclcpp::Node
{
public:
  explicit HikCameraRos2DriverNode(const rclcpp::NodeOptions & options);
  ~HikCameraRos2DriverNode() override;

private: // function
  bool initializeCamera();
  void configureParameters();
  void startCamera();
  void syncReceiveLoop();
  rcl_interfaces::msg::SetParametersResult dynamicParametersCallback(const std::vector<rclcpp::Parameter> & parameters);
  bool tryConnectGigE();
  bool tryConnectUSB();
  void captureLoop();

private: // MVS相关全局变量
  void *camera_handle_ = nullptr;
  int n_ret_ = MV_OK;
  int camera_type_;
  std::string cameraIp_ = ""; // 相机IP地址
  std::string pcIp_ = ""; // 电脑IP地址
  int deviceIndex_ = 0;

  MV_IMAGE_BASIC_INFO img_info_;
  MV_CC_PIXEL_CONVERT_PARAM convert_param_;

  bool trigger_mode_ = 0; // 触发模式，默认为关闭

private: // ROS相关全局变量
  sensor_msgs::msg::Image image_msg_;
  sensor_msgs::msg::CameraInfo camera_info_msg_;
  image_transport::CameraPublisher camera_pub_;
  std::unique_ptr<camera_info_manager::CameraInfoManager> camera_info_manager_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr params_callback_handle_;

  std::string camera_name_;
  std::string frame_id_;
  std::string camera_topic_;

  std::thread capture_thread_;
  std::thread sync_receive_thread_;
  rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
  uint64_t imu_time_us_ = 0;
  double imu_ax_ = 0.0;
  double imu_ay_ = 0.0;
  double imu_az_ = 0.0;
  double imu_gx_ = 0.0;
  double imu_gy_ = 0.0;
  double imu_gz_ = 0.0;
  int fail_count_ = 0;

private: // IP地址解析函数
  inline void parseIp(const std::string& ip, unsigned int& parsedIp) {
    int parts[4];
    sscanf(ip.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]);
    parsedIp = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
  }

#ifdef SYNC_BOARD_MACRO_HPP
private: // 时间同步相关
  std::unique_ptr<SyncClient> client_;
#endif

}; // class HikCameraRos2DriverNode

} // namespace hik_camera_ros2_driver

#endif // HIK_ROS_HPP_