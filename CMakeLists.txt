cmake_minimum_required(VERSION 2.8.11)
project(rl)

include(CMakeFiles/CMakeLibraryCheck)
include(CMakeFiles/CMakePlatformCheck)
include(CMakeFiles/CMakeGenerateMainHeader)

set(intrinsic "none" CACHE STRING "The possible intrinsics are the following: none, mmx, sse, sse2, sse3, sse4, sse4.1, sse4.2, sse4a, avx.")

# Default c++ flags.
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0 -Wall --pedantic ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-DDEBUG")
endif()

if (ENABLE_DB STREQUAL "true")
    MESSAGE(STATUS "DB Enabled: ${ENABLE_DB}")
    add_definitions("-DENABLE_DB")
    set(ENABLE_DB "true")
endif()

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(test)

# This is where the sub-headers will be installed in /usr/local/include directory.
set(SUB_HEADERS_PREFIX rl_bits)

# Include all header file to just one.
GenerateMainHeader(
        ${CMAKE_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${SUB_HEADERS_PREFIX}
        ${CMAKE_SOURCE_DIR}/lib)
MESSAGE(STATUS "Generated global header file")

set(INSTALL_DIRECTORY /usr/local/include)
install(FILES ${PROJECT_BINARY_DIR}/rl
        DESTINATION ${INSTALL_DIRECTORY})
install(DIRECTORY ${PROJECT_BINARY_DIR}/${SUB_HEADERS_PREFIX}/
        DESTINATION ${INSTALL_DIRECTORY}/${SUB_HEADERS_PREFIX})
