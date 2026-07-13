cmake_minimum_required(VERSION 3.28)
project(teste)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)

add_executable(test_ros2 src/main.cpp)
ament_target_dependencies(test_ros2 rclcpp geometry_msgs sensor_msgs tf2_ros)

install(TARGETS test_ros2 DESTINATION lib/${PROJECT_NAME})

ament_package()
