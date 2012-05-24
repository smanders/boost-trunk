
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_HPP_
#define BOOST_LOCAL_FUNCTION_HPP_

#ifndef DOXYGEN

#include <boost/local_function/aux_/macro/decl.hpp>
#include <boost/local_function/aux_/macro/name.hpp>
#include <boost/local_function/aux_/macro/typeof.hpp>
#include <boost/local_function/aux_/preprocessor/traits/decl.hpp>
#include <boost/local_function/detail/preprocessor/line_counter.hpp>
#include <boost/local_function/detail/preprocessor/void_list.hpp>
#include <boost/config.hpp>

// PUBLIC //

#ifdef BOOST_NO_VARIADIC_MACROS
#   define BOOST_LOCAL_FUNCTION_ID(id, declarations) \
        BOOST_LOCAL_FUNCTION_AUX_DECL(id, 0 /* not within template */, \
                BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS( \
                        BOOST_LOCAL_FUNCTION_DETAIL_PP_VOID_LIST( \
                                declarations)))
#   define BOOST_LOCAL_FUNCTION(declarations) \
        BOOST_LOCAL_FUNCTION_ID( \
                BOOST_LOCAL_FUNCTION_DETAIL_PP_LINE_COUNTER, declarations)
#   define BOOST_LOCAL_FUNCTION_ID_TPL(id, declarations) \
        BOOST_LOCAL_FUNCTION_AUX_DECL(id, 1 /* within template */, \
                BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS( \
                        BOOST_LOCAL_FUNCTION_DETAIL_PP_VOID_LIST( \
                                declarations)))
#   define BOOST_LOCAL_FUNCTION_TPL(declarations) \
        BOOST_LOCAL_FUNCTION_ID_TPL( \
                BOOST_LOCAL_FUNCTION_DETAIL_PP_LINE_COUNTER, declarations)
#else // VARIADIC
#   define BOOST_LOCAL_FUNCTION_ID(id, ...) \
        BOOST_LOCAL_FUNCTION_AUX_DECL(id, 0 /* not within template */, \
                BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS( \
                        BOOST_LOCAL_FUNCTION_DETAIL_PP_VOID_LIST(__VA_ARGS__)))
#   define BOOST_LOCAL_FUNCTION(...) \
        BOOST_LOCAL_FUNCTION_ID( \
                BOOST_LOCAL_FUNCTION_DETAIL_PP_LINE_COUNTER, __VA_ARGS__)
#   define BOOST_LOCAL_FUNCTION_ID_TPL(id, ...) \
        BOOST_LOCAL_FUNCTION_AUX_DECL(id, 1 /* within template */, \
                BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS( \
                        BOOST_LOCAL_FUNCTION_DETAIL_PP_VOID_LIST(__VA_ARGS__)))
#   define BOOST_LOCAL_FUNCTION_TPL(...) \
        BOOST_LOCAL_FUNCTION_ID_TPL( \
                BOOST_LOCAL_FUNCTION_DETAIL_PP_LINE_COUNTER, __VA_ARGS__)
#endif // VARIADIC

#define BOOST_LOCAL_FUNCTION_NAME(qualified_name) \
    BOOST_LOCAL_FUNCTION_AUX_NAME(0 /* not within template */, qualified_name)
#define BOOST_LOCAL_FUNCTION_NAME_TPL(qualified_name) \
    BOOST_LOCAL_FUNCTION_AUX_NAME(1 /* within template */, qualified_name)

#define BOOST_LOCAL_FUNCTION_TYPEOF(bound_variable_name) \
    BOOST_LOCAL_FUNCTION_AUX_TYPEOF_TYPE(bound_variable_name)

// DOCUMENTATION //

#else // DOXYGEN

/** @file
@brief Local functions allow to program functions locally, within other
functions, and directly within the scope where they are needed.
*/

