cmake_minimum_required(VERSION 3.10)
project(GraphicsHomework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# GLFW
# Check if GLFW directory exists, if so add it
if(EXISTS "${THIRDPARTY_DIR}/glfw/CMakeLists.txt")
    # Disable unnecessary GLFW subprojects to allow pruning thirdparty folders
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${THIRDPARTY_DIR}/glfw)
else()
    message(WARNING "GLFW not found in ${THIRDPARTY_DIR}/glfw")
endif()

# Assimp
set(ASSIMP_DIR ${THIRDPARTY_DIR}/assimp)
if(EXISTS "${ASSIMP_DIR}/CMakeLists.txt")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    add_subdirectory(${ASSIMP_DIR})
else()
    message(WARNING "Assimp not found in ${ASSIMP_DIR}. You can add it under thirdparty/assimp or let CMake fetch it.")
    include(FetchContent)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
    )
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(assimp)
endif()

# ImGui
set(IMGUI_DIR ${THIRDPARTY_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Include directories
include_directories(
    ${INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${THIRDPARTY_DIR}/glfw/include
)

# Source files
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)

# Executable
add_executable(GraphicsHomework ${SOURCES} ${IMGUI_SOURCES})

# Link libraries
# Note: glfw target is created by add_subdirectory(glfw)
if(TARGET glfw)
    target_link_libraries(GraphicsHomework PRIVATE glfw)
endif()
if(TARGET assimp)
    target_link_libraries(GraphicsHomework PRIVATE assimp)
endif()

if(WIN32)
    target_link_libraries(GraphicsHomework PRIVATE opengl32)
endif()
