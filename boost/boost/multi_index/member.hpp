/* Copyright Joaqu�n M L�pez Mu�oz 2003-2004. Use, modification, and
 * distribution are subject to the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_MEMBER_HPP
#define BOOST_MULTI_INDEX_MEMBER_HPP

#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_const.hpp>
#include <cstddef>

namespace boost{

template<class T> class reference_wrapper; /* fwd decl. */

namespace multi_index{

namespace detail{

/* member is a read/write key extractor for accessing a given
 * member of a class.
 * Additionally, member is overloaded to support referece_wrappers
 * of T and "chained pointers" to T's. By chained pointer to T we mean
 * a type P  such that, given a p of Type P
 *   *...n...*x is convertible to T&, for some n>=1.
 * Examples of chained pointers are raw and smart pointers, iterators and
 * arbitrary combinations of these (vg. T** or auto_ptr<T*>.)
 */

/* NB. Some overloads of operator() have an extra dummy parameter int=0.
 * This is so because MSVC++ 6.0 otherwise *incorrectly* regards these
 * overloads as specializations of a previous member function template.
 * Left for all compilers as it does no harm.
 */

template<class Class,typename Type,Type Class::*PtrToMember>
struct const_member_base
{
  typedef Type result_type;

  template<typename ChainedPtr>
  Type& operator()(const ChainedPtr& x)const
  {
    return operator()(*x);
  }

  Type& operator()(const Class& x)const
  {
    return x.*PtrToMember;
  }

  Type& operator()(const reference_wrapper<const Class>& x)const
  {
    return operator()(x.get());
  }

  Type& operator()(const reference_wrapper<Class> x)const
  { 
    return operator()(x.get());
  }
};

template<class Class,typename Type,Type Class::*PtrToMember>
struct non_const_member_base
{
  typedef Type result_type;

  template<typename ChainedPtr>
  Type& operator()(const ChainedPtr& x)const
  {
    return operator()(*x);
  }

  const Type& operator()(const Class& x,int=0)const
  {
    return x.*PtrToMember;
  }

  Type& operator()(Class& x)const
  { 
    return x.*PtrToMember;
  }

  const Type& operator()(const reference_wrapper<const Class>& x,int=0)const
  {
    return operator()(x.get());
  }

  Type& operator()(const reference_wrapper<Class>& x)const
  { 
    return operator()(x.get());
  }
};

} /* namespace multi_index::detail */

template<class Class,typename Type,Type Class::*PtrToMember>
struct member:
  mpl::if_c<
    is_const<Type>::value,
    detail::const_member_base<Class,Type,PtrToMember>,
    detail::non_const_member_base<Class,Type,PtrToMember>
  >::type
{
};

namespace detail{

/* MSVC++ 6.0 does not support properly pointers to members as
 * non-type template arguments, as reported in
 *   http://support.microsoft.com/default.aspx?scid=kb;EN-US;249045
 * We provide an alternative to member<> accepting offsets instead
 * of pointers to members. This happens to work even for non-POD
 * types (although the standard forbids use of offsetof on these),
 * so it serves as a workaround in this compiler for all practical
 * purposes.
 * Surprisingly enough, other compilers (Intel C++ 7.1 at least)
 * have similar bugs. This replacement of member<> can be used for
 * them too.
 */

template<class Class,typename Type,std::size_t OffsetOfMember>
struct const_member_offset_base
{
  typedef Type result_type;

  template<typename ChainedPtr>
  Type& operator()(const ChainedPtr& x)const
  {
    return operator()(*x);
  }

  Type& operator()(const Class& x)const
  {
    return *static_cast<const Type*>(
      static_cast<const void*>(
        static_cast<const char*>(
          static_cast<const void *>(&x))+OffsetOfMember));
  }

  Type& operator()(const reference_wrapper<const Class>& x)const
  {
    return operator()(x.get());
  }

  Type& operator()(const reference_wrapper<Class>& x)const
  {
    return operator()(x.get());
  }
};

template<class Class,typename Type,std::size_t OffsetOfMember>
struct non_const_member_offset_base
{
  typedef Type result_type;

  template<typename ChainedPtr>
  Type& operator()(const ChainedPtr& x)const
  {
    return operator()(*x);
  }

  const Type& operator()(const Class& x,int=0)const
  {
    return *static_cast<const Type*>(
      static_cast<const void*>(
        static_cast<const char*>(
          static_cast<const void *>(&x))+OffsetOfMember));
  }

  Type& operator()(Class& x)const
  { 
    return *static_cast<Type*>(
      static_cast<void*>(
        static_cast<char*>(static_cast<void *>(&x))+OffsetOfMember));
  }

  const Type& operator()(const reference_wrapper<const Class>& x,int=0)const
  {
    return operator()(x.get());
  }

  Type& operator()(const reference_wrapper<Class>& x)const
  {
    return operator()(x.get());
  }
};

} /* namespace multi_index::detail */

template<class Class,typename Type,std::size_t OffsetOfMember>
struct member_offset:
  mpl::if_c<
    is_const<Type>::value,
    detail::const_member_offset_base<Class,Type,OffsetOfMember>,
    detail::non_const_member_offset_base<Class,Type,OffsetOfMember>
  >::type
{
};

/* A proposal has been issued to add a defect macro in Boost.Config to detect
 * this problem with pointer to members as template arguments. While
 * the macro gets into the library, we follow our own heuristics in order to
 * define BOOST_MULTI_INDEX_MEMBER as a convenient wrapper of member<> and
 * member_offset<>
 */

#if defined(BOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS) ||\
    defined(BOOST_MSVC)&&(BOOST_MSVC<1300) ||\
    defined(BOOST_INTEL_CXX_VERSION)&&defined(_MSC_VER)&&\
           (BOOST_INTEL_CXX_VERSION<=700)
#define BOOST_MULTI_INDEX_MEMBER(Class,Type,MemberName) \
::boost::multi_index::member_offset<Class,Type,offsetof(Class,MemberName)>
#else
#define BOOST_MULTI_INDEX_MEMBER(Class,Type,MemberName) \
::boost::multi_index::member<Class,Type,&Class::MemberName>
#endif

} /* namespace multi_index */

} /* namespace boost */

#endif
