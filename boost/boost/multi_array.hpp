#ifndef BOOST_MULTI_ARRAY_RG071801_HPP
#define BOOST_MULTI_ARRAY_RG071801_HPP

//
// multi_array.hpp - contains the multi_array class template
// declaration and definition
//

#include "boost/multi_array/base.hpp"
#include "boost/multi_array/collection_concept.hpp"
#include "boost/multi_array/copy_array.hpp"
#include "boost/multi_array/iterator.hpp"
#include "boost/multi_array/subarray.hpp"
#include "boost/multi_array_ref.hpp"
#include "boost/multi_array/algorithm.hpp"
#include "boost/array.hpp"
#include "boost/type_traits.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <vector>

namespace boost {

template<typename T, std::size_t NumDims,
  typename Allocator>
class multi_array : 
  public multi_array_ref<T,NumDims>
{
  typedef multi_array_ref<T,NumDims> super_type;
public: 
  typedef typename super_type::value_type value_type;
  typedef typename super_type::reference reference;
  typedef typename super_type::const_reference const_reference;
  typedef typename super_type::iterator iterator;
  typedef typename super_type::const_iterator const_iterator;
  typedef typename super_type::iter_base iter_base;
  typedef typename super_type::const_iter_base const_iter_base;
  typedef typename super_type::reverse_iterator reverse_iterator;
  typedef typename super_type::const_reverse_iterator const_reverse_iterator;
  typedef typename super_type::element element;
  typedef typename super_type::size_type size_type;
  typedef typename super_type::difference_type difference_type;
  typedef typename super_type::index index;
  typedef typename super_type::extent_range extent_range;


  // concept checks
  // BOOST_CLASS_REQUIRES(T,DefaultConstructibleConcept);
  // BOOST_CLASS_REQUIRES(T,AssignableConcept);
  // BOOST_CLASS_REQUIRES(T,LessThanComparableConcept);
  // BOOST_CLASS_REQUIRES(T,CopyConstructibleConcept);
  template <std::size_t NDims>
  struct const_array_view {
    typedef boost::detail::multi_array::const_multi_array_view<T,NDims> type;
  };

  template <std::size_t NDims>
  struct array_view {
    typedef boost::detail::multi_array::multi_array_view<T,NDims> type;
  };

  template <class ExtentList>
  explicit multi_array(ExtentList const& extents) :
    super_type((T*)initial_base_,extents) {
    boost::function_requires<
      detail::multi_array::CollectionConcept<ExtentList> >();
    allocate_space();
  }

  template <class ExtentList>
  explicit multi_array(ExtentList const& extents,
		       const general_storage_order<NumDims>& so) : 
    super_type((T*)initial_base_,extents,so) {
    boost::function_requires<
      detail::multi_array::CollectionConcept<ExtentList> >();
    allocate_space();
  }

  template <class ExtentList>
  explicit multi_array(ExtentList const& extents,
		       const general_storage_order<NumDims>& so,
		       Allocator const& alloc) :
    super_type((T*)initial_base_,extents,so), allocator_(alloc) {
    boost::function_requires<
      detail::multi_array::CollectionConcept<ExtentList> >();
    allocate_space();
  }


  explicit multi_array(const detail::multi_array
		       ::extent_gen<NumDims>& ranges) :
    super_type((T*)initial_base_,ranges) {

    allocate_space();
  }


  explicit multi_array(const detail::multi_array
		       ::extent_gen<NumDims>& ranges,
		       const general_storage_order<NumDims>& so) :
    super_type((T*)initial_base_,ranges,so) {

    allocate_space();
  }


  explicit multi_array(const detail::multi_array
		       ::extent_gen<NumDims>& ranges,
		       const general_storage_order<NumDims>& so,
		       Allocator const& alloc) :
    super_type((T*)initial_base_,ranges,so), allocator_(alloc) {

    allocate_space();
  }

  multi_array(const multi_array& rhs) :
  super_type(rhs), allocator_(rhs.allocator_) {
    allocate_space();
    boost::copy_n(rhs.base_,rhs.num_elements(),base_);
  }

  template <typename OPtr>
  multi_array(const detail::multi_array::
	      const_sub_array<T,NumDims,OPtr>& rhs) :
    super_type(rhs) {
    allocate_space();
    std::copy(rhs.begin(),rhs.end(),begin());
  }

  // For some reason, gcc 2.95.2 doesn't pick the above template
  // member function when passed a subarray, so i was forced to
  // duplicate the functionality here...
  multi_array(const detail::multi_array::
	      sub_array<T,NumDims>& rhs) :
    super_type(rhs) {
    allocate_space();
    std::copy(rhs.begin(),rhs.end(),begin());
  }

  // Since assignment is a deep copy, multi_array_ref 
  // contains all the necessary code.
  template <typename ConstMultiArray>
  multi_array& operator=(const ConstMultiArray& other) {
    super_type::operator=(other);
    return *this;
  }

  multi_array& operator=(const multi_array& other) {
    if (&other != this) {
      super_type::operator=(other);
    }
    return *this;
  }


  ~multi_array() {
    deallocate_space();
  }

private:
  void allocate_space() {
    typename Allocator::const_pointer no_hint=0;
    base_ = allocator_.allocate(super_type::num_elements(),no_hint);
    super_type::set_base_ptr(base_);
    allocated_elements_ = super_type::num_elements();
    std::uninitialized_fill_n(base_,allocated_elements_,T());
  }

  void deallocate_space() {
    if(base_) {
      for(T* i = base_; i != base_+allocated_elements_; ++i)
	allocator_.destroy(i);
      allocator_.deallocate(base_,allocated_elements_);
    }
  }    
  
  typedef boost::array<size_type,NumDims> size_list;
  typedef boost::array<index,NumDims> index_list;

  Allocator allocator_;
  T* base_;
  size_type allocated_elements_;
  enum {initial_base_ = 0};
};

} // namespace boost

#endif // BOOST_MULTI_ARRAY_RG071801_HPP
