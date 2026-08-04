
/// \file type_name_test.cpp
/// Contains unit tests for parsing/writing type_name

#include <cxxtn/type_name.hpp>
#include <cxxtn/type_name_parser.hpp>
#include <sstream>
#include <boost/test/unit_test.hpp>


namespace cxxtn::test {


struct type_name_test_fixture {
    std::ostringstream str;
};


BOOST_FIXTURE_TEST_SUITE(type_name_test, type_name_test_fixture)


/// Tests writing user defined qualified type name
BOOST_AUTO_TEST_CASE(qual_type_name_write_user) {
    qual_type_name t{basic_type_name::make("cls"), true};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "const cls");
}


/// Tests writing builtin qualified type name with gcc style
BOOST_AUTO_TEST_CASE(qual_type_name_write_builtin_gcc) {
    qual_type_name t{builtin_type_name::int_(), true};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int const");
}


/// Tests writing builtin qualified type name with clang style
BOOST_AUTO_TEST_CASE(qual_type_name_write_builtin_clang) {
    qual_type_name t{builtin_type_name::int_(), true};
    t.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "const int");
}


/// Tests writing basic type name to output stream
BOOST_AUTO_TEST_CASE(basic_type_name_write) {
    basic_type_name t{"type"};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "type");
}


/// Tests writing basic type name with scope to output stream
BOOST_AUTO_TEST_CASE(basic_type_name_write_scope) {
    basic_type_name t{"string", basic_type_name::make("std")};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "std::string");
}


/// Tests writing array type name in gcc style
BOOST_AUTO_TEST_CASE(array_type_name_write_gcc) {
    array_type_name t{builtin_type_name::int_(), 10};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int [10]");
}


/// Tests writing array type name in clang style
BOOST_AUTO_TEST_CASE(array_type_name_write_clang) {
    array_type_name t{builtin_type_name::int_(), 10};
    t.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "int [10]");
}


/// Tests writing array of arrays in gcc style
BOOST_AUTO_TEST_CASE(array_of_array_type_name_write_gcc) {
    auto t1 = std::make_shared<array_type_name>(builtin_type_name::int_(), 10);
    array_type_name t2{t1, 10};
    t2.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int [10][10]");
}


/// Tests writing array of arrays in clang style
BOOST_AUTO_TEST_CASE(array_of_array_type_name_write_clang) {
    auto t1 = std::make_shared<array_type_name>(builtin_type_name::int_(), 10);
    array_type_name t2{t1, 10};
    t2.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "int [10][10]");
}


/// Tests writing qualified array of user defined types
BOOST_AUTO_TEST_CASE(array_type_write_const_user) {
    std::shared_ptr<array_type_name> at{new array_type_name{basic_type_name::make("cls"), 10}};
    qual_type_name qt{at, true};
    qt.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "const cls [10]");
}


/// Tests writing qualified array of builtin types with gcc style
BOOST_AUTO_TEST_CASE(array_type_write_const_builtin_gcc) {
    auto int_t = builtin_type_name::int_();
    std::shared_ptr<array_type_name> at{new array_type_name{int_t, 10}};
    qual_type_name qt{at, true};
    qt.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int const [10]");
}


/// Tests writing qualified array of builtin types with clang style
BOOST_AUTO_TEST_CASE(array_type_write_const_builtin_clang) {
    auto int_t = builtin_type_name::int_();
    std::shared_ptr<array_type_name> at{new array_type_name{int_t, 10}};
    qual_type_name qt{at, true};
    qt.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "int const[10]");
}


/// Tests writing pointer type name to output stream with gcc style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_gcc) {
    pointer_type_name pt{{basic_type_name::make("foo")}};
    pt.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "foo*");
}


/// Tests writing pointer type name to output stream with clang style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_clang) {
    pointer_type_name pt{{basic_type_name::make("foo")}};
    pt.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "foo *");
}


/// Tests writing pointer to array type name in gcc style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_gcc) {
    auto el_type = builtin_type_name::int_();
    std::shared_ptr<array_type_name> arr{new array_type_name{el_type, 20}};
    pointer_type_name ptr{{arr}};
    ptr.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int (*)[20]");
}


/// Tests writing pointer to array type name in clang style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_clang) {
    auto el_type = builtin_type_name::int_();
    std::shared_ptr<array_type_name> arr{new array_type_name{el_type, 20}};
    pointer_type_name ptr{{arr}};
    ptr.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "int (*)[20]");
}



/// Tests writing pointer to qualified array of user defined types with gcc style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_const_user_gcc) {
    auto el_type = basic_type_name::make("cls");
    std::shared_ptr<array_type_name> arr{new array_type_name{el_type, 20}};
    pointer_type_name ptr{{arr, true}};
    ptr.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "const cls (*)[20]");
}


