cmake_minimum_required(VERSION 3.6)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(target gl3cv4jni)
project(${target} CXX)

set(ANDROID_OPENCV_COMPONENTS "opencv_java" CACHE STRING "")
message(STATUS "ANDROID_ABI=${ANDROID_ABI}")
find_package(OpenCV REQUIRED COMPONENTS ${ANDROID_OPENCV_COMPONENTS})
find_library( EGL_LIBRARY EGL "EGL 1.4 library" )
file(GLOB srcs *.cpp *.c)
file(GLOB hdrs *.hpp *.h)

include_directories("./aruco/")

SET(sources
    "./aruco/cameraparameters.cpp"
	"./aruco/debug.cpp"
	"./aruco/dictionary.cpp"
	"./aruco/ippe.cpp"
	"./aruco/markerdetector.cpp" 
	"./aruco/markerlabeler.cpp"
	"./aruco/posetracker.cpp"
    "./aruco/cvdrawingutils.cpp"
	"./aruco/dictionary_based.cpp"
	"./aruco/marker.cpp"
	"./aruco/markerdetector_impl.cpp" 
	"./aruco/markermap.cpp"
	"./aruco/fractaldetector.cpp"
    )
SET(headers
    "./aruco/aruco_cvversioning.h"
	"./aruco/cameraparameters.h"
	"./aruco/dictionary_based.h"
	"./aruco/ippe.h"
	"./aruco/markerdetector_impl.h" 
	"./aruco/markermap.h"
	"./aruco/timers.h"
    "./aruco/aruco_export.h"
	"./aruco/cvdrawingutils.h"
	"./aruco/dictionary.h"
	"./aruco/levmarq.h"
	"./aruco/marker.h" 
	"./aruco/picoflann.h"
    "./aruco/aruco.h"
	"./aruco/debug.h"
	"./aruco/markerdetector.h"
	"./aruco/markerlabeler.h" 
	"./aruco/posetracker.h"
	"./aruco/fractaldetector.h"
    )
 set(fractal_sources
    "./aruco/fractallabelers/fractalposetracker.cpp"
    "./aruco/fractallabelers/fractalmarkerset.cpp"
    "./aruco/fractallabelers/fractalmarker.cpp"
    "./aruco/fractallabelers/fractallabeler.cpp"
    )
set(fractal_headers   
    "./aruco/fractallabelers/fractalposetracker.h"
    "./aruco/fractallabelers/fractalmarkerset.h"
    "./aruco/fractallabelers/fractalmarker.h"
    "./aruco/fractallabelers/fractallabeler.h"
    )

include_directories("${CMAKE_CURRENT_LIST_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/aruco/3rdparty/eigen3)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
add_library(${target} SHARED ${srcs} ${hdrs} ${sources} ${fractal_sources} Shader.cpp Texture.cpp VideoRenderer.cpp ARRenderer.cpp)
target_link_libraries(${target} ${ANDROID_OPENCV_COMPONENTS} ${EGL_LIBRARY} -lGLESv1_CM -lGLESv3)
