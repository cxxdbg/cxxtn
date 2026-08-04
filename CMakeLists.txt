# Copyright (c) 2026, Alexandr Esilevich
#
# Licensed under the Apache License, Version 2.0.
# See accompanying file LICENSE for license information.
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20)
project(cxxtn VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if("${BUILD_TESTING}")
    find_package(Boost 1.74 REQUIRED COMPONENTS unit_test_framework)
endif()

# Type and declaration name parsing and writing library
add_library(cxxtn
            include/cxxtn/function_name_parser.hpp
            include/cxxtn/tn.hpp
            include/cxxtn/type_name.hpp
            include/cxxtn/type_name_lexer.hpp
            include/cxxtn/type_name_parser.hpp
            src/function_name_parser.cpp
            src/type_name.cpp
            src/type_name_lexer.cpp
            src/type_name_parser.cpp
           )
add_library(cxxtn::cxxtn ALIAS cxxtn)

target_include_directories(cxxtn PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if("${BUILD_TESTING}")
    add_subdirectory(src/test)
endif()

# find_package(cxxtn) support for consumers outside this repo tree.
install(TARGETS cxxtn
        EXPORT cxxtnTargets
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(DIRECTORY include/cxxtn DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

set(cxxtn_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/cxxtn")

install(EXPORT cxxtnTargets
        FILE cxxtnTargets.cmake
        NAMESPACE cxxtn::
        DESTINATION "${cxxtn_install_cmakedir}")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxxtnConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cxxtnConfig.cmake"
    INSTALL_DESTINATION "${cxxtn_install_cmakedir}")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cxxtnConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cxxtnConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxxtnConfigVersion.cmake"
        DESTINATION "${cxxtn_install_cmakedir}")
