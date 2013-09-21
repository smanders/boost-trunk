/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/mutexes/timed_mutex_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_

#include <cstddef>
#include <limits>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/throw_exception.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/synchronization.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/interlocked.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/mutexes/mutex_windows.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class timed_mutex
{
private:
    mutex m_mutex;

public:
    BOOST_CONSTEXPR timed_mutex() BOOST_NOEXCEPT : m_mutex()
    {
    }

    void lock()
    {
        m_mutex.lock();
    }

    void unlock() BOOST_NOEXCEPT
    {
        m_mutex.unlock();
    }

    bool try_lock()
    {
        return m_mutex.try_lock();
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_lock(Time const& t)
    {
        return priv_timed_lock(sync::detail::time_traits< Time >::to_sync_unit(t));
    }

    template< typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type try_lock_for(Duration const& rel_time)
    {
        return priv_timed_lock(sync::detail::time_traits< Duration >::to_sync_unit(rel_time));
    }

    template< typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type try_lock_until(TimePoint const& abs_time)
    {
        return priv_timed_lock(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

    BOOST_DELETED_FUNCTION(timed_mutex(timed_mutex const&))
    BOOST_DELETED_FUNCTION(timed_mutex& operator= (timed_mutex const&))

private:
    bool priv_timed_lock(sync::detail::system_time_point const& t)
    {
        return priv_timed_lock(t - sync::detail::system_time_point::now());
    }

    bool priv_timed_lock(sync::detail::system_duration const& t)
    {
        if (m_mutex.try_lock())
            return true;

        long old_count = m_mutex.m_active_count;
        m_mutex.mark_waiting_and_try_lock(old_count);

        if ((old_count & mutex::lock_flag_value) != 0)
        {
            bool lock_acquired = false;
            boost::detail::winapi::HANDLE_ const evt = m_mutex.get_event();

            uint64_t time_left = t.get();
            do
            {
                const unsigned int dur = time_left > (std::numeric_limits< int >::max)() ?
                    static_cast< unsigned int >((std::numeric_limits< int >::max)()) : static_cast< unsigned int >(time_left);
                const boost::detail::winapi::DWORD_ res = boost::detail::winapi::WaitForSingleObject(evt, dur);
                switch (res)
                {
                case boost::detail::winapi::wait_object_0:
                    m_mutex.clear_waiting_and_try_lock(old_count);
                    lock_acquired = (old_count & mutex::lock_flag_value) == 0;
                    break;

                case boost::detail::winapi::wait_timeout:
                    time_left -= dur;
                    break;

                default:
                    BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(&m_mutex.m_active_count, -1);
                    BOOST_THROW_EXCEPTION(lock_error(res, "boost: timed_mutex timedlock failed in WaitForSingleObject"));
                }
            }
            while (!lock_acquired && time_left > 0);

            if (!lock_acquired)
            {
                BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(&m_mutex.m_active_count, -1);
                return false;
            }
        }

        return true;
    }

    template< typename TimePoint >
    bool priv_timed_lock(sync::detail::chrono_time_point< TimePoint > const& t)
    {
        if (m_mutex.try_lock())
            return true;

        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (priv_timed_lock(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        return false;
    }
};

} // namespace winnt

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_
