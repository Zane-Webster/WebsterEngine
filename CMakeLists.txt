cmake_minimum_required(VERSION 3.21)

project(WebsterEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ===============================
# C++ Standard
# ===============================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===============================
# Default build type
# ===============================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ===============================
# Output directories
# ===============================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ===============================
# Paths
# ===============================
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

set(ASSIMP_ROOT ${EXTERNAL_DIR}/assimp)
set(ASSIMP_BUILD ${ASSIMP_ROOT}/build)

set(ZLIB_ROOT ${EXTERNAL_DIR}/zlib)
set(ZLIB_BUILD ${ZLIB_ROOT}/build)

# ===============================
# OpenGL
# ===============================
find_package(OpenGL REQUIRED)

# ===============================
# SDL3 + SDL3_image
# ===============================
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

# ===============================
# Source files
# ===============================
file(GLOB_RECURSE ENGINE_SRC
    src/*.cpp
    src/*.c
)

# ===============================
# ImGui sources
# ===============================
set(IMGUI_SRC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# ===============================
# Executable
# ===============================
add_executable(WebsterEngine
    ${ENGINE_SRC}
    ${IMGUI_SRC}
    external/glad/src/glad.c
)

# ===============================
# Include directories
# ===============================
target_include_directories(WebsterEngine PRIVATE
    src
    include

    external/glad/include
    external/glm
    external/imgui
    external/imgui/backends

    ${ASSIMP_ROOT}/include
    ${ZLIB_ROOT}
)

# ===============================
# Link libraries
# ===============================
target_link_libraries(WebsterEngine PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    OpenGL::GL

    ${ASSIMP_BUILD}/lib/libassimp.a
    ${ZLIB_BUILD}/libz.dll.a
)

# ===============================
# Windows / MinGW system libs
# ===============================
if (WIN32)
    target_link_libraries(WebsterEngine PRIVATE
        winmm
        imm32
        setupapi
        version
        ws2_32
    )
endif()

# ===============================
# Warnings
# ===============================
if (MINGW)
    target_compile_options(WebsterEngine PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
    )
endif()