/**
@brief This macro is used to start a local function declaration.

This macro must be used within a declarative context, it must follow the local
function result type, it must be followed by the local function body code, and
then by the @RefMacro{BOOST_LOCAL_FUNCTION_NAME} macro (see the
@RefSect{tutorial, Tutorial} and @RefSect{advanced_topics, Advanced Topics}
sections):
@code
{ // Some declarative context.
    ...
    result_type BOOST_LOCAL_FUNCTION(declarations) {
        ... // Body code.
    } BOOST_LOCAL_FUNCTION_NAME(qualified_name)
    ...
}
@endcode

As usual, exceptions specifications can be optionally programmed just after the
macro and before the body code block <c>{ ... }</c> (but the exception
specifications will only apply to the body code and not to the library code
automatically generated by the macro expansion, see the
@RefSect{advanced_topics, Advanced Topics} section).

Within templates, the special macros @RefMacro{BOOST_LOCAL_FUNCTION_TPL}
and @RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL} must be used.

@Params
@Param{declarations,
On compilers that support variadic macros\, the parameter declarations are
defined by the following grammar:
@code
    declarations:
            void | declaration_tuple | declaration_sequence
    declaration_tuple:
            declaration\, declaration\, ...
    declaration_sequence:
            (declaration) (declaration) ...
    declaration:
            bound_variable | parameter | default_value | result_type
    bound_variable:
            [const] bind [(variable_type)] [&] variable_name
    parameter:
            [auto | register] parameter_type parameter_name
    default_value:
            default parameter_default_value
    result_type:
            return function_result_type
@endcode
On compilers that do not support variadic macros\, <c>declaration_tuple</c>
cannot be used:
@code
    declarations:
            void | declaration_sequence
@endcode

(Lexical conventions: <c>token1 | token2</c> means either <c>token1</c> or
<c>token2</c>; <c>[token]</c> means either <c>token</c> or nothing;
<c>{expression}</c> means the token resulting from the expression.)
}
@EndParams
 
Note that on compilers that support variadic macros, commas can be used to
separate the declarations resembling more closely the usual C++ function
declaration syntax (this is the preferred syntax).
However, for portability, on all C++ compilers (with and without variadic
macros) the same library macros also accept parameter declarations specified as
a Boost.Preprocessor sequence separated by round parenthesis <c>()</c>.

When binding the object <c>this</c>, the special symbol <c>this_</c> needs to
be used instead of <c>this</c> as the name of the variable to bind and also
within the local function body to access the object.
(Mistakenly using <c>this</c> instead of <c>this_</c> might not always result in a compiler error and will in general result in undefined behaviour.)

The result type must either be specified just before the macro or within the
macro declarations prefixed by <c>return</c> (but not in both places).

Within the local function body it possible to access the result type using <c>result_type</c>, the type of the first parameter using <c>arg1_type</c>, the type of the second parameter using <c>arg2_type</c>, etc.
The bound variable types can be accessed using @RefMacro{BOOST_LOCAL_FUNCTION_TYPEOF}.

This macro cannot be portably expanded multiple times on the same line.
In these cases, use the @RefMacro{BOOST_LOCAL_FUNCTION_ID} macro instead.

The maximum number of local function parameters (excluding bound variables) is
specified by the configuration macro
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_ARITY_MAX}.
The maximum number of bound variables is specified by the configuration macro
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_BIND_MAX}.
The configuration macro
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_LOCALS_AS_TPARAMS} can be used to force
optimizations that reduce the local function call run-time overhead.

@Note Local functions are functors so they can be assigned to other functors
like <c>boost::function</c> (see Boost.Function).

@See @RefSect{tutorial, Tutorial} section,
@RefSect{advanced_topics, Advanced Topics} section,
@RefMacro{BOOST_LOCAL_FUNCTION_NAME}, @RefMacro{BOOST_LOCAL_FUNCTION_TPL},
@RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL},
@RefMacro{BOOST_LOCAL_FUNCTION_TYPEOF}, @RefMacro{BOOST_LOCAL_FUNCTION_ID},
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_ARITY_MAX},
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_BIND_MAX},
@RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_LOCALS_AS_TPARAMS}.
*/
#define BOOST_LOCAL_FUNCTION(declarations)

