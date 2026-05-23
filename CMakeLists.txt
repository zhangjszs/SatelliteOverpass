cmake_minimum_required(VERSION 3.20)
project(SatelliteOverpass VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------
# SatelliteOverpass Library
# ---------------------------------------------------------------------------
add_library(SatelliteOverpassLib STATIC
    SatelliteOverpass/CoordinateSystem.cpp
    SatelliteOverpass/COORTRAN.CPP
    SatelliteOverpass/CholeskyDecomposition.cpp
    SatelliteOverpass/DateTimeZ.cpp
    SatelliteOverpass/GreenwichSiderealTime.cpp
    SatelliteOverpass/MATRIX.CPP
    SatelliteOverpass/TLE2PosVel.cpp
    SatelliteOverpass/TWOBODY.CPP
)

target_include_directories(SatelliteOverpassLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SatelliteOverpass
    ${CMAKE_CURRENT_SOURCE_DIR}/SatelliteOverpass/include
)

# Compiler definitions
if(MSVC)
    target_compile_definitions(SatelliteOverpassLib PUBLIC
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        _USE_MATH_DEFINES
    )
    # Enable C++20 standard conformance for MSVC
    target_compile_options(SatelliteOverpassLib PRIVATE /permissive- /utf-8)
endif()

# ---------------------------------------------------------------------------
# Modern Predictor Executable
# ---------------------------------------------------------------------------
add_executable(SatelliteOverpassModern
    SatelliteOverpass/SatelliteOverpassModern.cpp
)

target_link_libraries(SatelliteOverpassModern PRIVATE SatelliteOverpassLib)

# ---------------------------------------------------------------------------
# Legacy Predictor Executable
# ---------------------------------------------------------------------------
add_executable(SatelliteOverpassLegacy
    SatelliteOverpass/SatelliteOverpass.cpp
)
target_link_libraries(SatelliteOverpassLegacy PRIVATE SatelliteOverpassLib)

# ---------------------------------------------------------------------------
# Testing (Google Test Integration)
# ---------------------------------------------------------------------------
option(BUILD_TESTING "Build the testing suite" ON)

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows MSVC, prevent override of compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(SatelliteOverpassTests
        SatelliteOverpass/tests/test_main.cpp
        SatelliteOverpass/tests/test_constants.cpp
        SatelliteOverpass/tests/test_coordinates.cpp
        SatelliteOverpass/tests/test_exceptions.cpp
    )

    target_link_libraries(SatelliteOverpassTests PRIVATE
        SatelliteOverpassLib
        gtest
        gtest_main
    )

    target_include_directories(SatelliteOverpassTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SatelliteOverpass
        ${CMAKE_CURRENT_SOURCE_DIR}/SatelliteOverpass/include
    )

    include(GoogleTest)
    gtest_discover_tests(SatelliteOverpassTests)
endif()
