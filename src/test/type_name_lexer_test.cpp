
/// \file type_name_lexer_test.cpp
/// Contains unit tests for the type_name_lexer class.

#include <cxxtn/type_name_lexer.hpp>
#include <sstream>
#include <boost/test/unit_test.hpp>


namespace cxxtn::test {


BOOST_AUTO_TEST_SUITE(type_name_lexer_test)


/// Tests lexing EOF
BOOST_AUTO_TEST_CASE(test_eof) {
    std::istringstream str{""};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_eof);
}


/// Tests eating whitespaces
BOOST_AUTO_TEST_CASE(test_ws) {
    std::istringstream str{" "};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_eof);
}


/// Tests lexing string
BOOST_AUTO_TEST_CASE(test_string) {
    std::istringstream str{"abc"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "abc");
}


/// Tests lexing string with underline
BOOST_AUTO_TEST_CASE(test_string_underline) {
    std::istringstream str{"abc_def"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "abc_def");
}



/// Tests lexing single token
#define TEST_SINGLE_TOKEN(k, string) \
    BOOST_AUTO_TEST_CASE(test_single_##k) { \
        std::istringstream str{string}; \
        type_name_lexer lex{str}; \
        auto tok = lex.next(); \
        BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_##k); \
    }


TEST_SINGLE_TOKEN(lparen, "(")
TEST_SINGLE_TOKEN(rparen, ")")
TEST_SINGLE_TOKEN(lbracket, "[")
TEST_SINGLE_TOKEN(rbracket, "]")
TEST_SINGLE_TOKEN(langle, "<")
TEST_SINGLE_TOKEN(rangle, ">")
TEST_SINGLE_TOKEN(star, "*")
TEST_SINGLE_TOKEN(and, "&")
TEST_SINGLE_TOKEN(comma, ",")
TEST_SINGLE_TOKEN(scope, "::")


/// Tests error lexing unknown symbol
#define TEST_SINGLE_TOKEN_ERROR(name, string) \
    BOOST_AUTO_TEST_CASE(test_single_error_##name) { \
        std::istringstream str{string}; \
        type_name_lexer lex{str}; \
        auto tok = lex.next(); \
        BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_error); \
    }


/// Tests stop lexing strin at character
#define TEST_STOP_LEX_STRING(name, s) \
    BOOST_AUTO_TEST_CASE(test_stop_lex_string_##name) { \
        std::istringstream str{"string" s}; \
        type_name_lexer lex{str}; \
        auto tok = lex.next(); \
        BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string); \
        BOOST_CHECK(tok.value() == "string"); \
    }


TEST_STOP_LEX_STRING(space, " ")
TEST_STOP_LEX_STRING(lparen, "(")
TEST_STOP_LEX_STRING(rparen, ")")
TEST_STOP_LEX_STRING(lbracket, "[")
TEST_STOP_LEX_STRING(rbracket, "]")
TEST_STOP_LEX_STRING(langle, "<")
TEST_STOP_LEX_STRING(rangle, ">")
TEST_STOP_LEX_STRING(star, "*")
TEST_STOP_LEX_STRING(and, "&")
TEST_STOP_LEX_STRING(comma, ",")
TEST_STOP_LEX_STRING(scope, "::")


// Tests parsing abi:cxx11 string
BOOST_AUTO_TEST_CASE(lex_abi_cxx11) {
    std::istringstream str{"abi:cxx11"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "abi:cxx11");
}


// Tests parsing : at the end
BOOST_AUTO_TEST_CASE(lex_abi_end_colon) {
    std::istringstream str{"abi:"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "abi:");
}


// Tests parsing : at the begining
BOOST_AUTO_TEST_CASE(lex_abi_begin_colon) {
    std::istringstream str{":cxx"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == ":cxx");
}



// Tests parsing :
BOOST_AUTO_TEST_CASE(lex_abi_colon) {
    std::istringstream str{":"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == ":");
}


/// Tests parsing string containing "'" character
BOOST_AUTO_TEST_CASE(lex_single_quote) {
    std::istringstream str{"aaa'bbb"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "aaa'bbb");
}


/// Tests parsing "~" string
BOOST_AUTO_TEST_CASE(lex_dtor) {
    std::istringstream str{"~"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "~");
}


/// Tests parsing "!" string (for operators decl)
BOOST_AUTO_TEST_CASE(lex_not) {
    std::istringstream str{"!"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "!");
}


/// Tests parsing "=" string (for operators decl)
BOOST_AUTO_TEST_CASE(lex_assign) {
    std::istringstream str{"="};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "=");
}


/// Tests parsing "$" string (allowed in identifier names)
BOOST_AUTO_TEST_CASE(lex_dollar) {
    std::istringstream str{"$"};
    type_name_lexer lex{str};
    auto tok = lex.next();
    BOOST_CHECK(tok.kind() == type_name_lexer::token::kind_string);
    BOOST_CHECK(tok.value() == "$");
}


BOOST_AUTO_TEST_SUITE_END()


}
