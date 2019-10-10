cmake_minimum_required(VERSION 3.4.1)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories("../include" ${SDL2_INCLUDE_DIRS})
add_executable (objDemo objDemo.cpp)
target_link_libraries (objDemo LINK_PUBLIC vrb ${SDL2_LIBRARIES} OpenGL)
