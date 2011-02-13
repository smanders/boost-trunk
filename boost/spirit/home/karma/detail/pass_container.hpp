/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_PASS_CONTAINER_MAR_15_2009_0114PM)
#define SPIRIT_PASS_CONTAINER_MAR_15_2009_0114PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/detail/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/handles_container.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail
{
    // has_same_elements: utility to check if the LHS attribute
    // is an STL container and that its value_type is convertible
    // to the RHS.

    template <typename RHS, typename LHSAttribute
      , bool IsContainer = traits::is_container<LHSAttribute>::value
      , bool IsSequence = fusion::traits::is_sequence<LHSAttribute>::value>
    struct has_same_elements : mpl::false_ {};

    template <typename RHS, typename LHSAttribute>
    struct has_same_elements<RHS, LHSAttribute, true, false>
      : mpl::or_<
            is_convertible<RHS, typename LHSAttribute::value_type>
          , traits::is_hold_any<typename LHSAttribute::value_type>
        > {};

    template <typename RHS, typename T>
    struct has_same_elements<RHS, optional<T>, false, false>
      : has_same_elements<RHS, T> {};

    template <typename RHS, typename T>
    struct has_same_elements<RHS, optional<T>, true, false>
      : has_same_elements<RHS, T> {};

    template <typename RHS, typename T>
    struct has_same_elements<RHS, optional<T>, false, true>
      : has_same_elements<RHS, T> {};

#define BOOST_SPIRIT_IS_CONVERTIBLE(z, N, data)                               \
        has_same_elements<RHS, BOOST_PP_CAT(T, N)>::value ||                  \
    /***/

    // Note: variants are treated as containers if one of the held types is a
    //       container (see support/container.hpp).
    template <typename RHS, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct has_same_elements<
            RHS, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>, true, false>
      : mpl::bool_<BOOST_PP_REPEAT(BOOST_VARIANT_LIMIT_TYPES
          , BOOST_SPIRIT_IS_CONVERTIBLE, _) false> {};

#undef BOOST_SPIRIT_IS_CONVERTIBLE

    // Specialization for fusion sequences, in this case we check whether the
    // lhs attribute is convertible to the types in the sequence.
    // We return false if the rhs attribute itself is a fusion sequence.
    template <typename RHS, typename LHSAttribute>
    struct has_same_elements<RHS, LHSAttribute, false, true>
    {
        typedef typename mpl::find_if<
            LHSAttribute, mpl::not_<is_convertible<RHS, mpl::_1> >
        >::type iter;
        typedef typename mpl::end<LHSAttribute>::type end;

        typedef typename is_same<iter, end>::type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // This function handles the case where the attribute (Attr) given
    // to the sequence is an STL container. This is a wrapper around F.
    // The function F does the actual generating.
    template <typename F, typename Attr, typename Iterator, typename Strict>
    struct pass_container
    {
        typedef typename F::context_type context_type;

        pass_container(F const& f, Iterator begin, Iterator end)
          : f(f), iter(begin), end(end) 
        {}

        bool is_at_end() const
        {
            return traits::compare(iter, end);
        }

        void next()
        {
            traits::next(iter);
        }

        // this is for the case when the current element expects an attribute
        // which is taken from the next entry in the container
        template <typename Component>
        bool dispatch_attribute_element(Component const& component, mpl::false_) const
        {
            // get the next value to generate from container
            if (!is_at_end() && !f(component, traits::deref(iter))) 
            {
                // needs to return false as long as everything is ok
                traits::next(iter);
                return false;
            }

//             // in non-strict mode increment iterator if the underlying 
//             // generator failed
//             if (!Strict::value && !is_at_end())
//                 traits::next(iter);

            // either no elements available any more or generation failed
            return true;
        }

        // This is for the case when the current element expects an attribute
        // which is a container itself, this element will get the rest of the 
        // attribute container.
        template <typename Component>
        bool dispatch_attribute_element(Component const& component, mpl::true_) const
        {
            return f(component, make_iterator_range(iter, end));
        }

        // This handles the distinction between elements in a sequence expecting
        // containers themselves and elements expecting non-containers as their 
        // attribute. Note: is_container treats optional<T>, where T is a 
        // container as a container as well.
        template <typename Component>
        bool dispatch_attribute(Component const& component, mpl::true_) const
        {
            typedef typename traits::attribute_of<
                Component, context_type>::type attribute_type;

//             typedef mpl::and_<
//                 traits::is_container<attribute_type>
//               , is_convertible<Attr, attribute_type> > predicate;

            typedef mpl::and_<
                traits::is_container<attribute_type>
              , traits::handles_container<Component, Attr, context_type>
            > predicate;

            return dispatch_attribute_element(component, predicate());
        }

        // this is for the case when the current element doesn't expect an 
        // attribute
        template <typename Component>
        bool dispatch_attribute(Component const& component, mpl::false_) const
        {
            return f(component, unused);
        }

        // This handles the case where the attribute of the component
        // is not a STL container or which elements are not 
        // convertible to the target attribute (Attr) value_type.
        template <typename Component>
        bool dispatch_main(Component const& component, mpl::false_) const
        {
            // we need to dispatch again depending on the type of the attribute
            // of the current element (component). If this is has no attribute
            // we shouldn't use an element of the container but unused_type 
            // instead
            typedef traits::not_is_unused<
                typename traits::attribute_of<Component, context_type>::type
            > predicate;

            return dispatch_attribute(component, predicate());
        }

        // This handles the case where the attribute of the component is
        // an STL container *and* its value_type is convertible to the
        // target attribute's (Attr) value_type.
        template <typename Component>
        bool dispatch_main(Component const& component, mpl::true_) const
        {
            return f(component, make_iterator_range(iter, end));
        }

        // Dispatches to dispatch_main depending on the attribute type
        // of the Component
        template <typename Component>
        bool operator()(Component const& component) const
        {
            typedef typename traits::container_value<Attr>::type rhs;
            typedef typename traits::attribute_of<
                Component, context_type>::type lhs_attribute;

            typedef mpl::and_<
                mpl::or_<
                    has_same_elements<rhs, lhs_attribute>
                  , has_same_elements<Attr, lhs_attribute> >
              , traits::handles_container<Component, Attr, context_type>
            > predicate;

            // false means everything went ok
            return dispatch_main(component, predicate());
        }

        F f;
        mutable Iterator iter;
        mutable Iterator end;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        pass_container& operator= (pass_container const&);
    };

    // Utility function to make a pass_container
    template <typename Strict, typename F, typename Attr, typename Iterator>
    pass_container<F, Attr, Iterator, Strict>
    inline make_pass_container(F const& f, Attr& attr, Iterator b, Iterator e)
    {
        return pass_container<F, Attr, Iterator, Strict>(f, attr, b, e);
    }
}}}}

#endif
