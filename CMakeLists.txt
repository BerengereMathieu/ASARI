cmake_minimum_required(VERSION 2.8)
project(ASARI)

file(

        GLOB_RECURSE

        src_files

        src/*.cpp



)


file(

        GLOB_RECURSE

        header_files

        include/*.h

)

set(CMAKE_CXX_FLAGS "-v -std=c++11")
#add includes directories
include_directories(  "./include" )

add_executable(${PROJECT_NAME} ${src_files} ${header_files})

