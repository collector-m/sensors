cmake_minimum_required(VERSION 2.8.3)
project(sensors)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

set(${PROJECT_NAME}_CATKIN_DEPS
    pcl_ros
    roscpp
    roslib
    sensor_msgs
    std_msgs
    genmsg
    tf
    dynamic_reconfigure
    diagnostic_updater
    )


find_package(catkin REQUIRED COMPONENTS
    ${${PROJECT_NAME}_CATKIN_DEPS}
    pcl_conversions
    pcl_ros
    roscpp
    rospy
    std_msgs
    genmsg
    cv_bridge
    message_generation
    )
## Generate messages in the 'msg' folder
add_message_files(
    FILES
    rtcm.msg
    gprmc.msg
    )

## Generate added messages and services with any dependencies listed here
generate_messages( DEPENDENCIES std_msgs  # Or other packages containing msgs
    )
catkin_package(
    CATKIN_DEPENDS ${${PROJECT_NAME}_CATKIN_DEPS}
    DEPENDS yaml-cpp python-yaml
    DEPENDENCIES
    CATKIN_DEPENDS message_runtime std_msgs
)

catkin_package(
    #  INCLUDE_DIRS include
    #  LIBRARIES wheelspeed
    #  CATKIN_DEPENDS other_catkin_pkg
    #  DEPENDS system_lib
)


## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
    include/sensors
    include
    ${catkin_INCLUDE_DIRS}
)
find_library(SERIAL  serial)

set (SENSOR src/sensor.cpp include/sensors/sensor.h)

add_executable(gps  src/gps.cpp ${SENSOR} ${SERIAL})
add_dependencies(gps sensors_generate_messages_cpp)
target_link_libraries(gps ${catkin_LIBRARIES} ${SERIAL} )

add_executable(encoder src/encoder.cpp ${SENSOR} ${SERIAL} )
add_dependencies(encoder sensors_generate_messages_cpp)
target_link_libraries(encoder ${catkin_LIBRARIES} ${SERIAL} )

add_executable(nav440 src/nav440.cpp src/memsic_type.cpp include/sensors/memsic_type.h ${SENSOR} ${SERIAL} )
add_dependencies(nav440 sensors_generate_messages_cpp)
target_link_libraries(nav440 ${catkin_LIBRARIES} ${SERIAL} )


add_executable(canbus src/canbus.cpp ${SENSOR} ${SERIAL} )
add_dependencies(canbus sensors_generate_messages_cpp)
target_link_libraries(canbus ${catkin_LIBRARIES} ${SERIAL} )

add_executable(findm src/FindM.cpp include/sensors/qxwz_rtcm.h ${SENSOR} ${SERIAL} )
add_dependencies(findm ${sensors_EXPORTED_TARGETS})
target_link_libraries(findm ${catkin_LIBRARIES} ${SERIAL}  ${CMAKE_CURRENT_SOURCE_DIR}/src/librtcm.so )


# install runtime files
install(TARGETS gps
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    LIBRARY DESTINATION lib
    )
# install runtime files
install(TARGETS encoder
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    LIBRARY DESTINATION lib
    )

# install runtime files
install(TARGETS nav440
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    LIBRARY DESTINATION lib
    )
# install runtime files
install(TARGETS canbus
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    LIBRARY DESTINATION lib
    )

