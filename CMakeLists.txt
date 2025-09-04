cmake_minimum_required(VERSION 3.20)
project(Aperture VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DEMOS "Build demo programs" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
    
    if(ENABLE_COVERAGE)
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# Main library
add_library(aperture_core STATIC
    aperture.cpp
)
target_include_directories(aperture_core 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Demo executables
if(BUILD_DEMOS)
    add_executable(aperture_demo demo.cpp)
    target_link_libraries(aperture_demo PRIVATE aperture_core)
    
    add_executable(secure_computation_demo secure_computation_demo.cpp)
    target_link_libraries(secure_computation_demo PRIVATE aperture_core)
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS aperture_core
    EXPORT ApertureTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES 
    secure_lang.hpp
    test_framework.hpp
    DESTINATION include/aperture
)

# Export targets
install(EXPORT ApertureTargets
    FILE ApertureTargets.cmake
    NAMESPACE Aperture::
    DESTINATION lib/cmake/Aperture
)