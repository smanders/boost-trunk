#ifndef BOOST_PREPROCESSOR_MAX_HPP
#define BOOST_PREPROCESSOR_MAX_HPP

/* Copyright (C) 2001
 * Housemarque Oy
 * http://www.housemarque.com
 *
 * Permission to copy, use, modify, sell and distribute this software is
 * granted provided this copyright notice appears in all copies. This
 * software is provided "as is" without express or implied warranty, and
 * with no claim as to its suitability for any purpose.
 *
 * See http://www.boost.org for most recent version.
 */

#include <boost/preprocessor/comparison/less_equal.hpp>
#include <boost/preprocessor/if.hpp>

/** <P>Expands to the maximum of X and Y.</P>

<P>For example, BOOST_PP_MAX(5,7) expands to 7 (a single token).</P>

<H3>Uses</H3>
<UL>
  <LI>BOOST_PP_WHILE()
</UL>
*/
#define BOOST_PP_MAX(X,Y) BOOST_PP_MAX_D(0,X,Y)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define BOOST_PP_MAX_D(D,X,Y) BOOST_PP_IF(BOOST_PP_LESS_EQUAL_D(D,X,Y),Y,X)
#endif

/** <P>Obsolete. Use BOOST_PP_MAX().</P> */
#define BOOST_PREPROCESSOR_MAX(X,Y) BOOST_PP_MAX(X,Y)
#endif
