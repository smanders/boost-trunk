/*
 *
 * Copyright (c) 1998-2000
 * Dr John Maddock
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Dr John Maddock makes no representations
 * about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex.h
  *   VERSION      3.02
  *   DESCRIPTION: Declares POSIX API functions
  */

#ifndef BOOST_RE_REGEX_H
#define BOOST_RE_REGEX_H

#include <boost/cregex.hpp>

//
// add using declarations to bring POSIX API functions into
// global scope, only if this is C++ (and not C).
//
#ifdef __cplusplus

using boost::regoff_t;
using boost::regex_tA;
using boost::regmatch_t;
using boost::REG_BASIC;
using boost::REG_EXTENDED;
using boost::REG_ICASE;
using boost::REG_NOSUB;
using boost::REG_NEWLINE;
using boost::REG_NOSPEC;
using boost::REG_PEND;
using boost::REG_DUMP;
using boost::REG_NOCOLLATE;
using boost::REG_ESCAPE_IN_LISTS;
using boost::REG_NEWLINE_ALT;
using boost::REG_PERL;
using boost::REG_AWK;
using boost::REG_GREP;
using boost::REG_EGREP;
using boost::REG_ASSERT;
using boost::REG_INVARG;
using boost::REG_ATOI;
using boost::REG_ITOA;

using boost::REG_NOTBOL;
using boost::REG_NOTEOL;
using boost::REG_STARTEND;

using boost::reg_comp_flags;
using boost::reg_exec_flags;
using boost::regcompA;
using boost::regerrorA;
using boost::regexecA;
using boost::regfreeA;

#ifndef BOOST_RE_NO_WCSTRING
using boost::regcompW;
using boost::regerrorW;
using boost::regexecW;
using boost::regfreeW;
using boost::regex_tW;
#endif

using boost::REG_NOERROR;
using boost::REG_NOMATCH;
using boost::REG_BADPAT;
using boost::REG_ECOLLATE;
using boost::REG_ECTYPE;
using boost::REG_EESCAPE;
using boost::REG_ESUBREG;
using boost::REG_EBRACK;
using boost::REG_EPAREN;
using boost::REG_EBRACE;
using boost::REG_BADBR;
using boost::REG_ERANGE;
using boost::REG_ESPACE;
using boost::REG_BADRPT;
using boost::REG_EEND;
using boost::REG_ESIZE;
using boost::REG_ERPAREN;
using boost::REG_EMPTY;
using boost::REG_E_MEMORY;
using boost::REG_E_UNKNOWN;
using boost::reg_errcode_t;

#endif // __cplusplus

#endif // BOOST_RE_REGEX_H


