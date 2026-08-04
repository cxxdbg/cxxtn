
/// \file cxxdbg_function_name_parser_test.cpp
/// Contains unit tests for the cxxdbg_function_name_parser_test class.

#include <cxxtn/function_name_parser.hpp>
#include <boost/test/unit_test.hpp>


namespace cxxtn::test {


BOOST_AUTO_TEST_SUITE(cxxdbg_function_name_parser_test)


/// Tests parsing simple function name
BOOST_AUTO_TEST_CASE(test_simple) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("foo", name_no_ret_type, name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "foo()");
    BOOST_CHECK_EQUAL(name_no_params, "foo");
}


/// Tests parsing function name with parameters
BOOST_AUTO_TEST_CASE(test_params) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("foo(int, float)", name_no_ret_type, name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "foo(int, float)");
    BOOST_CHECK(name_no_params == "foo");
}


/// Tests parsing function name with return type
BOOST_AUTO_TEST_CASE(test_ret_type) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("int foo(int, float)", name_no_ret_type, name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "foo(int, float)");
    BOOST_CHECK(name_no_params == "foo");
}


/// Tests parsing name of function that returns pointer to function
BOOST_AUTO_TEST_CASE(test_ret_type_fptr) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("void (*cls<float>::zopa<int>(int))()", name_no_ret_type, name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "cls<float>::zopa<int>(int)");
    BOOST_CHECK_EQUAL(name_no_params, "cls<float>::zopa<int>");
}


/// Tests parsing complex function name from functional header from libstdc++
BOOST_AUTO_TEST_CASE(test_any_data_access) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("void (* const&std::_Any_data::_M_access<void (*)()>() const)()",
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "std::_Any_data::_M_access<void (*)()>()");
    BOOST_CHECK(name_no_params == "std::_Any_data::_M_access<void (*)()>");
}


/// Tests parsing std::_Mu::operator() function from libstdc++
BOOST_AUTO_TEST_CASE(test_std_mu_operator_call) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("int& std::_Mu<int, false, false>::operator()<int&, std::tuple<> >(int&, std::tuple<>&) const volatile",
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "std::_Mu<int, false, false>::operator()<int&, std::tuple<> >(int&, std::tuple<>&)");
    BOOST_CHECK(name_no_params == "std::_Mu<int, false, false>::operator()<int&, std::tuple<> >");
}


/// Tests parsing of operator()
BOOST_AUTO_TEST_CASE(test_operator_call) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("cls::operator()(int)",
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "cls::operator()(int)");
    BOOST_CHECK(name_no_params == "cls::operator()");
}


/// Test case for CXXDBG-298
BOOST_AUTO_TEST_CASE(cxxdbg_298) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("foo[abi:cxx11](int, int)",
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "foo(int, int)");
    BOOST_CHECK(name_no_params == "foo");
}


/// Tests parsing function name with [abi::cxx11] tag
BOOST_AUTO_TEST_CASE(parse_abicxx11_tag) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto res = parse_function_name("int ns::foo[abi:cxx11](int, int)",
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "ns::foo(int, int)");
    BOOST_CHECK(name_no_params == "ns::foo");
}


/// Tests parsing function name with decltype() from CXXDBG-299
BOOST_AUTO_TEST_CASE(parse_decltype_func) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "decltype (__invoke((*this)._M_pmf, (forward<cls*&>)({parm#1}))) "
                 "std::_Mem_fn_base<void (cls::*)(), true>::operator()<cls*&>(cls*&) const";
    auto res = parse_function_name(fname,
                                   name_no_ret_type,
                                   name_no_params);
    BOOST_CHECK(res);
    BOOST_CHECK(name_no_ret_type == "std::_Mem_fn_base<void (cls::*)(), true>::operator()<cls*&>(cls*&)");
    BOOST_CHECK(name_no_params == "std::_Mem_fn_base<void (cls::*)(), true>::operator()<cls*&>");
}


/// Tests parsing template instantiation with local type
BOOST_AUTO_TEST_CASE(parse_templ_local_type) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "templ<main()::str>::foo() const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "templ<main()::str>::foo()");
    BOOST_CHECK_EQUAL(name_no_params, "templ<main()::str>::foo");
}


