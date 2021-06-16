# CMAKE Minimum Version
cmake_minimum_required(VERSION 3.17)

# Project name
project("Battery Test")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Automake Qt files and add build dir as include
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the required library packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Clump source files
file(GLOB BATTERY_SRC "include/*.h" "src/*.cpp" "src/*.ui")

# Add the source code to this project's executable.
add_executable("Battery" ${BATTERY_SRC})

# Link the needed libraries along with the executable
target_link_libraries("Battery" PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories("Battery" PRIVATE include ${CMAKE_CURRENT_BINARY_DIR})