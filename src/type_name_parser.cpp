
/// \file type_name_parser.cpp
/// Contains implementations of functions for parsing type names.

#include <cxxtn/type_name_parser.hpp>
#include <cxxtn/type_name_lexer.hpp>
#include <cassert>
#include <sstream>


namespace cxxtn {


namespace {


/// Type name parser
class type_name_parser {
public:
    using token = type_name_lexer::token;

    /// Constructor, makes type name parser which parses type name
    /// from specified input stream
    type_name_parser(std::istream & str, bool parse_decl = false);

    /// Parses type name
    qual_type_name parse();

    /// Returns declaration name
    auto decl_name() const { return decl_name_; }

    /// Returns parse error
    auto & error() const { return error_; }

private:
    /// Parses qualified type name with specified optional
    /// user defined type name. Optionally stores parsed
    /// declaration name
    qual_type_name parse_qual_type(const type_name_sp & tname = {},
                                   scope_type_name_sp * decl_name = nullptr);

    /// Returns current lexer token
    const type_name_lexer::token & tok() const { return lex_.tok(); }

    /// Parses cv-qualified return type with specified optional
    /// user defined type name
    qual_type_name parse_return_type(const type_name_sp & tname);

    /// Parses type name with scope or pointer to member op
    /// using specified optional scope as parent
    type_name_sp parse_name_with_scope_or_ptr(const scope_name_sp & parent = {});

    /// Parses type ops and applies them to return type.
    /// Optionally fills parsed declaration name
    qual_type_name parse_ops(scope_type_name_sp & dname);

    /// Parses type ops in () parens. Returns op with no arg type.
    /// Optionally fills parsed declaration name.
    qual_type_name parse_parens(scope_type_name_sp & dname);

    /// Sets argument in chain of op types
    void set_op_types_arg(qual_type_name & type, const qual_type_name & arg);

    /// Checks that current token is equal to specified token
    /// and extracts next token
    bool consume(type_name_lexer::token::kind_t k);

    /// Parses cv_qualifiers and appends them to qual_type_name
    void parse_cv(qual_type_name & type);

    /// Parses array size
    bool parse_array_size(size_t & sz, bool lbracked_consumed = false);

    /// Parses function parameters and qualifiers with specified optional
    /// user defined type name for first parameter. Returns function type
    qual_type_name parse_func(const type_name_sp & first);

    /// Parses template parameters list. Returns template type
    std::shared_ptr<template_type_name>
    parse_template(const basic_type_name & name);

    /// Parses decltype expression
    std::shared_ptr<decltype_type_name> parse_decltype();

    /// Parses all input until ending bracket taking into account nested
    /// brackets. Writes all parsed input to output stream
    bool parse_until_end_bracket(token::kind_t begin_kind,
                                 token::kind_t end_kind,
                                 std::ostream & str);

    /// Parses <lambda...> type name
    std::shared_ptr<basic_type_name> parse_lambda(const scope_name_sp & parent);

    /// Parses 'lambda'(...) type name
    std::shared_ptr<basic_type_name> parse_quoted_lambda(const scope_name_sp & parent);

    /// Parses (anonymous namesapce) scope. Return pointer to parsed type.
    std::shared_ptr<basic_type_name> parse_anon_ns(const scope_name_sp & parent);

    /// Tries parse (anonymous namespace) scope. Returns pointer to parsed
    /// type of empty pointer if type can not be parsed.
    std::shared_ptr<basic_type_name> try_parse_anon_ns(const scope_name_sp & parent);

    /// Parses basic type name
    std::shared_ptr<basic_type_name> parse_basic(const scope_name_sp & parent);

    /// Parses builtin type for single token
    std::shared_ptr<builtin_type_name> try_parse_builtin();


