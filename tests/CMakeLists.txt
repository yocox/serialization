find_package(GTest REQUIRED)

add_executable(tests
    arithmetic.cpp
    pointer.cpp
    string.cpp
    struct.cpp
    visit_member.cpp
)

target_link_libraries(tests PRIVATE
    GTest::GTest
    GTest::Main
)

target_include_directories(tests PRIVATE
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
