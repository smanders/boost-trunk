// (C) Copyright David Abrahams 2001. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
// Template class is_signed and its documentation is:
// (C) Copyright Howard Hinnant 2001. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
// Template class numeric_traits<Number> --
//
//    Supplies:
//
//      typedef difference_type -- a type used to represent the difference
//      between any two values of Number.
//
//    Support:
//      1. Not all specializations are supplied
//
//      2. Use of specializations that are not supplied will cause a
//      compile-time error
//
//      3. Users are free to specialize numeric_traits for any type.
//
//      4. Right now, specializations are only supplied for integer types.
//
//      5. On implementations which do not supply compile-time constants in
//      std::numeric_limits<>, only specializations for built-in integer types
//      are supplied.
//
//      6. Handling of numbers whose range of representation is at least as
//      great as boost::intmax_t can cause some differences to be
//      unrepresentable in difference_type:
//
//        Number    difference_type
//        ------    ---------------
//        signed    Number
//        unsigned  intmax_t
//
// template <class Number> typename numeric_traits<Number>::difference_type
// numeric_distance(Number x, Number y)
//    computes (y - x), attempting to avoid overflows.
//

// See http://www.boost.org for most recent version including documentation.

// Revision History
// 21 Jan 2001 - Created

#ifndef BOOST_NUMERIC_TRAITS_HPP_DWA20001901
# define BOOST_NUMERIC_TRAITS_HPP_DWA20001901

# include <boost/config.hpp>
# include <boost/cstdint.hpp>
# include <boost/static_assert.hpp>
# include <boost/type_traits.hpp>
# ifndef BOOST_NO_LIMITS
#  include <limits>
# endif

namespace boost { namespace detail {

  // Template class if_true -- select among 2 types based on a bool constant expression
  // Usage:
  //   typename if_true<(bool_const_expression)>::template then<true_type, false_type>::type
  template <bool> struct if_true;

  template <>
  struct if_true<true>
  {
      template <class T1, class T2>
      struct then { typedef T1 type; };
  };

  template <>
  struct if_true<false>
  {
      template <class T1, class T2>
      struct then { typedef T2 type; };
  };

  // Template class is_signed -- determine whether a numeric type is signed
  // Requires that T is constructable from the literals -1 and 0.  Compile-time
  // error results if that requirement is not met (and thus signedness is not
  // likely to have meaning for that type).
  template <class Number>
  struct is_signed
  {
      enum { value = (Number(-1) < Number(0)) };
  };

  // Template class integer_traits<Integer> -- traits of various integer types
  // This should probably be rolled into boost::integer_traits one day, but I
// need it to work without <limits>
  template <class Integer>
  struct integer_traits
  {
# ifndef BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
   private:
      typedef Integer integer_type;
      typedef std::numeric_limits<integer_type> x;
#   ifdef BOOST_MSVC
      enum { is_integer = x::is_integer };
#   endif
   public:
#  ifndef BOOST_MSVC // for some reason, this asserts when it shouldn't
      BOOST_STATIC_ASSERT(x::is_integer);
#  else
      BOOST_STATIC_ASSERT(is_integer);
#  endif
      typedef typename
      if_true<x::is_signed
              & !(x::is_bounded
                  | x::digits >= std::numeric_limits<boost::intmax_t>::digits)>::template then<
        Integer,
          
      typename if_true<(x::digits < std::numeric_limits<signed int>::digits)>::template then<
        signed int,

      typename if_true<(x::digits < std::numeric_limits<signed long>::digits)>::template then<
        signed long,

   // else
        intmax_t
      >::type>::type>::type difference_type;
# else
      BOOST_STATIC_ASSERT(boost::is_integral<Integer>::value);
      
      typedef typename
      if_true<(sizeof(Integer) >= sizeof(intmax_t))>::template then<
               
        typename if_true<is_signed<Integer>::value>::template then<
          Integer,
          intmax_t
        >::type,

        typename if_true<(sizeof(Integer) < sizeof(std::ptrdiff_t))>::template then<
          std::ptrdiff_t,
          intmax_t
        >::type
      >::type difference_type;
# endif
  };

  // Right now, only supports integers, but should be expanded.
  template <class Number>
  struct numeric_traits
  {
      typedef typename integer_traits<Number>::difference_type difference_type;
  };

  template <class Number>
  typename numeric_traits<Number>::difference_type numeric_distance(Number x, Number y)
  {
      typedef typename numeric_traits<Number>::difference_type difference_type;
      return difference_type(y) - difference_type(x);
  }
}}

#endif // BOOST_NUMERIC_TRAITS_HPP_DWA20001901
