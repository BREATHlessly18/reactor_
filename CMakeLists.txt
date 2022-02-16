cmake_minimum_required(VERSION 3.5)

project(SERVER)

aux_source_directory(${PROJECT_SOURCE_DIR} DIR_MAIN_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/utils DIR_OTHER_SRCS)

# -I
#include_directories(${PROJECT_SOURCE_DIR}/utils)

# parameterss
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} "-g -Wall")

# macro

# -L

# -o
add_executable(reactor_server 
    ${DIR_MAIN_SRCS} 
    ${DIR_OTHER_SRCS}
)

# -l
set(LIB_NAME 
    # avcodec 
    # avdevice 
    # avfilter 
    # avformat 
    # avutil 
    # postproc 
    # swresample 
    # swscale 
    # x264 
    pthread)
target_link_libraries(reactor_server ${LIB_NAME})