/**
@brief This macro is used to start a local function declaration within
templates.

This macro must be used instead of @RefMacro{BOOST_LOCAL_FUNCTION} when
declaring a local function within a template.
A part from that, this macro has the exact same syntax a
@RefMacro{BOOST_LOCAL_FUNCTION} (see @RefMacro{BOOST_LOCAL_FUNCTION} for more
information):
@code
{ // Some declarative context within a template.
    ...
    result_type BOOST_LOCAL_FUNCTION_TPL(declarations) {
        ... // Body code.
    } BOOST_LOCAL_FUNCTION_NAME_TPL(qualified_name)
    ...
}
@endcode

Note that @RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL} must be used with this
macro instead of @RefMacro{BOOST_LOCAL_FUNCTION_NAME}.

This macro cannot be portably expanded multiple times on the same line.
In these cases, use the @RefMacro{BOOST_LOCAL_FUNCTION_ID_TPL} macro instead.

@Note C++03 does not allow to use <c>typename</c> outside templates.
This library internally manipulates types, these operations require
<c>typename</c> but only within templates.
This macro is used to indicate to the library when the enclosing scope is a
template so the library can correctly use <c>typename</c>.

@See @RefSect{tutorial, Tutorial} section, @RefMacro{BOOST_LOCAL_FUNCTION},
@RefMacro{BOOST_LOCAL_FUNCTION_ID_TPL},
@RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL}.
*/
#define BOOST_LOCAL_FUNCTION_TPL(declarations)

/**
@brief This macro allows to declare multiple local functions on the same line.

This macro is equivalent to @RefMacro{BOOST_LOCAL_FUNCTION} but it can be
expanded multiple times on the same line if different identifiers <c>id</c> are
provided for each expansion (see the
@RefSect{advanced_topics, Advanced Topics} section).

@Params
@Param{id,
A unique identifier token which can be concatenated by the preprocessor
(<c>__LINE__</c>\, <c>local_function_number_1_on_line_123</c>\, etc).
}
@Param{declarations,
Same as the <c>declarations</c> parameter of the
@RefMacro{BOOST_LOCAL_FUNCTION} macro.
}
@EndParams

The @RefMacro{BOOST_LOCAL_FUNCTION_NAME} macro should be used to end each one
of the multiple local function declarations as usual (and it will specify a
unique name for each local function).

Within templates, the special macros @RefMacro{BOOST_LOCAL_FUNCTION_ID_TPL}
must be used.

@Note This macro can be useful when the local function macros are expanded
within user-defined macros (because macros all expand on the same line).
On some compilers (e.g., MSVC which supports the non-standard
<c>__COUNTER__</c> macro) it might not be necessary to use this macro but
the use of this macro when expanding multiple local function macros on the same
line is always necessary to ensure portability (this is because this library
can only portably use <c>__LINE__</c> to internally generate unique
identifiers).

@See @RefSect{advanced_topics, Advanced Topics} section,
@RefMacro{BOOST_LOCAL_FUNCTION}, @RefMacro{BOOST_LOCAL_FUNCTION_NAME},
@RefMacro{BOOST_LOCAL_FUNCTION_ID_TPL}.
*/
#define BOOST_LOCAL_FUNCTION_ID(id, declarations)

