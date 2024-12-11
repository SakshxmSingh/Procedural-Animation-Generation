cmake_minimum_required(VERSION 3.0)
project(CG_PROJECT)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
# message(STATUS "SFML_INCLUDE_DIRS: ${SFML_INCLUDE_DIRS}")

# # Manually set SFML include directories if not set by find_package
# if(NOT SFML_INCLUDE_DIRS)
#     set(SFML_INCLUDE_DIRS "/opt/homebrew/include")
#     message(STATUS "Manually set SFML_INCLUDE_DIRS: ${SFML_INCLUDE_DIRS}")
# endif()

# Find OpenGL package
find_package(OpenGL REQUIRED)

add_executable(mazeSpider src/main.cpp src/mazegen.hpp src/mazegen.cpp src/spider.hpp src/spider.cpp)
add_executable(stickAnimation src/animation.cpp)


file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})


# Include ImGui folder
# include_directories(${CMAKE_SOURCE_DIR}/imgui)
include_directories(${SFML_INCLUDE_DIRS})

# # Add ImGui library
# add_library(imgui STATIC
#     imgui/imgui.cpp
#     imgui/imgui_draw.cpp
#     imgui/imgui_widgets.cpp
#     imgui/imgui-SFML.cpp
# )

# Link ImGui library
# target_link_libraries(MyGame imgui)

# Link SFML libraries
target_link_libraries(mazeSpider sfml-graphics sfml-window sfml-system)
target_link_libraries(stickAnimation sfml-graphics sfml-window sfml-system)

# Link OpenGL libraries
target_link_libraries(mazeSpider ${OPENGL_LIBRARIES})
target_link_libraries(stickAnimation ${OPENGL_LIBRARIES})