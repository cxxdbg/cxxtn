
/// \file type_name.cpp
/// Contains implementation of type name classes.

#include <cxxtn/type_name.hpp>
#include <cxxtn/type_name_parser.hpp>
#include <cassert>
#include <sstream>


namespace cxxtn {


qual_type_name::qual_type_name(const type_name_sp & t, bool c, bool v):
type_{t},
is_const_{c},
is_volatile_{v} {
}


bool qual_type_name::is_valid() const {
    return type_ ? true : false;
}


type_name * qual_type_name::type() const {
    return type_.get();
}


std::shared_ptr<type_name> qual_type_name::type_sp() const {
    return type_;
}


void qual_type_name::set_type(const type_name_sp & t) {
    type_ = t;
}


bool qual_type_name::is_const() const {
    return is_const_;
}


void qual_type_name::set_const(bool v) {
    is_const_ = v;
}


bool qual_type_name::is_volatile() const {
    return is_volatile_;
}


void qual_type_name::set_volatile(bool v) {
    is_volatile_ = v;
}


void qual_type_name::write_qual(std::ostream & str,
                                bool first_space,
                                bool last_space) const {

    bool first = true;

    if (is_const()) {
        if (first_space) {
            str << ' ';
        }

        str << "const";
        first = false;
    }

    if (is_volatile()) {
        if (!first || first_space) {
            str << ' ';
        }

        str << "volatile";
        first = false;
    }

    if (!first && last_space) {
        str << ' ';
    }
}


void qual_type_name::write(std::ostream & str, type_name_write_style style) const {
    assert(is_valid() && "invalid qual type name");

    write_result(str, style);
    write_prefix(str, false, style);
    write_suffix(str, false, style);
}


void qual_type_name::write_result(std::ostream & str, type_name_write_style style) const {
    assert(is_valid() && "invalid qual type name");

    // write qualifiers before result for array of user defined types in gcc style
    if (style == type_name_write_style::gcc) {
        if (const array_type_name * at = dynamic_cast<const array_type_name*>(type())) {
            const basic_type_name * bt = dynamic_cast<const basic_type_name*>(at->el_type());
            const template_type_name * tt = dynamic_cast<const template_type_name*>(at->el_type());
            if (tt || bt) {
                write_qual(str, false, true);
                at->el_type()->write_result(str, style);
                return;
            }
        }
    }

    // writing qualifiers only for scope or builtin type names
    if (!dynamic_cast<const scope_name*>(type()) &&
        !dynamic_cast<const builtin_type_name*>(type())) {

        type()->write_result(str, style);
        return;
    }

    // qualifiers should be after builtin type name for gcc style
    if (style == type_name_write_style::gcc) {
        if (auto bt = dynamic_cast<const builtin_type_name*>(type())) {
            bt->write_result(str, style);
            write_qual(str, true, false);
            return;
        }
    }

    // qualifiers should be before user defined type names
    write_qual(str, false, true);
    type()->write_result(str, style);
}


void qual_type_name::write_prefix(std::ostream & str, bool end_sp, type_name_write_style style) const {
    assert(is_valid() && "invalid qual type name");
    type()->write_prefix(str, false, style);

    bool last_space = (style == type_name_write_style::clang);

    // write qualifiers after prefix for prefix ops
    if (type()->is_prefix_op()) {
        write_qual(str, true, false);

        if (end_sp) {
            str << ' ';
        }
        return;
    }


    // write qualifiers in prefix for array types
    // except of arrays of user defined types in gcc mode (we print cv for such types in result)

    bool should_write_quals = true;

    const array_type_name * at = dynamic_cast<const array_type_name*>(type());
    if (!at) {
        should_write_quals = false;
    } else {
        if (style == type_name_write_style::gcc) {
            const basic_type_name * bt = dynamic_cast<const basic_type_name*>(at->el_type());
            const template_type_name * tt = dynamic_cast<const template_type_name*>(at->el_type());
            if (tt || bt) {
                should_write_quals = false;
            }
        }
    }

    if (should_write_quals) {
        write_qual(str, true, false);
    }
    
    if (end_sp) {
        str << ' ';
    }
}


void qual_type_name::write_suffix(std::ostream & str, bool parent_op_is_suffix, type_name_write_style style) const {

    // in clang style, if we have array type with qualifiers, then we don't need space before brackets
    bool p_op_is_suffx = parent_op_is_suffix;
    if (style == type_name_write_style::clang &&
        dynamic_cast<const array_type_name*>(type()) != nullptr &&
        (is_const() || is_volatile())) {

        p_op_is_suffx = true;
    }

    assert(is_valid() && "invalid qual type name");
    type()->write_suffix(str, p_op_is_suffx, style);

    if (type()->is_suffix_op() && !dynamic_cast<const array_type_name*>(type()))
        write_qual(str, true, false);
}


type_name::~type_name() {
}


void type_name::write(std::ostream & str, write_style style) const {
    write_result(str, style);
    write_prefix(str, false, style);
    write_suffix(str, false, style);
}


void scope_type_name::write(std::ostream & str, write_style style) const {
    type_name::write(str, style);
}


std::string builtin_type_name::name() const {
    switch (kind()) {
    case kind_t::void_:
        return "void";
    case kind_t::char_:
        return "char";
    case kind_t::short_:
        return "short";
    case kind_t::int_:
        return "int";
    case kind_t::long_:
        return "long int";
    case kind_t::long_long_:
        return "long long int";
    case kind_t::unsigned_char_:
        return "char unsigned";
    case kind_t::unsigned_short_:
        return "short unsigned int";
    case kind_t::unsigned_int_:
        return "unsigned int";
    case kind_t::unsigned_long_:
        return "long unsigned int";
    case kind_t::unsigned_long_long_:
        return "long long unsigned int";
    case kind_t::float_:
        return "float";
    case kind_t::double_:
        return "double'";
    case kind_t::long_double_:
        return "long double";
    case kind_t::wchar_t_:
        return "wchar_t";
    default:
        assert(false && "unknown builtin type name kind");
        return {};
    }
}


void builtin_type_name::write_result(std::ostream & str, write_style style) const {
    str << name();
}


void builtin_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    // doing nothing for builtin type name execpt of writing space
    // if asked
    if (end_sp) {
        str << ' ';
    }
}