/// Tests parsing local type member function
BOOST_AUTO_TEST_CASE(parse_templ_local_type_mem_fun) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "int foo<int>(int)::local_type::bar(int, float)";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "int foo<int>(int)::local_type::bar(int, float)");
    BOOST_CHECK_EQUAL(name_no_params, "int foo<int>(int)::local_type::bar");
}


/// Tests parsing member function of template instantiated with local type
BOOST_AUTO_TEST_CASE(parse_templ_mem_local_type) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "xtype str<float foo<float>(float)::local_type>::bar(int)";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "str<float foo<float>(float)::local_type>::bar(int)");
    BOOST_CHECK_EQUAL(name_no_params, "str<float foo<float>(float)::local_type>::bar");
}


// Test cases for lambdas from CXXDBG-309

BOOST_AUTO_TEST_CASE(test_parse_lambda_1) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
                 "<lambda()>()>::_M_invoke<>(std::_Index_tuple<>) const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
        "<lambda()>()>::_M_invoke<>(std::_Index_tuple<>)");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
        "<lambda()>()>::_M_invoke<>");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_2) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
                 "<lambda()>()>::operator()() const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
        "<lambda()>()>::operator()()");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::_Bind_simple<cxxdbg::core::debugger::debugger(cxxdbg::async::event_queue&)::"
        "<lambda()>()>::operator()");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_3) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::thread::_State_impl<std::_Bind_simple<cxxdbg::core::debugger::debugger"
                 "(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run() const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::thread::_State_impl<std::_Bind_simple<cxxdbg::core::debugger::debugger"
        "(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run()");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::thread::_State_impl<std::_Bind_simple<cxxdbg::core::debugger::debugger"
        "(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_4) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "cxxdbg::async::command_executor<long unsigned int, cxxdbg::appcore::core_target_impl::"
                 "launch(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler"
                 "<long unsigned int>&)::<lambda()> >::execute(cxxdbg::async::result<unsigned long> "
                 "&const, const <lambda()> &const)";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    // NOTE: type name printer prints "const <lambda()> &const" as
    //       "const <lambda()>& const" (expected behaviour),
    //       "unsigned long" is printed as "long unsigned int"

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "cxxdbg::async::command_executor<long unsigned int, cxxdbg::appcore::core_target_impl::"
        "launch(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler"
        "<long unsigned int>&)::<lambda()> >::execute(cxxdbg::async::result<long unsigned int>"
        "& const, const <lambda()>& const)");
    BOOST_CHECK_EQUAL(name_no_params,
        "cxxdbg::async::command_executor<long unsigned int, cxxdbg::appcore::core_target_impl::"
        "launch(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler"
        "<long unsigned int>&)::<lambda()> >::execute");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_5) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "cxxdbg::async::command<long unsigned int, cxxdbg::appcore::core_target_impl::launch"
                 "(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler<long unsigned int>&)::"
                 "<lambda()>, cxxdbg::appcore::core_target_impl::launch(const cxxdbg::app::launch_options&, "
                 "cxxdbg::async::result_handler<long unsigned int>&)::<lambda(const auto:1&)>, false>::"
                 "execute(cxxdbg::async::event_queue &const) const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    // NOTE: type name printer prints "cxxdbg::async::event_queue &const" as
    //       "cxxdbg::async::event_queue& const" (expected behaviour)

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "cxxdbg::async::command<long unsigned int, cxxdbg::appcore::core_target_impl::launch"
        "(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler<long unsigned int>&)::"
        "<lambda()>, cxxdbg::appcore::core_target_impl::launch(const cxxdbg::app::launch_options&, "
        "cxxdbg::async::result_handler<long unsigned int>&)::<lambda(const auto:1&)>, false>::"
        "execute(cxxdbg::async::event_queue& const)");
    BOOST_CHECK_EQUAL(name_no_params,
        "cxxdbg::async::command<long unsigned int, cxxdbg::appcore::core_target_impl::launch"
        "(const cxxdbg::app::launch_options&, cxxdbg::async::result_handler<long unsigned int>&)::"
        "<lambda()>, cxxdbg::appcore::core_target_impl::launch(const cxxdbg::app::launch_options&, "
        "cxxdbg::async::result_handler<long unsigned int>&)::<lambda(const auto:1&)>, false>::"
        "execute");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_6) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
                 "(cxxdbg::async::event_queue&)::<lambda()>()>::_M_invoke<>(std::_Index_tuple<>) const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
        "(cxxdbg::async::event_queue&)::<lambda()>()>::_M_invoke<>(std::_Index_tuple<>)");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
        "(cxxdbg::async::event_queue&)::<lambda()>()>::_M_invoke<>");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_7) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
                 "(cxxdbg::async::event_queue&)::<lambda()>()>::operator()() const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
        "(cxxdbg::async::event_queue&)::<lambda()>()>::operator()()");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::core_debugger_impl"
        "(cxxdbg::async::event_queue&)::<lambda()>()>::operator()");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_8) {
    std::string name_no_ret_type;
    std::string name_no_params;
    auto fname = "std::thread::_State_impl<std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::"
                 "core_debugger_impl(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run() const";
    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "std::thread::_State_impl<std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::"
        "core_debugger_impl(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run()");
    BOOST_CHECK_EQUAL(name_no_params,
        "std::thread::_State_impl<std::_Bind_simple<cxxdbg::appcore::core_debugger_impl::"
        "core_debugger_impl(cxxdbg::async::event_queue&)::<lambda()>()> >::_M_run");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_9) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "QtPrivate::FunctorCall<QtPrivate::IndexesList<>, QtPrivate::List<>, void, "
                 "CxxdbgApplication::initActions()::<lambda()> >::call(<lambda()>, void **)";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    // NOTE: "void **" is printed as "void**" (expected behavior)

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "QtPrivate::FunctorCall<QtPrivate::IndexesList<>, QtPrivate::List<>, void, "
        "CxxdbgApplication::initActions()::<lambda()> >::call(<lambda()>, void**)");
    BOOST_CHECK_EQUAL(name_no_params,
        "QtPrivate::FunctorCall<QtPrivate::IndexesList<>, QtPrivate::List<>, void, "
        "CxxdbgApplication::initActions()::<lambda()> >::call");
}


