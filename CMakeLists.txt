cmake_minimum_required(VERSION 3.10.0)
project(scppr)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_subdirectory(dep/glm)
add_subdirectory(dep/glfw)

include_directories(src)
include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

include_directories()
link_directories()
link_libraries(glm glfw)

add_executable(scppr_example ${SOURCES})
