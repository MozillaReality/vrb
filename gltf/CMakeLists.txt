cmake_minimum_required(VERSION 3.4.1)

# add_subdirectory(../src)

include_directories("../include" "../thirdparty")
add_library(
        #library name
        vrb-gltf
        STATIC
        LoaderGLTF.cpp
)

