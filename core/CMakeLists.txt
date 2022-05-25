cmake_minimum_required(VERSION 3.5)


project(SERVER C CXX)

aux_source_directory(
    ${PROJECT_SOURCE_DIR} 
    DIR_MAIN_SRCS
)

# parameterss
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} "-Wall")

# -o
add_library(
    cppreactor
    SHARED
    ${DIR_MAIN_SRCS}
)

# -l
set(LIB_NAME 
    pthread
)
target_link_libraries(
    cppreactor 
    ${LIB_NAME} 
)


install(TARGETS cppreactor DESTINATION ${PROJECT_SOURCE_DIR}/../install/lib)