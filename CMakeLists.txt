cmake_minimum_required(VERSION 3.10.0)
project(scppr)

file(GLOB_RECURSE LIB_SOURCES "src/lib/*.cpp" "src/lib/*.c")
file(GLOB_RECURSE EX01_SOURCES "src/example/01/*.cpp")
file(GLOB_RECURSE EX02_SOURCES "src/example/02/*.cpp")

add_subdirectory(dep/glm)
add_subdirectory(dep/glfw)

include_directories(src)
include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

include_directories()
link_directories()
link_libraries(glm glfw)

add_library(scppr STATIC ${LIB_SOURCES})

add_executable(scppr_example01 ${EX01_SOURCES})
target_link_libraries(scppr_example01 scppr)

add_executable(scppr_example02 ${EX02_SOURCES})
target_link_libraries(scppr_example02 scppr)
