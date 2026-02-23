cmake_minimum_required(VERSION 3.20)
project(VoxEngine)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(assimp REQUIRED)

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        include/*.cpp
        include/*.h
)

add_library(VoxEngine ${VOX_SOURCES})

target_link_libraries(VoxEngine
        Vulkan::Vulkan
        glfw
        shaderc_shared
        dw
        GPUOpen::VulkanMemoryAllocator
        assimp::assimp
        VoxCore
)
target_link_options(VoxEngine PUBLIC -g -rdynamic)

target_include_directories(VoxEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(VoxEngine PRIVATE GLFW_INCLUDE_VULKAN VOX_DEBUG BACKWARD_HAS_DW=1)
