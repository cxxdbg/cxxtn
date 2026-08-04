
/// \file type_name_lexer.hpp
/// Contains definition of the type_name_lexer class.

#pragma once

#include <istream>
#include <stack>


namespace cxxtn {


/// Lexer for C++ type names
class type_name_lexer {
public:
    /// Lexer token
    class token {
    public:
        /// Token kind
        enum kind_t {
            kind_eof,       ///< end of file
            kind_string,    ///< string
            kind_error,     ///< error
            kind_lparen,    ///< (
            kind_rparen,    ///< )
            kind_lbracket,  ///< [
            kind_rbracket,  ///< ]
            kind_langle,    ///< <
            kind_rangle,    ///< >
            kind_star,      ///< *
            kind_and,       ///< &
            kind_comma,     ///< ,
            kind_scope      ///< ::
        };


        /// Constructor, makes token with specified kind and value
        token(kind_t k, const std::string & val);

        /// Returns token kind
        kind_t kind() const;

        /// Returns token value
        const std::string & value() const;

        /// Converts token kind to string
        static const std::string & kind_to_string(kind_t k);

    private:
        kind_t k_;          ///< Token kind
        std::string val_;   ///< Token value
    };

    /// Constructor, makes type name lexer with specified reference
    /// to input stream
    type_name_lexer(std::istream & str);

    /// Returns current token
    const token & tok() const;

    /// Reads next token from input stream and sets current token.
    /// Looks first at the stack of additional tokens
    token next();

    /// Puts token on the top of stack of additional tokens
    void put(const token & tok);

private:
    /// Reads next token from input stream
    token read_next();

    /// Puts previously read character at the beggining of the reading sequence

    /// Reads next char from input stream taking into account character previously
    /// put at the beginning. Removes character from read sequence
    char get_next_char();

    /// Reads next char from input stream taking into account character previously
    /// put at the beginning. Does not remove character from read sequence
    char peek_next_char();

    /// Puts character at the beginning of the read sequence. May be called only
    /// once after get_next_char
    void put_char();

    /// Returns true if state of input sequence is good
    bool is_good() const;

    std::istream & str_;        ///< Reference to input stream

    /// True if read_next_char should return first_char_
    bool first_char_exists_ = false;

    char first_char_ = 0;       ///< First character to read

    std::stack<token> tokens_;      ///< Stack of tokens to return first
};


}
