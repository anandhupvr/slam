cmake_minimum_required(VERSION 3.1.0)
project(slam)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/")

# Warnings / Errors
add_compile_options(-std=c++14 -Wall -Wno-unused-function -Wno-unused-variable
  -Wno-unused-but-set-variable -Wno-unused-but-set-variable -Wno-write-strings
  -Wno-deprecated -Wno-deprecated-declarations -Wno-unknown-pragmas)


find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg cv_bridge image_transport)  
include_directories(${catkin_INCLUDE_DIRS}) 
catkin_package()

# Don't follow symlinks when FILE GLOB_RECURSE (and don't warn)
cmake_policy(SET CMP0009 NEW)


# project-wide paths
include_directories(${PROJECT_SOURCE_DIR})

# -- CORE
# add_subdirectory(Core)

# -- GUI
add_subdirectory(gui)


