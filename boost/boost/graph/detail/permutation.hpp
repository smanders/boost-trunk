#ifndef BOOST_PERMUTATION_HPP
#define BOOST_PERMUTATION_HPP

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <boost/graph/detail/shadow_iterator.hpp>

namespace boost {

template <class Iter1, class Iter2>
inline void permute(Iter1 permuter, Iter1 last, Iter2 result)
{
  typedef typename std::iterator_traits<Iter1>::difference_type D;
  D n = 0;
  while (permuter != last) {
    std::swap(result[n], result[*permuter]);
    ++n;
    ++permuter;
  }
}

// Knuth 1.3.3, Vol. 1 p 176
// modified for zero-based arrays
// time ?
//
template <class PermIter>
inline void invert_permutation(PermIter X, PermIter Xend)
{
  typedef typename std::iterator_traits<PermIter>::value_type T;
  T n = Xend - X;
  T m = n;
  T j = -1;

  while (m > 0) {
    T i = X[m-1] + 1;
    if (i > 0) {
      do {
	X[m-1] = j - 1;
	j = -m;
	m = i;
	i = X[m-1] + 1;
      } while (i > 0);
      i = j;
    }
    X[m-1] = -i - 1;
    --m;
  }
}

// Takes a "normal" permutation array (and its inverse), and turns it
// into a BLAS-style permutation array (which can be thought of as a
// serialized permutation).
template <class Iter1, class Iter2, class Iter3>
inline void serialize_permutation(Iter1 q, Iter1 q_end, Iter2 q_inv, Iter3 p)
{
  typedef typename std::iterator_traits<Iter1>::value_type P1;
  typedef typename std::iterator_traits<Iter2>::value_type P2;
  typedef typename std::iterator_traits<Iter1>::difference_type D;
  D n = q_end - q;
  for (D i = 0; i < n; ++i) {
    P1 qi = q[i];
    P2 qii = q_inv[i];
    *p++ = qii;
    std::swap(q[i], q[qii]);
    std::swap(q_inv[i], q_inv[qi]);
  }
}

// Not used anymore, leaving it here for future reference.
template <typename Iter, typename Compare>
void merge_sort(Iter first, Iter last, Compare cmp)
{
  if (first + 1 < last) {
    Iter mid = first + (last - first)/2;
    merge_sort(first, mid, cmp);
    merge_sort(mid, last, cmp);
    std::inplace_merge(first, mid, last, cmp);
  }
}


// time: N log N + 3N + ?
// space: 2N
template <class Iter, class IterP, class Cmp, class Alloc>
inline void sortp(Iter first, Iter last, IterP p, Cmp cmp, Alloc alloc)
{
  typedef typename std::iterator_traits<IterP>::value_type P;
  typedef typename std::iterator_traits<IterP>::difference_type D;
  D n = last - first;
  std::vector<P, Alloc> q(n);
  for (D i = 0; i < n; ++i) 
    q[i] = i;
  std::sort(make_shadow_iter(first, q.begin()),
	    make_shadow_iter(last, q.end()),
	    shadow_cmp<Cmp>(cmp));
  invert_permutation(q.begin(), q.end());
  std::copy(q.begin(), q.end(), p);
}

template <class Iter, class IterP, class Cmp>
inline void sortp(Iter first, Iter last, IterP p, Cmp cmp)
{
  typedef typename std::iterator_traits<IterP>::value_type P;  
  sortp(first, last, p, cmp, std::allocator<P>());
}

template <class Iter, class IterP>
inline void sortp(Iter first, Iter last, IterP p)
{
  typedef typename std::iterator_traits<Iter>::value_type T;  
  typedef typename std::iterator_traits<IterP>::value_type P;  
  sortp(first, last, p, std::less<T>(), std::allocator<P>());
}

template <class Iter, class IterP, class Cmp, class Alloc>
inline void sortv(Iter first, Iter last, IterP p, Cmp cmp, Alloc alloc)
{
  typedef typename std::iterator_traits<IterP>::value_type P;
  typedef typename std::iterator_traits<IterP>::difference_type D;
  D n = last - first;
  std::vector<P, Alloc> q(n), q_inv(n);
  for (D i = 0; i < n; ++i) 
    q_inv[i] = i;
  std::sort(make_shadow_iter(first, q_inv.begin()), 
	    make_shadow_iter(last, q_inv.end()), 
	    shadow_cmp<Cmp>(cmp));
  std::copy(q_inv, q_inv.end(), q.begin());
  invert_permutation(q.begin(), q.end());
  serialize_permutation(q.begin(), q.end(), q_inv.end(), p);
}


} // namespace boost

#endif // BOOST_PERMUTATION_HPP
