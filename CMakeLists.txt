cmake_minimum_required(VERSION 2.8)
project(dbscan-gps)

add_definitions(-Wall -Wextra -Wno-missing-field-initializers -Wno-psabi)
set(CMAKE_CXX_FLAGS "-std=c++0x")

include_directories(libnmea)
add_subdirectory(libnmea)

add_executable(dbscan-gps main.cpp)
target_link_libraries(dbscan-gps nmea)

install(TARGETS dbscan-gps RUNTIME DESTINATION .)
