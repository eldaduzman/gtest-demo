cmake_minimum_required(VERSION 3.10.2)

project(MyProject)
include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_subdirectory(modules)
add_subdirectory(tests)
add_executable(${PROJECT_NAME} mainapp.c)
target_link_libraries(${PROJECT_NAME} calculations)