/// Tests writing pointer to qualified array of user defined types with clang style
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_const_user_clang) {
    auto el_type = basic_type_name::make("cls");
    std::shared_ptr<array_type_name> arr{new array_type_name{el_type, 20}};
    pointer_type_name ptr{{arr, true}};
    ptr.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "cls const (*)[20]");
}


/// Tests writing pointer to qualified array of builtin types
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_const_builtin) {
    auto el_type = builtin_type_name::int_();
    std::shared_ptr<array_type_name> arr{new array_type_name{el_type, 20}};
    pointer_type_name ptr{{arr, true}};
    ptr.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int const (*)[20]");
}


/// Tests writing reference type name to output stream with gcc style
BOOST_AUTO_TEST_CASE(reference_type_name_write_gcc) {
    reference_type_name rt{{basic_type_name::make("foo")}};
    rt.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "foo&");
}


/// Tests writing reference type name to output stream with clang style
BOOST_AUTO_TEST_CASE(reference_type_name_write_clang) {
    reference_type_name rt{{basic_type_name::make("foo")}};
    rt.write(str, type_name::write_style::clang);
    BOOST_CHECK(str.str() == "foo &");
}


/// Tests writing template type name to output stream
BOOST_AUTO_TEST_CASE(template_type_name_write) {
    template_type_name t{{"vector"}};
    t.params().push_back({builtin_type_name::int_()});
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "vector<int>");
}


/// Tests writing template type name with template parameter
BOOST_AUTO_TEST_CASE(template_type_name_write_templ) {
    auto t2 = template_type_name::make({"foo"});
    t2->params().push_back({builtin_type_name::int_()});
    template_type_name t{{"vector"}};
    t.params().push_back({t2});
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "vector<foo<int> >");
}


/// Tests writing function type name with gcc style
BOOST_AUTO_TEST_CASE(function_type_name_write_gcc) {
    function_type_name ft{{builtin_type_name::int_()}};
    ft.params().push_back({builtin_type_name::int_()});
    ft.params().push_back({builtin_type_name::float_()});
    ft.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int(int, float)");
}


/// Tests writing function type name with clang style
BOOST_AUTO_TEST_CASE(function_type_name_write_clang) {
    function_type_name ft{{builtin_type_name::int_()}};
    ft.params().push_back({builtin_type_name::int_()});
    ft.params().push_back({builtin_type_name::float_()});
    ft.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "int (int, float)");
}


/// Tests writing member pointer type nmae
BOOST_AUTO_TEST_CASE(mem_ptr_type_name_write) {
    mem_ptr_type_name t{basic_type_name::make("cls"), {builtin_type_name::int_()}};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int cls::*");
}


/// Tests writing member function pointer type name
BOOST_AUTO_TEST_CASE(mem_func_ptr_type_name_write) {
    auto ft = function_type_name::make({builtin_type_name::int_()});
    ft->params().push_back({builtin_type_name::int_()});
    ft->params().push_back({builtin_type_name::float_()});
    mem_ptr_type_name t{basic_type_name::make("cls"), {ft}};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int (cls::*)(int, float)");
}


/// Tests writing pointer to array of poiners to arrays with gcc style
BOOST_AUTO_TEST_CASE(write_ptr_arr_ptr_arr_gcc) {
    auto ar = array_type_name::make(builtin_type_name::int_(), 30);
    auto ptr = pointer_type_name::make({ar});
    auto ar2 = array_type_name::make(ptr, 20);
    auto ptr2 = pointer_type_name::make({ar2});
    qual_type_name t{ptr2};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int (* (*)[20])[30]");
}


/// Tests writing pointer to array of poiners to arrays with clang style
BOOST_AUTO_TEST_CASE(write_ptr_arr_ptr_arr_clang) {
    auto ar = array_type_name::make(builtin_type_name::int_(), 30);
    auto ptr = pointer_type_name::make({ar});
    auto ar2 = array_type_name::make(ptr, 20);
    auto ptr2 = pointer_type_name::make({ar2});
    qual_type_name t{ptr2};
    t.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "int (*(*)[20])[30]");
}


/// Tests writing pointer to function in gcc style
BOOST_AUTO_TEST_CASE(write_func_ptr_gcc) {
    auto ft = function_type_name::make({builtin_type_name::int_()});
    auto pt = pointer_type_name::make({ft});
    qual_type_name t{pt};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "int (*)()");
}


/// Tests writing pointer to function in clang style
BOOST_AUTO_TEST_CASE(write_func_ptr_clang) {
    auto ft = function_type_name::make({builtin_type_name::int_()});
    auto pt = pointer_type_name::make({ft});
    qual_type_name t{pt};
    t.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "int (*)()");
}


