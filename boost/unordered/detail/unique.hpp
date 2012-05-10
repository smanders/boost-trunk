
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_UNIQUE_HPP_INCLUDED
#define BOOST_UNORDERED_DETAIL_UNIQUE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/unordered/detail/table.hpp>
#include <boost/unordered/detail/extract_key.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace boost { namespace unordered { namespace detail {

    template <typename A, typename T> struct unique_node;
    template <typename T> struct ptr_node;
    template <typename Types> struct table_impl;

    template <typename A, typename T>
    struct unique_node :
        boost::unordered::detail::node_base<
            typename ::boost::unordered::detail::rebind_wrap<
                A, unique_node<A, T> >::type::pointer
        >,
        boost::unordered::detail::value_base<T>
    {
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, unique_node<A, T> >::type::pointer link_pointer;
        typedef boost::unordered::detail::node_base<link_pointer> node_base;

        std::size_t hash_;

#if BOOST_UNORDERED_DETAIL_FULL_CONSTRUCT
        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        unique_node(BOOST_UNORDERED_EMPLACE_ARGS) :
            node_base(),
            hash_(0)
        {
            boost::unordered::detail::construct_impl(
                this->value_ptr(), BOOST_UNORDERED_EMPLACE_FORWARD);
        }

        ~unique_node() {
            boost::unordered::detail::destroy(this->value_ptr());
        }
#else
        unique_node() :
            node_base(),
            hash_(0)
        {}
#endif

        void init(link_pointer)
        {
        }
    };

    template <typename T>
    struct ptr_node :
        boost::unordered::detail::value_base<T>,
        boost::unordered::detail::ptr_bucket
    {
        typedef boost::unordered::detail::ptr_bucket bucket_base;
        typedef bucket_base node_base;
        typedef ptr_bucket* link_pointer;

        std::size_t hash_;

#if BOOST_UNORDERED_DETAIL_FULL_CONSTRUCT
        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        ptr_node(BOOST_UNORDERED_EMPLACE_ARGS) :
            bucket_base(),
            hash_(0)
        {
            boost::unordered::detail::construct_impl(
                this->value_ptr(), BOOST_UNORDERED_EMPLACE_FORWARD);
        }

        ~ptr_node() {
            boost::unordered::detail::destroy(this->value_ptr());
        }
#else
        ptr_node() :
            bucket_base(),
            hash_(0)
        {}
#endif

        void init(link_pointer)
        {
        }
    };

    // If the allocator uses raw pointers use ptr_node
    // Otherwise use node.

    template <typename A, typename T, typename NodePtr, typename BucketPtr>
    struct pick_node2
    {
        typedef boost::unordered::detail::unique_node<A, T> node;

        typedef typename boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A, node>::type
        >::pointer node_pointer;

        typedef boost::unordered::detail::bucket<node_pointer> bucket;
        typedef node_pointer link_pointer;
    };

    template <typename A, typename T>
    struct pick_node2<A, T,
        boost::unordered::detail::ptr_node<T>*,
        boost::unordered::detail::ptr_bucket*>
    {
        typedef boost::unordered::detail::ptr_node<T> node;
        typedef boost::unordered::detail::ptr_bucket bucket;
        typedef bucket* link_pointer;
    };

    template <typename A, typename T>
    struct pick_node
    {
        typedef boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A,
                boost::unordered::detail::ptr_node<T> >::type
        > tentative_node_traits;

        typedef boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A,
                boost::unordered::detail::ptr_bucket >::type
        > tentative_bucket_traits;

        typedef pick_node2<A, T,
            typename tentative_node_traits::pointer,
            typename tentative_bucket_traits::pointer> pick;

        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;
    };

    template <typename A, typename T, typename H, typename P>
    struct set
    {
        typedef boost::unordered::detail::set<A, T, H, P> types;

        typedef T value_type;
        typedef H hasher;
        typedef P key_equal;
        typedef T key_type;

        typedef typename boost::unordered::detail::rebind_wrap<
                A, value_type>::type allocator;

        typedef boost::unordered::detail::allocator_traits<allocator> traits;
        typedef boost::unordered::detail::pick_node<allocator, value_type> pick;
        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;

        typedef boost::unordered::detail::table_impl<types> table;
        typedef boost::unordered::detail::set_extractor<value_type> extractor;

        typedef boost::unordered::detail::pick_policy::type policy;
    };

    template <typename A, typename K, typename M, typename H, typename P>
    struct map
    {
        typedef boost::unordered::detail::map<A, K, M, H, P> types;

        typedef std::pair<K const, M> value_type;
        typedef H hasher;
        typedef P key_equal;
        typedef K key_type;

        typedef typename boost::unordered::detail::rebind_wrap<
                A, value_type>::type allocator;

        typedef boost::unordered::detail::allocator_traits<allocator> traits;
        typedef boost::unordered::detail::pick_node<allocator, value_type> pick;
        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;

        typedef boost::unordered::detail::table_impl<types> table;
        typedef boost::unordered::detail::map_extractor<key_type, value_type>
            extractor;

        typedef boost::unordered::detail::pick_policy::type policy;
    };

    template <typename Types>
    struct table_impl : boost::unordered::detail::table<Types>
    {
        typedef boost::unordered::detail::table<Types> table;
        typedef typename table::value_type value_type;
        typedef typename table::bucket bucket;
        typedef typename table::buckets buckets;
        typedef typename table::policy policy;
        typedef typename table::node_pointer node_pointer;
        typedef typename table::node_allocator node_allocator;
        typedef typename table::node_allocator_traits node_allocator_traits;
        typedef typename table::bucket_pointer bucket_pointer;
        typedef typename table::link_pointer link_pointer;
        typedef typename table::previous_pointer previous_pointer;
        typedef typename table::hasher hasher;
        typedef typename table::key_equal key_equal;
        typedef typename table::key_type key_type;
        typedef typename table::node_constructor node_constructor;
        typedef typename table::extractor extractor;
        typedef typename table::iterator iterator;
        typedef typename table::c_iterator c_iterator;

        typedef std::pair<iterator, bool> emplace_return;

        // Constructors

        table_impl(std::size_t n,
                hasher const& hf,
                key_equal const& eq,
                node_allocator const& a)
          : table(n, hf, eq, a)
        {}

        table_impl(table_impl const& x)
          : table(x, node_allocator_traits::
                select_on_container_copy_construction(x.node_alloc())) {}

        table_impl(table_impl const& x,
                node_allocator const& a)
          : table(x, a)
        {}

        table_impl(table_impl& x,
                boost::unordered::detail::move_tag m)
          : table(x, m)
        {}

        table_impl(table_impl& x,
                node_allocator const& a,
                boost::unordered::detail::move_tag m)
          : table(x, a, m)
        {}

        // Accessors

        template <class Key, class Pred>
        iterator find_node_impl(
                std::size_t key_hash,
                Key const& k,
                Pred const& eq) const
        {
            std::size_t bucket_index =
                policy::to_bucket(this->bucket_count_, key_hash);
            iterator n = this->get_start(bucket_index);

            for (;;)
            {
                if (!n.node_) return n;

                std::size_t node_hash = n.node_->hash_;
                if (key_hash == node_hash)
                {
                    if (eq(k, this->get_key(*n)))
                        return n;
                }
                else
                {
                    if (policy::to_bucket(this->bucket_count_, node_hash)
                            != bucket_index)
                        return iterator();
                }

                ++n;
            }
        }

        std::size_t count(key_type const& k) const
        {
            return this->find_node(k).node_ ? 1 : 0;
        }

        value_type& at(key_type const& k) const
        {
            if (this->size_) {
                iterator it = this->find_node(k);
                if (it.node_) return *it;
            }

            boost::throw_exception(
                std::out_of_range("Unable to find key in unordered_map."));
        }

        std::pair<iterator, iterator>
            equal_range(key_type const& k) const
        {
            iterator n = this->find_node(k);
            iterator n2 = n;
            if (n2.node_) ++n2;
            return std::make_pair(n, n2);
        }

        // equals

        bool equals(table_impl const& other) const
        {
            if(this->size_ != other.size_) return false;
            if(!this->size_) return true;
    
            for(iterator n1 = this->get_start(); n1.node_; ++n1)
            {
                iterator n2 = other.find_matching_node(n1);

#if !defined(BOOST_UNORDERED_DEPRECATED_EQUALITY)
                if (!n2.node_ || *n1 != *n2)
                    return false;
#else
                if (!n2.node_ || !extractor::compare_mapped(*n1, *n2))
                    return false;
#endif
            }
    
            return true;
        }

        // Emplace/Insert

        inline iterator add_node(
                node_constructor& a,
                std::size_t key_hash)
        {
            node_pointer n = a.release();
            n->hash_ = key_hash;
    
            bucket_pointer b = this->get_bucket(
                policy::to_bucket(this->bucket_count_, key_hash));

            if (!b->next_)
            {
                previous_pointer start_node = this->get_previous_start();
                
                if (start_node->next_) {
                    this->get_bucket(policy::to_bucket(this->bucket_count_,
                        static_cast<node_pointer>(start_node->next_)->hash_)
                    )->next_ = n;
                }

                b->next_ = start_node;
                n->next_ = start_node->next_;
                start_node->next_ = static_cast<link_pointer>(n);
            }
            else
            {
                n->next_ = b->next_->next_;
                b->next_->next_ = static_cast<link_pointer>(n);
            }

            ++this->size_;
            return iterator(n);
        }

        value_type& operator[](key_type const& k)
        {
            typedef typename value_type::second_type mapped_type;
    
            std::size_t key_hash = this->hash(k);
            iterator pos = this->find_node(key_hash, k);
    
            if (pos.node_) return *pos;
    
            // Create the node before rehashing in case it throws an
            // exception (need strong safety in such a case).
            node_constructor a(this->node_alloc());
            a.construct_node();
#if defined(BOOST_UNORDERED_VARIADIC_MOVE)
            a.construct_value(boost::unordered::piecewise_construct,
                boost::make_tuple(k), boost::make_tuple());
#else
            a.construct_value(
                boost::unordered::detail::create_emplace_args(
                    boost::unordered::piecewise_construct,
                    boost::make_tuple(k),
                    boost::make_tuple()));
#endif
    
            this->reserve_for_insert(this->size_ + 1);
            return *add_node(a, key_hash);
        }

#if defined(BOOST_NO_RVALUE_REFERENCES)
        emplace_return emplace(boost::unordered::detail::emplace_args1<
                boost::unordered::detail::please_ignore_this_overload> const&)
        {
            BOOST_ASSERT(false);
            return emplace_return(this->begin(), false);
        }
#endif

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace(BOOST_UNORDERED_EMPLACE_ARGS)
        {
#if defined(BOOST_UNORDERED_VARIADIC_MOVE)
            return emplace_impl(
                extractor::extract(BOOST_UNORDERED_EMPLACE_FORWARD),
                BOOST_UNORDERED_EMPLACE_FORWARD);

#else
            return emplace_impl(
                extractor::extract(args.a0, args.a1),
                BOOST_UNORDERED_EMPLACE_FORWARD);
#endif
        }

#if !defined(BOOST_UNORDERED_VARIADIC_MOVE)
        template <typename A0>
        emplace_return emplace(
                boost::unordered::detail::emplace_args1<A0> const& args)
        {
            return emplace_impl(extractor::extract(args.a0), args);
        }
#endif

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace_impl(key_type const& k,
            BOOST_UNORDERED_EMPLACE_ARGS)
        {
            std::size_t key_hash = this->hash(k);
            iterator pos = this->find_node(key_hash, k);
    
            if (pos.node_) return emplace_return(pos, false);
    
            // Create the node before rehashing in case it throws an
            // exception (need strong safety in such a case).
            node_constructor a(this->node_alloc());
            a.construct_node();
            a.construct_value(BOOST_UNORDERED_EMPLACE_FORWARD);
    
            // reserve has basic exception safety if the hash function
            // throws, strong otherwise.
            this->reserve_for_insert(this->size_ + 1);
            return emplace_return(this->add_node(a, key_hash), true);
        }

        emplace_return emplace_impl_with_node(node_constructor& a)
        {
            key_type const& k = this->get_key(a.value());
            std::size_t key_hash = this->hash(k);
            iterator pos = this->find_node(key_hash, k);

            if (pos.node_) return emplace_return(pos, false);

            // reserve has basic exception safety if the hash function
            // throws, strong otherwise.
            this->reserve_for_insert(this->size_ + 1);
            return emplace_return(this->add_node(a, key_hash), true);
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace_impl(no_key, BOOST_UNORDERED_EMPLACE_ARGS)
        {
            // Don't have a key, so construct the node first in order
            // to be able to lookup the position.
            node_constructor a(this->node_alloc());
            a.construct_node();
            a.construct_value(BOOST_UNORDERED_EMPLACE_FORWARD);
            return emplace_impl_with_node(a);
        }

        ////////////////////////////////////////////////////////////////////////
        // Insert range methods
        //
        // if hash function throws, or inserting > 1 element, basic exception
        // safety strong otherwise

        template <class InputIt>
        void insert_range(InputIt i, InputIt j)
        {
            if(i != j)
                return insert_range_impl(extractor::extract(*i), i, j);
        }

        template <class InputIt>
        void insert_range_impl(key_type const& k, InputIt i, InputIt j)
        {
            node_constructor a(this->node_alloc());

            // Special case for empty buckets so that we can use
            // max_load_ (which isn't valid when buckets_ is null).
            if (!this->buckets_) {
                insert_range_empty(a, k, i, j);
                if (++i == j) return;
            }

            do {
                // Note: can't use get_key as '*i' might not be value_type - it
                // could be a pair with first_types as key_type without const or
                // a different second_type.
                //
                // TODO: Might be worth storing the value_type instead of the
                // key here. Could be more efficient if '*i' is expensive. Could
                // be less efficient if copying the full value_type is
                // expensive.
                insert_range_impl2(a, extractor::extract(*i), i, j);
            } while(++i != j);
        }

        template <class InputIt>
        void insert_range_empty(node_constructor& a, key_type const& k,
            InputIt i, InputIt j)
        {
            std::size_t key_hash = this->hash(k);
            a.construct_node();
            a.construct_value2(*i);
            this->reserve_for_insert(this->size_ +
                boost::unordered::detail::insert_size(i, j));
            this->add_node(a, key_hash);
        }

        template <class InputIt>
        void insert_range_impl2(node_constructor& a, key_type const& k,
            InputIt i, InputIt j)
        {
            // No side effects in this initial code
            std::size_t key_hash = this->hash(k);
            iterator pos = this->find_node(key_hash, k);
    
            if (!pos.node_) {
                a.construct_node();
                a.construct_value2(*i);
    
                if(this->size_ + 1 > this->max_load_)
                    this->reserve_for_insert(this->size_ +
                        boost::unordered::detail::insert_size(i, j));
    
                // Nothing after this point can throw.
                this->add_node(a, key_hash);
            }
        }

        template <class InputIt>
        void insert_range_impl(no_key, InputIt i, InputIt j)
        {
            node_constructor a(this->node_alloc());

            do {
                a.construct_node();
                a.construct_value2(*i);
                emplace_impl_with_node(a);
            } while(++i != j);
        }

        ////////////////////////////////////////////////////////////////////////
        // Erase
        //
        // no throw

        std::size_t erase_key(key_type const& k)
        {
            if(!this->size_) return 0;

            std::size_t key_hash = this->hash(k);
            std::size_t bucket_index =
                policy::to_bucket(this->bucket_count_, key_hash);
            bucket_pointer this_bucket = this->get_bucket(bucket_index);

            previous_pointer prev = this_bucket->next_;
            if (!prev) return 0;

            for (;;)
            {
                if (!prev->next_) return 0;
                std::size_t node_hash =
                    static_cast<node_pointer>(prev->next_)->hash_;
                if (policy::to_bucket(this->bucket_count_, node_hash)
                        != bucket_index)
                    return 0;
                if (node_hash == key_hash &&
                        this->key_eq()(k, this->get_key(
                        static_cast<node_pointer>(prev->next_)->value())))
                    break;
                prev = static_cast<previous_pointer>(prev->next_);
            }

            node_pointer pos = static_cast<node_pointer>(prev->next_);
            node_pointer end = static_cast<node_pointer>(pos->next_);
            prev->next_ = pos->next_;
            this->fix_buckets(this_bucket, prev, end);
            return this->delete_nodes(c_iterator(pos), c_iterator(end));
        }

        iterator erase(c_iterator r)
        {
            BOOST_ASSERT(r.node_);
            iterator next(r.node_);
            ++next;

            bucket_pointer this_bucket = this->get_bucket(
                policy::to_bucket(this->bucket_count_, r.node_->hash_));
            previous_pointer prev = unlink_node(*this_bucket, r.node_);

            this->fix_buckets(this_bucket, prev, next.node_);

            this->delete_node(r);

            return next;
        }

        iterator erase_range(c_iterator r1, c_iterator r2)
        {
            if (r1 == r2) return iterator(r2.node_);

            std::size_t bucket_index =
                policy::to_bucket(this->bucket_count_, r1.node_->hash_);
            previous_pointer prev = unlink_nodes(
                *this->get_bucket(bucket_index), r1.node_, r2.node_);
            this->fix_buckets_range(bucket_index, prev, r1.node_, r2.node_);
            this->delete_nodes(r1, r2);

            return iterator(r2.node_);
        }

        static previous_pointer unlink_node(bucket& b, node_pointer n)
        {
            return unlink_nodes(b, n, static_cast<node_pointer>(n->next_));
        }

        static previous_pointer unlink_nodes(bucket& b,
                node_pointer begin, node_pointer end)
        {
            previous_pointer prev = b.next_;
            link_pointer begin_void = static_cast<link_pointer>(begin);
            while(prev->next_ != begin_void)
                prev = static_cast<previous_pointer>(prev->next_);
            prev->next_ = static_cast<link_pointer>(end);
            return prev;
        }

        ////////////////////////////////////////////////////////////////////////
        // copy_buckets_to
        //
        // Basic exception safety. If an exception is thrown this will
        // leave dst partially filled and the buckets unset.

        static void copy_buckets_to(buckets const& src, buckets& dst)
        {
            BOOST_ASSERT(!dst.buckets_);

            dst.create_buckets();

            node_constructor a(dst.node_alloc());

            iterator n = src.get_start();
            previous_pointer prev = dst.get_previous_start();

            while(n.node_) {
                a.construct_node();
                a.construct_value2(*n);

                node_pointer node = a.release();
                node->hash_ = n.node_->hash_;
                prev->next_ = static_cast<link_pointer>(node);
                ++dst.size_;
                ++n;

                prev = place_in_bucket(dst, prev);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // move_buckets_to
        //
        // Basic exception safety. The source nodes are left in an unusable
        // state if an exception throws.

        static void move_buckets_to(buckets& src, buckets& dst)
        {
            BOOST_ASSERT(!dst.buckets_);

            dst.create_buckets();

            node_constructor a(dst.node_alloc());

            iterator n = src.get_start();
            previous_pointer prev = dst.get_previous_start();

            while (n.node_) {
                a.construct_node();
                a.construct_value2(boost::move(*n));

                node_pointer node = a.release();
                node->hash_ = n.node_->hash_;
                prev->next_ = static_cast<link_pointer>(node);
                ++dst.size_;
                ++n;

                prev = place_in_bucket(dst, prev);
            }
        }

        // strong otherwise exception safety
        void rehash_impl(std::size_t num_buckets)
        {
            BOOST_ASSERT(this->size_);

            buckets dst(this->node_alloc(), num_buckets);
            dst.create_buckets();

            previous_pointer src_start = this->get_previous_start();
            previous_pointer dst_start = dst.get_previous_start();

            dst_start->next_ = src_start->next_;
            src_start->next_ = link_pointer();
            dst.size_ = this->size_;
            this->size_ = 0;

            previous_pointer prev = dst.get_previous_start();
            while (prev->next_)
                prev = place_in_bucket(dst, prev);

            // Swap the new nodes back into the container and setup the
            // variables.
            dst.swap(*this); // no throw
        }

        // Iterate through the nodes placing them in the correct buckets.
        // pre: prev->next_ is not null.
        static previous_pointer place_in_bucket(buckets& dst,
                previous_pointer prev)
        {
            node_pointer n = static_cast<node_pointer>(prev->next_);
            bucket_pointer b = dst.get_bucket(
                buckets::to_bucket(dst.bucket_count_, n->hash_));

            if (!b->next_) {
                b->next_ = prev;
                return static_cast<previous_pointer>(n);
            }
            else {
                prev->next_ = n->next_;
                n->next_ = b->next_->next_;
                b->next_->next_ = static_cast<link_pointer>(n);
                return prev;
            }
        }
    };
}}}

#endif