BOOST_AUTO_TEST_CASE(test_parse_lambda_10) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "QtPrivate::Functor<CxxdbgApplication::initActions()::<lambda()>, 0>::"
                 "call<QtPrivate::List<>, void>(<lambda()> &const, void *, void **)";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);

    // NOTE: "void **" is printed as "void**" (expected behavior)
    //       "<lambda()> &const" is printed as "<lambda()>& const (expected behavior)

    BOOST_CHECK_EQUAL(name_no_ret_type,
        "QtPrivate::Functor<CxxdbgApplication::initActions()::<lambda()>, 0>::"
        "call<QtPrivate::List<>, void>(<lambda()>& const, void*, void**)");
    BOOST_CHECK_EQUAL(name_no_params,
        "QtPrivate::Functor<CxxdbgApplication::initActions()::<lambda()>, 0>::"
        "call<QtPrivate::List<>, void>");
}


/// Tests parsing operator() function name
BOOST_AUTO_TEST_CASE(test_parse_operator_call) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "foo::operator()";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "foo::operator()()");
    BOOST_CHECK_EQUAL(name_no_params, "foo::operator()");
}


/// Tests parsing operator() function name with template parameters
BOOST_AUTO_TEST_CASE(test_parse_operator_call_templ) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "foo::operator()<int>";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "foo::operator()<int>()");
    BOOST_CHECK_EQUAL(name_no_params, "foo::operator()<int>");
}


/// Tests parsing function name containing '(anonymous namespace)' (CXXDBG-322)
BOOST_AUTO_TEST_CASE(parse_anon_namespace) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "templ<(anonymous namespace)::str>::foo<int>(int) const";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "templ<(anonymous namespace)::str>::foo<int>(int)");
    BOOST_CHECK_EQUAL(name_no_params, "templ<(anonymous namespace)::str>::foo<int>");
}


