/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_SEGMENTED_BEGIN_HPP_INCLUDED)
#define BOOST_FUSION_SEGMENTED_BEGIN_HPP_INCLUDED

#include <boost/fusion/sequence/intrinsic/detail/segmented_begin_impl.hpp>
#include <boost/fusion/iterator/segmented_iterator.hpp>

namespace boost { namespace fusion { namespace detail
{
    //auto segmented_begin( seq )
    //{
    //    return make_segmented_iterator( segmented_begin_impl( seq, nil ) );
    //}

    template <typename Sequence>
    struct segmented_begin
    {
        typedef
            segmented_iterator<
                typename segmented_begin_impl<Sequence, fusion::nil>::type
            >
        type;

        static type call(Sequence& seq)
        {
            return type(
                segmented_begin_impl<Sequence, fusion::nil>::call(seq, fusion::nil()));
        }
    };

}}}

#endif
