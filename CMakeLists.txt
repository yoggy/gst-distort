cmake_minimum_required(VERSION 2.8)

project(gst-distort)

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules (GST_VIDEO REQUIRED gstreamer-video-1.0)
find_package( OpenCV REQUIRED )

# for debug... https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script
#get_cmake_property(_variableNames VARIABLES)
#foreach(_variableName ${_variableNames})
#        message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

add_library(gst-distort SHARED gst-distort.cpp)
include_directories(gst-distort ${GST_INCLUDE_DIRS} ${GST_VIDEO_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(gst-distort ${GST_LIBRARIES} ${GST_VIDEO_LIBRARIES} ${OpenCV_LIBS})

