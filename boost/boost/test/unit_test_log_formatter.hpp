//  (C) Copyright Gennadiy Rozental 2003.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision$
//
//  Description : 
// ***************************************************************************

#ifndef BOOST_UNIT_TEST_LOG_FORMATTER_HPP_071894GER
#define BOOST_UNIT_TEST_LOG_FORMATTER_HPP_071894GER

// Boost.Test
#include <boost/test/detail/unit_test_config.hpp>
#include <boost/test/detail/log_level.hpp>
#include <boost/test/fwd_decl.hpp>

// STL
#include <iosfwd>
#include <string> // need only forward decl

#include <boost/test/detail/suppress_warnings.hpp>

namespace boost {

namespace unit_test {

// ************************************************************************** //
// **************                log_entry_data                ************** //
// ************************************************************************** //

struct log_entry_data {
    std::string     m_file;
    std::size_t     m_line;
    log_level       m_level;

    void clear()
    {
        m_file    = std::string();
        m_line    = 0;
        m_level   = log_nothing;
    }
};

// ************************************************************************** //
// **************                checkpoint_data               ************** //
// ************************************************************************** //

struct log_checkpoint_data
{
    std::string     m_file;
    std::size_t     m_line;
    std::string     m_message;

    void clear()
    {
        m_file    = std::string();
        m_line    = 0;
        m_message = std::string();
    }
};

// ************************************************************************** //
// **************            unit_test_log_formatter           ************** //
// ************************************************************************** //

class unit_test_log_formatter {
public:
    enum log_entry_types { BOOST_UTL_ET_INFO, 
                           BOOST_UTL_ET_MESSAGE,
                           BOOST_UTL_ET_WARNING,
                           BOOST_UTL_ET_ERROR,
                           BOOST_UTL_ET_FATAL_ERROR };

    // Destructor
    virtual             ~unit_test_log_formatter() {}

    // Formatter interface
    virtual void        start_log( std::ostream&, bool log_build_info ) = 0;
    virtual void        log_header( std::ostream&, unit_test_counter test_cases_amount ) = 0;
    virtual void        finish_log( std::ostream& ) = 0;

    virtual void        track_test_case_enter( std::ostream&, test_case const& tc ) = 0;
    virtual void        track_test_case_exit( std::ostream&, test_case const& tc, long testing_time_in_mks ) = 0;

    virtual void        log_exception( std::ostream&, log_checkpoint_data const&, 
                                       const_string test_case_name, const_string explanation ) = 0;

    virtual void        begin_log_entry( std::ostream&, log_entry_data const&, log_entry_types let ) = 0;
    virtual void        log_entry_value( std::ostream&, const_string value ) = 0;
    virtual void        end_log_entry( std::ostream& ) = 0;
};

} // namespace unit_test

} // namespace boost

#include <boost/test/detail/enable_warnings.hpp>

// ***************************************************************************
//  Revision History :
//  
//  $Log$
//  Revision 1.9  2005/01/21 07:30:24  rogeeff
//  to log testing time log formatter interfaces changed
//
//  Revision 1.8  2005/01/18 08:26:12  rogeeff
//  unit_test_log rework:
//     eliminated need for ::instance()
//     eliminated need for << end and ...END macro
//     straitend interface between log and formatters
//     change compiler like formatter name
//     minimized unit_test_log interface and reworked to use explicit calls
//
//  Revision 1.7  2004/07/19 12:16:23  rogeeff
//  guard rename
//  warning suppressed
//
//  Revision 1.6  2004/05/21 06:19:35  rogeeff
//  licence update
//
//  Revision 1.5  2004/05/11 11:00:51  rogeeff
//  basic_cstring introduced and used everywhere
//  class properties reworked
//
//  Revision 1.4  2003/12/01 00:41:56  rogeeff
//  prerelease cleaning
//
// ***************************************************************************

#endif // BOOST_UNIT_TEST_LOG_FORMATTER_HPP_071894GER

