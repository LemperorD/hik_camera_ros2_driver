#ifndef HIK_ROS_HPP_
#define HIK_ROS_HPP_

#include <string>

#include "MvCameraControl.h"
#include "camera_info_manager/camera_info_manager.hpp"
#include "image_transport/image_transport.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/utilities.hpp"

enum CameraType {
  GIGE_CAMERA, // 使用网口连接相机
  USB_CAMERA // 使用USB连接相机
};

namespace hik_camera_ros2_driver
{

class HikCameraRos2DriverNode : public rclcpp::Node
{
public:
  explicit HikCameraRos2DriverNode(const rclcpp::NodeOptions & options);
  ~HikCameraRos2DriverNode() override;

private:
  bool initializeCamera();
  void declareParameters();
  void startCamera();
  rcl_interfaces::msg::SetParametersResult dynamicParametersCallback(const std::vector<rclcpp::Parameter> & parameters);
  void tryConnectGigE();
  void tryConnectUSB();
  void captureLoop();
  void publishFrame(unsigned char * pData, MV_IMAGE_BASIC_INFO & img_info);

private:
  void *camera_handle_ = nullptr;
  int n_ret_ = MV_OK;
  MV_IMAGE_BASIC_INFO img_info_;
  MV_CC_PIXEL_CONVERT_PARAM convert_param_;

  sensor_msgs::msg::Image image_msg_;
  sensor_msgs::msg::CameraInfo camera_info_msg_;
  image_transport::CameraPublisher camera_pub_;
  std::unique_ptr<camera_info_manager::CameraInfoManager> camera_info_manager_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr params_callback_handle_;

  std::string camera_name_;
  std::string frame_id_;
  std::string camera_topic_;
  CameraType camera_type_;

  std::thread capture_thread_;
  int fail_count_ = 0;

}; // class HikCameraRos2DriverNode

} // hik_camera_ros2_driver

#endif // HIK_ROS_HPP_