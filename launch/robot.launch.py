from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    sensor_node = Node(
        package='nem_yrc_robot',
        executable='robot_sensor_node',
        name='robot_sensor_node',
        output='screen',
        parameters=[{
            'min_range': 0.2,
            'max_range': 4.0,
            'publish_rate_hz': 10.0,
        }],
    )

    controller_node = Node(
        package='nem_yrc_robot',
        executable='robot_controller_node',
        name='robot_controller_node',
        output='screen',
        parameters=[{
            'stop_distance': 0.8,
            'slow_distance': 2.0,
            'max_speed': 1.0,
            'turn_speed': 0.6,
        }],
    )

    return LaunchDescription([sensor_node, controller_node])
