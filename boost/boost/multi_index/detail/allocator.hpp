/* Copyright 2003-2004 Joaqu�n M L�pez Mu�oz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_DETAIL_ALLOCATOR_HPP
#define BOOST_MULTI_INDEX_DETAIL_ALLOCATOR_HPP

#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <boost/detail/workaround.hpp>
#include <boost/mpl/aux_/msvc_never_true.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/type_traits/is_same.hpp>
#include <cstddef>
#include <memory>
#include <new>

namespace boost{

namespace multi_index{

namespace detail{

/* Allocator adaption layer. Some stdlibs provide allocators without rebind
 * and template ctors. These facilities are simulated with the external
 * template class rebind_to and the aid of partial_std_allocator_wrapper.
 */

namespace allocator{

/* Detects whether a given allocator belongs to a defective stdlib not
 * having the required member templates.
 * Note that it does not suffice to check the Boost.Config stdlib
 * macros, as the user might have passed a custom, compliant allocator.
 * The checks also considers partial_std_allocator_wrapper to be
 * a standard defective allocator.
 */

#if defined(BOOST_NO_STD_ALLOCATOR)&&\
  (defined(BOOST_HAS_PARTIAL_STD_ALLOCATOR)||defined(BOOST_DINKUMWARE_STDLIB))

template<typename Allocator>
struct is_partial_std_allocator
{
  BOOST_STATIC_CONSTANT(bool,
    value=
      (is_same<
        std::allocator<BOOST_DEDUCED_TYPENAME Allocator::value_type>,
        Allocator
      >::value)||
      (is_base_and_derived<
        std::allocator<BOOST_DEDUCED_TYPENAME Allocator::value_type>,
        Allocator
      >::value));
};

#else

template<typename Allocator>
struct is_partial_std_allocator
{
  BOOST_STATIC_CONSTANT(bool,value=false);
};

#endif

/* partial_std_allocator_wrapper inherits the functionality of a std
 * allocator while providing a templatized ctor.
 */

template<typename Type>
class partial_std_allocator_wrapper:public std::allocator<Type>
{
public:
  partial_std_allocator_wrapper(){};

  template<typename Other>
  partial_std_allocator_wrapper(const partial_std_allocator_wrapper<Other>&){}

#if defined(BOOST_DINKUMWARE_STDLIB)
  /* Dinkumware guys didn't provide the single-argument allocate memfun */

  Type* allocate(std::size_t n)
  {
    std::allocator<Type>& a=*this;
    return a.allocate(n,0);
  }
#endif

};

/* rebind operations for defective std allocators */

template<typename Allocator,typename Type>
struct partial_std_allocator_rebind_to
{
  typedef partial_std_allocator_wrapper<Type> type;
};

/* rebind operation in all other cases */

#if BOOST_WORKAROUND(BOOST_MSVC,<1300)
/* Workaround for a problem in MSVC with dependent template typedefs
 * when doing rebinding of allocators.
 * Modeled after <boost/mpl/aux_/msvc_dtw.hpp> (thanks, Aleksey!)
 */

template<typename Allocator>
struct rebinder
{
  template<bool> struct fake_allocator:Allocator{};
  template<> struct fake_allocator<true>
  {
    template<typename Type> struct rebind{};
  };

  template<typename Type>
  struct result:
    fake_allocator<mpl::aux::msvc_never_true<Allocator>::value>::
      template rebind<Type>
  {
  };
};
#else
template<typename Allocator>
struct rebinder
{
  template<typename Type>
  struct result:Allocator::BOOST_NESTED_TEMPLATE rebind<Type>
  {
  };
};
#endif

template<typename Allocator,typename Type>
struct compliant_allocator_rebind_to
{
  typedef typename rebinder<Allocator>::
      BOOST_NESTED_TEMPLATE result<Type>::other type;
};

/* rebind front-end */

template<typename Allocator,typename Type>
struct rebind_to:
  mpl::if_c<
    is_partial_std_allocator<Allocator>::value,
    partial_std_allocator_rebind_to<Allocator,Type>,
    compliant_allocator_rebind_to<Allocator,Type>
  >::type
{
};

/* allocator-independent versions of construct and destroy */

template<typename Type>
void construct(void* p,const Type& t)
{
  new (p) Type(t);
}

template<typename Type>
void destroy(const Type* p)
{
  p->~Type();
}

} /* namespace multi_index::detail::allocator */

} /* namespace multi_index::detail */

} /* namespace multi_index */

} /* namespace boost */

#endif
