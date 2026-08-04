
/// \file type_name.hpp
/// Contains definition of type name classes.

#pragma once

#include <memory>
#include <ostream>
#include <vector>


namespace cxxtn {


class type_name;


/// Write style (gcc or clang)
enum class type_name_write_style {
    gcc,
    clang
};


/// Type of shared pointer to type name
typedef std::shared_ptr<type_name> type_name_sp;


/// Represents CV-qualified type name
class qual_type_name {
public:
    /// Constructor, makes qualified type
    qual_type_name(const type_name_sp & t = type_name_sp{},
                   bool c = false,
                   bool v = false);

    /// Returns true if type is valid
    bool is_valid() const;

    /// Returns pointer to type name
    type_name * type() const;

    /// Returns shared pointer ty type name
    std::shared_ptr<type_name> type_sp() const;

    /// Sets type name
    void set_type(const type_name_sp & t);

    /// Returns true if type has const qualifier
    bool is_const() const;

    /// Sets const qualifier
    void set_const(bool v);

    /// Returns true if type has volatile qualifier
    bool is_volatile() const;

    /// Sets volatile qualifier
    void set_volatile(bool v);

    /// Writes qualifiers to output stream
    void write_qual(std::ostream & str,
                    bool first_space,
                    bool last_space) const;

    /// Writes type name to output stream
    void write(std::ostream & str, type_name_write_style style) const;

    /// Writes result to output stream
    void write_result(std::ostream & str, type_name_write_style style) const;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, type_name_write_style style) const;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool parent_op_is_suffix, type_name_write_style style) const;

private:
    type_name_sp type_;         ///< Shared pointer to type name
    bool is_const_;             ///< Is volatile qualifier
    bool is_volatile_;          ///< Is const qualifier
};



/// Represents name of type
class type_name {
public:
    using write_style = type_name_write_style;

    /// Destructor, destroys object
    virtual ~type_name();

    /// Writes type name to output stream
    virtual void write(std::ostream & str, write_style style) const;

    /// Writes result to output stream
    virtual void write_result(std::ostream & str, write_style style) const = 0;

    /// Writes prefix to output stream.
    /// If end_sp parameter is true then ensures that there is
    /// space or other non-alnum symbol written at the end
    virtual void write_prefix(std::ostream & str, bool end_sp, write_style style) const = 0;

    /// Writes suffix to output stream
    virtual void write_suffix(std::ostream & str, bool p_s, write_style style) const = 0;

    /// Returns true if type is prefix op
    virtual bool is_prefix_op() const = 0;

    /// Returns true if type is suffix op
    virtual bool is_suffix_op() const = 0;
};


/// Represents scope name
class scope_name {
public:
    /// Destructor, destroys object
    virtual ~scope_name() = default;

    /// Writes scope to output stream
    virtual void write(std::ostream & str, type_name::write_style style) const = 0;
};


/// Represents type name that can be used as scope for other types
class scope_type_name: public type_name, public scope_name {
public:
    /// Destructor, destroys object
    virtual ~scope_type_name() = default;

    /// Writes scope type to output stream
    void write(std::ostream & str, write_style style) const override;
};


using scope_name_sp = std::shared_ptr<scope_name>;
using scope_type_name_sp = std::shared_ptr<scope_type_name>;


/// Represents builtin type name
class builtin_type_name: public type_name {
public:
    /// Builtin type kind
    enum class kind_t {
        void_,
        char_,
        short_,
        int_,
        long_,
        long_long_,
        unsigned_char_,
        unsigned_short_,
        unsigned_int_,
        unsigned_long_,
        unsigned_long_long_,
        float_,
        double_,
        long_double_,
        wchar_t_
    };


    // builtin type constructors

#define BUILTIN_TYPE_NAME_DEFINE_CTOR(name) \
    static auto name##_() { return std::make_shared<builtin_type_name>(kind_t::name##_); }

    BUILTIN_TYPE_NAME_DEFINE_CTOR(void)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(char)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(short)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(int)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(long)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(long_long)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(unsigned_char)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(unsigned_short)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(unsigned_int)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(unsigned_long)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(unsigned_long_long)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(float)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(double)
    BUILTIN_TYPE_NAME_DEFINE_CTOR(long_double)


