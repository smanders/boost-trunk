//-----------------------------------------------------------------------------
// boost variant/variant_fwd.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman, Itay Maman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#ifndef BOOST_VARIANT_VARIANT_FWD_HPP
#define BOOST_VARIANT_VARIANT_FWD_HPP

#include "boost/config.hpp"
#include "boost/empty_fwd.hpp"
#include "boost/mpl/arg.hpp"
#include "boost/mpl/void.hpp"
#include "boost/preprocessor/cat.hpp"
#include "boost/preprocessor/enum_params.hpp"
#include "boost/preprocessor/enum_shifted.hpp"
#include "boost/preprocessor/enum_shifted_params.hpp"
#include "boost/preprocessor/repeat.hpp"

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_LIMIT_TYPES
//
// Implementation-defined preprocessor symbol describing the actual
// length of variant's pseudo-variadic template parameter list.
//
#include "boost/mpl/limits/list.hpp"
#define BOOST_VARIANT_LIMIT_TYPES \
    BOOST_MPL_LIMIT_LIST_SIZE

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_NO_REFERENCE_SUPPORT
//
// Defined if variant does not support references as bounded types.
//
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
#   define BOOST_VARIANT_NO_REFERENCE_SUPPORT
#endif

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_NO_TYPE_SEQUENCE_SUPPORT
//
// Defined if variant does not support variant<Types> syntax (see below). 
//
#if defined(BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE)
#   define BOOST_VARIANT_NO_TYPE_SEQUENCE_SUPPORT
#endif

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT
//
// Defined if MPL lambda facility should be used as workaround for broken
// compilers. (Thus, only types w/ MPL lambda workarounds can be accepted.)
//

#include "boost/mpl/aux_/config/ctps.hpp"
#include "boost/mpl/aux_/config/ttp.hpp"

#if defined(BOOST_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
 || defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
 && !defined(BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT)
#   define BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT
#endif

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_RECURSIVE_VARIANT_MAX_ARITY
//
// Exposes maximum allowed arity of class templates with recursive_variant
// arguments. (That is, variant< ..., T<[1], recursive_variant, ... [N]> >.)
//
#include "boost/mpl/limits/arity.hpp"
#define BOOST_VARIANT_RECURSIVE_VARIANT_MAX_ARITY \
    BOOST_MPL_METAFUNCTION_MAX_ARITY

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_ENUM_PARAMS
//
// Convenience macro for enumeration of BOOST_VARIANT_LIMIT_TYPES params.
//
// Rationale: Cleaner, simpler code for clients of variant library.
//
#define BOOST_VARIANT_ENUM_PARAMS( param )  \
    BOOST_PP_ENUM_PARAMS(BOOST_VARIANT_LIMIT_TYPES, param)

///////////////////////////////////////////////////////////////////////////////
// macro BOOST_VARIANT_ENUM_SHIFTED_PARAMS
//
// Convenience macro for enumeration of BOOST_VARIANT_LIMIT_TYPES-1 params.
//
#define BOOST_VARIANT_ENUM_SHIFTED_PARAMS( param )  \
    BOOST_PP_ENUM_SHIFTED_PARAMS(BOOST_VARIANT_LIMIT_TYPES, param)


namespace boost {

namespace detail { namespace variant {

///////////////////////////////////////////////////////////////////////////////
// (detail) class void_ and class template convert_void
// 
// Provides the mechanism by which void(NN) types are converted to
// mpl::void_ (and thus can be passed to mpl::list).
//
// Rationale: This is particularly needed for the using-declarations
// workaround (below), but also to avoid associating mpl namespace with
// variant in argument dependent lookups (which used to happen because of
// defaulting of template parameters to mpl::void_).
//

struct void_;

template <typename T>
struct convert_void
{
    typedef T type;
};

template <>
struct convert_void< void_ >
{
    typedef mpl::void_ type;
};

///////////////////////////////////////////////////////////////////////////////
// (workaround) BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE
//
// Needed to work around compilers that don't support using-declaration
// overloads. (See the variant::initializer workarounds below.)
//

#if defined(BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE)

// (detail) tags voidNN -- NN defined on [0, BOOST_VARIANT_LIMIT_TYPES)
//
// Defines void types that are each unique and specializations of
// convert_void that yields mpl::void_ for each voidNN type.
//

#define BOOST_VARIANT_DETAIL_DEFINE_VOID_N(z,N,_)          \
    struct BOOST_PP_CAT(void,N);                           \
                                                           \
    template <>                                            \
    struct convert_void< BOOST_PP_CAT(void,N) >            \
    {                                                      \
        typedef mpl::void_ type;                           \
    };                                                     \
    /**/

BOOST_PP_REPEAT(
      BOOST_VARIANT_LIMIT_TYPES
    , BOOST_VARIANT_DETAIL_DEFINE_VOID_N
    , _
    )

#undef BOOST_VARIANT_DETAIL_DEFINE_VOID_N

#endif // BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE workaround

}} // namespace detail::variant

///////////////////////////////////////////////////////////////////////////////
// (detail) macro BOOST_VARIANT_AUX_DECLARE_PARAM
//
// Template parameter list for variant and recursive_variant declarations.
//

#if !defined(BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE)

#   define BOOST_VARIANT_AUX_DECLARE_PARAMS_IMPL(z, N, T) \
    typename BOOST_PP_CAT(T,N) = detail::variant::void_ \
    /**/

#else // defined(BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE)

#   define BOOST_VARIANT_AUX_DECLARE_PARAMS_IMPL(z, N, T) \
    typename BOOST_PP_CAT(T,N) = BOOST_PP_CAT(detail::variant::void,N) \
    /**/

#endif // BOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE workaround

#define BOOST_VARIANT_AUX_DECLARE_PARAMS \
      typename T0 = boost::empty \
    , BOOST_PP_ENUM_SHIFTED( \
          BOOST_VARIANT_LIMIT_TYPES \
        , BOOST_VARIANT_AUX_DECLARE_PARAMS_IMPL \
        , T \
        ) \
    /**/

///////////////////////////////////////////////////////////////////////////////
// class template variant (concept inspired by Andrei Alexandrescu)
//
// Efficient, type-safe bounded discriminated union.
//
// Preconditions:
//  - Each type must be unique.
//  - No type may be const-qualified.
//
// Proper declaration form:
//   variant<types>    (where types is a type-sequence)
// or
//   variant<T0,T1,...,Tn>  (where T0 is NOT a type-sequence)
// or
//   variant<>, which is variant<boost::empty>
//
template < BOOST_VARIANT_AUX_DECLARE_PARAMS > class variant;

///////////////////////////////////////////////////////////////////////////////
// metafunction recursive_variant
//
// Exposes a boost::variant with recursive_variant_ tags (below) substituted
// with the variant itself (wrapped as needed with boost::incomplete).
//
template < BOOST_VARIANT_AUX_DECLARE_PARAMS > struct recursive_variant;

#undef BOOST_VARIANT_AUX_DECLARE_PARAMS_IMPL
#undef BOOST_VARIANT_AUX_DECLARE_PARAMS

///////////////////////////////////////////////////////////////////////////////
// type recursive_variant_
//
// Tag type indicates where recursive variant substitution should occur.
//
#if !defined(BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT)
    struct recursive_variant_;
#else
    typedef mpl::arg<1> recursive_variant_;
#endif

} // namespace boost

#endif // BOOST_VARIANT_VARIANT_FWD_HPP
