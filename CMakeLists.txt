cmake_minimum_required(VERSION 3.0)
project(CG_PROJECT)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(MyGame src/main.cpp)

# Link SFML libraries
target_link_libraries(MyGame sfml-graphics sfml-window sfml-system)