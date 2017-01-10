cmake_minimum_required(VERSION 2.8.11)
project(lab04-05)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

#required CMake options for QT Apps
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#for window widgets
find_package(OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})

#get symlink_data macro
include(${CMAKE_SOURCE_DIR}/cmake_modules/helpers.cmake)

#QT headers requiring MOC (meta object compiler)
set(QTHDRS
  mainwindow.h
  mypanelopengl.h
  )

#window design files
set(QTFORMS mainwindow.ui)

#Run qt tools to auto generate C++ compatible files
QT5_WRAP_CPP(QTHDRS_MOC ${QTHDRS})
QT5_WRAP_UI(QTFORMS_HDRS ${QTFORMS})
SET(MYQTFILES ${QTHDRS_MOC} ${QTFORMS_HDRS})

#normal source files
set(SRC
  main.cpp
  mainwindow.cpp
  mypanelopengl.cpp
)

set(SHADERS
  shaders/vshader.glsl
  shaders/fshader.glsl
)

set(SHDRS sphere.h tub.h square.h walls.h matrixstack.h planet.h printmatrix.h camera.h)
set(SSRC sphere.cpp tub.cpp square.cpp walls.cpp matrixstack.cpp planet.cpp printmatrix.cpp camera.cpp)

add_library(libproj4 ${SHDRS} ${SSRC})

ADD_EXECUTABLE(tubtime ${SRC} ${SHDRS} ${MYQTFILES} ${SHADERS} solarData.txt)
ADD_EXECUTABLE(parseSolarData parseData.cpp)

set(EXTLIBS ${OPENGL_LIBRARY} Qt5::Widgets Qt5::OpenGL)

TARGET_LINK_LIBRARIES(tubtime libproj4 ${EXTLIBS})

symlink_data(proj4_shaders shaders)
symlink_data(proj4_png data)
symlink_data(proj4_data solarData.txt)
