project(yomm2_example)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT /home/yoco/libs/boost_1_87_0)

find_package(Boost REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")

add_subdirectory(tests)