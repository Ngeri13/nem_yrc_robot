// Vezérlő node: feliratkozik a /robot/range topicra és a távolság alapján
// sebességparancsot (geometry_msgs/Twist) hirdet a /cmd_vel topicban.
#include <algorithm>
#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

class RobotControllerNode : public rclcpp::Node
{
public:
  RobotControllerNode()
  : Node("robot_controller_node")
  {
    this->declare_parameter<double>("stop_distance", 0.8);
    this->declare_parameter<double>("slow_distance", 2.0);
    this->declare_parameter<double>("max_speed", 1.0);
    this->declare_parameter<double>("turn_speed", 0.6);

    stop_distance_ = this->get_parameter("stop_distance").as_double();
    slow_distance_ = this->get_parameter("slow_distance").as_double();
    max_speed_ = this->get_parameter("max_speed").as_double();
    turn_speed_ = this->get_parameter("turn_speed").as_double();

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    range_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "robot/range", 10,
      std::bind(&RobotControllerNode::range_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Vezérlő node elindult");
  }

private:
  void range_callback(const sensor_msgs::msg::Range::SharedPtr msg)
  {
    geometry_msgs::msg::Twist cmd;
    const char * state = "HALAD";

    if (msg->range < stop_distance_) {
      // Túl közel az akadály: megáll és fordul
      cmd.linear.x = 0.0;
      cmd.angular.z = turn_speed_;
      state = "MEGALL, FORDUL";
    } else if (msg->range < slow_distance_) {
      // Lassítási zóna: a sebesség lineárisan nő a távolsággal
      const double ratio =
        (msg->range - stop_distance_) / (slow_distance_ - stop_distance_);
      cmd.linear.x = max_speed_ * std::clamp(ratio, 0.0, 1.0);
      state = "LASSIT";
    } else {
      cmd.linear.x = max_speed_;
    }

    cmd_pub_->publish(cmd);

    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 1000,
      "tavolsag: %.2f m -> v: %.2f m/s, w: %.2f rad/s [%s]",
      msg->range, cmd.linear.x, cmd.angular.z, state);
  }

  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr range_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  double stop_distance_;
  double slow_distance_;
  double max_speed_;
  double turn_speed_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotControllerNode>());
  rclcpp::shutdown();
  return 0;
}
