cmake_minimum_required(VERSION 3.20)
project(VoxCore)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

find_package(glm REQUIRED)

file(GLOB_RECURSE VOX_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        include/*.h
)

add_library(VoxCore ${VOX_SOURCES})

target_link_libraries(VoxCore
        glm::glm
        dw
        quill
        project_options
)

target_precompile_headers(VoxCore PUBLIC include/VoxCore/Define.h include/VoxCore/Assert.h include/VoxCore/Logger.h)
target_include_directories(VoxCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/ThirdParty/backward)
#TODO: remove debugging options from there
target_compile_definitions(VoxCore PRIVATE BACKWARD_HAS_DW=1)