    /// Constructs builtin type name with specified kind
    builtin_type_name(kind_t knd): knd_{knd} {}

    /// Destroys object
    virtual ~builtin_type_name() = default;

    /// Returns name of builtin type
    std::string name() const;

    /// Returns type kind
    auto kind() const { return knd_; }

    /// Sets type kind
    void set_kind(kind_t k) { knd_ = k; }

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream.
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override {}

    /// Returns true if type is prefix op
    bool is_prefix_op() const override { return false; }

    /// Returns true if type is suffix op
    bool is_suffix_op() const override { return false; }

    /// Converts type to unsigned. Returns true if conversion is possible
    bool to_unsigned();

private:
    kind_t knd_;                ///< Type kind
};


/// Represents basic type name containing single identifier with scope
class basic_type_name: public scope_type_name {
public:
    /// Constructor, makes basic type name with specified type identifier
    /// and scope
    basic_type_name(const std::string & ident,
                    const scope_name_sp & sc = scope_name_sp{});

    /// Returns type identifier
    const std::string & identifier() const;

    /// Sets type identifier
    void set_identifier(const std::string & v);

    /// Returns true if type has scope
    bool has_scope() const;

    /// Returns type scope
    const scope_name * scope() const;

    /// Writes type name to output stream
    void write(std::ostream & str, write_style style) const override;

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Creates basic type name and returns shared pointer to it
    static std::shared_ptr<basic_type_name> make(const std::string & ident,
                                                 const scope_name_sp & sc = scope_name_sp{});

private:
    scope_name_sp scope_;      ///< Type scope
    std::string identifier_;        ///< Type identifier
};


/// Represents array type name
class array_type_name: public type_name {
public:
    /// Constructor, makes array type name with specified element
    /// type name and optional array size
    array_type_name(const type_name_sp & el_type,
                    std::size_t sz = SIZE_MAX);

    /// Returns const element type name
    const type_name * el_type() const;

    /// Returns element type name
    type_name * el_type();

    /// Returns shared pointer to element type name
    type_name_sp el_type_sp();

    /// Sets element type name
    void set_el_type(const type_name_sp & et);

    /// Returns array size of SIZE_MAX if size is not specified
    std::size_t size() const;

    /// Writes size in square brackets to output stream
    void write_size(std::ostream & str) const;

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Makes array type name and wraps it into shared pointer
    static std::shared_ptr<array_type_name>
    make(const type_name_sp & el, std::size_t sz = SIZE_MAX);

private:
    type_name_sp el_type_;      ///< Type of array element
    std::size_t size_;          ///< Array size
};


/// Represents pointer type name
class pointer_type_name: public type_name {
public:
    /// Constructor, makes pointer type name with specified base type
    pointer_type_name(const qual_type_name & base);

    /// Returns base type name
    const qual_type_name & base() const;

    /// Retruns base type name
    qual_type_name & base();

    /// Sets base type name
    void set_base(const qual_type_name & t);

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Makes pointer type name and wraps it into shared pointer
    static std::shared_ptr<pointer_type_name> make(const qual_type_name & base);

private:
    qual_type_name base_;       ///< Base qualified type name
};


/// Represents reference type name
class reference_type_name: public type_name {
public:
    /// Constructor, makes reference type name with specified base type
    reference_type_name(const qual_type_name & base);

    /// Returns base type name
    const qual_type_name & base() const;

    /// Returns base type name
    qual_type_name & base();

    /// Sets base type name
    void set_base(const qual_type_name & t);

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Makes reference type name and wraps it into shared pointer
    static std::shared_ptr<reference_type_name> make(const qual_type_name & base);

private:
    qual_type_name base_;       ///< Base qualified type name
};


/// Represents template instantiation type name
class template_type_name: public scope_type_name {
public:
    /// Constructor, makes template type name with specified
    /// template name
    template_type_name(const basic_type_name & tname);

    /// Returns template name
    const basic_type_name & template_name() const;

    /// Returns const reference to vector of template parameters
    const std::vector<qual_type_name> & params() const;

    /// Returns reference to vector of template parameters
    std::vector<qual_type_name> & params();