/// Tests writing pointer to function returning pointer
BOOST_AUTO_TEST_CASE(write_func_ptr_ret_ptr) {
    auto ret = pointer_type_name::make({builtin_type_name::int_()});
    auto ft = function_type_name::make({ret});
    auto pt = pointer_type_name::make({ft});
    qual_type_name t{pt};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int* (*)()");
}


/// Tests writing function type in gcc style
BOOST_AUTO_TEST_CASE(write_func_gcc) {
    auto ft = function_type_name::make({builtin_type_name::int_()});
    qual_type_name t{ft};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int()");
}


/// Tests writing function type in clang style
BOOST_AUTO_TEST_CASE(write_func_clang) {
    auto ft = function_type_name::make({builtin_type_name::int_()});
    qual_type_name t{ft};
    t.write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "int ()");
}


/// Tests writing function type with not return type
BOOST_AUTO_TEST_CASE(write_func_no_ret) {
    auto ft = std::make_shared<function_type_name>(qual_type_name{});
    qual_type_name t{ft};
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "()");
}


/// Tests writing function declaration
BOOST_AUTO_TEST_CASE(func_decl_write) {
    auto ret = builtin_type_name::int_();
    auto ftype = std::make_shared<function_type_name>(qual_type_name{ret});
    auto fname = std::make_shared<basic_type_name>("foo");
    auto fdecl = std::make_shared<function_decl>(fname, ftype);

    fdecl->write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "int foo()");
}


/// Tests writing function declaration with not return type
BOOST_AUTO_TEST_CASE(func_decl_write_no_ret) {
    auto ftype = std::make_shared<function_type_name>(qual_type_name{});
    auto fname = std::make_shared<basic_type_name>("foo");
    auto fdecl = std::make_shared<function_decl>(fname, ftype);

    fdecl->write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "foo()");
}


/// Tests writing template type name with 2 parameters
BOOST_AUTO_TEST_CASE(template_type_name_write_2) {
    auto t2 = template_type_name::make({"foo"});
    t2->params().push_back({builtin_type_name::int_()});
    template_type_name t{{"vector"}};
    t.params().push_back({t2});
    t.params().push_back({builtin_type_name::void_()});
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "vector<foo<int>, void>");
}


/// Tests printing template with lambda parameter
BOOST_AUTO_TEST_CASE(template_type_name_write_lambda_par) {
    template_type_name t{{"vector"}};
    t.params().push_back({basic_type_name::make("<lambda()>")});
    t.write(str, type_name::write_style::gcc);
    BOOST_CHECK(str.str() == "vector<<lambda()> >");
}


/// Tests writing array of pointers to functions with gcc style
BOOST_AUTO_TEST_CASE(func_ptr_array_gcc) {
    auto ftype = function_type_name::make({builtin_type_name::void_()});
    auto fptr_type = pointer_type_name::make({ftype});
    auto arr_type = array_type_name::make(fptr_type, 10);
    arr_type->write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "void (* [10])()");
}


/// Tests writing array of pointers to functions with clang style
BOOST_AUTO_TEST_CASE(func_ptr_array_clang) {
    auto ftype = function_type_name::make({builtin_type_name::void_()});
    auto fptr_type = pointer_type_name::make({ftype});
    auto arr_type = array_type_name::make(fptr_type, 10);
    arr_type->write(str, type_name::write_style::clang);
    BOOST_CHECK_EQUAL(str.str(), "void (*[10])()");
}


/// Tests writing function returning pointer to function
BOOST_AUTO_TEST_CASE(func_ret_func) {
    auto f_ret_type = function_type_name::make({builtin_type_name::void_()});
    auto fptr_ret_type = pointer_type_name::make({f_ret_type});
    function_type_name f_type{{fptr_ret_type}};
    f_type.write(str, type_name::write_style::gcc);
    BOOST_CHECK_EQUAL(str.str(), "void (*())()");
}


/// Tests writing complex libc++ std::function related type name in clang style
BOOST_AUTO_TEST_CASE(libcxx_std_func_complex) {
    auto tname = "__func<std::__1::__bind<void (*)(int, int, int), const std::__1::placeholders::__ph<1> &, int, "
                 "const std::__1::placeholders::__ph<2> &>, std::__1::allocator<std::__1::__bind<void (*)(int, int, int), "
                 "const std::__1::placeholders::__ph<1> &, int, const std::__1::placeholders::__ph<2> &> >, void (int, int)>";
    std::istringstream istr{tname};
    auto tn = parse_type_name(istr);

    BOOST_REQUIRE(tn.is_valid());

    std::ostringstream ostr;
    tn.write(ostr, cxxtn::type_name_write_style::clang);

    BOOST_CHECK_EQUAL(ostr.str(), tname);
}


BOOST_AUTO_TEST_SUITE_END()


}
