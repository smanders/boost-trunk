
// Copyright 2005-2008 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>

template <class T>
void compile_time_tests(T*)
{
    BOOST_STATIC_ASSERT((boost::is_base_and_derived<
        std::unary_function<T, std::size_t>, HASH_NAMESPACE::hash<T> >::value));
}

