/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp> // msvc 6.0 needs this to suppress warnings

#include <cassert>
#include <set>
#include <list>
#include <vector>

#include <boost/limits.hpp>
#include <boost/state_saver.hpp>
#include <boost/throw_exception.hpp>

#include <boost/archive/detail/basic_iserializer.hpp>
#include <boost/archive/detail/basic_pointer_iserializer.hpp>
#include <boost/archive/detail/basic_iarchive.hpp>

#include <boost/serialization/tracking.hpp>
#include <boost/serialization/extended_type_info.hpp>

#include <boost/archive/archive_exception.hpp>

using namespace boost::serialization;

namespace boost {
namespace archive {
namespace detail {

class basic_iserializer;
class basic_pointer_iserializer;

class basic_iarchive_impl 
{
    friend class basic_iarchive;
    //////////////////////////////////////////////////////////////////////
    // information about each serialized object loaded
    // indexed on object_id
    struct aobject
    {
        void * address;
        class_id_type class_id;
        aobject(
            void *a,
            class_id_type class_id_
        ) :
            address(a),
            class_id(class_id_)
        {}
        aobject() : address(NULL), class_id(-2) {}
    };
    typedef std::vector<aobject> object_id_vector_type;
    object_id_vector_type object_id_vector;

    //////////////////////////////////////////////////////////////////////
    // used by load object to look up class id given basic_serializer
    struct cobject_type
    {
        const basic_iserializer * bis;
        const class_id_type class_id;
        cobject_type(
            size_t class_id_,
            const basic_iserializer & bis_
        ) : 
            bis(& bis_),
            class_id(class_id_)
        {}
        cobject_type(const cobject_type & rhs) : 
            bis(rhs.bis),
            class_id(rhs.class_id)
        {}
        // the following cannot be defined because of the const
        // member.  This will generate a link error if an attempt
        // is made to assign.  This should never be necessary
        cobject_type & operator=(const cobject_type & rhs);
        bool operator<(const cobject_type &rhs) const
        {
            return *bis < *(rhs.bis);
        }
    };
    typedef std::set<cobject_type> cobject_info_set_type;
    cobject_info_set_type cobject_info_set;

    //////////////////////////////////////////////////////////////////////
    // information about each serialized class indexed on class_id
    class cobject_id 
    {
    public:
        cobject_id & operator=(const cobject_id & rhs){
            bis_ptr = rhs.bis_ptr;
            bpis_ptr = rhs.bpis_ptr;
            file_version = rhs.file_version;
            tracking_level = rhs.tracking_level;
            initialized = rhs.initialized;
            return *this;
        }
        const basic_iserializer * bis_ptr;
        const basic_pointer_iserializer * bpis_ptr;
        version_type file_version;
        tracking_type tracking_level;
        bool initialized;

        cobject_id(const basic_iserializer & bis_) :
            bis_ptr(& bis_),
            bpis_ptr(NULL),
            file_version(0),
            tracking_level(track_never),
            initialized(false)
        {}
        cobject_id(const cobject_id &rhs): 
            bis_ptr(rhs.bis_ptr),
            bpis_ptr(rhs.bpis_ptr),
            file_version(rhs.file_version),
            tracking_level(rhs.tracking_level),
            initialized(rhs.initialized)
        {}
    };
    typedef std::vector<cobject_id> cobject_id_vector_type;
    cobject_id_vector_type cobject_id_vector;

    // list of objects created by de-serialization.  Used to implement
    // clean up after exceptions.
    class created_pointer_type
    {
    public:
        created_pointer_type(
            class_id_type class_id_,
            void * address_
        ) :
            class_id(class_id_),
            address(address_)
        {}
        created_pointer_type(const created_pointer_type &rhs) :
            class_id(rhs.class_id),
            address(rhs.address)
        {}
        created_pointer_type & operator=(const created_pointer_type &){
            assert(false);
            return *this;
        }
        void * get_address() const {
            return address;
        }
        // object to which this item refers
        const class_id_type class_id;
    private:
        void * address;
    };

    std::list<created_pointer_type> created_pointers;

    bool is_object; // pass forward indicater that we're saving an object
                    // directly rather than result of a pointer

    basic_iarchive_impl()
        : is_object(true)
    {}
    bool
    track(
        basic_iarchive & ar,
        void * & t
    );
    void
    load_preamble(
        basic_iarchive & ar,
        cobject_id & co
    );
    class_id_type register_type(
        const basic_iserializer & bis
    );

