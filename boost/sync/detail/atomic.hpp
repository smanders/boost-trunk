// atomic.hpp: pull in std::atomic or boost::atomic
//
// Copyright (C) 2013 Tim Blechmann
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_ATOMIC_HPP
#define BOOST_SYNC_DETAIL_ATOMIC_HPP

#ifdef BOOST_SYNC_USE_STD_ATOMIC
#include <atomic>
#else
#include <boost/atomic.hpp>
#endif

namespace boost  {
namespace sync   {
namespace detail {

#ifdef BOOST_SYNC_USE_STD_ATOMIC

using std::atomic;
using std::atomic_int;
using std::atomic_bool;

using std::memory_order_acquire;
using std::memory_order_acq_rel;
using std::memory_order_release;
using std::memory_order_consume;
using std::memory_order_relaxed;
using std::memory_order_seq_cst;

#else

using boost::atomic;
using boost::atomic_int;
using boost::atomic_bool;

using boost::memory_order_acquire;
using boost::memory_order_release;
using boost::memory_order_acq_rel;
using boost::memory_order_consume;
using boost::memory_order_relaxed;
using boost::memory_order_seq_cst;

#endif

}
}
}

#endif // BOOST_SYNC_DETAIL_ATOMIC_HPP
