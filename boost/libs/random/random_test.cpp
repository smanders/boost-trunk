/* boost random_test.cpp various tests
 *
 * Copyright Jens Maurer 2000
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * Jens Maurer makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * $Id$
 */

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <iterator>
#include <vector>
#include <boost/random.hpp>
#include <boost/config.hpp>

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

#ifdef BOOST_NO_STDC_NAMESPACE
  namespace std { using ::fabs; }
#endif


/*
 * General portability note:
 * MSVC mis-compiles explicit function template instantiations.
 * For example, f<A>() and f<B>() are both compiled to call f<A>().
 * BCC is unable to implicitly convert a "const char *" to a std::string
 * when using explicit function template instantiations.
 *
 * Therefore, avoid explicit function template instantiations.
 */

/*
 * Validate correct implementation
 */

// own run
bool check(unsigned long x, const boost::mt11213b&) { return x == 0xa37d3c92; }

// validation by experiment from mt19937.c
bool check(unsigned long x, const boost::mt19937&) { return x == 3346425566U; }

// validation values from the publications
bool check(int x, const boost::minstd_rand0&) { return x == 1043618065; }

// validation values from the publications
bool check(int x, const boost::minstd_rand&) { return x == 399268537; }

// by experiment from lrand48()
bool check(unsigned long x, const boost::rand48&) { return x == 1993516219; }

// ????
bool check(unsigned long x, const boost::taus88&) { return x == 3535848941U; }

// ????
bool check(int x, const boost::ecuyer1988&) { return x == 2060321752; }

// validation by experiment from Harry Erwin's generator.h (private e-mail)
bool check(unsigned int x, const boost::kreutzer1986&) { return x == 139726; }

bool check(double x, const boost::lagged_fibonacci607&) { return x == 0.4293817707235914; }

// principal operation validated with CLHEP, values by experiment
bool check(unsigned long x, const boost::ranlux3&) { return x == 5957620; }
bool check(unsigned long x, const boost::ranlux4&) { return x == 8587295; }

bool check(float x, const boost::ranlux3_01&)
{ return std::abs(x-5957620/std::pow(2.0f,24)) < 1e-6; }
bool check(float x, const boost::ranlux4_01&)
{ return std::abs(x-8587295/std::pow(2.0f,24)) < 1e-6; }

template<class PRNG>
void validate(const std::string & name, const PRNG &)
{
  std::cout << "validating " << name << ": ";
  PRNG rng;  // default ctor
  for(int i = 0; i < 9999; i++)
    rng();
  typename PRNG::result_type val = rng();
  // make sure the validation function is a static member
  bool result = check(val, rng);
  
  // allow for a simple eyeball check for MSVC instantiation brokenness
  // (if the numbers for all generators are the same, it's obviously broken)
  std::cout << val << std::endl;
  BOOST_TEST(result);
}

void validate_all()
{
  using namespace boost;
#if !defined(BOOST_NO_INT64_T) && !defined(BOOST_NO_INTEGRAL_INT64_T)
  validate("rand48", rand48());
#endif
  validate("minstd_rand", minstd_rand());
  validate("minstd_rand0", minstd_rand0());
  validate("ecuyer combined", ecuyer1988());
  validate("mt11213b", mt11213b());
  validate("mt19937", mt19937());
  validate("kreutzer1986", kreutzer1986());
  validate("ranlux3", ranlux3());
  validate("ranlux4", ranlux4());
  validate("ranlux3_01", ranlux3_01());
  validate("ranlux4_01", ranlux4_01());
  validate("taus88", taus88());
  validate("lagged_fibonacci607", lagged_fibonacci607());
}


/*
 * Check function signatures
 */

template<class Dist>
void instantiate_dist(const Dist& dist)
{
  // check reference maintenance throughout
  typename Dist::base_type& b = dist.base();
  Dist d = dist;       // copy ctor
  typename Dist::result_type result = d();
  (void) &result;      // avoid "unused variable" warning
  b();
  BOOST_TEST(d.base() == b);
  d.reset();
  // d = dist;            // copy assignment
  // b();
  // BOOST_TEST(d.base() == b);
}

