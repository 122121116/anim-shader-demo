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
    add_subdirectory(${THIRDPARTY_DIR}/glfw)
else()
    message(WARNING "GLFW not found in ${THIRDPARTY_DIR}/glfw")
endif()

# GLM
set(GLM_DIR ${THIRDPARTY_DIR}/glm)
if(EXISTS "${GLM_DIR}/CMakeLists.txt")
    add_subdirectory(${GLM_DIR})
endif()

# Assimp
set(ASSIMP_DIR ${THIRDPARTY_DIR}/assimp)
if(EXISTS "${ASSIMP_DIR}/CMakeLists.txt")
    add_subdirectory(${ASSIMP_DIR})
else()
    message(WARNING "Assimp not found in ${ASSIMP_DIR}. You can add it under thirdparty/assimp or let CMake fetch it.")
    include(FetchContent)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
    )
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
if(EXISTS "${ASSIMP_DIR}/contrib/stb")
    set(STB_INCLUDE_DIR "${ASSIMP_DIR}/contrib/stb")
endif()
include_directories(
    ${INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${THIRDPARTY_DIR}/glfw/include
    ${GLM_DIR}
    ${STB_INCLUDE_DIR}
)

# Source files
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE C_SOURCES ${SRC_DIR}/*.c)

# Executable
add_executable(GraphicsHomework ${C_SOURCES} ${SOURCES} ${IMGUI_SOURCES} )
target_compile_definitions(GraphicsHomework PRIVATE PROJECT_MODEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resource/model")

# Link libraries
# Note: glfw target is created by add_subdirectory(glfw)
if(TARGET glfw)
    target_link_libraries(GraphicsHomework PRIVATE glfw)
endif()
if(TARGET assimp)
    target_link_libraries(GraphicsHomework PRIVATE assimp)
endif()
if(TARGET glm)
    target_link_libraries(GraphicsHomework PRIVATE glm)
elseif(TARGET glm::glm)
    target_link_libraries(GraphicsHomework PRIVATE glm::glm)
endif()

if(WIN32)
    target_link_libraries(GraphicsHomework PRIVATE opengl32)
endif()
add_custom_command(TARGET GraphicsHomework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:GraphicsHomework>/resource"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resource" "$<TARGET_FILE_DIR:GraphicsHomework>/resource"
)
