
//  (C) Copyright John Maddock 2005.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef BOOST_TT_IS_UNSIGNED_HPP_INCLUDED
#define BOOST_TT_IS_UNSIGNED_HPP_INCLUDED

#include "boost/type_traits/is_integral.hpp"
#include "boost/type_traits/is_enum.hpp"
#include "boost/type_traits/detail/ice_or.hpp"

// should be the last #include
#include "boost/type_traits/detail/bool_trait_def.hpp"

namespace boost {

namespace detail{

template <class T>
struct is_unsigned_helper
{
   BOOST_STATIC_CONSTANT(bool, value = (static_cast<T>(-1) > 0));
};

template <bool integral_type>
struct is_unsigned_select_helper
{
   template <class T>
   struct rebind
   {
      typedef is_unsigned_helper<T> type;
   };
};

template <>
struct is_unsigned_select_helper<false>
{
   template <class T>
   struct rebind
   {
      typedef false_type type;
   };
};

template <class T>
struct is_unsigned_imp
{
   typedef is_unsigned_select_helper< 
      ::boost::type_traits::ice_or<
         ::boost::is_integral<T>::value,
         ::boost::is_enum<T>::value>::value 
   > selector;
   typedef typename selector::template rebind<T> binder;
   typedef typename binder::type type;
   BOOST_STATIC_CONSTANT(bool, value = type::value);
};

}

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_unsigned,T,::boost::detail::is_unsigned_imp<T>::value)

} // namespace boost

#include "boost/type_traits/detail/bool_trait_undef.hpp"

#endif // BOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
