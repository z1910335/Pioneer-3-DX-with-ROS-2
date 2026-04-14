#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <rviz_common/panel.hpp>
#include <rviz_common/ros_integration/ros_node_abstraction_iface.hpp>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QTimer>
#include <QImage>

namespace pioneer_dashboard_rviz
{

class LidarWidget : public QWidget
{
  Q_OBJECT
public:
  explicit LidarWidget(QWidget * parent = nullptr);

  void setScan(const sensor_msgs::msg::LaserScan & scan);

protected:
  void paintEvent(QPaintEvent * event) override;

private:
  std::mutex m_;
  float angle_min_{0.0f};
  float angle_inc_{0.0f};
  float range_max_{10.0f};
  std::vector<float> ranges_;
};

class DashboardPanel : public rviz_common::Panel
{
  Q_OBJECT
public:
  explicit DashboardPanel(QWidget * parent = nullptr);
  ~DashboardPanel() override = default;

  void onInitialize() override;

protected:
  // ROS via RViz node abstraction (best practice for RViz plugins)
  std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> node_ptr_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr img_sub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;

  // UI
  QLabel * cam_label_{nullptr};
  LidarWidget * lidar_widget_{nullptr};

  QLineEdit * image_topic_edit_{nullptr};
  QLineEdit * scan_topic_edit_{nullptr};
  QLineEdit * cmdvel_topic_edit_{nullptr};

  QPushButton * apply_topics_btn_{nullptr};

  QCheckBox * enable_teleop_{nullptr};
  QCheckBox * enable_keyboard_{nullptr};

  QPushButton * fwd_btn_{nullptr};
  QPushButton * back_btn_{nullptr};
  QPushButton * left_btn_{nullptr};
  QPushButton * right_btn_{nullptr};
  QPushButton * stop_btn_{nullptr};

  QSlider * lin_slider_{nullptr};
  QSlider * ang_slider_{nullptr};

  QTimer teleop_timer_;

  enum class DriveCmd { STOP, FWD, BACK, LEFT, RIGHT };
  DriveCmd drive_cmd_{DriveCmd::STOP};

  // Thread-safe handoff to GUI thread
  Q_SIGNAL void newCameraFrame(const QImage & img);

  void setupUi();
  void connectSignals();
  void makeOrRemakeRosInterfaces();

  void publishStop();
  void publishFromState();

  // ROS callbacks
  void imageCb(const sensor_msgs::msg::Image::SharedPtr msg);
  void scanCb(const sensor_msgs::msg::LaserScan::SharedPtr msg);

  // Keyboard control (optional)
  void keyPressEvent(QKeyEvent * event) override;
  void keyReleaseEvent(QKeyEvent * event) override;

private Q_SLOTS:
  void onApplyTopics();
  void onTeleopTick();
  void onEnableTeleopChanged(int state);

  void onFwdPressed();   void onFwdReleased();
  void onBackPressed();  void onBackReleased();
  void onLeftPressed();  void onLeftReleased();
  void onRightPressed(); void onRightReleased();
  void onStopClicked();

  void onNewCameraFrame(const QImage & img);
};

}  // namespace pioneer_dashboard_rviz
