cmake_minimum_required(VERSION 3.28)
project(teste)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)

add_executable(twist_burst_publisher ros2.cpp)
ament_target_dependencies(twist_burst_publisher rclcpp geometry_msgs sensor_msgs tf2_ros)

install(TARGETS twist_burst_publisher DESTINATION lib/${PROJECT_NAME})

ament_package()
