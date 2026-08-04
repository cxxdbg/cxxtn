# Copyright (c) 2026, Alexandr Esilevich
#
# Licensed under the Apache License, Version 2.0.
# See accompanying file LICENSE for license information.
# SPDX-License-Identifier: Apache-2.0
#

# Type name library test
add_executable(tntest
               function_name_parser_test.cpp
               test.cpp
               type_name_lexer_test.cpp
               type_name_parser_test.cpp
               type_name_test.cpp
              )

target_link_libraries(tntest PRIVATE cxxtn Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(tntest PRIVATE rt)
endif()

add_test(NAME tntest COMMAND tntest)
