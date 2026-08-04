# Copyright (c) 2026, Alexandr Esilevich
#
# Licensed under the Apache License, Version 2.0.
# See accompanying file LICENSE for license information.
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20)
project(cxxtn CXX)

set(CMAKE_CXX_STANDARD 20)
include(CTest)

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

target_include_directories(cxxtn PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if("${BUILD_TESTING}")
    add_subdirectory(src/test)
endif()
