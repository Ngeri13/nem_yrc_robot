// Szimulált távolságszenzor: sensor_msgs/Range üzeneteket hirdet a /robot/range topicban.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

using namespace std::chrono_literals;

class RobotSensorNode : public rclcpp::Node
{
public:
  RobotSensorNode()
  : Node("robot_sensor_node"),
    t_(0.0),
    gen_(std::random_device{}()),
    noise_(0.0, 0.03)
  {
    this->declare_parameter<double>("min_range", 0.2);
    this->declare_parameter<double>("max_range", 4.0);
    this->declare_parameter<double>("publish_rate_hz", 10.0);

    min_range_ = this->get_parameter("min_range").as_double();
    max_range_ = this->get_parameter("max_range").as_double();
    period_ = 1.0 / this->get_parameter("publish_rate_hz").as_double();

    publisher_ = this->create_publisher<sensor_msgs::msg::Range>("robot/range", 10);
    timer_ = this->create_wall_timer(
      std::chrono::duration<double>(period_),
      std::bind(&RobotSensorNode::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Szenzor node elindult (%.1f Hz)", 1.0 / period_);
  }

private:
  void timer_callback()
  {
    t_ += period_;

    // A robot "ide-oda mozog" egy akadály előtt: szinuszos távolság + zaj
    const double mid = 0.5 * (min_range_ + max_range_);
    const double amplitude = 0.5 * (max_range_ - min_range_);
    double distance = mid + amplitude * std::sin(0.5 * t_) + noise_(gen_);
    distance = std::clamp(distance, min_range_, max_range_);

    sensor_msgs::msg::Range msg;
    msg.header.stamp = this->now();
    msg.header.frame_id = "robot_range_link";
    msg.radiation_type = sensor_msgs::msg::Range::ULTRASOUND;
    msg.field_of_view = 0.26;  // kb. 15 fok
    msg.min_range = static_cast<float>(min_range_);
    msg.max_range = static_cast<float>(max_range_);
    msg.range = static_cast<float>(distance);

    publisher_->publish(msg);
  }

  rclcpp::Publisher<sensor_msgs::msg::Range>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double t_;
  double period_;
  double min_range_;
  double max_range_;
  std::mt19937 gen_;
  std::normal_distribution<double> noise_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotSensorNode>());
  rclcpp::shutdown();
  return 0;
}