    /// Writes type name to output stream
    void write(std::ostream & str, write_style style) const override;

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Makes template type name and returns shared pointer to it
    static std::shared_ptr<template_type_name> make(const basic_type_name & tname);

private:
    basic_type_name template_name_;         ///< Name of template
    std::vector<qual_type_name> params_;    ///< Template parameters
};


/// Represents function type name
class function_type_name: public type_name {
public:
    /// Constructor, makes function type name with specified
    /// return type
    function_type_name(const qual_type_name & r);

    /// Returns function return type name
    const qual_type_name & ret_type() const;

    /// Returns function return type name
    qual_type_name & ret_type();

    /// Sets function return type name
    void set_ret_type(const qual_type_name & r);

    /// Returns reference to vector of function parameters names
    std::vector<qual_type_name> & params();

    /// Returns const reference to vector of function parameters names
    const std::vector<qual_type_name> & params() const;

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Writes parameters to output stream
    void write_params(std::ostream & str, write_style style) const;

    /// Makes function type name and wraps it into shared pointer
    static std::shared_ptr<function_type_name> make(const qual_type_name & r);

private:
    qual_type_name ret_;                    ///< Return type name
    std::vector<qual_type_name> params_;    ///< Function parameters
};


/// Represents member pointer type name
class mem_ptr_type_name: public type_name {
public:
    /// Constructor, makes member pointer type name with specified
    /// base name and member type
    mem_ptr_type_name(const scope_name_sp & b,
                      const qual_type_name & mtp);

    /// Returns base type name
    const scope_name * base() const;

    /// Returns member type name
    const qual_type_name & mem_type() const;

    /// Returns member type name
    qual_type_name & mem_type();

    /// Sets member type name
    void set_mem_type(const qual_type_name & nm);

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override;

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override;

    /// Returns true if type is prefix op
    bool is_prefix_op() const override;

    /// Returns true if type is suffix op
    bool is_suffix_op() const override;

    /// Makes pointer to member type name and wraps it into shared pointer
    static std::shared_ptr<mem_ptr_type_name> make(const scope_name_sp & b,
                                                   const qual_type_name & mtp);

private:
    scope_name_sp base_;       ///< Base class type name
    qual_type_name mem_type_;       ///< Member type names
};



/// Represents decltype() type name
class decltype_type_name: public type_name {
public:
    /// Constructor, creates decltype with specified expression
    decltype_type_name(const std::string & expr):
        expr_{expr} {}

    /// Destructor, destroys object
    virtual ~decltype_type_name() = default;

    /// Writes result to output stream
    void write_result(std::ostream & str, write_style style) const override;

    /// Writes prefix to output stream
    void write_prefix(std::ostream & str, bool end_sp, write_style style) const override {}

    /// Writes suffix to output stream
    void write_suffix(std::ostream & str, bool p_s, write_style style) const override {}

    /// Returns true if type is prefix op
    bool is_prefix_op() const override { return false; }

    /// Returns true if type is suffix op
    bool is_suffix_op() const override { return false; }

    /// Returns decltype expression
    auto & expr() const { return expr_; }

private:
    std::string expr_;          ///< decltype expression
};


/// Represents function declaration
class function_decl: public scope_name {
public:
    /// Constructs function name with specified name and type
    function_decl(const scope_type_name_sp & nm,
                  const std::shared_ptr<function_type_name> & ftype):
        name_{nm}, type_{ftype} {}

    /// Destructor, destroys object
    virtual ~function_decl() = default;

    /// Returns pointer to function decl name
    const auto * name() const { return name_.get(); }

    /// Sets function decl name
    void set_name(const scope_type_name_sp & nm) { name_ = nm; }

    /// Returns pointer to function type
    const auto * type() const { return type_.get(); }

    /// Sets function type
    void set_type(const std::shared_ptr<function_type_name> & t) { type_ = t; }

    /// Writes function decl to output stream
    void write(std::ostream & str, type_name_write_style style) const override;

private:
    /// Function name
    scope_type_name_sp name_;

    /// Function type name
    std::shared_ptr<function_type_name> type_;
};


/// Parses and rewrites type name to canonical forms used in debug info for gcc and clang.
/// Returns true on success
bool make_canonical_type_names(const std::string & tname,
                               std::string & gcc_can_name,
                               std::string & clang_can_name);


}
