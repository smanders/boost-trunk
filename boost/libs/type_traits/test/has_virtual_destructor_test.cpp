
//  (C) Copyright John Maddock 2005. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include "check_integral_constant.hpp"
#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/has_virtual_destructor.hpp>
#endif

#include <iostream>
#include <stdexcept>

TT_TEST_BEGIN(has_virtual_destructor)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<const int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<volatile int>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<int*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<int* const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<int&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<mf4>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<f1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<enum_UDT>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<empty_UDT>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<UDT*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<UDT[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<UDT&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<void>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<VB>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<VD>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<test_abc2>::value, false);
#ifndef BOOST_NO_STD_LOCALE
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::iostream>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::basic_streambuf<char> >::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::basic_ios<char> >::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::basic_istream<char> >::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::basic_streambuf<char> >::value, false);
// some libraries don't make this one a polymorphic class, since this is a library
// rather than a type traits issue we won't test this for now...
//BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::ios_base>::value, false);
#endif
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::exception>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::bad_alloc>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::runtime_error>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::out_of_range>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::has_virtual_destructor<std::range_error>::value, false);

TT_TEST_END








