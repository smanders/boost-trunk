/*
 *          Copyright Andrey Semashev 2007 - 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   settings_parser.cpp
 * \author Andrey Semashev
 * \date   20.07.2012
 *
 * \brief  This header is the Boost.Log library implementation, see the library documentation
 *         at http://www.boost.org/libs/log/doc/log.html.
 */

#ifndef BOOST_LOG_WITHOUT_SETTINGS_PARSERS

#include <string>
#include <locale>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <boost/throw_exception.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/move/core.hpp>
#include <boost/move/utility.hpp>
#include <boost/log/detail/config.hpp>
#include <boost/log/detail/code_conversion.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>
#include <boost/log/exceptions.hpp>
#include "parser_utils.hpp"
#include "spirit_encoding.hpp"
#include <boost/log/detail/header.hpp>

namespace boost {

BOOST_LOG_OPEN_NAMESPACE

BOOST_LOG_ANONYMOUS_NAMESPACE {

//! Settings parser
template< typename CharT >
class settings_parser
{
private:
    typedef CharT char_type;
    typedef const char_type* iterator_type;
    typedef typename log::aux::encoding< char_type >::type encoding;
    typedef settings_parser< char_type > this_type;

    typedef std::basic_string< char_type > string_type;
    typedef log::aux::char_constants< char_type > constants;
    typedef basic_settings< char_type > settings_type;

private:
    //! Current section name
    std::string m_SectionName;
    //! Current parameter name
    std::string m_ParameterName;
    //! Settings instance
    settings_type& m_Settings;
    //! Locale from the source stream
    std::locale m_Locale;
    //! Current line number
    unsigned int& m_LineCounter;

public:
    //! Constructor
    explicit settings_parser(settings_type& setts, unsigned int& line_counter, std::locale const& loc) :
        m_Settings(setts),
        m_Locale(loc),
        m_LineCounter(line_counter)
    {
    }

    //! Parses a line of the input
    void parse_line(iterator_type& begin, iterator_type end)
    {
        iterator_type p = begin;
        trim_spaces_left(p, end);
        if (p != end)
        {
            char_type c = *p;
            if (c == constants::char_section_bracket_left)
            {
                // We have a section name
                iterator_type start = ++p;
                trim_spaces_left(start, end);
                iterator_type stop = std::find(start, end, constants::char_section_bracket_right);
                if (stop == end)
                    BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Section header is invalid.", (m_LineCounter));

                p = stop + 1;
                trim_spaces_right(start, stop);

                set_section_name(start, stop);
            }
            else if (c != constants::char_comment)
            {
                // We have a parameter
                iterator_type eq = std::find(p, end, constants::char_equal);
                if (eq == end)
                    BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameter description is invalid.", (m_LineCounter));

                // Parameter name
                iterator_type stop = eq;
                trim_spaces_right(p, stop);

                set_parameter_name(p, stop);

                // Parameter value
                p = eq + 1;
                trim_spaces_left(p, end);
                if (p == end || *p == constants::char_comment)
                    BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameter value is not specified.", (m_LineCounter));

                c = *p;
                if (c == constants::char_quote)
                {
                    // The value is specified as a quoted string
                    iterator_type start = ++p;
                    for (; p != end; ++p)
                    {
                        c = *p;
                        if (c == constants::char_quote)
                        {
                            break;
                        }
                        else if (c == constants::char_backslash)
                        {
                            ++p;
                            if (p == end)
                                BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Invalid escape sequence in the parameter value.", (m_LineCounter));
                        }
                    }
                    if (p == end)
                        BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Unterminated quoted string in the parameter value.", (m_LineCounter));

                    set_parameter_quoted_value(start, p);

                    ++p; // skip the closing quote
                }
                else
                {
                    // The value is specified as a single word
                    iterator_type start = p;
                    for (++p; p != end; ++p)
                    {
                        c = *p;
                        if (c == constants::char_comment || !encoding::isgraph(c))
                            break;
                    }

                    set_parameter_value(start, p);
                }
            }

            // In the end of the line we may have a comment
            trim_spaces_left(p, end);
            if (p != end)
            {
                c = *p;
                if (c == constants::char_comment)
                {
                    // The comment spans until the end of the line
                    p = end;
                }
                else
                {
                    BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Unexpected characters in the end of the line.", (m_LineCounter));
                }
            }
        }

        begin = p;
    }

private:
    //! The method sets the parsed section name
    void set_section_name(iterator_type begin, iterator_type end)
    {
        // Check that the section name is valid
        if (begin == end)
            BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Section name is empty.", (m_LineCounter));

        for (iterator_type p = begin; p != end; ++p)
        {
            char_type c = *p;
            if (c != constants::char_dot && !encoding::isalnum(c))
                BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Section name is invalid.", (m_LineCounter));
        }

        m_SectionName = log::aux::to_narrow(string_type(begin, end), m_Locale);

        // For compatibility with Boost.Log v1, we replace the "Sink:" prefix with "Sinks."
        // so that all sink parameters are placed in the common Sinks section.
        if (m_SectionName.compare(0, 5, "Sink:") == 0)
            m_SectionName = "Sinks." + m_SectionName.substr(5);
    }

    //! The method sets the parsed parameter name
    void set_parameter_name(iterator_type begin, iterator_type end)
    {
        if (m_SectionName.empty())
        {
            // The parameter encountered before any section starter
            BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameters are only allowed within sections.", (m_LineCounter));
        }

        // Check that the parameter name is valid
        if (begin == end)
            BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameter name is empty.", (m_LineCounter));

        iterator_type p = begin;
        if (!encoding::isalpha(*p))
            BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameter name is invalid.", (m_LineCounter));
        for (++p; p != end; ++p)
        {
            char_type c = *p;
            if (!encoding::isgraph(c))
                BOOST_LOG_THROW_DESCR_PARAMS(parse_error, "Parameter name is invalid.", (m_LineCounter));
        }

        m_ParameterName = log::aux::to_narrow(string_type(begin, end), m_Locale);
    }

    //! The method sets the parsed parameter value (non-quoted)
    void set_parameter_value(iterator_type begin, iterator_type end)
    {
        m_Settings[m_SectionName][m_ParameterName] = string_type(begin, end);
        m_ParameterName.clear();
    }

    //! The method sets the parsed parameter value (quoted)
    void set_parameter_quoted_value(iterator_type begin, iterator_type end)
    {
        // Cut off the quotes
        string_type val(begin, end);
        constants::translate_escape_sequences(val);
        m_Settings[m_SectionName][m_ParameterName] = val;
        m_ParameterName.clear();
    }

    //! Skips spaces in the beginning of the input
    static void trim_spaces_left(iterator_type& begin, iterator_type end)
    {
        while (begin != end && encoding::isspace(*begin))
            ++begin;
    }
    //! Skips spaces in the end of the input
    static void trim_spaces_right(iterator_type begin, iterator_type& end)
    {
        while (begin != end && encoding::isspace(*(end - 1)))
            --end;
    }

    //  Assignment and copying are prohibited
    BOOST_DELETED_FUNCTION(settings_parser(settings_parser const&))
    BOOST_DELETED_FUNCTION(settings_parser& operator= (settings_parser const&))
};

} // namespace

//! The function parses library settings from an input stream
template< typename CharT >
basic_settings< CharT > parse_settings(std::basic_istream< CharT >& strm)
{
    typedef CharT char_type;
    typedef std::basic_string< char_type > string_type;
    typedef settings_parser< char_type > settings_parser_type;
    typedef basic_settings< char_type > settings_type;

    if (!strm.good())
        BOOST_THROW_EXCEPTION(std::invalid_argument("The input stream for parsing settings is not valid"));

    io::basic_ios_exception_saver< char_type > exceptions_guard(strm, std::ios_base::badbit);

    // Engage parsing
    settings_type settings;
    unsigned int line_number = 1;
    std::locale loc = strm.getloc();
    settings_parser_type parser(settings, line_number, loc);

    string_type line;
    while (!strm.eof())
    {
        std::getline(strm, line);

        const char_type* p = line.c_str();
        parser.parse_line(p, p + line.size());

        line.clear();
        ++line_number;
    }

    return boost::move(settings);
}


#ifdef BOOST_LOG_USE_CHAR
template BOOST_LOG_SETUP_API basic_settings< char > parse_settings< char >(std::basic_istream< char >& strm);
#endif
#ifdef BOOST_LOG_USE_WCHAR_T
template BOOST_LOG_SETUP_API basic_settings< wchar_t > parse_settings< wchar_t >(std::basic_istream< wchar_t >& strm);
#endif

BOOST_LOG_CLOSE_NAMESPACE // namespace log

} // namespace boost

#include <boost/log/detail/footer.hpp>

#endif // BOOST_LOG_WITHOUT_SETTINGS_PARSERS
