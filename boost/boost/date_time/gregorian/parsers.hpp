#ifndef GREGORIAN_PARSERS_HPP___
#define GREGORIAN_PARSERS_HPP___

/* Copyright (c) 2002,2003 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE-1.0 or http://www.boost.org/LICENSE-1.0)
 * Author: Jeff Garland, Bart Garst
 * $Date$
 */

#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/date_parsing.hpp"
#include "boost/date_time/compiler_config.hpp"
#include <string>
#include <sstream>

namespace boost {
namespace gregorian {

  //! Deprecated: Use from_simple_string
  inline date from_string(std::string s) {
    return date_time::parse_date<date>(s);
  }

  //! From delimited date string where with order year-month-day eg: 2002-1-25 or 2003-Jan-25
  inline date from_simple_string(std::string s) {
    return date_time::parse_date<date>(s);
  }
  
#if !(defined(BOOST_NO_STD_ITERATOR_TRAITS))
  //! Stream should hold a date in the form of: 2002-1-25. Month number, abbrev, or name are accepted
  /* Arguments passed in by-value for convertability of char[] 
   * to iterator_type. Calls to from_stream_type are by-reference 
   * since conversion is already done */
  template<class iterator_type>
  inline date from_stream(iterator_type beg, iterator_type end) {
    if(beg == end)
    {
      return date(not_a_date_time);
    }
    typedef typename std::iterator_traits<iterator_type>::value_type value_type;
    std::string s = date_time::from_stream_type(beg, end, value_type());
    return date_time::parse_date<date>(s);
  }
#endif //BOOST_NO_STD_ITERATOR_TRAITS

  //! From iso type date string where with order year-month-day eg: 20020125
  inline date from_undelimited_string(std::string s) {
    return date_time::parse_undelimited_date<date>(s);
  }

} } //namespace gregorian

  
    
    




















#endif

