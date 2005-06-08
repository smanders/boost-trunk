/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serializer_map.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <set>

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/detail/basic_serializer.hpp>
#include <boost/archive/detail/basic_serializer_map.hpp>

namespace boost {
    namespace serialization {
        class extended_type_info;
    }
namespace archive {
namespace detail {

BOOST_DECL_ARCHIVE
basic_serializer_map::basic_serializer_map(){
}

bool 
BOOST_DECL_ARCHIVE 
basic_serializer_map::insert(const basic_serializer * bs){
    return map.insert(bs).second;
}

class basic_serializer_arg : public basic_serializer {
public:
    basic_serializer_arg(const serialization::extended_type_info & eti) :
        basic_serializer(eti)
    {}
};

#if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN) || defined(__MWERKS__)
    BOOST_DECL_ARCHIVE 
    const basic_serializer * 
#else
    const basic_serializer * 
    BOOST_DECL_ARCHIVE 
#endif
basic_serializer_map::tfind(
    const boost::serialization::extended_type_info & type_
) const {
    const basic_serializer_arg bs(type_);
    map_type::const_iterator it;
    it = map.find(& bs);
    if(it == map.end())
        return NULL;
    return *it;
}

} // namespace detail
} // namespace archive
} // namespace boost
