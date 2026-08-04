
/// \file cxxdbg_function_name_parser.hpp
/// Contains definition of cxxdbg_function_name_parser class.

#pragma once

#include <string>


namespace cxxtn {


/// Parses function name and stores its name without ret type and parameters
/// into specified output variables. Returns true on success.
bool parse_function_name(const std::string & name,
                         std::string & name_no_ret_type,
                         std::string & name_no_params);


}
