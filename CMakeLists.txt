cmake_minimum_required(VERSION 3.8)
project(backlightctl)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(backlightctl backlightctl.cpp)

target_link_libraries(backlightctl ${Boost_LIBRARIES})
