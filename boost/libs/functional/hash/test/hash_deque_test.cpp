
//  (C) Copyright Daniel James 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "./config.hpp"

#ifdef TEST_EXTENSIONS
#  ifdef TEST_STD_INCLUDES
#    include <functional>
#  else
#    include <boost/functional/hash.hpp>
#  endif
#endif

#include <boost/detail/lightweight_test.hpp>

#ifdef TEST_EXTENSIONS

#include <deque>

using std::deque;
#define CONTAINER_TYPE deque
#include "./hash_sequence_test.hpp"

#endif // TEST_EXTENSIONS

int main()
{
#ifdef TEST_EXTENSIONS
    deque_tests::deque_hash_integer_tests();
#endif

    return boost::report_errors();
}
