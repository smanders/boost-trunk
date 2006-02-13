
//  (C) Copyright Daniel James 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define HASH_NAMESPACE boost
#define BOOST_HASH_NO_EXTENSIONS
#include <boost/functional/hash.hpp>

extern int f(std::size_t, int*);

int main() {
    HASH_NAMESPACE::hash<int*> ptr_hasher;
    int x = 55;
    return f(ptr_hasher(&x), &x);
}
