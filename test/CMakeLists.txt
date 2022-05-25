cmake_minimum_required(VERSION 3.5)

project(TEST_MAIN  C CXX)

set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} "-Ofast -Wall")

include_directories(${PROJECT_SOURCE_DIR}/../install)
link_directories(${PROJECT_SOURCE_DIR}/../install/lib)


add_executable(
    test
    test.cc
)

# -l
set(LIB_NAME 
    cppreactor
)
target_link_libraries(
    test 
    ${LIB_NAME} 
)


