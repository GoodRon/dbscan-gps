cmake_minimum_required(VERSION 2.8)
project(dbscan-gps)

add_definitions(-Wall -Wextra -Wno-missing-field-initializers -Wno-psabi)
set(CMAKE_CXX_FLAGS "-std=c++0x")

include_directories(libnmea)
add_subdirectory(libnmea)

add_subdirectory(test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

include_directories("${CMAKE_SOURCE_DIR}/extern/include/")
link_directories("${CMAKE_SOURCE_DIR}/extern/lib/")

add_executable(dbscan-gps main.cpp dbscan-gps.cpp filtersFunctions.cpp GpsFilter.cxx FastFilter.cxx)
target_link_libraries(dbscan-gps ${GTK3_LIBRARIES} nmea osmgpsmap-1.0 cairo)

install(TARGETS dbscan-gps RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data DESTINATION . USE_SOURCE_PERMISSIONS)