/**
@brief This macro allows to declare multiple local functions on the same line
within templates.

This macro must be used instead of @RefMacro{BOOST_LOCAL_FUNCTION_TPL} when
declaring multiple local functions on the same line within a template.
A part from that, this macro has the exact same syntax as
@RefMacro{BOOST_LOCAL_FUNCTION_TPL} (see @RefMacro{BOOST_LOCAL_FUNCTION_TPL}
for more information).

@Params
@Param{id,
A unique identifier token which can be concatenated by the preprocessor
(<c>__LINE__</c>\, <c>local_function_number_1_on_line_123</c>\, etc).
}
@Param{declarations,
Same as the <c>declarations</c> parameter of the
@RefMacro{BOOST_LOCAL_FUNCTION_TPL} macro.
}
@EndParams

The @RefMacro{BOOST_LOCAL_FUNCTION_NAME} macro should be used to end each one
of the multiple local function declarations as usual (and it will specify a
unique name for each local function).

Outside template, the macro @RefMacro{BOOST_LOCAL_FUNCTION_ID} should be used
to declare multiple local functions on the same line.

@Note This macro can be useful when the local function macros are expanded
within user-defined macros (because macros all expand on the same line).
On some compilers (e.g., MSVC which supports the non-standard
<c>__COUNTER__</c> macro) it might not be necessary to use this macro but
the use of this macro when expanding multiple local function macros on the same
line is always necessary to ensure portability (this is because this library
can only portably use <c>__LINE__</c> to internally generate unique
identifiers).

@See @RefSect{advanced_topics, Advanced Topics} section,
@RefMacro{BOOST_LOCAL_FUNCTION_TPL}, @RefMacro{BOOST_LOCAL_FUNCTION_NAME},
@RefMacro{BOOST_LOCAL_FUNCTION_ID}.
*/
#define BOOST_LOCAL_FUNCTION_ID_TPL(id, declarations)

/**
@brief This macro is used to end a local function declaration specifying its
name.

This macro must follow the local function body code block <c>{ ... }</c>:
@code
{ // Some declarative context.
    ...
    result_type BOOST_LOCAL_FUNCTION(declarations) {
        ... // Body code.
    } BOOST_LOCAL_FUNCTION_NAME(qualified_name)
    ...
}
@endcode

Within templates, the special macros @RefMacro{BOOST_LOCAL_FUNCTION_TPL} and
@RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL} must be used.

@Params
@Param{qualified_name,
The name of the local function optionally qualified as follow:
@code
    name:
            [inline] [recursive] local_function_name
@endcode
(Lexical conventions: <c>token1 | token2</c> means either <c>token1</c> or
<c>token2</c>; <c>[token]</c> means either <c>token</c> or nothing;
<c>{expression}</c> means the token resulting from the expression.)
}
@EndParams

The local function name can be qualified by prefixing it with the keyword
<c>inline</c> (see the @RefSect{advanced_topics, Advanced Topics} section):
@code
    BOOST_LOCAL_FUNCTION_NAME(inline local_function_name)
@endcode
This increases the chances that the compiler will be able to inline the local
function calls (thus reducing run-time).
However, inline local functions cannot be passed as template parameters (e.g., to <c>std::for_each</c>) or assigned to other functors (e.g., to
<c>boost::function</c>).
That is true on C++03 compilers but inline local functions can instead be
passed as template parameters on C++11 compilers.
On C++11 compilers, there is no need to declare a local function lined because
this library will automatically use C++11 specific features to inline the local
function while always allowing to pass it as a template parameter.
This optimization is automatically enabled when the Boost.Config macro
<c>BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS</c> is not defined but it also be
forced using @RefMacro{BOOST_LOCAL_FUNCTION_CONFIG_LOCALS_AS_TPARAMS}.

The local function name can also be qualified by prefixing it with the
"keyword" <c>recursive</c> (see the
@RefSect{advanced_topics, Advanced Topics} section):
@code
    BOOST_LOCAL_FUNCTION_NAME(recursive local_function_name)
@endcode
This allows the local function to recursively call itself from its body (as
usual in C++).
However, recursive local functions should only be called within their
declaration scope (otherwise the result is undefined behaviour).
Finally, compilers have not been observed to be able to inline recursive local
function calls, not even when the recursive local function is also declared
inline:
@code
    BOOST_LOCAL_FUNCTION(inline recursive local_function_name)
@endcode

@Note The local function name cannot be the name of an operator
<c>operator...</c> and it cannot be the same name of another local function
declared within the same enclosing scope (but <c>boost::overloaded_function</c>
can be used to overload local functions, see
Boost.Functional/OverloadedFunction and the
@RefSect{advanced_topics, Advanced Topics} section).

@See @RefSect{tutorial, Tutorial} section,
@RefSect{advanced_topics, Advanced Topics} section,
@RefMacro{BOOST_LOCAL_FUNCTION},
@RefMacro{BOOST_LOCAL_FUNCTION_NAME_TPL}.
*/
#define BOOST_LOCAL_FUNCTION_NAME(qualified_name)

