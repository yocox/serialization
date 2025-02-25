project(yomm2_example)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT /home/yoco/libs/boost_1_87_0)

find_package(Boost REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")

add_executable(a.out main.cpp)
target_include_directories(
    a.out PRIVATE
)

target_link_libraries(a.out PRIVATE pthread)
target_include_directories(a.out PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(tests)