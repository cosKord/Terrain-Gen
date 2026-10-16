cmake_minimum_required(VERSION 3.20)
project(ProceduralTerrain LANGUAGES CXX)

# Enforce Modern C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependency: Raylib (Automatic FetchContent)
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# Header Search Paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# Project Source Files
set(SOURCES
    src/main.cpp
    src/Heightmap.cpp
    src/BiomeColor.cpp
    src/TerrainRenderer.cpp
)

# Executable Target
add_executable(terrain_gen ${SOURCES})

# Link Libraries
target_link_libraries(terrain_gen PRIVATE raylib)

# Compiler Warning Flags
if (MSVC)
    target_compile_options(terrain_gen PRIVATE /W4 /permissive-)
else()
    target_compile_options(terrain_gen PRIVATE -Wall -Wextra -Wpedantic)
endif()
