
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <cstdlib>
#include <iostream>

#include <boost/coroutine/all.hpp>

#include "tree.h"

node::ptr_t create_tree1()
{
    return branch::create(
        leaf::create( "A"),
        branch::create(
            leaf::create( "B"),
            leaf::create( "C") ) );
}

node::ptr_t create_tree2()
{
    return branch::create(
        branch::create(
            leaf::create( "A"),
            leaf::create( "B") ),
        leaf::create( "C") );
}

#ifdef BOOST_COROUTINES_UNIDIRECT
class coro_visitor : public visitor
{
private:
    boost::coroutines::coroutine< leaf& >::push_type   &   c_;

public:
    coro_visitor( boost::coroutines::coroutine< leaf& >::push_type & c) :
        c_( c)
    {}

    void visit( branch & b)
    {
        if ( b.left) b.left->accept( * this);
        if ( b.right) b.right->accept( * this);
    }

    void visit( leaf & l)
    { c_( l); }
};

int main()
{
    node::ptr_t t1 = create_tree1();
    boost::coroutines::coroutine< leaf& >::pull_type c1(
        [&]( boost::coroutines::coroutine< leaf & >::push_type & c) {
            coro_visitor v( c);
            t1->accept( v);
        });

    node::ptr_t t2 = create_tree2();
    boost::coroutines::coroutine< leaf& >::pull_type c2(
        [&]( boost::coroutines::coroutine< leaf & >::push_type & c) {
            coro_visitor v( c);
            t2->accept( v);
        });

    bool result = std::equal(
            boost::begin( c1),
            boost::end( c1),
            boost::begin( c2) );

    std::cout << std::boolalpha << "same fringe == " << result << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
#else
class coro_visitor : public visitor
{
private:
    boost::coroutines::coroutine< void( leaf&) >   &   c_;

public:
    coro_visitor( boost::coroutines::coroutine< void( leaf&) > & c) :
        c_( c)
    {}

    void visit( branch & b)
    {
        if ( b.left) b.left->accept( * this);
        if ( b.right) b.right->accept( * this);
    }

    void visit( leaf & l)
    { c_( l); }
};

int main()
{
    node::ptr_t t1 = create_tree1();
    boost::coroutines::coroutine< leaf&() > c1(
        [&]( boost::coroutines::coroutine< void( leaf &) > & c) {
            coro_visitor v( c);
            t1->accept( v);
        });

    node::ptr_t t2 = create_tree2();
    boost::coroutines::coroutine< leaf&() > c2(
        [&]( boost::coroutines::coroutine< void( leaf &) > & c) {
            coro_visitor v( c);
            t2->accept( v);
        });

    bool result = std::equal(
            boost::begin( c1),
            boost::end( c1),
            boost::begin( c2) );

    std::cout << std::boolalpha << "same fringe == " << result << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
#endif