    type_name_lexer lex_;           ///< Type name lexer
    std::string error_;             ///< Error message
    bool parse_decl_name_;          ///< Parse declaration name?
    scope_type_name_sp decl_name_;  ///< Declaration name
};


type_name_parser::type_name_parser(std::istream & str, bool parse_decl):
lex_{str},
parse_decl_name_{parse_decl} {
}


qual_type_name type_name_parser::parse() {
    // reading first token
    lex_.next();

    // parsing qualified type name
    auto dname_ptr = parse_decl_name_ ? &decl_name_ : nullptr;
    qual_type_name t = parse_qual_type({}, dname_ptr);

    // checking that eof is reached
    if (lex_.tok().kind() != type_name_lexer::token::kind_eof)
        return {};

    return t;
}


qual_type_name type_name_parser::parse_qual_type(const type_name_sp & tname,
                                                 scope_type_name_sp * decl_name) {
    // parsing return type
    qual_type_name ret_type = parse_return_type(tname);
    if (!ret_type.is_valid())
        return {};

    // parsing ops
    scope_type_name_sp dname;
    qual_type_name ops = parse_ops(dname);
    if (!ops.is_valid() && !error_.empty())
        return {};

    // checking for scope token after function type
    auto ftype = std::dynamic_pointer_cast<function_type_name>(ops.type_sp());
    if (ftype && tok().kind() == type_name_lexer::token::kind_scope) {
        // we have parsed function type with "::" after it. We
        // interpret it as function scope for local type. Parsed
        // return type should be included in function declararation.

        if (!dname) {
            // if declaration was not parsed then we are trying interpret
            // return type as declaration name

            dname = std::dynamic_pointer_cast<scope_type_name>(ret_type.type_sp());
            if (dname && !ret_type.is_const() && !ret_type.is_volatile()) {
                ret_type = {};
            } else {
                error_ = "got '::' after function type";
                return {};
            }
        }

        // creating function declaration
        ftype->set_ret_type(ret_type);
        auto fdecl = std::make_shared<function_decl>(dname, ftype);

        // skipping :: scope token
        lex_.next();

        // parsing rest of return type
        auto ret_type = parse_name_with_scope_or_ptr(fdecl);
        if (!ret_type) {
            return {};
        }

        // parsing qual type using parsed first part of return type
        return parse_qual_type(ret_type, decl_name);
    }


    if (decl_name != nullptr) {
        // we need parse declaration name
        if (dname) {
            *decl_name = dname;
        } else {
            // We parsed type but need parse declaration name.
            // If type is function type then it can be interpreted
            // as function declaration without return type.
            // (return type is used as declaration name)

            // first checking that ret_type we are going to use as declaration
            // name does is a scope_type_name without cv-qualifiers
            auto new_dname = std::dynamic_pointer_cast<scope_type_name>(ret_type.type_sp());
            if (new_dname &&
                !ret_type.is_const() &&
                !ret_type.is_volatile()) {

                if (!ops.type()) {
                    // we have not parsed any ops, only function name.
                    // Using return type as declaration name and making
                    // function type with no return type
                    auto ftype = std::make_shared<function_type_name>(qual_type_name{});
                    *decl_name = new_dname;
                    return {ftype};

                } else if (auto fn_type = dynamic_cast<function_type_name*>(ops.type())) {
                    // we have parsed function type. Use it's return type as
                    // declaration name
                    fn_type->set_ret_type({});

                    *decl_name = new_dname;
                    return ops;
                }
            }

            error_ = "expected declaration but parsed type";
            return {};
        }
    } else {
        // we need parse type
        if (dname) {
            std::ostringstream msg;
            msg << "expected type but parsed declaration name '";
            dname->write(msg, type_name::write_style::gcc);
            msg << "'";
            error_ = msg.str();
            return {};
        }
    }

    // setting ret type in ops chain
    if (ops.is_valid()) {
        set_op_types_arg(ops, ret_type);
        ret_type = ops;
    }

    return ret_type;
}


qual_type_name type_name_parser::parse_return_type(const type_name_sp & tname) {
    bool is_const = false;
    bool is_volatile = false;
    bool is_unsigned = false;
    bool is_short = false;
    type_name_sp type = tname;
    unsigned int num_long = 0;

    while (true) {

        if (tok().kind() == type_name_lexer::token::kind_string) {
            if (tok().value() == "const") {
                if (is_const) {
                    error_ = "double const is not allowed";
                    return {};
                }

                is_const = true;
                lex_.next();
            } else if (tok().value() == "volatile") {
                if (is_volatile) {
                    error_ = "double volatile is not allowed";
                    return {};
                }

                is_volatile = true;
                lex_.next();
            } else if (tok().value() == "unsigned") {
                if (is_unsigned) {
                    error_ = "double unsigned is not allowed";
                    return {};
                }

                is_unsigned = true;
                lex_.next();

            } else if (tok().value() == "long") {
                if (is_short) {
                    error_ = "can't use long with short";
                    return {};
                } else if (num_long == 2) {
                    error_ = "triple long is not allowed";
                    return {};
                }

                ++num_long;
                lex_.next();

            } else if (tok().value() == "short") {
                if (num_long != 0) {
                    error_ = "can't use short with long";
                    return {};
                } else if (is_short) {
                    error_ = "double short is not allowed";
                    return {};
                }

                is_short = true;
                lex_.next();

            } else {

                if (!type) {

                    if (tok().value() == "decltype") {
                        // parsing decltype
                        type = parse_decltype();
                        if (!type) {
                            return {};
                        }

                    } else {
                        // trying parse builtin type name
                        type = try_parse_builtin();

                        if (!type) {
                            // parsing type name with scope
                            type = parse_name_with_scope_or_ptr();
                            if (!type)
                                return {};

                            // checking that type is not pointer to member
                            if (dynamic_cast<mem_ptr_type_name*>(type.get())) {
                                error_ = "expected type name, got pointer-to-member";
                                return {};
                            }
                        }
                    }
                } else {
                    // type already parsed, we reached end of result type,
                    // the next token is probably begin of pointer to member op
                    break;
                }
            }

        } else if (tok().kind() == token::kind_scope) {
            // type name with :: prefix

            if (!type) {
                // parsing type name with scope
                type = parse_name_with_scope_or_ptr();
                if (!type)
                    return {};

                // checking that type is not pointer to member
                if (dynamic_cast<mem_ptr_type_name*>(type.get())) {
                    error_ = "expected type name, got pointer-to-member";
                    return {};
                }

            } else {
                // type already parsed, we reached end of result type,
                // the next token is probably begin of pointer to member op
                break;
            }

        } else if (tok().kind() == type_name_lexer::token::kind_langle) {

            // start of <lambda...> type

            // parsing type name with scope
            type = parse_name_with_scope_or_ptr();
            if (!type) {
                return {};
            }

            // checking that type is not pointer to member
            if (dynamic_cast<mem_ptr_type_name*>(type.get())) {
                error_ = "expected type name, got pointer-to-member";
                return {};
            }

        } else if (tok().kind() == type_name_lexer::token::kind_lparen) {

            // possible '(anonymous namespace)' scope
            auto anon_ns = try_parse_anon_ns({});
            if (!anon_ns) {
                // end of type
                break;
            }

            // parsing rest of scoped type name

            if (!consume(type_name_lexer::token::kind_scope)) {
                return {};
            }

            type = parse_name_with_scope_or_ptr(anon_ns);
            if (!type) {
                return {};
            }

        } else {
            // end of type reached
            break;
        }
    }


    // implicit int type declaration
    if (!type && (is_unsigned || is_short || num_long > 0)) {
        type = std::make_shared<builtin_type_name>(builtin_type_name::kind_t::int_);
    }


    // checking that we parsed something
    if (!type) {
        error_ = "can't parse result qual type";
        return {};
    }


    // builtin type conversion
    if (is_unsigned || is_short || num_long > 0) {
        // checking that we parsed builtin type
        auto bt = dynamic_cast<builtin_type_name*>(type.get());
        if (!bt) {
            std::ostringstream msg;
            msg << "can't use unsigned, short, or long with type '";
            type->write(msg, type_name::write_style::gcc);
            msg << "'";
            error_ = msg.str();
            return {};
        }

        // converting type to short or long
        if (is_short || num_long > 0) {
            if (bt->kind() != builtin_type_name::kind_t::int_) {
                std::ostringstream msg;
                msg << "can't use short or long with type '";
                type->write(msg, type_name::write_style::gcc);
                msg << "'";
                error_ = msg.str();
                return {};
            }

            if (is_short) {
                bt->set_kind(builtin_type_name::kind_t::short_);
            } else if (num_long == 1) {
                bt->set_kind(builtin_type_name::kind_t::long_);
            } else if (num_long == 2) {
                bt->set_kind(builtin_type_name::kind_t::long_long_);
            }
        }

        // converting type to unsigned
        if (is_unsigned) {
            switch (bt->kind()) {
            case builtin_type_name::kind_t::char_:
                bt->set_kind(builtin_type_name::kind_t::unsigned_char_);
                break;
            case builtin_type_name::kind_t::short_:
                bt->set_kind(builtin_type_name::kind_t::unsigned_short_);
                break;
            case builtin_type_name::kind_t::int_:
                bt->set_kind(builtin_type_name::kind_t::unsigned_int_);
                break;
            case builtin_type_name::kind_t::long_:
                bt->set_kind(builtin_type_name::kind_t::unsigned_long_);
                break;
            case builtin_type_name::kind_t::long_long_:
                bt->set_kind(builtin_type_name::kind_t::unsigned_long_long_);
                break;
            default:
                {
                std::ostringstream msg;
                msg << "can't use unsigned with type '";
                bt->write(msg, type_name::write_style::gcc);
                msg << "'";
                error_ = msg.str();
                return {};
                }
            }
        }
    }

    return {type, is_const, is_volatile};
}


// Returns true if token is start of name with scope or mem ptr
static bool is_start_of_name_or_ptr(const type_name_lexer::token & tok) {
    return tok.kind() == type_name_lexer::token::kind_string ||
           tok.kind() == type_name_lexer::token::kind_langle ||
           tok.kind() == type_name_lexer::token::kind_scope;
}


type_name_sp
type_name_parser::parse_name_with_scope_or_ptr(const scope_name_sp & parent) {

    if (!is_start_of_name_or_ptr(tok())) {
        std::ostringstream msg;
        msg << "expected string, '::', or '<', got '" << tok().value() << "'";
        error_ = msg.str();
        return {};
    }

    // skipping :: prefix
    if (tok().kind() == token::kind_scope) {
        lex_.next();
    }

    std::shared_ptr<scope_type_name> scope = parse_basic(parent);
    if (!scope) {
        return {};
    }

    while (true) {
        if (tok().kind() == type_name_lexer::token::kind_scope) {
            // sope, checking next token

            lex_.next();
            if (tok().kind() == token::kind_string ||
                tok().kind() == token::kind_langle ||
                tok().kind() == token::kind_lparen) {
                // next scope part

                scope = parse_basic(scope);
                if (!scope) {
                    return {};
                }
            } else if (tok().kind() == type_name_lexer::token::kind_star) {
                // poiner to member
                lex_.next();
                return {mem_ptr_type_name::make(scope, {})};
            } else {
                std::ostringstream msg;
                msg << "expected string, '<' or '*' after ::, got '"
                    << tok().value() << "'";
                error_ = msg.str();
                return {};
            }

        } else if (tok().kind() == type_name_lexer::token::kind_langle) {
            // checking that last scope is basic type name
            basic_type_name * bt = dynamic_cast<basic_type_name*>(scope.get());
            if (!bt) {
                // double template parameters: <><>
                error_ = "double template parameters";
                return {};
            }

            auto templ = parse_template(*bt);
            if (!templ)
                return {};

            scope = templ;

        } else {
            // end of identifier with scope
            return scope;
        }
    }
}


qual_type_name type_name_parser::parse_ops(scope_type_name_sp & dname) {

    qual_type_name ret_type;
    qual_type_name parens_ops_type;

    while (true) {
        // checking for eof
        if (tok().kind() == type_name_lexer::token::kind_eof)
            break;

        // checking for end of type
        if (tok().kind() == type_name_lexer::token::kind_rparen ||
            tok().kind() == type_name_lexer::token::kind_rbracket ||
            tok().kind() == type_name_lexer::token::kind_rangle ||
            tok().kind() == type_name_lexer::token::kind_comma ||
            // scope after end of ops means local type inside function
            tok().kind() == type_name_lexer::token::kind_scope)
            break;

        // parsing op

        if (tok().kind() == type_name_lexer::token::kind_star) {
            // pointer
            ret_type = {pointer_type_name::make(ret_type)};
            lex_.next();
            parse_cv(ret_type);

        } else if (tok().kind() == type_name_lexer::token::kind_and) {
            // reference
            ret_type = {reference_type_name::make(ret_type)};
            lex_.next();
            parse_cv(ret_type);

        } else if (tok().kind() == type_name_lexer::token::kind_lbracket) {
            lex_.next();

            // ignoring [abi:cxx11] tags
            if (tok().kind() == type_name_lexer::token::kind_string &&
                tok().value() == "abi:cxx11") {

                lex_.next();
                consume(type_name_lexer::token::kind_rbracket);
            } else {

                // not a tag, parsing array
                std::size_t len = SIZE_MAX;
                if (!parse_array_size(len, true)) {
                    return {};
                }

                qual_type_name new_ret{array_type_name::make(ret_type.type_sp(), len),
                                       ret_type.is_const(),
                                       ret_type.is_volatile()};
                ret_type = new_ret;
            }

        } else if (tok().kind() == type_name_lexer::token::kind_lparen) {
            // () parens: function parameters or grouping type ops

            scope_type_name_sp new_dname;
            qual_type_name ops = parse_parens(new_dname);
            if (!ops.is_valid())
                return {};

            // checking if parse_ops parsed a declaration
            if (new_dname) {
                if (dname) {
                    std::ostringstream msg;
                    msg << "found second declaration: '";
                    dname->write(msg, type_name::write_style::gcc);
                    msg << "', '";
                    new_dname->write(msg, type_name::write_style::gcc);
                    msg << "'";
                    error_ = msg.str();
                    return {};
                }

                dname = new_dname;
            }

            if (parens_ops_type.is_valid()) {
                set_op_types_arg(parens_ops_type, ops);
            } else {
                parens_ops_type = ops;
            }

        } else if (tok().kind() == type_name_lexer::token::kind_string) {
            // pointer to member or declaration name

            qual_type_name op = parse_name_with_scope_or_ptr();
            if (!op.is_valid())
                return {};

            // checking that we parsed pointer to member, not type
            if (auto mptr = dynamic_cast<mem_ptr_type_name*>(op.type())) {
                // setting return type in pointer
                mptr->set_mem_type(ret_type);
                ret_type = {op};

                // parsing cv
                parse_cv(ret_type);
            } else {
                // parsed declaration name

                if (op.is_const() || op.is_volatile()) {
                    std::ostringstream msg;
                    msg << "const and volatile are not allowed with decl name '";
                    op.write(msg, type_name::write_style::gcc);
                    msg << "'";
                    error_ = msg.str();
                    return {};
                }

                auto parsed_decl = std::dynamic_pointer_cast<scope_type_name>(op.type_sp());
                assert(parsed_decl && "parse_name_with_scope_or_ptr returned invalid type");

                if (dname) {
                    std::ostringstream msg;
                    msg << "double declaration name is not allowed: '";
                    op.write(msg, type_name::write_style::gcc);
                    msg << "'";
                    error_ = msg.str();
                    return {};
                }

                dname = parsed_decl;
            }

        } else {
            std::ostringstream msg;
            msg << "unknown op: '" << tok().value() << "'";
            error_ = msg.str();
            return {};
        }
    }

    // setting arg in ops inside ()
    if (parens_ops_type.is_valid()) {
        if (ret_type.is_valid()) {
            set_op_types_arg(parens_ops_type, ret_type);

        }

        ret_type = parens_ops_type;
    }

    return ret_type;
}


qual_type_name type_name_parser::parse_parens(scope_type_name_sp & dname) {
    assert(tok().kind() == type_name_lexer::token::kind_lparen &&
           "invalid first parens token");
    lex_.next();

    // checking for empty parens
    if (tok().kind() == type_name_lexer::token::kind_rparen) {
        // function with no parameters
        return parse_func({});
    }

    qual_type_name op_type;

    // detecting if this is function parameters
    if (tok().kind() == type_name_lexer::token::kind_langle) {
        // lambda function parameter
        auto lambda = parse_lambda({});
        if (!lambda) {
            return {};
        }

        return parse_func(lambda);

    } else if (tok().kind() == type_name_lexer::token::kind_string) {
        // may be function parameter or member pointer

        const std::string val = tok().value();
        if (val == "const" ||
            val == "volatile" ||
            val == "unsigned" ||
            val == "long" ||
            val == "short") {

            return parse_func({});
        }

        // trying parse builtin type
        auto bt = try_parse_builtin();
        if (bt) {
            return parse_func(bt);
        }

        // trying parse name with scope
        type_name_sp type = parse_name_with_scope_or_ptr();
        if (!dynamic_cast<mem_ptr_type_name*>(type.get())) {
            return parse_func(type);
        }

        // pointer to member
        op_type = {type};
        parse_cv(op_type);
    }

    // parsing ops
    qual_type_name ops = parse_ops(dname);
    if (!ops.is_valid() && !error_.empty())
        return {};

    // setting ops arg if first op is laready parsed
    if (ops.is_valid()) {
        if (op_type.is_valid()) {
            set_op_types_arg(ops, op_type);
        }
        op_type = ops;
    }

    // eating )
    consume(type_name_lexer::token::kind_rparen);

    assert(op_type.is_valid() && "no ops in ()");
    return op_type;
}


void type_name_parser::set_op_types_arg(qual_type_name & type,
                                        const qual_type_name & arg) {

    if (dynamic_cast<basic_type_name*>(type.type())) {
        assert(false && "can't set op type arg for basic type");
    } else if (array_type_name * at = dynamic_cast<array_type_name*>(type.type())) {
        if (at->el_type() == nullptr) {
            at->set_el_type(arg.type_sp());
            type.set_const(arg.is_const());
            type.set_volatile(arg.is_volatile());
        } else {
            qual_type_name el_cv{at->el_type_sp()};
            set_op_types_arg(el_cv, arg);
        }
    } else if (pointer_type_name * pt = dynamic_cast<pointer_type_name*>(type.type())) {
        if (pt->base().type() == nullptr) {
            pt->set_base(arg);
        } else {
            set_op_types_arg(pt->base(), arg);
        }
    } else if (reference_type_name * rt = dynamic_cast<reference_type_name*>(type.type())) {
        if (rt->base().type() == nullptr) {
            rt->set_base(arg);
        } else {
            set_op_types_arg(rt->base(), arg);
        }
    } else if (dynamic_cast<template_type_name*>(type.type())) {
        assert(false && "can't set op type arg for template type name");
    } else if (function_type_name * ft = dynamic_cast<function_type_name*>(type.type())) {
        if (ft->ret_type().type() == nullptr) {
            ft->set_ret_type(arg);
        } else {
            set_op_types_arg(ft->ret_type(), arg);
        }
    } else if (mem_ptr_type_name * mpt = dynamic_cast<mem_ptr_type_name*>(type.type())) {
        if (mpt->mem_type().type() == nullptr) {
            mpt->set_mem_type(arg);
        } else {
            set_op_types_arg(mpt->mem_type(), arg);
        }
    } else {
        assert(false && "unknown type");
    }
}


bool type_name_parser::consume(type_name_lexer::token::kind_t k) {

    if (tok().kind() == k) {
        lex_.next();
        return true;
    }

    std::ostringstream msg;
    msg << "expected '" << type_name_lexer::token::kind_to_string(k)
        << "', got '" << tok().value() << "'";
    error_ = msg.str();
    return false;
}


void type_name_parser::parse_cv(qual_type_name & type) {

    while (true) {
        if (tok().kind() != type_name_lexer::token::kind_string)
            break;

        if (tok().value() == "const") {
            type.set_const(true);
        } else if (tok().value() == "volatile") {
            type.set_volatile(true);
        } else {
            break;
        }

        lex_.next();
    }
}


bool type_name_parser::parse_array_size(size_t & sz, bool lbracked_consumed) {
    sz = SIZE_MAX;

    if (!lbracked_consumed) {
        assert(tok().kind() == type_name_lexer::token::kind_lbracket &&
               "invalid current token for parse_array_size");
        lex_.next();
    }

    if (tok().kind() == type_name_lexer::token::kind_string) {
        // array length
        std::istringstream istr{tok().value()};
        istr >> sz;

        if (istr.fail() || istr.bad() || sz == SIZE_MAX) {
            std::ostringstream msg;
            msg << "invalid array size: '" << tok().value() << "'";
            error_ = msg.str();
            return false;
        }

        lex_.next();
    }


    // checking for ]
    if (!consume(type_name_lexer::token::kind_rbracket)) {
        return false;
    }

    return true;
}


qual_type_name type_name_parser::parse_func(const type_name_sp & first) {

    auto ft = function_type_name::make({});

    // parsing first parameter
    if (first || tok().kind() != type_name_lexer::token::kind_rparen) {
        qual_type_name p = parse_qual_type(first);
        if (!p.is_valid())
            return {};

        ft->params().push_back(p);
    }

    // parsing other parameters

    while (true) {
        if (tok().kind() == type_name_lexer::token::kind_rparen) {
            // end of parameter list
            lex_.next();
            break;
        }

        // eating ,
        consume(type_name_lexer::token::kind_comma);

        // parsing parameter
        qual_type_name p = parse_qual_type({});
        if (!p.is_valid())
            return {};

        ft->params().push_back(p);
    }

    // parsing qualifiers
    qual_type_name res{ft};
    parse_cv(res);
    return res;
}


std::shared_ptr<template_type_name>
type_name_parser::parse_template(const basic_type_name & name) {
    // checking first token
    assert(tok().kind() == type_name_lexer::token::kind_langle &&
           "invalid first token for template");
    lex_.next();

    std::shared_ptr<template_type_name> tt = template_type_name::make(name);

    // parsing parameters
    bool first = true;
    while (true) {
        // checking for end of parameters
        if (tok().kind() == type_name_lexer::token::kind_rangle)
            break;

        // eating ,
        if (!first)
            consume(type_name_lexer::token::kind_comma);
        else
            first = false;

        auto par = parse_qual_type({});
        if (!par.is_valid())
            return {};

        tt->params().push_back(par);
    }

    lex_.next();
    return tt;
}


std::shared_ptr<decltype_type_name> type_name_parser::parse_decltype() {
    assert(tok().kind() == type_name_lexer::token::kind_string &&
           tok().value() == "decltype" &&
           "invalid token for decltype");

    // eating "decltype("
    lex_.next();
    consume(type_name_lexer::token::kind_lparen);

    // parsing all inside ()
    unsigned int num_parens = 1;
    std::ostringstream expr_str;
    while (true) {
        if (tok().kind() == type_name_lexer::token::kind_error) {
            std::ostringstream msg;
            msg << "lexer_error: " << tok().value();
            error_ = msg.str();
            return {};
        }

        if (tok().kind() == type_name_lexer::token::kind_eof) {
            error_ = "decltype without ending )";
            return {};
        }

        if (tok().kind() == type_name_lexer::token::kind_rparen) {
            assert(num_parens > 0 && "invalid number of parens");
            --num_parens;

            if (num_parens == 0) {
                // end of decltype()
                break;
            }
        }

        // adding current token to decltype expression
        expr_str << tok().value();

        if (tok().kind() == type_name_lexer::token::kind_lparen) {
            ++num_parens;
        }

        lex_.next();
    }

    // eating )
    lex_.next();

    return std::make_shared<decltype_type_name>(expr_str.str());
}


bool type_name_parser::parse_until_end_bracket(token::kind_t begin_kind,
                                               token::kind_t end_kind,
                                               std::ostream & str) {
    // parsing all until ending bracket
    unsigned int num_brackets = 1;
    bool prev_str = false;
    while (num_brackets != 0) {
        if (tok().kind() == token::kind_error) {
            std::ostringstream msg;
            msg << "lexer_error: " << tok().value();
            error_ = msg.str();
            return false;
        } else if (tok().kind() == token::kind_eof) {
            error_ = "lambda name without ending bracket";
            return false;
        } else if (tok().kind() == begin_kind) {
            ++num_brackets;
        } else if (tok().kind() == end_kind) {
            assert(num_brackets > 0 && "invalid num_angles value");
            --num_brackets;
        }

        // printing space between string tokens
        if (tok().kind() == token::kind_string) {
            if (prev_str) {
                str << ' ';
            }

            prev_str = true;
        } else {
            prev_str = false;
        }

        str << tok().value();

        // print space after ','
        if (tok().kind() == token::kind_comma) {
            str << ' ';
        }

        lex_.next();
    }

    return true;
}


std::shared_ptr<basic_type_name>
type_name_parser::parse_lambda(const scope_name_sp & parent) {

    assert(tok().kind() == type_name_lexer::token::kind_langle &&
           "invalid token for lambda name");

    std::ostringstream name;

    name << tok().value();
    lex_.next();

    // checking lambda
    if (tok().kind() != type_name_lexer::token::kind_string ||
        tok().value() != "lambda") {
        std::ostringstream msg;
        msg << "expected 'lambda', got '" << tok().value() << "'";
        error_ = msg.str();
        return {};
    }

    name << tok().value();
    lex_.next();

    // parsing all until ending >
    if (!parse_until_end_bracket(token::kind_langle, token::kind_rangle, name)) {
        return {};
    }

    return std::make_shared<basic_type_name>(name.str(), parent);
}


std::shared_ptr<basic_type_name>
type_name_parser::parse_quoted_lambda(const scope_name_sp & parent) {
    assert(tok().kind() == token::kind_string &&
           "invalid token for quoted lambda name");
    assert(tok().value() == "'lambda'" &&
           "invalid token value for quoted lambda name");

    std::ostringstream name;

    name << tok().value();
    lex_.next();

    // checking for '('
    if (tok().kind() != token::kind_lparen) {
        // end of lambda
        return std::make_shared<basic_type_name>(name.str(), parent);
    }

    name << tok().value();
    lex_.next();

    // parsing all until ending >
    if (!parse_until_end_bracket(token::kind_lparen, token::kind_rparen, name)) {
        return {};
    }

    return std::make_shared<basic_type_name>(name.str(), parent);
}


std::shared_ptr<basic_type_name>
type_name_parser::parse_anon_ns(const scope_name_sp & parent) {
    assert(tok().kind() == type_name_lexer::token::kind_lparen &&
           "invalid token for anon namespace name");

    auto res = try_parse_anon_ns(parent);
    if (!res) {
        error_ = "Can't parse '(anonymous namespace)'";
        return {};
    }

    return res;
}


std::shared_ptr<basic_type_name>
type_name_parser::try_parse_anon_ns(const scope_name_sp & parent) {
    assert(tok().kind() == type_name_lexer::token::kind_lparen &&
           "invalid token for anon namespace name");

    auto lparen_tok = lex_.tok();
    lex_.next();

    // checking for "anonymous"
    auto an_tok = lex_.tok();
    if (an_tok.kind() != type_name_lexer::token::kind_string ||
        an_tok.value() != "anonymous") {
        lex_.put(lparen_tok);
        return {};
    }

    lex_.next();

    // checking for "namespace"
    auto ns_tok = lex_.tok();
    if (ns_tok.kind() != type_name_lexer::token::kind_string ||
        ns_tok.value() != "namespace") {
        lex_.put(an_tok);
        lex_.put(lparen_tok);
        return {};
    }

    lex_.next();

    // checking for ")"
    if (lex_.tok().kind() != type_name_lexer::token::kind_rparen) {
        lex_.put(ns_tok);
        lex_.put(an_tok);
        lex_.put(lparen_tok);
        return {};
    }

    lex_.next();

    return std::make_shared<basic_type_name>("(anonymous namespace)", parent);
}


std::shared_ptr<basic_type_name>
type_name_parser::parse_basic(const scope_name_sp & parent) {
    if (tok().kind() == type_name_lexer::token::kind_string) {
        // checking of operators

        if (tok().value() == "operator") {
            lex_.next();
            if (lex_.tok().kind() == type_name_lexer::token::kind_lparen) {
                lex_.next();
                if (!consume(type_name_lexer::token::kind_rparen))
                    return {};

                return std::make_shared<basic_type_name>("operator()", parent);

            } else if (tok().kind() == token::kind_star) {
                lex_.next();
                return std::make_shared<basic_type_name>("operator*", parent);
            } else if (tok().kind() == token::kind_langle) {
                lex_.next();
                if (tok().kind() == token::kind_langle) {
                    lex_.next();
                    if (tok().kind() == token::kind_string && tok().value() == "=") {
                        lex_.next();
                        return std::make_shared<basic_type_name>("operator<<=", parent);
                    } else {
                        return std::make_shared<basic_type_name>("operator<<", parent);
                    }
                } else if (tok().kind() == token::kind_string && tok().value() == "=") {
                    lex_.next();
                    return std::make_shared<basic_type_name>("operator<=", parent);
                } else {
                    return std::make_shared<basic_type_name>("operator<", parent);
                }
            } else if (tok().kind() == token::kind_rangle) {
                lex_.next();
                if (tok().kind() == token::kind_rangle) {
                    lex_.next();
                    if (tok().kind() == token::kind_string && tok().value() == "=") {
                        lex_.next();
                        return std::make_shared<basic_type_name>("operator>>=", parent);
                    } else {
                        return std::make_shared<basic_type_name>("operator>>", parent);
                    }
                } else if (tok().kind() == token::kind_string && tok().value() == "=") {
                    lex_.next();
                    return std::make_shared<basic_type_name>("operator>=", parent);
                } else {
                    return std::make_shared<basic_type_name>("operator>", parent);
                }
            } else {
                std::ostringstream msg;
                msg << "unsupported operator type";
                error_ = msg.str();
                return {};
            }
        }

        // checking for 'lamblda'()
        if (tok().value() == "'lambda'") {
            return parse_quoted_lambda(parent);
        }

        // simple identifier
        auto res = std::make_shared<basic_type_name>(tok().value(), parent);
        lex_.next();
        return res;
    } else if (tok().kind() == type_name_lexer::token::kind_langle) {
        // lambda type name
        return parse_lambda(parent);
    } else if (tok().kind() == type_name_lexer::token::kind_lparen) {
        // '(anonymous namespace)'
        auto anon_ns = try_parse_anon_ns(parent);
        if (anon_ns) {
            return anon_ns;
        }

        // if parent exists then treat this as ctor with empty
        // name ("foo::()")
        if (parent) {
            return std::make_shared<basic_type_name>("", parent);
        }

        error_ = "expected string, got (";
        return {};
    } else {
        assert(false && "invalid token kind for basic type name");
        return {};
    }
}


std::shared_ptr<builtin_type_name> type_name_parser::try_parse_builtin() {
    assert(tok().kind() == type_name_lexer::token::kind_string &&
           "invalid token kind for builtin type");

    auto kind = builtin_type_name::kind_t::void_;

    if (tok().value() == "void") {
        kind = builtin_type_name::kind_t::void_;
    } else if (tok().value() == "char") {
        kind = builtin_type_name::kind_t::char_;
    } else if (tok().value() == "short") {
        kind = builtin_type_name::kind_t::short_;
    } else if (tok().value() == "int") {
        kind = builtin_type_name::kind_t::int_;
    } else if (tok().value() == "long") {
        kind = builtin_type_name::kind_t::long_;
    } else if (tok().value() == "float") {
        kind = builtin_type_name::kind_t::float_;
    } else if (tok().value() == "double") {
        kind = builtin_type_name::kind_t::double_;
    } else if (tok().value() == "wchar_t") {
        kind = builtin_type_name::kind_t::wchar_t_;
    } else {
        return {};
    }

    lex_.next();
    return std::make_shared<builtin_type_name>(kind);
}


}


qual_type_name parse_type_name(std::istream & str, std::string * err) {
    type_name_parser parser{str};
    auto res = parser.parse();
    if (err) {
        *err = parser.error();
    }

    return res;
}



void parse_declaration(std::istream & str,
                       qual_type_name & type,
                       scope_type_name_sp & decl_name) {

    type_name_parser parser{str, true};
    type = parser.parse();

    decl_name = parser.decl_name();
    if (!decl_name) {
        type.set_type({});
    }
}


void parse_declaration_no_ret(std::istream & str,
                              qual_type_name & type,
                              type_name_sp & decl_name) {
}


}
