cmake_minimum_required(VERSION 3.27)

# set the project name and version
project(OpenGLtest VERSION 1.0)
set(glfw_version 3.3.8)
set(vkfw_timestamp 21-1-21)
set(glm_version 0.9.9.8)

# Gl3w fix
execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

# specify the C++ standard
# INCBIN currently is broken on MSVC - this will be replaced once c23 is properly implemented by them
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# setting up variables
set(glfw_name "glfw-${glfw_version}")
set(vkfw_name "vkfw@${vkfw_timestamp}")
set(glm_name "glm-${glm_version}")
find_package(Vulkan REQUIRED)

# building

# Adding CMake sublibs, disable tests etc
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("libs/${glfw_name}")
add_subdirectory("libs/gl3w")
add_subdirectory("libs/${glm_name}")


add_executable(OPGL "src/main.cpp" "src/render/render.cpp" "src/util/stbinit.cpp")
target_include_directories(OPGL PUBLIC 
    # TODO: is including subdirectories required
    "libs/${glfw_name}/include"
    "libs/${vkfw_name}/include"
    "libs/gl3w/include"
    "libs/${glm_name}"
    "libs/incbin"
    "libs/stb"
    "src/incbin"
    ${Vulkan_INCLUDE_DIRS}
)
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()
target_link_libraries(OPGL PUBLIC opengl32 gl3w glfw ${GLFW_LIBRARIES})