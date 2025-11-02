cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(DetectOpenCL C)

#Set the CMake variable "OPENCL_HEADER_DIR" to the path of your OpenCL header directory
include_directories(${OPENCL_HEADER_DIR})
#Set the CMake variable "OPENCL_LIBRARY" to the path of your OpenCL Library
link_libraries(${OPENCL_LIBRARY})

add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)

file(GLOB_RECURSE EXECUTABLE_FILES "Chapters/*.c")
foreach(EXECUTABLE_FILE IN LISTS EXECUTABLE_FILES)
    get_filename_component(EXECUTABLE_NAME ${EXECUTABLE_FILE} NAME_WE)
    message(STATUS "Adding executable: ${EXECUTABLE_NAME} from ${EXECUTABLE_FILE}")
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_FILE})
endforeach()
