// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/iterator.hpp>
#include <boost/iterator_adaptors.hpp>
#include <list>
#include <utility>
#include <iterator>
#include <functional>

using namespace boost::python;

typedef std::list<int> list_int;

// Prove that we can handle InputIterators which return rvalues.  This
// input iterator example was stolen from the iterator_adaptors
// documentation
typedef std::binder1st<std::multiplies<int> > doubler;
typedef boost::transform_iterator_generator<doubler, list_int::iterator>::type doubling_iterator;
typedef std::pair<doubling_iterator,doubling_iterator> list_range2;
list_range2 range2(list_int& x)
{
    return list_range2(
        boost::make_transform_iterator<doubler>(x.begin(), std::bind1st(std::multiplies<int>(),2))
      , boost::make_transform_iterator<doubler>(x.end(), std::bind1st(std::multiplies<int>(),2)));
}

// We do this in a separate module from iterators_ext (iterators.cpp)
// to work around an MSVC6 linker bug, which causes it to complain
// about a "duplicate comdat" if the input iterator is instantiated in
// the same module with the others.
BOOST_PYTHON_MODULE_INIT(input_iterator)
{
    module("input_iterator")
        .def("range2", &::range2)
        .add(
            class_<list_range2>("list_range2")

            // We can wrap InputIterators which return by-value
            .def("__iter__"
                 , range(&list_range2::first, &list_range2::second))
            )
        ;
}

#include "module_tail.cpp"