/// Tests parsing function with template parameter of function type
BOOST_AUTO_TEST_CASE(parse_std_bind_result) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "foo<void (*())()>";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "foo<void (*())()>()");
    BOOST_CHECK_EQUAL(name_no_params, "foo<void (*())()>");
}


/// Tests parsing lambda name from CXXDBG-354
BOOST_AUTO_TEST_CASE(cxxdbg_354) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "auto main::'lambda'(int, int, int)::operator()<int>(int, int, int) const";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "main::'lambda'(int, int, int)::operator()<int>(int, int, int)");
    BOOST_CHECK_EQUAL(name_no_params, "main::'lambda'(int, int, int)::operator()<int>");
}



/// Tests parsing function decl from CXXDBG-363
BOOST_AUTO_TEST_CASE(cxxdbg_363) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "cxxdbg::cm::(anonymous namespace)::type_name_parser::parse() const";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "cxxdbg::cm::(anonymous namespace)::type_name_parser::parse()");
    BOOST_CHECK_EQUAL(name_no_params, "cxxdbg::cm::(anonymous namespace)::type_name_parser::parse");
}


/// Tests parsing function name from CXXDBG-346
BOOST_AUTO_TEST_CASE(cxxdbg_346_1) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "::MonitorChildProcessThreadFunction(void *)";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "MonitorChildProcessThreadFunction(void*)");
    BOOST_CHECK_EQUAL(name_no_params, "MonitorChildProcessThreadFunction");
}


/// Tests parsing function name from CXXDBG-346
BOOST_AUTO_TEST_CASE(cxxdbg_346_2) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "::GetCompleteQualType(clang::ASTContext *, clang::QualType, bool)";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "GetCompleteQualType(clang::ASTContext*, clang::QualType, bool)");
    BOOST_CHECK_EQUAL(name_no_params, "GetCompleteQualType");
}


/// Tests parsing allocator ctor from CXXDBG-331
BOOST_AUTO_TEST_CASE(allocator_ctor_cxxdbg_331) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "std::allocator<int>::()";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "std::allocator<int>::()");
    BOOST_CHECK_EQUAL(name_no_params, "std::allocator<int>::");
}


/// Tests parsing allocator dtor from CXXDBG-331
BOOST_AUTO_TEST_CASE(allocator_dtor_cxxdbg_331) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "std::allocator<int>::~()";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "std::allocator<int>::~()");
    BOOST_CHECK_EQUAL(name_no_params, "std::allocator<int>::~");
}


/// Tests parsing operator!= from CXXDBG-331
BOOST_AUTO_TEST_CASE(operator_ne_cxxdbg_331) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "bool __gnu_cxx::operator!=<int*, std::vector<int, std::allocator<int> > >(__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > > const&, __gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > > const&)";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "__gnu_cxx::operator!=<int*, std::vector<int, std::allocator<int> > >(const __gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >&, const __gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >&)");
    BOOST_CHECK_EQUAL(name_no_params, "__gnu_cxx::operator!=<int*, std::vector<int, std::allocator<int> > >");
}


/// Tests parsing operator* from CXXDBG-331
BOOST_AUTO_TEST_CASE(operator_deref_cxxdbg_331) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator*() const";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator*()");
    BOOST_CHECK_EQUAL(name_no_params, "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator*");
}


/// Tests parsing operator++ from CXXDBG-331
BOOST_AUTO_TEST_CASE(operator_post_inc_cxxdbg_331) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator++()";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator++()");
    BOOST_CHECK_EQUAL(name_no_params, "__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >::operator++");
}


/// Tests parsing $_X lambda
BOOST_AUTO_TEST_CASE(parse_dollar_lambda) {
    std::string name_no_ret_type;
    std::string name_no_params;

    auto fname = "main::$_0::operator()(int, int) const";

    auto res = parse_function_name(fname, name_no_ret_type, name_no_params);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(name_no_ret_type, "main::$_0::operator()(int, int)");
    BOOST_CHECK_EQUAL(name_no_params, "main::$_0::operator()");
}


BOOST_AUTO_TEST_SUITE_END()


}
