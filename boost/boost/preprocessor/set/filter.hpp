# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.  Permission to copy, use,        *
#  *     modify, sell, and distribute this software is granted provided       *
#  *     this copyright notice appears in all copies.  This software is       *
#  *     provided "as is" without express or implied warranty, and with       *
#  *     no claim at to its suitability for any purpose.                      *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_SET_FILTER_HPP
# define BOOST_PREPROCESSOR_SET_FILTER_HPP
#
# include <boost/preprocessor/config/config.hpp>
# include <boost/preprocessor/control/expr_if.hpp>
# include <boost/preprocessor/facilities/empty.hpp>
# include <boost/preprocessor/set/fold_left.hpp>
# include <boost/preprocessor/set/set.hpp>
# include <boost/preprocessor/tuple/elem.hpp>
# include <boost/preprocessor/tuple/rem.hpp>
#
# /* BOOST_PP_SET_FILTER */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_SET_FILTER(pred, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT(BOOST_PP_SET_FILTER_O, (pred, data, (nil)), set)))
# else
#    define BOOST_PP_SET_FILTER(pred, data, set) BOOST_PP_SET_FILTER_I(pred, data, set)
#    define BOOST_PP_SET_FILTER_I(pred, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT(BOOST_PP_SET_FILTER_O, (pred, data, (nil)), set)))
# endif
#
# if BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_STRICT()
#    define BOOST_PP_SET_FILTER_O(s, st, elem) BOOST_PP_SET_FILTER_O_IM(s, BOOST_PP_TUPLE_REM_3 st, elem)
#    define BOOST_PP_SET_FILTER_O_IM(s, im, elem) BOOST_PP_SET_FILTER_O_I(s, im, elem)
# else
#    define BOOST_PP_SET_FILTER_O(s, st, elem) BOOST_PP_SET_FILTER_O_I(s, BOOST_PP_TUPLE_ELEM(3, 0, st), BOOST_PP_TUPLE_ELEM(3, 1, st), BOOST_PP_TUPLE_ELEM(3, 2, st), elem)
# endif
#
# define BOOST_PP_SET_FILTER_O_I(s, pred, data, res, elem) (pred, data, res BOOST_PP_EXPR_IF(pred(s, data, elem), (elem)))
#
# /* BOOST_PP_SET_FILTER_S */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_SET_FILTER_S(s, pred, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT_ ## s(BOOST_PP_SET_FILTER_O, (pred, data, (nil)), set)))
# else
#    define BOOST_PP_SET_FILTER_S(s, pred, data, set) BOOST_PP_SET_FILTER_S_I(s, pred, data, set)
#    define BOOST_PP_SET_FILTER_S_I(s, pred, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT_ ## s(BOOST_PP_SET_FILTER_O, (pred, data, (nil)), set)))
# endif
#
# endif
