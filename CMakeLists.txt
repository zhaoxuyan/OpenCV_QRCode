cmake_minimum_required(VERSION 3.8)
project(openCV_QRCode)

set(CMAKE_CXX_STANDARD 11)

#find_library(OpenCV)
find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

add_executable(openCV_QRCode main.cpp QRCode_Locate.cpp)
target_link_libraries(openCV_QRCode ${OpenCV_LIBS} ${raspicam_CV_LIBS} ${ZBAR_LIBRARIES} zbar)