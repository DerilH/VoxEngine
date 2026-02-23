cmake_minimum_required(VERSION 3.20)
project(VoxEngine)

set(CMAKE_CXX_STANDARD 26)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
)

#list(REMOVE_ITEM VOX_SOURCES
#        ${CMAKE_SOURCE_DIR}/exec/main.cpp
#        ${CMAKE_SOURCE_DIR}/exec/precompileMain.cpp
#)

add_executable(VoxEditor src/exec/main.cpp)

target_sources(VoxEditor PRIVATE ${VOX_SOURCES})

target_link_libraries(VoxEditor
        VoxEngine
        backtrace
        imgui
)

target_compile_definitions(VoxEditor PRIVATE VOX_DEBUG)