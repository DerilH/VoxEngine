cmake_minimum_required(VERSION 3.28)
project(VoxEngine)

option(VK_ENABLE_VALIDATION "Enable Vulkan validation layers" ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
            -fmodules
            -fimplicit-modules
            -fimplicit-module-maps
            -fbuiltin-module-map
    )
endif()


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
        project_options
)

target_include_directories(VoxEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(VK_ENABLE_VALIDATION)
    target_compile_definitions(VoxEngine PRIVATE VK_ENABLE_VALIDATION)
endif()