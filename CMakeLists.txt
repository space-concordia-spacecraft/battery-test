# Minimum Version
cmake_minimum_required(VERSION 3.17)

# Project name
project(Zeus)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include(build/include/utils.cmake)

# Required libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)

# Clump source files from src
file(GLOB_RECURSE ZEUS_SRC "include/**.h" "src/**.cpp")

# Create executable from source
add_executable("Zeus" ${ZEUS_SRC})

# Link target
target_link_libraries("Zeus" PUBLIC glfw glad::glad imgui::imgui imguizmo::imguizmo implot::implot glm::glm spdlog::spdlog_header_only)
target_include_directories("Zeus" PUBLIC include ${STB_INCLUDE_DIRS})
target_precompile_headers("Zeus" PUBLIC include/pch.h)