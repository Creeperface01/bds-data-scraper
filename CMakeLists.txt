cmake_minimum_required(VERSION 3.13)
project(bds-api)

set(CMAKE_PREFIX_PATH ./sdk)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ModLoader REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

#add_library(bds-api SHARED src/main.cpp ${SOURCES})
add_library(bds-api SHARED src/main.cpp src/json.hpp src/api/WeakPtr.h src/api/SharedCounter.h src/api/SharedPtr.h)

include(cmake/Dependencies.cmake)
#include_directories(${CMAKE_SOURCE_DIR}/v8/include)
#include_directories(${CMAKE_SOURCE_DIR}/v8pp)

#target_link_libraries(bds-api PUBLIC ModLoader bds-symbols ${CMAKE_SOURCE_DIR}/v8/out.gn/lib/obj/libv8_libbase.a ${CMAKE_SOURCE_DIR}/v8/out.gn/lib/obj/libv8_monolith.a)
target_link_libraries(bds-api PUBLIC ModLoader bds-symbols)