bool builtin_type_name::to_unsigned() {
    switch (kind()) {
    case kind_t::char_:
        knd_ = kind_t::unsigned_char_;
        break;
    case kind_t::short_:
        knd_ = kind_t::unsigned_short_;
        break;
    case kind_t::int_:
        knd_ = kind_t::unsigned_int_;
        break;
    case kind_t::long_:
        knd_ = kind_t::unsigned_long_;
        break;
    case kind_t::long_long_:
        knd_ = kind_t::unsigned_long_long_;
        break;
    default:
        return false;
    }

    return true;
}


basic_type_name::basic_type_name(const std::string & ident, const scope_name_sp & sc):
identifier_{ident},
scope_{sc} {
}


const std::string & basic_type_name::identifier() const {
    return identifier_;
}


void basic_type_name::set_identifier(const std::string & v) {
    identifier_ = v;
}


bool basic_type_name::has_scope() const {
    return scope_ ? true : false;
}


const scope_name * basic_type_name::scope() const {
    return scope_.get();
}


void basic_type_name::write(std::ostream & str, write_style style) const {
    type_name::write(str, style);
}


void basic_type_name::write_result(std::ostream & str, write_style style) const {
    if (has_scope()) {
        scope()->write(str, style);
        str << "::";
    }

    str << identifier();
}


void basic_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    // doing nothing for basic type name execpt of writing space
    // if asked
    if (end_sp) {
        str << ' ';
    }
}


void basic_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // doing nothing for basic type name
}


bool basic_type_name::is_prefix_op() const {
    return false;
}


bool basic_type_name::is_suffix_op() const {
    return false;
}


std::shared_ptr<basic_type_name> basic_type_name::make(const std::string & ident,
                                                       const scope_name_sp & sc) {
    return std::shared_ptr<basic_type_name>{new basic_type_name{ident, sc}};
}


array_type_name::array_type_name(const type_name_sp & etype, std::size_t sz):
el_type_{etype},
size_{sz} {
}


const type_name * array_type_name::el_type() const {
    return el_type_.get();
}


type_name * array_type_name::el_type() {
    return el_type_.get();
}


type_name_sp array_type_name::el_type_sp() {
    return el_type_;
}


void array_type_name::set_el_type(const type_name_sp & et) {
    el_type_ = et;
}


void array_type_name::write_size(std::ostream & str) const {
    str << '[';

    if (size() != SIZE_MAX) {
        str << size();
    }

    str << ']';
}


std::size_t array_type_name::size() const {
    return size_;
}


void array_type_name::write_result(std::ostream & str, write_style style) const {
    el_type()->write_result(str, style);
}


void array_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    // writing element prefix
    el_type()->write_prefix(str, end_sp, style);
}


