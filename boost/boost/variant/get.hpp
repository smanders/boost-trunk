//-----------------------------------------------------------------------------
// boost variant/get.hpp header file
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

#ifndef BOOST_VARIANT_GET_HPP
#define BOOST_VARIANT_GET_HPP

#include <exception>

#include "boost/detail/workaround.hpp"
#include "boost/utility/addressof.hpp"
#include "boost/variant/variant_fwd.hpp"

#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
#   include "boost/mpl/bool.hpp"
#   include "boost/type_traits/is_same.hpp"
#endif

namespace boost {

//////////////////////////////////////////////////////////////////////////
// class bad_get
//
// The exception thrown in the event of a failed get of a value.
//
class bad_get
    : public std::exception
{
public: // std::exception implementation

    virtual const char * what() const throw()
    {
        return "boost::bad_get: "
               "failed value get using boost::get";
    }

};

//////////////////////////////////////////////////////////////////////////
// function template get<T>
//
// Retrieves content of given variant object if content is of type T.
// Otherwise: pointer ver. returns 0; reference ver. throws bad_get.
//

namespace detail { namespace variant {

// (detail) class template get_visitor
//
// Generic static visitor that: if the value is of the specified type,
// returns a pointer to the value it visits; else a null pointer.
//
template <typename T>
struct get_visitor
{
public: // visitor typedefs

    typedef T* result_type;

public: // visitor interfaces

#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1200)

    template <typename U>
    T* operator()(U&) const
    {
        return static_cast<T*>(0);
    }

    T* operator()(T& operand) const
    {
        return boost::addressof(operand);
    }

#else // MSVC6

private: // helpers, for visitor interfaces (below)

    T* execute_impl(T& operand, mpl::true_) const
    {
        return boost::addressof(operand);
    }

    template <typename U>
    T* execute_impl(U& operand, mpl::false_) const
    {
        return static_cast<T*>(0);
    }

public: // visitor interfaces

    template <typename U>
    T* operator()(U& operand) const
    {
        // MSVC6 finds normal implementation (above) ambiguous,
        // so we must explicitly disambiguate

        typedef typename is_same<U,T>::type
            U_is_T;

        return execute_impl(operand, U_is_T());
    }

#endif // MSVC6 workaround

};

}} // namespace detail::variant

template <typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
inline U* get(
      boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >* operand
    , U* = 0
    )
{
    if (!operand) return static_cast<U*>(0);

    detail::variant::get_visitor<U> v;
    return operand->apply_visitor(v);
}

template <typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
inline U* get(
      const boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >* operand
    , U* = 0
    )
{
    if (!operand) return static_cast<U*>(0);

    detail::variant::get_visitor<U> v;
    return operand->apply_visitor(v);
}

template <typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
inline U& get(
      boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >& operand
    , U* = 0
    )
{
    U* result = get<U>(&operand);
    if (!result)
        throw bad_get();
    return *result;
}

template <typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
inline U& get(
      const boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >& operand
    , U* = 0
    )
{
    U* result = get<U>(&operand);
    if (!result)
        throw bad_get();
    return *result;
}

} // namespace boost

#endif // BOOST_VARIANT_GET_HPP
