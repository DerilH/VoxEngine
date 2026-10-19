cmake_minimum_required(VERSION 3.28)
project(VoxEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SPIRV_CROSS_CLI              OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS     OFF CACHE BOOL "" FORCE)

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

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
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
        stb
)

target_include_directories(VoxEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
