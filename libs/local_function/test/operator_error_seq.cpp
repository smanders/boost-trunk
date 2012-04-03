
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/local_function

#include <boost/local_function.hpp>
#include <boost/detail/lightweight_test.hpp>

struct point {
    int x;
    int y;
};

int main(void) {
    bool BOOST_LOCAL_FUNCTION( (const point& p) (const point& q) ) {
        return p.x == q.x && p.y == q.y;
    } BOOST_LOCAL_FUNCTION_NAME(operator==)

    point a; a.x = 1; a.y = 2;
    point b = a;
    BOOST_TEST(a == b);
    return boost::report_errors();
}