template<class URNG, class ResultType>
void instantiate_urng(const std::string & s, const URNG &, const ResultType &)
{
  std::cout << "Basic tests for " << s;
  URNG urng;
  int a[URNG::has_fixed_range ? 5 : 10];        // compile-time constant
  (void) a;   // avoid "unused" warning
  typename URNG::result_type x1 = urng();
  ResultType x2 = x1;
  (void) &x2;           // avoid "unused" warning

#if !defined(BOOST_MSVC) || BOOST_MSVC > 1300 // MSVC brokenness
  URNG urng2 = urng;             // copy constructor
  BOOST_TEST(urng == urng2);     // operator==
  BOOST_TEST(!(urng != urng2));  // operator!=
  urng();
  urng2 = urng;                  // copy assignment
  BOOST_TEST(urng == urng2);
#endif // BOOST_MSVC

  const std::vector<int> v(9999u, 0x41);
  std::vector<int>::const_iterator it = v.begin();
  URNG urng3(it, v.end());
  BOOST_TEST(it != v.begin());
  std::cout << "; seeding uses " << (it - v.begin()) << " words" << std::endl;

  bool have_exception = false;
  try {
    // now check that exceptions are thrown
    it = v.end();
    urng3.seed(it, v.end());
  } catch(std::invalid_argument& x) {
    have_exception = true;
  }
  BOOST_TEST(have_exception);

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
  // Streamable concept not supported for broken compilers

  // advance a little so that state is relatively arbitrary
  for(int i = 0; i < 9307; ++i)
    urng();
  urng2 = urng;

  {
    // narrow stream first
    std::ostringstream file;
    file << urng;
    // move forward
    urng();
    // restore old state
    std::istringstream input(file.str());
    input >> urng;
    // std::cout << file.str() << std::endl;
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1300 // MSVC brokenness
    // advance some more so that state is exercised
    for(int i = 0; i < 10000; ++i) {
      urng();
      urng2();
    }
    BOOST_TEST(urng == urng2);
#endif // BOOST_MSVC
  }
  
  urng2 = urng;
  {
    // then wide stream
    std::wostringstream file;
    file << urng;
    // move forward
    urng();
    std::wistringstream input(file.str());
    input >> urng;
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1300 // MSVC brokenness
    BOOST_TEST(urng == urng2);
#endif // BOOST_MSVC
  }
#endif // BOOST_NO_OPERATORS_IN_NAMESPACE

  // instantiate various distributions with this URNG
  instantiate_dist(boost::uniform_01<URNG>(urng));
  instantiate_dist(boost::uniform_smallint<URNG>(urng, 0, 11));
  instantiate_dist(boost::uniform_int<URNG>(urng, -200, 20000));
  instantiate_dist(boost::uniform_real<URNG>(urng, 0, 2.1));

  instantiate_dist(boost::bernoulli_distribution<URNG>(urng, 0.2));
  instantiate_dist(boost::binomial_distribution<URNG>(urng, 4, 0.2));
  instantiate_dist(boost::geometric_distribution<URNG>(urng, 0.8));
  instantiate_dist(boost::triangle_distribution<URNG>(urng, 1, 1.5, 7));
  instantiate_dist(boost::exponential_distribution<URNG>(urng, 5));
  instantiate_dist(boost::normal_distribution<URNG>(urng));
  instantiate_dist(boost::lognormal_distribution<URNG>(urng, 1, 1));
  instantiate_dist(boost::poisson_distribution<URNG>(urng, 1));
  instantiate_dist(boost::uniform_on_sphere<URNG>(urng, 2));
}

void instantiate_all()
{
  using namespace boost;

#if !defined(BOOST_NO_INT64_T) && !defined(BOOST_NO_INTEGRAL_INT64_T)
  instantiate_urng("rand48", rand48(), 0);
  rand48 rnd(boost::int32_t(5));
  rand48 rnd2(boost::uint64_t(0x80000000) * 42);
  rnd.seed(boost::int32_t(17));
  rnd2.seed(boost::uint64_t(0x80000000) * 49);
#endif

  instantiate_urng("minstd_rand0", minstd_rand0(), 0);
  instantiate_urng("minstd_rand", minstd_rand(), 0);
  minstd_rand mstd(42);
  mstd.seed(17);

  instantiate_urng("ecuyer1988", ecuyer1988(), 0);
  instantiate_urng("kreutzer1986", kreutzer1986(), 0);
  instantiate_urng("hellekalek1995", hellekalek1995(), 0);
  
  instantiate_urng("mt11213b", mt11213b(), 0u);
  instantiate_urng("mt19937", mt19937(), 0u);
  mt19937 mt(boost::uint32_t(17));  // needs to be an exact type match for MSVC
  int i = 42;
  mt.seed(boost::uint32_t(i));
  mt19937 mt2(mstd);
  mt2.seed(mstd);


  random_number_generator<mt19937> std_rng(mt2);
  (void) std_rng(10);

  instantiate_urng("lagged_fibonacci",
                   boost::random::lagged_fibonacci<uint32_t, 24, 607, 273>(),
                   0u);
  instantiate_urng("lagged_fibonacci607", lagged_fibonacci607(), 0.0);

  instantiate_urng("ranlux3", ranlux3(), 0u);
  instantiate_urng("ranlux4", ranlux4(), 0u);

  instantiate_urng("ranlux3_01", ranlux3_01(), 0.0f);
  instantiate_urng("ranlux4_01", ranlux4_01(), 0.0f);

  instantiate_urng("taus88", taus88(), 0u);
}

