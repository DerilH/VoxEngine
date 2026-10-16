cmake_minimum_required(VERSION 3.28)
project(VoxCore)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

find_package(glm REQUIRED)

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        include/*.h
)

FetchContent_Declare(
        quill
        GIT_REPOSITORY https://github.com/odygrd/quill.git
        GIT_TAG        v8.2.0
)
FetchContent_MakeAvailable(quill)

add_library(VoxCore ${VOX_SOURCES})

target_link_libraries(VoxCore
        glm::glm
        dw
        quill::quill
        project_options
)

target_precompile_headers(VoxCore PUBLIC include/VoxCore/Define.h include/VoxCore/Assert.h include/VoxCore/Logger.h)
target_include_directories(VoxCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/ThirdParty/backward)
#TODO: remove debugging options from there
target_compile_definitions(VoxCore PRIVATE BACKWARD_HAS_DW=1)
