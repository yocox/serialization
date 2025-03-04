find_package(GTest REQUIRED)

add_executable(tests
    test_arithmetic.cpp
    test_pointer.cpp
    test_string.cpp
    test_struct.cpp
    test_visit_member.cpp
)

target_link_libraries(tests PRIVATE
    GTest::GTest
    GTest::Main
)

target_include_directories(tests PRIVATE
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
