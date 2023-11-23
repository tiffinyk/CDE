cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME CDE)
project(${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_PREFIX_PATH /opt/Qt5.14.2/5.14.2/gcc_64/lib/cmake) #You should change this directory to the actual Qt-cmake directory.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "Debug")

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options thread filesystem)

QT5_ADD_RESOURCES(ICONS ${CMAKE_SOURCE_DIR}/src/ui/CDE.qrc)

file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*)
add_executable(${PROJECT_NAME} ${HEADERS} ${SRCS} ${ICONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)
