// Example program of how to use interval<> of rational<> numbers.
//
// Sylvain Pion, Guillaume Melquiond, 2002.

// it would have been enough to only include:
//   <boost/numeric/interval/interval.hpp>
// but it's a bit overkill to include processor intrinsics
// and transcendental functions, so we do it by ourselves

#include <boost/numeric/interval/interval.hpp>      // base class
#include <boost/numeric/interval/rounded_arith.hpp> // default arithmetic rounding policy
#include <boost/numeric/interval/checking.hpp>      // default checking policy
#include <boost/numeric/interval/arith.hpp>         // += *= -= etc
#include <boost/numeric/interval/policies.hpp>      // default policy

#include <boost/rational.hpp>
#include <iostream>

typedef boost::rational<int> Rat;
typedef boost::numeric::interval<Rat> Interval;

std::ostream& operator<<(std::ostream& os, const Interval& r) {
  os << "[" << r.lower() << "," << r.upper() << "]";
  return os;
}

int main() {
  Rat p(2, 3), q(3, 4);
  Interval z(4, 5);
  Interval a(p, q);
  a += z;
  z *= q;
  a -= p;
  a /= q;
  std::cout << z << std::endl;
  std::cout << a << std::endl;
}
