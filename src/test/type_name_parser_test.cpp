
/// \file type_name_parser_test.cpp
/// Contains unit tests for type name parser.

#include <cxxtn/type_name_parser.hpp>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <iostream>


namespace cxxtn::test {


BOOST_AUTO_TEST_SUITE(type_name_parser_test)


/// Tests parsing basic type name
BOOST_AUTO_TEST_CASE(basic_type_name_parse) {
    std::istringstream str{"int"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing basic type name with scope
BOOST_AUTO_TEST_CASE(basic_type_name_parse_scope) {
    std::istringstream str{"aaa::cls"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);

    BOOST_CHECK(bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");

    const basic_type_name * sc = dynamic_cast<const basic_type_name*>(bt->scope());
    BOOST_REQUIRE(sc);
    BOOST_CHECK(!sc->has_scope());
    BOOST_CHECK(sc->identifier() == "aaa");
}


/// Tests parsing qualified type name
BOOST_AUTO_TEST_CASE(qualified_type_name_parse) {
    std::istringstream str{"const cls volatile"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(t.is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);

    BOOST_CHECK(!bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");
}


/// Tests parsing array type name with no size
BOOST_AUTO_TEST_CASE(array_type_name_parse_no_size) {
    std::istringstream str{"int []"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == SIZE_MAX);

    auto et = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(et->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing array type name with size
BOOST_AUTO_TEST_CASE(array_type_name_parse_size) {
    std::istringstream str{"int [10]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 10);

    auto et = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(et->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing qualified array of user defined type
BOOST_AUTO_TEST_CASE(array_type_name_parse_qual_user) {
    std::istringstream str{"const cls [10]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 10);

    const basic_type_name * et = dynamic_cast<const basic_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(!et->has_scope());
    BOOST_CHECK(et->identifier() == "cls");
}


/// Tests parsing qualified array of builtin type
BOOST_AUTO_TEST_CASE(array_type_name_parse_qual_builtin) {
    std::istringstream str{"int const [10]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 10);

    auto et = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(et->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer type
BOOST_AUTO_TEST_CASE(pointer_type_name_parse) {
    std::istringstream str{"int*"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(!pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(pt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to const type
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_const) {
    std::istringstream str{"int const*"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(pt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to const user defined type
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_const_user) {
    std::istringstream str{"const cls*"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(pt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(!bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");
}


/// Tests parsing const pointer
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_pconst) {
    std::istringstream str{"int* const"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(!pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(pt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing array of pointers
BOOST_AUTO_TEST_CASE(array_type_name_parse_pointer) {
    std::istringstream str{"int* [20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    const pointer_type_name * et = dynamic_cast<const pointer_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(!et->base().is_const());
    BOOST_CHECK(!et->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(et->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing const array of pointers
BOOST_AUTO_TEST_CASE(array_type_name_parse_const_pointer) {
    std::istringstream str{"int* const [20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    const pointer_type_name * et = dynamic_cast<const pointer_type_name*>(at->el_type());
    BOOST_REQUIRE(et);
    BOOST_CHECK(!et->base().is_const());
    BOOST_CHECK(!et->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(et->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to array
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_array) {
    std::istringstream str{"int (*)[20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(!pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(pt->base().type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    auto bt = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to const array
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_const_array) {
    std::istringstream str{"int const (*)[20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(pt->base().type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    auto bt = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to const array of user defined type
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_const_array_user) {
    std::istringstream str{"const cls (*)[20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(pt->base().type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(at->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(!bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");
}


/// Tests parsing const pointer to array
BOOST_AUTO_TEST_CASE(pointer_type_name_parse_pconst_array) {
    std::istringstream str{"int (* const)[20]"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const pointer_type_name * pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(!pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    const array_type_name * at = dynamic_cast<const array_type_name*>(pt->base().type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    auto bt = dynamic_cast<const builtin_type_name*>(at->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing reference
BOOST_AUTO_TEST_CASE(reference_type_name_parse) {
    std::istringstream str{"int&"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const reference_type_name * rt = dynamic_cast<const reference_type_name*>(t.type());
    BOOST_REQUIRE(rt);
    BOOST_REQUIRE(rt->base().is_valid());
    BOOST_CHECK(!rt->base().is_const());
    BOOST_CHECK(!rt->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(rt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing reference to const builtin type
BOOST_AUTO_TEST_CASE(reference_type_name_parse_const) {
    std::istringstream str{"int const&"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const reference_type_name * rt = dynamic_cast<const reference_type_name*>(t.type());
    BOOST_REQUIRE(rt);
    BOOST_REQUIRE(rt->base().is_valid());
    BOOST_CHECK(rt->base().is_const());
    BOOST_CHECK(!rt->base().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(rt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing reference to const user defined type
BOOST_AUTO_TEST_CASE(reference_type_name_parse_const_user) {
    std::istringstream str{"const cls&"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const reference_type_name * rt = dynamic_cast<const reference_type_name*>(t.type());
    BOOST_REQUIRE(rt);
    BOOST_REQUIRE(rt->base().is_valid());
    BOOST_CHECK(rt->base().is_const());
    BOOST_CHECK(!rt->base().is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(rt->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(!bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");
}


/// Tests parsing template
BOOST_AUTO_TEST_CASE(template_type_name_parse) {
    std::istringstream str{"std::basic_string<char>"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const template_type_name * tt = dynamic_cast<const template_type_name*>(t.type());
    BOOST_REQUIRE(tt);

    BOOST_REQUIRE(tt->params().size() == 1);
    const qual_type_name & par = tt->params()[0];
    BOOST_REQUIRE(par.is_valid());
    BOOST_CHECK(!par.is_const());
    BOOST_CHECK(!par.is_volatile());

    auto par_t = dynamic_cast<const builtin_type_name*>(par.type());
    BOOST_REQUIRE(par_t);
    BOOST_CHECK(par_t->kind() == builtin_type_name::kind_t::char_);

    BOOST_CHECK(tt->template_name().identifier() == "basic_string");
    BOOST_REQUIRE(tt->template_name().has_scope());

    const basic_type_name * scope = dynamic_cast<const basic_type_name*>(tt->template_name().scope());
    BOOST_REQUIRE(scope);
    BOOST_CHECK(!scope->has_scope());
    BOOST_CHECK(scope->identifier() == "std");
}


/// Tests parsing template scope
BOOST_AUTO_TEST_CASE(template_type_name_parse_scope) {
    std::istringstream str{"xx::cls<int, const float>::my_typedef"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->identifier() == "my_typedef");

    BOOST_REQUIRE(bt->has_scope());
    const template_type_name * tt = dynamic_cast<const template_type_name*>(bt->scope());
    BOOST_REQUIRE(tt);

    BOOST_REQUIRE(tt->params().size() == 2);
    const qual_type_name & par = tt->params()[0];
    BOOST_REQUIRE(par.is_valid());
    BOOST_CHECK(!par.is_const());
    BOOST_CHECK(!par.is_volatile());

    auto par_t = dynamic_cast<const builtin_type_name*>(par.type());
    BOOST_REQUIRE(par_t);
    BOOST_CHECK(par_t->kind() == builtin_type_name::kind_t::int_);

    const qual_type_name & par2 = tt->params()[1];
    BOOST_REQUIRE(par2.is_valid());
    BOOST_CHECK(par2.is_const());
    BOOST_CHECK(!par2.is_volatile());

    auto par2_t = dynamic_cast<const builtin_type_name*>(par2.type());
    BOOST_REQUIRE(par2_t);
    BOOST_CHECK(par2_t->kind() == builtin_type_name::kind_t::float_);

    BOOST_CHECK(tt->template_name().identifier() == "cls");
    BOOST_REQUIRE(tt->template_name().has_scope());

    const basic_type_name * scope = dynamic_cast<const basic_type_name*>(tt->template_name().scope());
    BOOST_REQUIRE(scope);
    BOOST_CHECK(!scope->has_scope());
    BOOST_CHECK(scope->identifier() == "xx");
}


/// Tests parsing nested template
BOOST_AUTO_TEST_CASE(template_type_name_parse_nested) {
    std::istringstream str{"cls<xx::cls2<float>>"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const template_type_name * tt = dynamic_cast<const template_type_name*>(t.type());
    BOOST_REQUIRE(tt);
    BOOST_CHECK(!tt->template_name().has_scope());
    BOOST_CHECK(tt->template_name().identifier() == "cls");

    BOOST_REQUIRE(tt->params().size() == 1);
    const qual_type_name & par = tt->params()[0];
    BOOST_REQUIRE(par.is_valid());
    BOOST_CHECK(!par.is_const());
    BOOST_CHECK(!par.is_volatile());

    const template_type_name * tt2 = dynamic_cast<const template_type_name*>(par.type());
    BOOST_REQUIRE(tt2);
    BOOST_CHECK(tt2->template_name().identifier() == "cls2");

    BOOST_REQUIRE(tt2->template_name().has_scope());
    const basic_type_name * tt2_scope = dynamic_cast<const basic_type_name*>(tt2->template_name().scope());
    BOOST_REQUIRE(tt2_scope);
    BOOST_CHECK(!tt2_scope->has_scope());
    BOOST_CHECK(tt2_scope->identifier() == "xx");

    BOOST_REQUIRE(tt2->params().size() == 1);
    const qual_type_name & tt2_par = tt2->params()[0];
    BOOST_REQUIRE(tt2_par.is_valid());
    BOOST_CHECK(!tt2_par.is_const());
    BOOST_CHECK(!tt2_par.is_volatile());

    auto tt2_par_t = dynamic_cast<const builtin_type_name*>(tt2_par.type());
    BOOST_REQUIRE(tt2_par_t);
    BOOST_CHECK(tt2_par_t->kind() == builtin_type_name::kind_t::float_);
}


/// Tests parsing function type name
BOOST_AUTO_TEST_CASE(function_type_name_parse) {
    std::istringstream str{"std::xxx (int*, const float &)"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    const basic_type_name * rt = dynamic_cast<const basic_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(rt->identifier() == "xxx");

    BOOST_REQUIRE(rt->has_scope());
    const basic_type_name * rt_scope = dynamic_cast<const basic_type_name*>(rt->scope());
    BOOST_REQUIRE(rt_scope);
    BOOST_CHECK(!rt_scope->has_scope());
    BOOST_CHECK(rt_scope->identifier() == "std");

    BOOST_REQUIRE(ft->params().size() == 2);
    const qual_type_name & par1 = ft->params()[0];
    BOOST_CHECK(!par1.is_const());
    BOOST_CHECK(!par1.is_volatile());

    const pointer_type_name * par1_t = dynamic_cast<const pointer_type_name*>(par1.type());
    BOOST_REQUIRE(par1_t);
    BOOST_CHECK(!par1_t->base().is_const());
    BOOST_CHECK(!par1_t->base().is_volatile());

    auto par1_b = dynamic_cast<const builtin_type_name*>(par1_t->base().type());
    BOOST_REQUIRE(par1_b);
    BOOST_CHECK(par1_b->kind() == builtin_type_name::kind_t::int_);

    const qual_type_name & par2 = ft->params()[1];
    BOOST_CHECK(!par2.is_const());
    BOOST_CHECK(!par2.is_volatile());

    const reference_type_name * par2_t = dynamic_cast<const reference_type_name*>(par2.type());
    BOOST_REQUIRE(par2_t);
    BOOST_CHECK(par2_t->base().is_const());
    BOOST_CHECK(!par2_t->base().is_volatile());

    auto par2_b = dynamic_cast<const builtin_type_name*>(par2_t->base().type());
    BOOST_REQUIRE(par2_b);
    BOOST_CHECK(par2_b->kind() == builtin_type_name::kind_t::float_);
}


/// Tests parsing function type name with no parameters
BOOST_AUTO_TEST_CASE(function_type_name_parse_no_pars) {
    std::istringstream str{"std::xxx ()"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    const basic_type_name * rt = dynamic_cast<const basic_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(rt->identifier() == "xxx");

    BOOST_REQUIRE(rt->has_scope());
    const basic_type_name * rt_scope = dynamic_cast<const basic_type_name*>(rt->scope());
    BOOST_REQUIRE(rt_scope);
    BOOST_CHECK(!rt_scope->has_scope());
    BOOST_CHECK(rt_scope->identifier() == "std");

    BOOST_REQUIRE(ft->params().empty());
}


/// Tests parsing pointer to member type name
BOOST_AUTO_TEST_CASE(mem_ptr_type_name_parse) {
    std::istringstream str{"int cls::*"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const mem_ptr_type_name * pt = dynamic_cast<const mem_ptr_type_name*>(t.type());
    BOOST_REQUIRE(pt);

    const basic_type_name * bpt = dynamic_cast<const basic_type_name*>(pt->base());
    BOOST_REQUIRE(bpt);

    BOOST_CHECK(!bpt->has_scope());
    BOOST_CHECK(bpt->identifier() == "cls");
    BOOST_CHECK(!pt->mem_type().is_const());
    BOOST_CHECK(!pt->mem_type().is_volatile());

    auto bt = dynamic_cast<const builtin_type_name*>(pt->mem_type().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing pointer to member function type name
BOOST_AUTO_TEST_CASE(mem_func_ptr_type_name_parse) {
    std::istringstream str{"int (cls::*)(int)"};
    qual_type_name t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const mem_ptr_type_name * mt = dynamic_cast<const mem_ptr_type_name*>(t.type());
    BOOST_REQUIRE(mt);
    BOOST_CHECK(!mt->mem_type().is_const());
    BOOST_CHECK(!mt->mem_type().is_volatile());

    const basic_type_name * bt = dynamic_cast<const basic_type_name*>(mt->base());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(!bt->has_scope());
    BOOST_CHECK(bt->identifier() == "cls");

    const function_type_name * ft = dynamic_cast<const function_type_name*>(mt->mem_type().type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto rt = dynamic_cast<const builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(rt->kind() == builtin_type_name::kind_t::int_);

    BOOST_REQUIRE(ft->params().size() == 1);
    const qual_type_name & par1 = ft->params()[0];
    BOOST_REQUIRE(!par1.is_const());
    BOOST_REQUIRE(!par1.is_volatile());

    auto par1_t = dynamic_cast<const builtin_type_name*>(par1.type());
    BOOST_REQUIRE(par1_t);
    BOOST_CHECK(par1_t->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing nested parents ()
BOOST_AUTO_TEST_CASE(parse_nested_parents) {
    std::istringstream str{"int (*(*)[20]) [30]"};
    auto t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto pt = dynamic_cast<const pointer_type_name*>(t.type());
    BOOST_REQUIRE(pt);
    BOOST_CHECK(!pt->base().is_const());
    BOOST_CHECK(!pt->base().is_volatile());

    auto at = dynamic_cast<const array_type_name*>(pt->base().type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    auto pt2 = dynamic_cast<const pointer_type_name*>(at->el_type());
    BOOST_REQUIRE(pt2);
    BOOST_CHECK(!pt2->base().is_const());
    BOOST_CHECK(!pt2->base().is_volatile());

    auto at2 = dynamic_cast<const array_type_name*>(pt2->base().type());
    BOOST_REQUIRE(at2);
    BOOST_CHECK(at2->size() == 30);

    auto bt = dynamic_cast<const builtin_type_name*>(at2->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing array of arrays
BOOST_AUTO_TEST_CASE(test_array_array) {
    std::istringstream str{"int [30][20]"};
    auto t = parse_type_name(str);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto at = dynamic_cast<const array_type_name*>(t.type());
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->size() == 20);

    auto at2 = dynamic_cast<const array_type_name*>(at->el_type());
    BOOST_REQUIRE(at2);
    BOOST_CHECK(at2->size() == 30);

    auto bt = dynamic_cast<const builtin_type_name*>(at2->el_type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::int_);
}


// test case for bug CXXDBG-174
BOOST_AUTO_TEST_CASE(test_cxxdbg_174) {
    auto tname = "std::_List_node<boost::shared_ptr<boost::signals2::detail::"
                 "connection_body<std::pair<boost::signals2::detail::slot_meta_group, "
                 "boost::optional<int> >, boost::signals2::slot<void (unsigned long, "
                 "unsigned long), boost::function<void (unsigned long, unsigned long)> "
                 ">, boost::signals2::mutex> > >";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_REQUIRE(t.is_valid());
}


// small test case for bug CXXDBG-174
BOOST_AUTO_TEST_CASE(test_cxxdbg_174_small) {
    auto tname = "void (unsigned long)";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_REQUIRE(t.is_valid());

    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto rt = dynamic_cast<const builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(rt->kind() == builtin_type_name::kind_t::void_);

    BOOST_REQUIRE(ft->params().size() == 1);
    const qual_type_name & par1 = ft->params()[0];
    BOOST_CHECK(!par1.is_const());
    BOOST_CHECK(!par1.is_volatile());

    auto par1_t = dynamic_cast<const builtin_type_name*>(par1.type());
    BOOST_REQUIRE(par1_t);
    BOOST_CHECK(par1_t->kind() == builtin_type_name::kind_t::unsigned_long_);
}


/// Tests parsing pointer to function returning pointer to function
BOOST_AUTO_TEST_CASE(test_fptr_fptr) {
    auto tname = "void (*())()";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_REQUIRE(t.is_valid());

    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(ft->params().size() == 0);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    const pointer_type_name * rt = dynamic_cast<const pointer_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(!rt->base().is_const());
    BOOST_CHECK(!rt->base().is_volatile());

    const function_type_name * rft = dynamic_cast<const function_type_name*>(rt->base().type());
    BOOST_REQUIRE(rft);
    BOOST_CHECK(rft->params().size() == 0);
    BOOST_CHECK(!rft->ret_type().is_const());
    BOOST_CHECK(!rft->ret_type().is_volatile());

    auto rrt = dynamic_cast<const builtin_type_name*>(rft->ret_type().type());
    BOOST_CHECK(rrt->kind() == builtin_type_name::kind_t::void_);
}


/// Tests parsing declaration of template member template function returning
/// pointer to function
BOOST_AUTO_TEST_CASE(test_decl_fptr_fptr) {
    auto tname = "void (*cls<float>::zopa<int>(int))()";
    std::istringstream str{tname};

    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);
    BOOST_REQUIRE(t.is_valid());

    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(ft->params().size() == 1);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    const pointer_type_name * rt = dynamic_cast<const pointer_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(!rt->base().is_const());
    BOOST_CHECK(!rt->base().is_volatile());

    const function_type_name * rft = dynamic_cast<const function_type_name*>(rt->base().type());
    BOOST_REQUIRE(rft);
    BOOST_CHECK(rft->params().size() == 0);
    BOOST_CHECK(!rft->ret_type().is_const());
    BOOST_CHECK(!rft->ret_type().is_volatile());

    auto rrt = dynamic_cast<const builtin_type_name*>(rft->ret_type().type());
    BOOST_REQUIRE(rrt);
    BOOST_CHECK(rrt->kind() == builtin_type_name::kind_t::void_);


    // checking declaration
    BOOST_REQUIRE(decl_name);
    auto decl_name_templ = dynamic_cast<const template_type_name*>(decl_name.get());
    BOOST_REQUIRE(decl_name_templ);

    BOOST_CHECK(decl_name_templ->params().size() == 1);
    BOOST_CHECK(decl_name_templ->template_name().identifier() == "zopa");

    BOOST_REQUIRE(decl_name_templ->template_name().has_scope());
    auto tmpl2 = dynamic_cast<const template_type_name*>(decl_name_templ->template_name().scope());
    BOOST_CHECK(tmpl2->template_name().identifier() == "cls");
}


/// Tests parsing declaration of template member template function with no return type
BOOST_AUTO_TEST_CASE(test_decl_func_no_ret) {
    auto tname = "cls<float>::zopa<int>(int)";
    std::istringstream str{tname};

    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);
    BOOST_REQUIRE(t.is_valid());

    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    const function_type_name * ft = dynamic_cast<const function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(ft->params().size() == 1);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK(!ft->ret_type().type());

    // checking declaration
    BOOST_REQUIRE(decl_name);
    auto decl_name_templ = dynamic_cast<const template_type_name*>(decl_name.get());
    BOOST_REQUIRE(decl_name_templ);

    BOOST_CHECK(decl_name_templ->params().size() == 1);
    BOOST_CHECK(decl_name_templ->template_name().identifier() == "zopa");

    BOOST_REQUIRE(decl_name_templ->template_name().has_scope());
    auto tmpl2 = dynamic_cast<const template_type_name*>(decl_name_templ->template_name().scope());
    BOOST_CHECK(tmpl2->template_name().identifier() == "cls");
}


/// Tests parsing array with bad length
BOOST_AUTO_TEST_CASE(parse_bad_array_length) {
    auto tname = "int[aaa]";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_CHECK(!t.is_valid());
}


/// Tests parsing type of function returning decltype()
BOOST_AUTO_TEST_CASE(parse_func_return_decltype) {
    auto tname = "decltype (bar({parm#1})) const (int)";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);

    BOOST_CHECK(ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    auto rtype = dynamic_cast<decltype_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rtype);
    BOOST_CHECK(rtype->expr() == "bar({parm#1})");

    BOOST_REQUIRE(ft->params().size() == 1);
    BOOST_CHECK(!ft->params()[0].is_const());
    BOOST_CHECK(!ft->params()[0].is_volatile());

    auto par1 = dynamic_cast<builtin_type_name*>(ft->params()[0].type());
    BOOST_REQUIRE(par1);
    BOOST_CHECK(par1->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing type of function with decltype parameter
BOOST_AUTO_TEST_CASE(parse_func_decltype_param) {
    auto tname = "int (int, decltype (bar({parm#1})))";
    std::istringstream str{tname};

    auto t = parse_type_name(str);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);

    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto rtype = dynamic_cast<builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rtype);
    BOOST_CHECK(rtype->kind() == builtin_type_name::kind_t::int_);

    auto par1 = dynamic_cast<builtin_type_name*>(ft->params()[0].type());
    BOOST_REQUIRE(par1);
    BOOST_CHECK(par1->kind() == builtin_type_name::kind_t::int_);

    auto par2 = dynamic_cast<decltype_type_name*>(ft->params()[1].type());
    BOOST_REQUIRE(par2);
    BOOST_CHECK(par2->expr() == "bar({parm#1})");
}


/// Tests parsing name of local type in function
BOOST_AUTO_TEST_CASE(parse_local_type) {
    auto tname = "int foo()::str";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "str");

    auto scope = bt->scope();
    BOOST_REQUIRE(scope);

    auto fdecl = dynamic_cast<const function_decl*>(scope);
    BOOST_REQUIRE(fdecl);

    auto fname = dynamic_cast<const basic_type_name*>(fdecl->name());
    BOOST_REQUIRE(fname);
    BOOST_CHECK_EQUAL(fname->identifier(), "foo");
    BOOST_CHECK(fname->scope() == nullptr);

    auto ft = fdecl->type();
    BOOST_REQUIRE(ft);

    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto rtype = dynamic_cast<builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rtype);
    BOOST_CHECK(rtype->kind() == builtin_type_name::kind_t::int_);
}


/// Tests parsing lambda type
BOOST_AUTO_TEST_CASE(parse_lambda) {
    auto tname = "<lambda(auto:1)>";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "<lambda(auto:1)>");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing lambda type in scope
BOOST_AUTO_TEST_CASE(parse_lambda_scope) {
    auto tname = "cls::<lambda(auto:1)>";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "<lambda(auto:1)>");

    auto scope = dynamic_cast<const basic_type_name*>(bt->scope());
    BOOST_REQUIRE(scope);
    BOOST_CHECK_EQUAL(scope->identifier(), "cls");
}


/// Tests parsing const reference to lambda
BOOST_AUTO_TEST_CASE(parse_lambda_const_ref) {
    auto tname = "const <lambda()> &const";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ref = dynamic_cast<reference_type_name*>(t.type());
    BOOST_REQUIRE(ref);
    BOOST_CHECK(ref->base().is_const());
    BOOST_CHECK(!ref->base().is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(ref->base().type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "<lambda()>");

    auto scope = dynamic_cast<const basic_type_name*>(bt->scope());
    BOOST_CHECK(!scope);
}


/// Tests parsing long unsigned int
BOOST_AUTO_TEST_CASE(parse_long_unsigned_int) {
    auto tname = "long unsigned int";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<builtin_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::unsigned_long_);
}


/// Tests parsing long
BOOST_AUTO_TEST_CASE(parse_long) {
    auto tname = "long int";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<builtin_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK(bt->kind() == builtin_type_name::kind_t::long_);
}


/// Tests parsing lambda type with complex expressions iside <>
BOOST_AUTO_TEST_CASE(parse_lambda_complex) {
    auto tname = "<lambda(const auto:1&)>";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "<lambda(const auto:1&)>");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing function type with lambda parameter
BOOST_AUTO_TEST_CASE(parse_lambda_par) {
    auto tname = "void (<lambda()>)";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);

    BOOST_REQUIRE(ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto rt = dynamic_cast<builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(rt);
    BOOST_CHECK(rt->kind() == builtin_type_name::kind_t::void_);

    BOOST_REQUIRE_EQUAL(ft->params().size(), 1);
    BOOST_CHECK(!ft->params()[0].is_const());
    BOOST_CHECK(!ft->params()[0].is_volatile());
    auto par = dynamic_cast<basic_type_name*>(ft->params()[0].type());
    BOOST_REQUIRE(par);
    BOOST_CHECK_EQUAL(par->identifier(), "<lambda()>");
}


/// Tests parsing anonumous namespace
BOOST_AUTO_TEST_CASE(parse_anon_ns) {
    auto tname = "(anonymous namespace)::cls";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);

    BOOST_CHECK_EQUAL(bt->identifier(), "cls");

    BOOST_REQUIRE(bt->has_scope());
    auto scope = bt->scope();
    BOOST_REQUIRE(scope);

    auto scope_bt = dynamic_cast<const basic_type_name*>(scope);
    BOOST_CHECK_EQUAL(scope_bt->identifier(), "(anonymous namespace)");
    BOOST_CHECK(!scope_bt->has_scope());
}


/// Tests parsing nested anonumous namespace
BOOST_AUTO_TEST_CASE(parse_anon_ns_nested) {
    auto tname = "aaa::(anonymous namespace)::cls";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);

    BOOST_CHECK_EQUAL(bt->identifier(), "cls");

    BOOST_REQUIRE(bt->has_scope());
    auto scope = bt->scope();
    BOOST_REQUIRE(scope);

    auto scope_bt = dynamic_cast<const basic_type_name*>(scope);
    BOOST_CHECK_EQUAL(scope_bt->identifier(), "(anonymous namespace)");
    BOOST_REQUIRE(scope_bt->has_scope());

    auto aaa_bt = dynamic_cast<const basic_type_name*>(scope_bt->scope());
    BOOST_REQUIRE(aaa_bt);
    BOOST_CHECK_EQUAL(aaa_bt->identifier(), "aaa");
    BOOST_CHECK(!aaa_bt->has_scope());
}


/// Tests parsing quoted lambda type
BOOST_AUTO_TEST_CASE(parse_lambda_quotes) {
    auto tname = "'lambda'(int, int)";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "'lambda'(int, int)");
    BOOST_CHECK(!bt->scope());
}



/// Tests parsing "'lambda'(args)" type with scope
BOOST_AUTO_TEST_CASE(parse_lambda_quotes_scope) {
    auto tname = "main::'lambda'(int)";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);

    BOOST_CHECK_EQUAL(bt->identifier(), "'lambda'(int)");

    BOOST_REQUIRE(bt->has_scope());
    auto scope = bt->scope();
    BOOST_REQUIRE(scope);

    auto scope_bt = dynamic_cast<const basic_type_name*>(scope);
    BOOST_CHECK_EQUAL(scope_bt->identifier(), "main");
    BOOST_CHECK(!scope_bt->has_scope());

}


/// Tests parsing "'lambda'(args)" declaration
BOOST_AUTO_TEST_CASE(parse_lambda_quotes_args_decl) {
    auto tname = "int main::'lambda'(int)::operator()()";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());

    auto ret_type = dynamic_cast<builtin_type_name*>(ft->ret_type().type());
    BOOST_REQUIRE(ret_type);
    BOOST_CHECK(ret_type->kind() == builtin_type_name::kind_t::int_);

    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bdecl = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bdecl);
    BOOST_CHECK_EQUAL(bdecl->identifier(), "operator()");
    BOOST_REQUIRE(bdecl->has_scope());

    auto lambda_scope = dynamic_cast<const basic_type_name*>(bdecl->scope());
    BOOST_REQUIRE(lambda_scope);
    BOOST_CHECK_EQUAL(lambda_scope->identifier(), "'lambda'(int)");
    BOOST_REQUIRE(lambda_scope->has_scope());

    auto main_scope = dynamic_cast<const basic_type_name*>(lambda_scope->scope());
    BOOST_REQUIRE(main_scope);
    BOOST_CHECK(!main_scope->has_scope());
    BOOST_CHECK_EQUAL(main_scope->identifier(), "main");
}


/// Tests parsing type with :: prefix
BOOST_AUTO_TEST_CASE(parse_global_prefix) {
    auto tname = "::foo";
    std::istringstream str{tname};

    std::string err;
    auto t = parse_type_name(str, &err);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto bt = dynamic_cast<basic_type_name*>(t.type());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "foo");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing ctor declaration without name
BOOST_AUTO_TEST_CASE(parse_ctor_no_name) {
    auto name = "foo::()";
    std::istringstream str{name};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto b_decl_name = dynamic_cast<const basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(b_decl_name);
    BOOST_CHECK_EQUAL(b_decl_name->identifier(), "");
    BOOST_REQUIRE(b_decl_name->has_scope());

    auto scope = dynamic_cast<const basic_type_name*>(b_decl_name->scope());
    BOOST_REQUIRE(scope);
    BOOST_CHECK_EQUAL(scope->identifier(), "foo");
    BOOST_CHECK(!scope->has_scope());
}


/// Tests parsing dtor declaration without name
BOOST_AUTO_TEST_CASE(parse_dtor_no_name) {
    auto name = "foo::~()";
    std::istringstream str{name};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto b_decl_name = dynamic_cast<const basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(b_decl_name);
    BOOST_CHECK_EQUAL(b_decl_name->identifier(), "~");
    BOOST_REQUIRE(b_decl_name->has_scope());

    auto scope = dynamic_cast<const basic_type_name*>(b_decl_name->scope());
    BOOST_REQUIRE(scope);
    BOOST_CHECK_EQUAL(scope->identifier(), "foo");
    BOOST_CHECK(!scope->has_scope());
}


/// Tests parsing operator!= declaration
BOOST_AUTO_TEST_CASE(parse_operator_ne_decl) {
    auto tname = "operator!=";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator!=");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator* declaration
BOOST_AUTO_TEST_CASE(parse_operator_deref_decl) {
    auto tname = "operator*";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator*");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator< declaration
BOOST_AUTO_TEST_CASE(parse_operator_l_decl) {
    auto tname = "operator<";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator<");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator> declaration
BOOST_AUTO_TEST_CASE(parse_operator_g_decl) {
    auto tname = "operator>";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator>");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator<= declaration
BOOST_AUTO_TEST_CASE(parse_operator_le_decl) {
    auto tname = "operator<=";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator<=");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator>= declaration
BOOST_AUTO_TEST_CASE(parse_operator_ge_decl) {
    auto tname = "operator>=";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator>=");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator<< declaration
BOOST_AUTO_TEST_CASE(parse_operator_lshift_decl) {
    auto tname = "operator<<";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator<<");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator>> declaration
BOOST_AUTO_TEST_CASE(parse_operator_rshift_decl) {
    auto tname = "operator>>";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator>>");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator<<= declaration
BOOST_AUTO_TEST_CASE(parse_operator_lshifte_decl) {
    auto tname = "operator<<=";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator<<=");
    BOOST_CHECK(!bt->scope());
}


/// Tests parsing operator>> declaration
BOOST_AUTO_TEST_CASE(parse_operator_rshifte_decl) {
    auto tname = "operator>>=";
    std::istringstream str{tname};

    std::string err;
    qual_type_name t;
    scope_type_name_sp decl_name;
    parse_declaration(str, t, decl_name);

    BOOST_REQUIRE(t.is_valid());
    BOOST_CHECK(!t.is_const());
    BOOST_CHECK(!t.is_volatile());

    auto ft = dynamic_cast<function_type_name*>(t.type());
    BOOST_REQUIRE(ft);
    BOOST_CHECK(!ft->ret_type().is_valid());
    BOOST_CHECK(!ft->ret_type().is_const());
    BOOST_CHECK(!ft->ret_type().is_volatile());
    BOOST_CHECK_EQUAL(ft->params().size(), 0);

    auto bt = dynamic_cast<basic_type_name*>(decl_name.get());
    BOOST_REQUIRE(bt);
    BOOST_CHECK_EQUAL(bt->identifier(), "operator>>=");
    BOOST_CHECK(!bt->scope());
}


BOOST_AUTO_TEST_SUITE_END()

}
