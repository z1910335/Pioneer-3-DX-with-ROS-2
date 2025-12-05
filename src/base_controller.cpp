#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <Aria.h>

class PioneerBaseNode : public rclcpp::Node
{
public:
  explicit PioneerBaseNode(ArRobot* robot)
  : Node("pioneer_base"), robot_(robot)
  {
    using std::placeholders::_1;
    cmd_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10,
      std::bind(&PioneerBaseNode::cmdVelCallback, this, _1));
  }

private:
  void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    if (!robot_->isConnected()) {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 2000,
        "Robot not connected; ignoring cmd_vel");
      return;
    }

    double v_lin = msg->linear.x * 1000.0; // m/s -> mm/s
    double v_rot = msg->angular.z * 180.0 / M_PI; // rad/s -> deg/s

    robot_->lock();
    robot_->setVel(v_lin);
    robot_->setRotVel(v_rot);
    robot_->unlock();
  }

  ArRobot* robot_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_sub_;
};

int main(int argc, char** argv)
{
  // ARIA init and connect
  Aria::init();

  ArRobot robot;
  ArArgumentParser parser(&argc, argv);
  parser.loadDefaultArguments();

  ArRobotConnector robotConnector(&parser, &robot);

  if (!robotConnector.connectRobot()) {
    ArLog::log(ArLog::Terse, "Could not connect to the robot. Check -robotPort etc.");
    return 1;
  }

  ArLog::log(ArLog::Normal, "Connected to robot.");

  // Enable motors and set some safe limits
  robot.lock();
  robot.enableMotors();
  robot.setTransVelMax(300.0); // mm/s (0.3 m/s)
  robot.setRotVelMax(40.0); // deg/s
  robot.unlock();

  robot.runAsync(true); // start ARIA background loop

  // ROS2 init and spin
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PioneerBaseNode>(&robot);
  rclcpp::spin(node);

  // Cleanup
  robot.lock();
  robot.stop();
  robot.disableMotors();
  robot.unlock();
  robot.disconnect();
  Aria::exit(0);
  rclcpp::shutdown();

  return 0;
}
