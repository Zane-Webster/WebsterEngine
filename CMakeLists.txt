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
    external/glad/src/gl.c
)

# ===============================
# Include directories
# ===============================
target_include_directories(WebsterEngine PRIVATE
    src
    include

    external/SDL3/include
    external/SDL3_image/include

    external/glad/include
    external/glm
    external/imgui
    external/imgui/backends

    ${ASSIMP_ROOT}/include
    ${ASSIMP_BUILD}/include
    ${ZLIB_ROOT}
)

# ===============================
# Link libraries (MSVC)
# ===============================
target_link_libraries(WebsterEngine PRIVATE
    OpenGL::GL

    ${CMAKE_SOURCE_DIR}/external/SDL3/lib/libSDL3.dll.a
    ${CMAKE_SOURCE_DIR}/external/SDL3_image/lib/libSDL3_image.dll.a

    ${ASSIMP_BUILD}/lib/Release/assimp-vc143-mt.lib
    ${ZLIB_BUILD}/Release/zs.lib
)

# ===============================
# Copy runtime DLLs
# ===============================
add_custom_command(TARGET WebsterEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/external/SDL3/bin/SDL3.dll
        $<TARGET_FILE_DIR:WebsterEngine>
)

add_custom_command(TARGET WebsterEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/external/SDL3_image/bin/SDL3_image.dll
        $<TARGET_FILE_DIR:WebsterEngine>
)

# ===============================
# Warnings
# ===============================
if (MSVC)
    target_compile_options(WebsterEngine PRIVATE
        /W4
        /wd4100   # unreferenced formal parameter
    )
endif()
