# Code by Caleb Biasco - biasc007
# for CSCI 5607, Fall Semester 2016

# This is the CMake instructional file for my ray tracing program.
# Simply run "cmake .." in the build directory, then follow the appropriate
# build steps to build the program in your platform of choice.
# 
# The executable will be named 'raytracer', and it takes one argument.
# The output file of the program will be in the same directory as the
# input file.

cmake_minimum_required(VERSION 2.6)

project (raytracer)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

set (MY_SOURCES
	src/main.cpp
	src/Color.h
	src/Color.cpp
	src/Ellipsoid.h
	src/Ellipsoid.cpp
	src/Object.h
	src/Object.cpp
	src/Parser.h
	src/Parser.cpp
	src/Sphere.h
	src/Sphere.cpp
	src/Vec3.h
	src/Vec3.cpp
)

add_executable(raytracer ${MY_SOURCES})

set_property(TARGET raytracer PROPERTY FOLDER "executables")

install (TARGETS raytracer
	 RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)