cmake_minimum_required(VERSION 3.10)
project(tinyrenderer)
set(CMAKE_CXX_STANDARD 17)
add_executable(tinyrenderer main.cpp tgaimage.cpp model.cpp)