/*
 * A few equidistribution tests
 */

// yet to come...

template<class Generator>
void check_uniform_int(Generator & gen, int iter)
{
  std::cout << "testing uniform_int(" << gen.min() << "," << gen.max() 
            << ")" << std::endl;
  int range = gen.max()-gen.min()+1;
  std::vector<int> bucket(range);
  for(int j = 0; j < iter; j++) {
    int result = gen();
    if(result < gen.min() || result > gen.max())
      std::cerr << "   ... delivers " << result << std::endl;
    else
      bucket[result-gen.min()]++;
  }
  int sum = 0;
  // use a different variable name "k", because MSVC has broken "for" scoping
  for(int k = 0; k < range; k++)
    sum += bucket[k];
  double avg = static_cast<double>(sum)/range;
  double threshold = 2*avg/std::sqrt(static_cast<double>(iter));
  for(int i = 0; i < range; i++) {
    if(std::fabs(bucket[i] - avg) > threshold) {
      // 95% confidence interval
      std::cout << "   ... has bucket[" << i << "] = " << bucket[i] 
                << "  (distance " << (bucket[i] - avg) << ")" 
                << std::endl;
    }
  }
}

template<class Generator>
void test_uniform_int(Generator & gen)
{
  typedef boost::uniform_int<Generator, int>  int_gen;

  // large range => small range (modulo case)
  int_gen uint12(gen,1,2);
  BOOST_TEST(uint12.min() == 1);
  BOOST_TEST(uint12.max() == 2);
  check_uniform_int(uint12, 100000);
  int_gen uint16(gen,1,6);
  check_uniform_int(uint16, 100000);

  // test chaining to get all cases in operator()
  typedef boost::uniform_int<int_gen, int> intint_gen;

  // identity map
  intint_gen uint01(uint12, 0, 1);
  check_uniform_int(uint01, 100000);

  // small range => larger range
  intint_gen uint05(uint12, -3, 2);
  check_uniform_int(uint05, 100000);

  typedef boost::uniform_int<intint_gen, int> intintint_gen;

#if 0
  // This takes a lot of time to run and is of questionable net effect:
  // avoid for now.

  // small => larger range, not power of two
  // (for unknown reasons, this has noticeably uneven distribution)
  intintint_gen uint1_49(uint05, 1, 49);
  check_uniform_int(uint1_49, 500000);
#endif

  // larger => small range, rejection case
  intintint_gen uint1_4(uint05, 1, 4);
  check_uniform_int(uint1_4, 100000);
}

#if defined(BOOST_MSVC) && _MSC_VER <= 1200

// These explicit instantiations are necessary, otherwise MSVC does
// not find the <boost/operators.hpp> inline friends.
// We ease the typing with a suitable preprocessor macro.
#define INSTANT(x) \
template class boost::uniform_smallint<x>; \
template class boost::uniform_int<x>; \
template class boost::uniform_real<x>; \
template class boost::bernoulli_distribution<x>; \
template class boost::geometric_distribution<x>; \
template class boost::triangle_distribution<x>; \
template class boost::exponential_distribution<x>; \
template class boost::normal_distribution<x>; \
template class boost::uniform_on_sphere<x>; \
template class boost::lognormal_distribution<x>;

INSTANT(boost::minstd_rand0)
INSTANT(boost::minstd_rand)
INSTANT(boost::ecuyer1988)
INSTANT(boost::kreutzer1986)
INSTANT(boost::hellekalek1995)
INSTANT(boost::mt19937)
INSTANT(boost::mt11213b)

#undef INSTANT
#endif

int test_main(int, char*[])
{
  instantiate_all();
  validate_all();
  boost::mt19937 mt;
  test_uniform_int(mt);

  // bug report from Ken Mahler:  This used to lead to an endless loop.
  boost::minstd_rand r1;
  boost::uniform_int<boost::minstd_rand, unsigned int> r2(r1, 0, 0xffffffff);
  r2();
  r2();

  // bug report from Fernando Cacciola:  This used to lead to an endless loop.
  // also from Douglas Gregor
  boost::minstd_rand rnd;
  boost::uniform_int<boost::minstd_rand> x(rnd,0,8361);
  (void) x();

  return 0;
}