void array_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // write space before array size if parent op did not write suffix
    if (!p_s)
        str << ' ';

    // writing array size
    write_size(str);

    // writing element suffix
    el_type()->write_suffix(str, true, style);
}


bool array_type_name::is_prefix_op() const {
    return false;
}


bool array_type_name::is_suffix_op() const {
    return true;
}


std::shared_ptr<array_type_name>
array_type_name::make(const type_name_sp & el, std::size_t sz) {
    return std::shared_ptr<array_type_name>{new array_type_name{el, sz}};
}


pointer_type_name::pointer_type_name(const qual_type_name & b):
base_{b} {
}


const qual_type_name & pointer_type_name::base() const {
    return base_;
}


qual_type_name & pointer_type_name::base() {
    return base_;
}


void pointer_type_name::set_base(const qual_type_name & t) {
    base_ = t;
}


void pointer_type_name::write_result(std::ostream & str, write_style style) const {
    base().write_result(str, style);
}


void pointer_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    // for clang style, we should write space after prefix before pointer
    bool prefix_end_sp = (style == write_style::clang);

    base().write_prefix(str, prefix_end_sp, style);

    // writing ( parens if child is suffix op
    if (base().type()->is_suffix_op()) {
        if (style == write_style::gcc) {
            str << ' ';
        }

        str << '(';
    }

    str << '*';
}


void pointer_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // writing ) parens if child is suffix op
    bool suf = false;
    if (base().type()->is_suffix_op()) {
        str << ')';
        suf = true;
    }

    base().write_suffix(str, suf, style);
}


bool pointer_type_name::is_prefix_op() const {
    return true;
}


bool pointer_type_name::is_suffix_op() const {
    return false;
}


std::shared_ptr<pointer_type_name> pointer_type_name::make(const qual_type_name & base) {
    return std::shared_ptr<pointer_type_name>{new pointer_type_name{base}};
}


reference_type_name::reference_type_name(const qual_type_name & b):
base_{b} {
}


const qual_type_name & reference_type_name::base() const {
    return base_;
}


qual_type_name & reference_type_name::base() {
    return base_;
}


void reference_type_name::set_base(const qual_type_name & t) {
    base_ = t;
}


void reference_type_name::write_result(std::ostream & str, write_style style) const {
    base().write_result(str, style);
}


void reference_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    // for clang style, we should write space after prefix before reference
    bool prefix_end_sp = (style == write_style::clang);

    base().write_prefix(str, prefix_end_sp, style);

    // writing ( parens if child is suffix op
    if (base().type()->is_suffix_op()) {
        str << '(';
    }

    str << '&';
}


void reference_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // writing ) parens if child is suffix op
    bool suf = false;
    if (base().type()->is_suffix_op()) {
        str << ')';
        suf = true;
    }

    base().write_suffix(str, suf, style);
}


bool reference_type_name::is_prefix_op() const {
    return true;
}


bool reference_type_name::is_suffix_op() const {
    return false;
}


std::shared_ptr<reference_type_name> reference_type_name::make(const qual_type_name & base) {
    return std::shared_ptr<reference_type_name>{new reference_type_name{base}};
}


template_type_name::template_type_name(const basic_type_name & tname):
template_name_{tname} {
}


const basic_type_name & template_type_name::template_name() const {
    return template_name_;
}


const std::vector<qual_type_name> & template_type_name::params() const {
    return params_;
}


std::vector<qual_type_name> & template_type_name::params() {
    return params_;
}


void template_type_name::write(std::ostream & str, write_style style) const {
    return type_name::write(str, style);
}


void template_type_name::write_result(std::ostream & str, write_style style) const {
    template_name().write(str, style);

    str << '<';
    bool first = true;

    // Is last parameter ends with>?
    bool last_rangle = false;

    for (const auto & p : params()) {
        if (!first) {
            str << ", ";
        } else {
            first = false;
        }

        p.write(str, style);

        if (dynamic_cast<const template_type_name*>(p.type())) {
            last_rangle = true;
        } else {
            auto bt = dynamic_cast<const basic_type_name*>(p.type());
            if (bt && !bt->identifier().empty() && bt->identifier().back() == '>') {
                last_rangle = true;
            } else {
                last_rangle = false;
            }
        }
    }

    if (last_rangle) {
        str << ' ';
    }

    str << '>';
}


void template_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
}


void template_type_name::write_suffix(std::ostream & str, bool end_sp, write_style style) const {
}


bool template_type_name::is_prefix_op() const {
    return false;
}


bool template_type_name::is_suffix_op() const {
    return false;
}


