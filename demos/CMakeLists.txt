cmake_minimum_required(VERSION 3.4.1)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories("../include" "../gltf" ${SDL2_INCLUDE_DIRS})
add_executable (gltfDemo gltfDemo.cpp)
add_executable (objDemo objDemo.cpp)
target_link_libraries (gltfDemo LINK_PUBLIC vrb vrb-gltf ${SDL2_LIBRARIES} OpenGL)
target_link_libraries (objDemo LINK_PUBLIC vrb vrb-gltf ${SDL2_LIBRARIES} OpenGL)
