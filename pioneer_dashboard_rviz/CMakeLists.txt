cmake_minimum_required(VERSION 3.8)
project(pioneer_dashboard_rviz)

find_package(ament_cmake REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rviz_common REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)

# IMPORTANT: Only request the OpenCV components we actually use (core + imgproc).
# This avoids pulling in opencv_imgcodecs, which is what tends to drag PNG libs.
find_package(OpenCV REQUIRED COMPONENTS core imgproc)

# Qt5 (your rviz2 is linked to Qt5 on this machine)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(pioneer_dashboard_panel SHARED
  src/dashboard_panel.cpp
  include/pioneer_dashboard_rviz/dashboard_panel.hpp
)

target_include_directories(pioneer_dashboard_panel PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(pioneer_dashboard_panel
  pluginlib
  rclcpp
  rviz_common
  sensor_msgs
  geometry_msgs
  cv_bridge
)

# Link only what we need
target_link_libraries(pioneer_dashboard_panel
    Qt5::Widgets
    opencv_core
    opencv_imgproc
)

install(TARGETS pioneer_dashboard_panel
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

install(FILES rviz_common_plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

pluginlib_export_plugin_description_file(rviz_common rviz_common_plugins.xml)

ament_package()
