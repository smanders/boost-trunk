//  (C) Copyright Gennadiy Rozental 2005.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision$
//
//  Description : privide core implementation for Unit Test Framework.
//  Extensions could be provided in separate files
// ***************************************************************************

#ifndef BOOST_TEST_UNIT_TEST_SUITE_IPP_012205GER
#define BOOST_TEST_UNIT_TEST_SUITE_IPP_012205GER

// Boost.Test
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/utils/foreach.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>

// Boost
#include <boost/timer.hpp>

// STL
#include <algorithm>
#include <vector>

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace boost {

namespace unit_test {

// ************************************************************************** //
// **************                   test_unit                  ************** //
// ************************************************************************** //

test_unit::test_unit( const_string name, test_unit_type t )
: p_name( std::string( name.begin(), name.size() ) )
, p_type( t )
, p_type_name( t == tut_case ? "case" : "suite" )
, p_id( INV_TEST_UNIT_ID )
{
}

//____________________________________________________________________________//

void
test_unit::depends_on( test_unit* tu )
{
    m_dependencies.push_back( tu->p_id );
}

//____________________________________________________________________________//

bool
test_unit::check_dependencies() const
{
    BOOST_TEST_FOREACH( test_unit_id, tu_id, m_dependencies ) {
        if( !unit_test::results_collector.results( tu_id ).passed() )
            return false;
    }

    return true;
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                   test_case                  ************** //
// ************************************************************************** //

test_case::test_case( const_string name, callback0<> const& test_func )
: test_unit( name, (test_unit_type)type )
, m_test_func( test_func )
{
    // !! weirdest MSVC BUG; try to remove this statement; looks like it eats first token of next statement
#if BOOST_WORKAROUND(BOOST_MSVC,<=1200)
    0;
#endif

    framework::register_test_unit( this );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                  test_suite                  ************** //
// ************************************************************************** //

//____________________________________________________________________________//

test_suite::test_suite( const_string name )
: test_unit( name, (test_unit_type)type )
{
    framework::register_test_unit( this );
}

//____________________________________________________________________________//

// !! need to prevent modifing test unit once it is added to tree

void
test_suite::add( test_unit* tu, counter_t expected_failures, unsigned timeout )
{
    if( expected_failures != 0 )
        tu->p_expected_failures.value = expected_failures;

    p_expected_failures.value += tu->p_expected_failures;

    if( timeout != 0 )
        tu->p_timeout.value = timeout;

    m_members.push_back( tu->p_id );
}

//____________________________________________________________________________//

void
test_suite::add( test_unit_generator const& gen, unsigned timeout )
{
    test_unit* tu;
    while((tu = gen.next(), tu))
        add( tu, 0, timeout );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************               traverse_test_tree             ************** //
// ************************************************************************** //

void
traverse_test_tree( test_case const& tc, test_tree_visitor& V )
{
    V.visit( tc );
}

//____________________________________________________________________________//

void    traverse_test_tree( test_suite const& suite, test_tree_visitor& V )
{
    if( !V.test_suite_start( suite ) )
        return;

    try {
        if( runtime_config::random_seed() == 0 ) {
            BOOST_TEST_FOREACH( test_unit_id, id, suite.m_members )
                traverse_test_tree( id, V );
        }
        else {
            std::vector<test_unit_id> members( suite.m_members );
            std::random_shuffle( members.begin(), members.end() );

            BOOST_TEST_FOREACH( test_unit_id, id, members )
                traverse_test_tree( id, V );
        }
        
    } catch( test_aborted const& ) {
        V.test_suite_finish( suite );

        throw;
    }

    V.test_suite_finish( suite );
}

//____________________________________________________________________________//

void
traverse_test_tree( test_unit_id id, test_tree_visitor& V )
{
    if( test_id_2_unit_type( id ) == tut_case )
        traverse_test_tree( framework::get<test_case>( id ), V );
    else
        traverse_test_tree( framework::get<test_suite>( id ), V );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************               object generators              ************** //
// ************************************************************************** //

namespace ut_detail {

std::string
normalize_test_case_name( const_string name )
{
    return ( name[0] == '&'
                ? std::string( name.begin()+1, name.size()-1 )
                : std::string( name.begin(), name.size() ) );
}

//____________________________________________________________________________//

} // namespace ut_detail

} // namespace unit_test

} // namespace boost

//____________________________________________________________________________//

#include <boost/test/detail/enable_warnings.hpp>

// ***************************************************************************
//  Revision History :
//
//  $Log$
//  Revision 1.6  2005/02/21 10:12:24  rogeeff
//  Support for random order of test cases implemented
//
//  Revision 1.5  2005/02/20 08:27:07  rogeeff
//  This a major update for Boost.Test framework. See release docs for complete list of fixes/updates
//
// ***************************************************************************

#endif // BOOST_TEST_UNIT_TEST_SUITE_IPP_012205GER
