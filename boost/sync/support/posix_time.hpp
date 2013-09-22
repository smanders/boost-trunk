/*
 *          Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   support/posix_time.hpp
 *
 * \brief  This header enables support for POSIX time units.
 */

#ifndef BOOST_SYNC_SUPPORT_POSIX_TIME_HPP_INCLUDED_
#define BOOST_SYNC_SUPPORT_POSIX_TIME_HPP_INCLUDED_

#include <time.h>
#include <boost/static_assert.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

template< >
struct time_traits< struct ::timespec >
{
    typedef time_point_tag tag;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static system_time_point to_sync_unit(struct ::timespec const& point)
    {
        BOOST_STATIC_ASSERT_MSG(system_time_point::subsecond_fraction == 1000000000u, "Boost.Sync: Unexpected system time point resolution");
        return system_time_point(point.tv_sec, point.tv_nsec);
    }
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_SUPPORT_POSIX_TIME_HPP_INCLUDED_
