
/// \file type_name_lexer.cpp
/// Contains implementation of the type_name lexer class.

#include <cxxtn/type_name_lexer.hpp>
#include <cassert>
#include <cctype>
#include <sstream>


namespace cxxtn {


static const std::string kind_names[] = {
    "<eof>",
    "<string>",
    "<error>",
    "(",
    ")",
    "[",
    "]",
    "<",
    ">",
    "*",
    "&",
    ",",
    "::"
};


type_name_lexer::token::token(kind_t k, const std::string & val):
k_{k},
val_{val} {
}


type_name_lexer::token::kind_t type_name_lexer::token::kind() const {
    return k_;
}


const std::string & type_name_lexer::token::value() const {
    return val_;
}


const std::string & type_name_lexer::token::kind_to_string(kind_t k) {
    std::size_t index = static_cast<std::size_t>(k);
    assert(index < sizeof(kind_names) / sizeof(kind_names[0]) &&
           "No token kind in name table");

    return kind_names[index];
}


type_name_lexer::type_name_lexer(std::istream & str):
str_{str} {
}


const type_name_lexer::token & type_name_lexer::tok() const {
    assert(!tokens_.empty() && "no current token");
    return tokens_.top();
}


type_name_lexer::token type_name_lexer::next() {
    if (!tokens_.empty()) {
        tokens_.pop();
    }

    // parsing next tokens if stack is empty
    if (tokens_.empty()) {
        tokens_.push(read_next());
    }

    return tok();
}


void type_name_lexer::put(const token & tok) {
    tokens_.push(tok);
}


type_name_lexer::token type_name_lexer::read_next() {
    std::string tok_str;

    while (true) {
        // reading character
        char c = peek_next_char();

        // checking for EOF
        if (!is_good()) {
            if (tok_str.empty()) {
                return {token::kind_eof, {}};
            } else {
                return {token::kind_string, tok_str};
            }
        }

        // checking for string character
        if (std::isalnum(c) || c == '_' || c == '$' ||
            // special characters inside decltype
            c == '{' || c == '}' || c == '#' || c == '.' ||
            // characters used for 'lambda'
            c == '\'' ||
            // dtor character
            c == '~' ||
            // characters used only in operator names
            c == '!' || c == '=' || c == '+' || c == '-' || c == '/') {

            // string character
            tok_str.push_back(c);
            get_next_char();
            continue;
        }

        // checking for single : character (for abi:cxx11 tag)
        // or {}#. characters in decltype
        if (c == ':') {
            get_next_char();
            char c2 = peek_next_char();
            if (!is_good() || c2 != ':') {
                tok_str.push_back(c);

                // continue parsing string
                continue;
            }

            // scope token reached
            if (tok_str.empty()) {
                get_next_char();        // skip second ':'
                return {token::kind_scope, "::"};
            } else {
                // we need put back the first ':' character
                // to be able to read scope token at the next time
                put_char();
                return {token::kind_string, tok_str};
            }
        }

        // checking for end of string
        if (!tok_str.empty()) {
            // end of string
            return {token::kind_string, tok_str};
        }

        get_next_char();

        // checking for space
        if (c == ' ')
            continue;

        // special character

        switch (c) {
        case '(': return {token::kind_lparen, "("};
        case ')': return {token::kind_rparen, ")"};
        case '[': return {token::kind_lbracket, "["};
        case ']': return {token::kind_rbracket, "]"};
        case '<': return {token::kind_langle, "<"};
        case '>': return {token::kind_rangle, ">"};
        case '*': return {token::kind_star, "*"};
        case '&': return {token::kind_and, "&"};
        case ',': return {token::kind_comma, ","};

        default: {
            std::ostringstream msg;
            msg << "unknown special character '" << c << "' in type name";
            return {token::kind_error, msg.str()};
        }
        }
    }
}


char type_name_lexer::get_next_char() {
    if (first_char_exists_) {
        first_char_exists_ = false;
        return first_char_;
    }

    first_char_ = static_cast<char>(str_.get());
    return first_char_;
}


char type_name_lexer::peek_next_char() {
    if (first_char_exists_) {
        return first_char_;
    }

    return static_cast<char>(str_.peek());
}


void type_name_lexer::put_char() {
    assert(!first_char_exists_ && "can't put character more than once");
    first_char_exists_ = true;
}


bool type_name_lexer::is_good() const {
    if (first_char_exists_)
        return true;

    return str_.good();
}


}
