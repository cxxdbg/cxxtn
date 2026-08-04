
/// \file cxxdbg_function_name_parser.cpp
/// Contains implementation of the cxxdbg_function_name_parser class.

#include <cxxtn/function_name_parser.hpp>
#include <cxxtn/type_name_parser.hpp>
#include <sstream>
#include <iostream>


namespace cxxtn {


bool parse_function_name(const std::string & name,
                         std::string & name_no_ret_type,
                         std::string & name_no_params) {

    std::istringstream str{name};
    qual_type_name type;
    scope_type_name_sp decl_name;
    parse_declaration(str, type, decl_name);

    if (auto ftype = dynamic_cast<const function_type_name*>(type.type())) {
        // parsed function with
        std::ostringstream res;
        decl_name->write(res, type_name::write_style::gcc);
        name_no_params = res.str();
        ftype->write_params(res, type_name::write_style::gcc);
        name_no_ret_type = res.str();

//        std::cerr << "PARSE '" << name << "' -> ('" << name_no_ret_type
//                  << "', '" << name_no_params << "')\n";

        return true;
    }

    std::cerr << "PARSE ERROR: '" << name << "'\n";

    return false;
}


}
