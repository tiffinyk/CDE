cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME llvm-cdfg)
project(${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_BUILD_TYPE "Release")

set(LLVM_INCLUDE_DIRS "/opt/llvm/include") #You should modify this directory to the actual LLVM include directory.
set(LLVM_LIBRARY_DIRS "/opt/llvm/lib") #You should modify this directory to the actual LLVM library directory.
#find_package(LLVM 10 REQUIRED)
# add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

#include_directories(
#  "/usr/include/llvm-10"
#  "/usr/include/llvm-c-10"
#)

#link_directories(
#  "/usr/lib/llvm-10/lib"
#)

add_subdirectory(llvm-pass)  