/**
@brief This macro is used to end a local function declaration specifying its
name within templates.

This macro must be used instead of @RefMacro{BOOST_LOCAL_FUNCTION_NAME} when
declaring a local function within a template.
A part from that, this macro has the exact same syntax a
@RefMacro{BOOST_LOCAL_FUNCTION_NAME} (see @RefMacro{BOOST_LOCAL_FUNCTION_NAME}
for more information):
@code
{ // Some declarative context within a template.
    ...
    result_type BOOST_LOCAL_FUNCTION_TPL(declarations) {
        ... // Body code.
    } BOOST_LOCAL_FUNCTION_NAME_TPL(qualified_name)
    ...
}
@endcode

Note that @RefMacro{BOOST_LOCAL_FUNCTION_TPL} must be used with this macro
instead of @RefMacro{BOOST_LOCAL_FUNCTION}.

@Note C++03 does not allow to use <c>typename</c> outside templates.
This library internally manipulates types, these operations require
<c>typename</c> but only within templates.
This macro is used to indicate to the library when the enclosing scope is a
template so the library can correctly use <c>typename</c>.

@See @RefSect{tutorial, Tutorial} section,
@RefMacro{BOOST_LOCAL_FUNCTION_NAME}, @RefMacro{BOOST_LOCAL_FUNCTION_TPL}.
*/
#define BOOST_LOCAL_FUNCTION_NAME_TPL(name)

/**
@brief This macro expands to the type of the specified bound variable.

This macro can be used within the local functions body to refer to the bound
variable types so to declare local variables, check concepts (using
Boost.ConceptCheck), etc (see the @RefSect{advanced_topics, Advanced Topics}
section).
This way the local function can be programmed entirely without explicitly
specifying the bound variable types thus facilitating maintenance (e.g., if
the type of a bound variable changes in the enclosing scope, the local function
code does not have to change).

@Params
@Param{bound_variable_name,
The name of one of the local function's bound variables.
}
@EndParams

The type returned by the macro is fully qualified in that it contains the extra
constant and reference qualifiers when the specified variable is bound by
constant and by reference.
For example, if a variable named <c>t</c> of type <c>T</c> is:
@li Bound by value using <c>bind t</c> then
<c>BOOST_LOCAL_FUNCTION_TYPEOF(t)</c> is <c>T</c>.
@li Bound by constant value using <c>const bind t</c> then
<c>BOOST_LOCAL_FUNCTION_TYPEOF(t)</c> is <c>const T</c>.
@li Bound by reference using <c>bind& t</c> then
<c>BOOST_LOCAL_FUNCTION_TYPEOF(t)</c> is <c>T&</c>.
@li Bound by constant reference using <c>const bind& t</c> then
<c>BOOST_LOCAL_FUNCTION_TYPEOF(t)</c> is <c>const T&</c>.

This macro must be prefixed by <c>typename</c> when used within templates.

@Note It is best to use this macro instead of Boost.Typeof so to reduce the
number of times Boost.Typeof is used to deduce types (see the
@RefSect{advanced_topics, Advanced Topics} section).

@See @RefSect{advanced_topics, Advanced Topics} section,
@RefMacro{BOOST_LOCAL_FUNCTION}.
*/
#define BOOST_LOCAL_FUNCTION_TYPEOF(bound_variable_name)

#endif // DOXYGEN

#endif // #include guard