std::shared_ptr<template_type_name> template_type_name::make(const basic_type_name & tname) {
    return std::make_shared<template_type_name>(tname);
}


function_type_name::function_type_name(const qual_type_name & r):
ret_{r} {
}


const qual_type_name & function_type_name::ret_type() const {
    return ret_;
}


qual_type_name & function_type_name::ret_type() {
    return ret_;
}


void function_type_name::set_ret_type(const qual_type_name & r) {
    ret_ = r;
}


std::vector<qual_type_name> & function_type_name::params() {
    return params_;
}


const std::vector<qual_type_name> & function_type_name::params() const {
    return params_;
}


void function_type_name::write_result(std::ostream & str, write_style style) const {
    if (ret_type().is_valid()) {
        ret_type().write_result(str, style);
    }
}


void function_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    if (ret_type().is_valid()) {
        ret_type().write_prefix(str, end_sp, style);
    }
}


void function_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // write space before () in clang style if previous op did not write suffix
    if (style == write_style::clang && !p_s) {
        str << ' ';
    }

    write_params(str, style);

    if(ret_type().is_valid()) {
        ret_type().write_suffix(str, true, style);
    }
}


bool function_type_name::is_prefix_op() const {
    return false;
}


bool function_type_name::is_suffix_op() const {
    return true;
}


void function_type_name::write_params(std::ostream & str, write_style style) const {
    str << '(';
    bool first = true;
    for (const auto & p : params()) {
        if (!first) {
            str << ", ";
        } else {
            first = false;
        }

        p.write(str, style);
    }
    str << ')';
}


std::shared_ptr<function_type_name> function_type_name::make(const qual_type_name & r) {
    return std::shared_ptr<function_type_name>{new function_type_name{r}};
}


mem_ptr_type_name::mem_ptr_type_name(const scope_name_sp & b,
                                     const qual_type_name & mtp):
base_{b},
mem_type_{mtp} {
}


const scope_name *mem_ptr_type_name::base() const {
    return base_.get();
}


const qual_type_name & mem_ptr_type_name::mem_type() const {
    return mem_type_;
}


qual_type_name & mem_ptr_type_name::mem_type() {
    return mem_type_;
}


void mem_ptr_type_name::set_mem_type(const qual_type_name & nm) {
    mem_type_ = nm;
}


void mem_ptr_type_name::write_result(std::ostream & str, write_style style) const {
    mem_type().write_result(str, style);
}


void mem_ptr_type_name::write_prefix(std::ostream & str, bool end_sp, write_style style) const {
    mem_type().write_prefix(str, false, style);

    // writing ( parens if child is suffix op
    if (mem_type().type()->is_suffix_op())
        str << " (";
    else
        str << ' ';

    base()->write(str, style);
    str << "::*";
}


void mem_ptr_type_name::write_suffix(std::ostream & str, bool p_s, write_style style) const {
    // writing ) parens if child is suffix op
    bool suf = false;
    if (mem_type().type()->is_suffix_op()) {
        str << ')';
        suf = true;
    }

    mem_type().write_suffix(str, suf, style);
}


bool mem_ptr_type_name::is_prefix_op() const {
    return true;
}


bool mem_ptr_type_name::is_suffix_op() const {
    return false;
}


std::shared_ptr<mem_ptr_type_name> mem_ptr_type_name::make(const scope_name_sp & b,
                                                           const qual_type_name & mtp) {
    return std::shared_ptr<mem_ptr_type_name>{new mem_ptr_type_name{b, mtp}};
}



void decltype_type_name::write_result(std::ostream & str, write_style style) const {
    str << "decltype (" << expr() << ")";
}


void function_decl::write(std::ostream & str, type_name_write_style style) const {
    // writing function type result and preifx
    type_->write_result(str, style);
    type_->write_prefix(str, true, style);

    // writing function name
    name_->write(str, style);

    // writing function type suffix
    type_->write_suffix(str, false, style);
}


bool make_canonical_type_names(const std::string & tname,
                               std::string & gcc_can_name,
                               std::string & clang_can_name) {
    std::istringstream istr{tname};
    qual_type_name tn = parse_type_name(istr);

    if (!tn.is_valid()) {
        return false;
    }

    std::ostringstream ostr_gcc;
    tn.write(ostr_gcc, type_name::write_style::gcc);
    gcc_can_name = ostr_gcc.str();

    std::ostringstream ostr_clang;
    tn.write(ostr_clang, type_name::write_style::clang);
    clang_can_name = ostr_clang.str();

    return true;
}


}
