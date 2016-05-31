find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

include_directories("${CMAKE_SOURCE_DIR}/extern/include/")
link_directories("${CMAKE_SOURCE_DIR}/extern/lib/")

add_executable(osm-track editable_track.c)
target_link_libraries(osm-track ${GTK3_LIBRARIES} osmgpsmap-1.0 cairo)
