#ifndef BOOST_ARCHIVE_BASIC_XML_IARCHIVE_HPP
#define BOOST_ARCHIVE_BASIC_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <string>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/archive/detail/common_iarchive.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/string.hpp> // makes strings prmitive

namespace boost {
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class xml_iarchive - read serialized objects from a input text stream
template<class Archive>
class basic_xml_iarchive : public detail::common_iarchive<Archive>
{
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
public:
#elif defined(BOOST_MSVC)
    // for some inexplicable reason insertion of "class" generates compile erro
    // on msvc 7.1
    friend detail::interface_iarchive<Archive>;
protected:
#else
    friend class detail::interface_iarchive<Archive>;
protected:
#endif
    bool header;
    bool no_checking;
    unsigned int depth;
    void load_start(const char *name);
    void load_end(const char *name);

#if 0
    // note: we have to go through a little extra funny business
    // to accommodate non-conforming compilers.
    template<class T>
    struct load_primitive_impl {
        static void invoke(Archive & ar, T & t){
            archive::load_access::load_primitive(ar, t);
        }
    };

    template<class T>
    struct load_non_primitive_impl {
        static void invoke(Archive & ar, T & t){
            archive::load(ar, t);
        }
    };

    template<class T>
    void load_impl(T & t){
        BOOST_STATIC_ASSERT(
        	serialization::not_serializable 
        	!=  serialization::implementation_level<T>::value
        );
        mpl::apply_if<
            #ifndef BOOST_NO_STD_WSTRING
                mpl::or_<
                	mpl::equal_to<
                		::boost::serialization::implementation_level<T>,
                		// don't forget the damn space between < and :: !
                		mpl::int_< ::boost::serialization::primitive_type>
                	>,
                	is_same<T, 	std::string>,
                	is_same<T, 	std::wstring>
                >,
            #else
                is_same<T, 	std::string>,
            #endif
            mpl::identity<load_primitive_impl<T> >,
            mpl::identity<load_non_primitive_impl<T> >
        >::type::invoke(* this->This(),  t);
    }
    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<class T>
    void load_override(T & t,  BOOST_PFTO int)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_STATIC_ASSERT(0 == sizeof(T));
    }
#endif
    // Anything not an attribute and not a name-value pair is an
    // should be trapped here.
    template<class T>
    void load_override(T & t,  BOOST_PFTO int)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_STATIC_ASSERT(0 == sizeof(T));
    }

    // Anything not an attribute - see below - should be a name value
    // pair and be processed here
    template<class T>
    void load_override(boost::serialization::nvp<T> & t, int)
    {
        load_start(t.name());
        archive::load(* this->This(), t.value());
        load_end(t.name());
    }

    // specific overrides for attributes - handle as
    // primitives. These are not name-value pairs
    // so they have to be intercepted here and passed on to load.
    // although the class_id is included in the xml text file in order
    // to make the file self describing, it isn't used when loading
    // an xml archive.  So we can skip it here.  Note: we MUST override
    // it otherwise it will be loaded as a normal primitive w/o tag and
    // leaving the archive in an undetermined state
    void load_override(class_id_optional_type & /* t */, int){}
    void load_override(object_id_type & t, int);
    void load_override(version_type & t, int);
    void load_override(class_id_type & t, int);
    void load_override(tracking_type & t, int);
    // class_name_type can't be handled here as it depends upon the
    // char type used by the stream.  So require the derived implementation
    // handle this.
    // void load_override(class_name_type & t, int);

    basic_xml_iarchive(unsigned int flags = 0) :
        header(false),
        depth(0)
    {}
};

} // namespace archive
} // namespace boost

#endif // BOOST_ARCHIVE_BASIC_XML_IARCHIVE_HPP
