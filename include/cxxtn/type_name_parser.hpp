
/// \file type_name_parser.hpp
/// Contains definition of functions for parsing type names

#pragma once

#include "type_name.hpp"
#include <istream>


namespace cxxtn {


/// Parses qualified type name from input stream
qual_type_name parse_type_name(std::istream & str, std::string * err = nullptr);


/// Parses declaration from input stream
void parse_declaration(std::istream & str,
                       qual_type_name & type,
                       scope_type_name_sp & decl_name);


}
