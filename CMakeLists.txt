cmake_minimum_required(VERSION 3.20)
project(VoxEngine)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

add_subdirectory(ThirdParty)
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Editor)
