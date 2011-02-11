/* test_normal.cpp
 *
 * Copyright Steven Watanabe 2011
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * $Id$
 *
 */

#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/distributions/normal.hpp>

#define BOOST_RANDOM_DISTRIBUTION boost::random::normal_distribution<>
#define BOOST_RANDOM_DISTRIBUTION_NAME normal
#define BOOST_MATH_DISTRIBUTION boost::math::normal
#define BOOST_RANDOM_ARG1_TYPE double
#define BOOST_RANDOM_ARG1_NAME m
#define BOOST_RANDOM_ARG1_DEFAULT 1000.0
#define BOOST_RANDOM_ARG1_DISTRIBUTION(n) boost::uniform_real<>(-n, n)
#define BOOST_RANDOM_ARG2_TYPE double
#define BOOST_RANDOM_ARG2_NAME s
#define BOOST_RANDOM_ARG2_DEFAULT 1000.0
#define BOOST_RANDOM_ARG2_DISTRIBUTION(n) boost::uniform_real<>(0.00001, n)

#include "test_real_distribution.ipp"