	// redirect through virtual functions to load functions for this archive
	template<class T>
	void load(basic_iarchive & ar, T & t){
		ar.vload(t);
	}

public:
    void delete_created_pointers();
    class_id_type register_type(
        const basic_pointer_iserializer & bpis
    );
    void load_object(
        basic_iarchive & ar,
        void * & t,
        const basic_iserializer & bis
    );
    const basic_pointer_iserializer * load_pointer(
        basic_iarchive & ar,
        void * & t, 
        const basic_pointer_iserializer * bpis,
        const basic_pointer_iserializer * (*finder)(
        	const boost::serialization::extended_type_info & type
        )
    );
}; 

inline void 
basic_iarchive_impl::delete_created_pointers()
{
    while(created_pointers.size() > 0){
        const created_pointer_type & cp = created_pointers.front();

        // figure out the class of the object to be deleted
 		// note: extra line used to evand borland issue
        const int id = cp.class_id;
        const cobject_id & co = cobject_id_vector[id];
        // with the appropriate input serializer, 
        // delete the indicated object
        co.bis_ptr->destroy(cp.get_address());
        created_pointers.pop_front();
    }
}

inline class_id_type
basic_iarchive_impl::register_type(
    const basic_iserializer & bis
){
    cobject_type co(cobject_info_set.size(), bis);
    std::pair<cobject_info_set_type::const_iterator, bool>
        result = cobject_info_set.insert(co);

    if(result.second){
        cobject_id_vector.push_back(cobject_id(bis));
        assert(cobject_info_set.size() == cobject_id_vector.size());
    }
    const int id = result.first->class_id;
    cobject_id & coid = cobject_id_vector[id];
    coid.bpis_ptr = bis.get_bpis_ptr();
    return result.first->class_id;
}

void
basic_iarchive_impl::load_preamble(
    basic_iarchive & ar,
    cobject_id & co
){
    if(! co.initialized){
        if(co.bis_ptr->class_info()){
            class_id_optional_type cid;
            load(ar, cid);    // to be thrown away
            load(ar, co.tracking_level);
            load(ar, co.file_version);
        }
        else{
            // override tracking with indicator from class information
        	co.tracking_level = co.bis_ptr->tracking();
            co.file_version = version_type(
                co.bis_ptr->version()
            );
        }
        co.initialized = true;
    }
}

bool
basic_iarchive_impl::track(
    basic_iarchive & ar,
    void * & t
){
    object_id_type oid;
    load(ar, oid);

    // if its a reference to a old object
    if(object_id_type(object_id_vector.size()) > oid){
        // we're done
        t =  object_id_vector[oid].address;
        return false;
    }
    return true;
}

inline void
basic_iarchive_impl::load_object(
    basic_iarchive & ar,
    void * & t,
    const basic_iserializer & bis
){
    bool result = true;
    const class_id_type cid = register_type(bis);
    // note: extra line used to evand borland issue
    const int id = cid;
    cobject_id & co = cobject_id_vector[id];
    if(is_object){
        load_preamble(ar, co);
        // note: extra line used to evand borland issue
        const bool b = co.tracking_level;
        if(b){ 
            result = track(ar, t);
            if(! result)
                return;
            object_id_vector.push_back(aobject(t, cid));
        }
    }

    // read data if required
    state_saver<bool> x(is_object);
    is_object = true;
    (bis.load_object_data)(ar, t, co.file_version);
}

inline const basic_pointer_iserializer *
basic_iarchive_impl::load_pointer(
    basic_iarchive &ar,
    void * & t,
    const basic_pointer_iserializer * bpis_ptr,
    const basic_pointer_iserializer * (*finder)(
        const boost::serialization::extended_type_info & type_
    )
){
    class_id_type cid;
    load(ar, cid);

    if(null_pointer_tag == cid){
        t = NULL;
        return bpis_ptr;
    }

    // if its a new class type - i.e. never been registered
    if(class_id_type(cobject_info_set.size()) <= cid){
        // if its either abstract
        if(NULL == bpis_ptr
        // or polymorphic
        || bpis_ptr->get_basic_serializer().is_polymorphic()){
        	// is must have been exported
            char key[BOOST_SERIALIZATION_MAX_KEY_SIZE];
            class_name_type class_name(key);
            load(ar, class_name);
            // if it has a class name
            const serialization::extended_type_info *eti = NULL;
            if(0 != key[0])
                eti = serialization::extended_type_info::find(key);
            if(NULL == eti)
                boost::throw_exception(
                    archive_exception(archive_exception::unregistered_class)
                );
            bpis_ptr = (*finder)(*eti);
        }
        assert(NULL != bpis_ptr);
        class_id_type new_cid = register_type(bpis_ptr->get_basic_serializer());
		int i = cid;
        cobject_id_vector[i].bpis_ptr = bpis_ptr;
        assert(new_cid == cid);
    }
	int i = cid;
    cobject_id & co = cobject_id_vector[i];
    const basic_iserializer * bis_ptr = co.bis_ptr;
    bpis_ptr = co.bpis_ptr;

    load_preamble(ar, co);

    bool track_object = true;
    // extra line to evade borland issue
    const bool b = co.tracking_level;
    if(b)
        track_object = track(ar, t);

    // if required
    if(track_object){
        // read data 
        state_saver<bool> x(is_object);
        is_object = false;

		if(bis_ptr->tracking()){
			// predict next object id to be created
			const unsigned int i = object_id_vector.size();

			// because the following operation could move the items
			// don't use co after this
			object_id_vector.push_back(aobject(t, cid));
			bpis_ptr->load_object_ptr(
				ar, 
				object_id_vector[i].address, 
				co.file_version
			);
			t = object_id_vector[i].address;
            // and add to list of created pointers
            created_pointers.push_back(created_pointer_type(cid, t));
		}
		else{
			bpis_ptr->load_object_ptr(ar, t, co.file_version);
		}
        assert(NULL != t);
    }

    return bpis_ptr;
}

//////////////////////////////////////////////////////////////////////
// implementation of basic_iarchive functions

basic_iarchive::basic_iarchive() : 
    pimpl(new basic_iarchive_impl),
    archive_library_version(ARCHIVE_VERSION)
{}

basic_iarchive::~basic_iarchive()
{
	delete pimpl;
}

void basic_iarchive::load_object(
    void *t, 
    const basic_iserializer & bis
){
    pimpl->load_object(*this, t, bis);
}

// load a pointer object
const basic_pointer_iserializer * 
basic_iarchive::load_pointer(
    void * &t, 
    const basic_pointer_iserializer * bpis_ptr,
    const basic_pointer_iserializer * (*finder)(
        const boost::serialization::extended_type_info & type_
    )
){
    return pimpl->load_pointer(*this, t, bpis_ptr, finder);
}

void basic_iarchive::register_basic_serializer(const basic_iserializer & bis){
    pimpl->register_type(bis);
}

void basic_iarchive::delete_created_pointers()
{
    pimpl->delete_created_pointers();
}

} // namespace detail
} // namespace archive
} // namespace boost
