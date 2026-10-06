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

FetchContent_Declare(
        spirv_cross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
        GIT_TAG main
)
FetchContent_MakeAvailable(spirv_cross)

FetchContent_Declare(
        yyjson
        GIT_REPOSITORY https://github.com/ibireme/yyjson.git
        GIT_TAG        0.10.0
)
FetchContent_MakeAvailable(yyjson)

include(FetchContent)
FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(VoxEngine ${VOX_SOURCES})

target_link_libraries(VoxEngine
        PUBLIC
        spirv-cross-core
        yyjson
        magic_enum
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