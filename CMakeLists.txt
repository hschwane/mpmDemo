# -------------------------------------------------------------
# general setup
# -------------------------------------------------------------
cmake_minimum_required(VERSION 3.8...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# -------------------------------------------------------------
# dependencies
# -------------------------------------------------------------
enable_language(C)
enable_language(CXX)
find_package(mpUtils REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP)

# -------------------------------------------------------------
# set up project
# -------------------------------------------------------------

# Appends the cmake/ path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${mpUtils_CMAKE_SCRIPTS_PATH} ${CMAKE_MODULE_PATH})

project(mpmDemo)

# default build configuration
include(setDefaultTypeRelease)

# -------------------------------------------------------------
# create target
# -------------------------------------------------------------
add_executable(mpmDemo
        "src/main.cpp"
        "src/mpmSolver2D.cpp"
        )

# -------------------------------------------------------------
# set target properties
# -------------------------------------------------------------

# set required language standard
set_target_properties(mpmDemo PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        )

target_compile_definitions(mpmDemo PRIVATE PROJECT_SHADER_PATH="${CMAKE_CURRENT_LIST_DIR}/shader/")
target_compile_definitions(mpmDemo PRIVATE PROJECT_RESOURCE_PATH="${CMAKE_CURRENT_LIST_DIR}/shader/")

target_compile_definitions(mpmDemo PRIVATE _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)

# set -Wa,-I for resources search path
# target_compile_options(mpmDemo PRIVATE -Wa,-I${CMAKE_SOURCE_DIR})

# -------------------------------------------------------------
# link dependencies (this will also link the dependencies of dependencies and set required compiler flags)
# -------------------------------------------------------------
if(UNIX)
    target_link_libraries(mpmDemo PUBLIC stdc++fs)
endif()

target_link_libraries(mpmDemo PUBLIC Threads::Threads mpUtils::mpUtils)

if(OpenMP_FOUND)
    target_link_libraries(mpmDemo PRIVATE OpenMP::OpenMP_CXX)
endif()
