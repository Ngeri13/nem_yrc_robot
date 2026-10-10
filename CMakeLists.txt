cmake_minimum_required(VERSION 3.8)
project(nem_yrc_robot)

# C++17 (std::clamp miatt)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

set(ament_dependencies
  rclcpp
  std_msgs
  geometry_msgs
  sensor_msgs
)

add_executable(robot_sensor_node src/robot_sensor_node.cpp)
ament_target_dependencies(robot_sensor_node ${ament_dependencies})

add_executable(robot_controller_node src/robot_controller_node.cpp)
ament_target_dependencies(robot_controller_node ${ament_dependencies})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

install(TARGETS
  robot_sensor_node
  robot_controller_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
