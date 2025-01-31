cmake_minimum_required(VERSION 2.8.3)
project(vizzy_navigation)

set(CMAKE_BUILD_TYPE Release)
add_compile_options(-std=c++11)

find_package(catkin REQUIRED
  COMPONENTS
    roscpp
    sensor_msgs
    vizzy_msgs
    geometry_msgs
    nav2d_operator
    dynamic_reconfigure
    tf2
    interactive_markers
    visualization_msgs
    eigen_conversions
    rospy
    actionlib_msgs)

  #add dynamic reconfigure api
 generate_dynamic_reconfigure_options(
  cfg/Docking.cfg
 )

find_package(yaml-cpp)
find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})

find_package(Boost)  
if (${Boost_FOUND})
  message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
  message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif(${Boost_FOUND})

find_package(Eigen3 REQUIRED)
if (${EIGEN3_FOUND})
  include_directories(${EIGEN3_INCLUDE_DIR})
  message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
else(${EIGEN3_FOUND})
  message(WARNING "Cannot find Eigen3 Library")
endif(${EIGEN3_FOUND})


#add_action_files(
#  DIRECTORY action
#  FILES Charge.action
#)

generate_messages(
  DEPENDENCIES actionlib_msgs std_msgs geometry_msgs  # Or other packages containing msgs
)


catkin_package(
  INCLUDE_DIRS include
  LIBRARIES falkolib
  CATKIN_DEPENDS
    sensor_msgs
    geometry_msgs
    roscpp
    dynamic_reconfigure
    actionlib_msgs
    std_msgs
    geometry_msgs
    rospy
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  falkolib/include
  falkolib/EXTERNAL
)

## Declare a C++ library
add_library(falkolib
falkolib/src/Common/HoughSpectrum.cpp
falkolib/src/Feature/FALKOExtractor.cpp
falkolib/src/Feature/OCExtractor.cpp
falkolib/src/Feature/CGH.cpp
falkolib/src/Feature/BSC.cpp
falkolib/EXTERNAL/mcqd/mcqd.cpp
)

## Declare a cpp executable
add_executable(teleop_joy_node src/teleop_joy.cpp)
target_link_libraries(teleop_joy_node
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
)

add_executable(reroute_twist2nav2d_node src/twist2nav2d.cpp)
target_link_libraries(reroute_twist2nav2d_node
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
)
add_dependencies(reroute_twist2nav2d_node vizzy_msgs_generate_messages_cpp vizzy_navigation_generate_messages_cpp ${PROJECT_NAME}_gencfg ${vizzy_navigation_EXPORTED_TARGETS})

add_executable(docking_controller src/docking_controller.cpp src/docking_controller_ros.cpp src/docking_controller_node.cpp)
target_link_libraries(docking_controller
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  falkolib
)
add_dependencies(docking_controller ${PROJECT_NAME}_gencfg ${vizzy_navigation_EXPORTED_TARGETS})


add_executable(docking_learner src/docking_learner.cpp src/docking_learner_node.cpp src/falko_database.cpp)
target_link_libraries(docking_learner
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  falkolib
  yaml-cpp
)

add_executable(docking_estimator src/docking_estimator.cpp src/docking_estimator_node.cpp src/falko_database.cpp src/pattern_pose_estimation.cpp)
target_link_libraries(docking_estimator
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  ${PCL_LIBRARIES}
  falkolib
  yaml-cpp
)

add_executable(charging_action_server src/charging_action_server.cpp src/charging_action_server_node.cpp src/docking_estimator.cpp src/falko_database.cpp src/pattern_pose_estimation.cpp src/docking_controller.cpp src/docking_controller_ros.cpp)
target_link_libraries(charging_action_server
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  ${PCL_LIBRARIES}
  falkolib
  yaml-cpp
)

foreach(dir config launch rviz)
  install(DIRECTORY ${dir}/ 
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${dir})
endforeach()

if (CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)
  roslaunch_add_file_check(launch/amcl.launch)
  roslaunch_add_file_check(launch/gmapping.launch)
  roslaunch_add_file_check(launch/move_base_eband_local.launch)
  roslaunch_add_file_check(launch/move_base_eband_remote.launch)
  roslaunch_add_file_check(launch/rviz.launch)
  ## launch/navigation.launch not tested until
  ## ros-drivers/openni_launch#10 is fixed.
endif ()
