cmake_minimum_required(VERSION 3.20)
project(VoxEngine)

set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE IMNODES_SOURCES CONFIGURE_DEPENDS
        ../../ThirdParty/imnodes/*.cpp
        ../../ThirdParty/imnodes/backend/*.cpp
)

add_library(imnodes ${IMNODES_SOURCES})

target_link_libraries(imnodes PUBLIC
        imgui
)

target_include_directories(imnodes PUBLIC
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui
)