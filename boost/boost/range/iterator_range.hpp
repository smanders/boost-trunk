// Boost.Range library
//
//  Copyright Thorsten Ottosen & Pavol Droba 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_ITERATOR_RANGE_HPP
#define BOOST_RANGE_ITERATOR_RANGE_HPP

#include <boost/range/config.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/result_iterator.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <iterator>
#include <algorithm>
#include <ostream>
#include <cstddef>


/*! \file
    Defines the \c iterator_class and related functions. 
    \c iterator_range is a simple wrapper of iterator pair idiom. It provides
    a rich subset of Container interface.
*/

namespace boost {

//  iterator range template class -----------------------------------------//

        //! iterator_range class
        /*!
            An \c iterator_range delimits a range in a sequence by beginning and ending iterators. 
            An iterator_range can be passed to an algorithm which requires a sequence as an input. 
            For example, the \c toupper() function may be used most frequently on strings, 
            but can also be used on iterator_ranges: 
            
            \code
                boost::tolower( find( s, "UPPERCASE STRING" ) );
            \endcode

            Many algorithms working with sequences take a pair of iterators, 
            delimiting a working range, as an arguments. The \c iterator_range class is an 
            encapsulation of a range identified by a pair of iterators. 
            It provides a collection interface, 
            so it is possible to pass an instance to an algorithm requiring a collection as an input. 
        */
        template<typename IteratorT> 
        class iterator_range
        {
        public:
            //! this type
            typedef iterator_range<IteratorT> type;
            //BOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(value_type);

            //! Encapsulated value type
            typedef BOOST_DEDUCED_TYPENAME 
                iterator_value<IteratorT>::type value_type;

            //! Difference type
            typedef BOOST_DEDUCED_TYPENAME 
                iterator_difference<IteratorT>::type difference_type;
            //! Size type
            typedef std::size_t size_type; // note: must be unsigned
            
            //! const_iterator type
            /*! 
                There is no distinction between const_iterator and iterator.
                These typedefs are provides to fulfill container interface
            */ 
            typedef IteratorT const_iterator;
            //! iterator type
            typedef IteratorT iterator;

            iterator_range() : m_Begin( iterator() ), m_End( iterator() ), 
                               singular( true )
            { }
            
            //! Constructor from a pair of iterators
            template< class Iterator >
            iterator_range( Iterator Begin, Iterator End ) : 
                m_Begin(Begin), m_End(End), singular(false) {}

            //! Constructor from a Range
            template< class Range >
            iterator_range( const Range& r ) : 
                m_Begin( adl_begin( r ) ), m_End( adl_end( r ) ), 
                singular(false) {}

            //! Constructor from a Range
            template< class Range >
            iterator_range( Range& r ) : 
                m_Begin( adl_begin( r ) ), m_End( adl_end( r ) ), 
                singular(false) {}
            
            template< class Iterator >
            iterator_range& operator=( const iterator_range<Iterator>& r )    
            {
                m_Begin  = r.begin(); 
                m_End    = r.end();
                //
                // remark: this need not necessarily be true, but it does no harm
                //
                singular = r.empty();
                return *this;
            }
                                      
            template< class ForwardRange >
            iterator_range& operator=( ForwardRange& r )
            {
                m_Begin  = adl_begin( r ); 
                m_End    = adl_end( r );
                singular = false;
                return *this;
            }

            template< class ForwardRange >
            iterator_range& operator=( const ForwardRange& r )
            {
                m_Begin  = adl_begin( r ); 
                m_End    = adl_end( r );
                singular = false;
                return *this;
            }

            //! begin access
            /*! 
                Retrieve the begin iterator
            */
            IteratorT begin() const 
            { 
                return m_Begin; 
            }

            //! end access
            /*!
                Retrieve the end iterator
            */
            IteratorT end() const 
            { 
                return m_End; 
            } 

            //! Size of the range
            /*!
                Retrieve the size of the range
            */
            size_type size() const
            { 
                if( singular )
                    return 0;
                
                return std::distance( m_Begin, m_End );
            }
            
            bool empty() const
            {
                if( singular )
                    return true;
                
                return m_Begin == m_End;
            }

            //! Safe bool conversion
            /*!
                Check whenever the range is empty.
                Allows to use construction like this:
                \code
                    iterator_range r;
                    if (!r)
                    {
                    ...
                    }
                \endcode
            */
            typedef iterator (iterator_range::*unspecified_bool_type) () const;
            operator unspecified_bool_type() const
            {
                return empty() ? 0: &iterator_range::end;
            }
            
        private:
            template< class ForwardRange >
            iterator adl_begin( ForwardRange& r )
            {
                #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))  
                return boost::begin( r );
                #else
                using boost::begin;
                return begin( r );
                #endif
            }
            
            template< class ForwardRange >
            iterator adl_end( ForwardRange& r )
            {
                #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))  
                return boost::end( r );
                #else
                using boost::end;
                return end( r );
                #endif
            }
            
        private:
            // begin and end iterators
            IteratorT m_Begin;
            IteratorT m_End;
            bool      singular;
        };

//  iterator range free-standing operators ---------------------------//

#ifdef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
#else
        template< class Iterator >
        inline bool empty( const iterator_range<Iterator>& r )
        {
            //
            // this will preserve the well-defined empty() even 
            // though 'r' is singular.
            //
            return r.empty();
        }
#endif
        
        //! iterator_range output operator
        /*!
            Output the range to an ostream. Elements are outputed
            in a sequence without separators.
        */
        template< typename IteratorT, typename Elem, typename Traits >
        inline std::basic_ostream<Elem,Traits>& operator<<( 
                    std::basic_ostream<Elem, Traits>& Os,
                    const iterator_range<IteratorT>& r )
        {
            std::copy( begin(r), end(r), std::ostream_iterator<Elem>(Os));
            return Os;
        }

        //! Comparison operator ( equal )
        /*! 
            Compare operands for equality
        */
        template< class IteratorT, class IteratorT2 > 
        inline bool operator==( const iterator_range<IteratorT>& l,
                                const iterator_range<IteratorT2>& r )
        {
            return ! (l != r);
        }

        //! Comparison operator ( not-equal )
        /*! 
            Compare operands for non-equality
        */
        template< class IteratorT, class IteratorT2 > 
        inline bool operator!=( const iterator_range<IteratorT>& l,
                                const iterator_range<IteratorT2>& r )
        {
            return l.begin() !=  r.begin() || l.end() != r.end();
        }

//  iterator range utilities -----------------------------------------//

        //! iterator_range construct helper 
        /*!
            Construct an \c iterator_range from a pair of iterators

            \param Begin A begin iterator
            \param End An end iterator
            \return iterator_range object
        */
        template< typename IteratorT >
        inline iterator_range< IteratorT > 
        make_iterator_range( IteratorT Begin, IteratorT End ) 
        {   
            return iterator_range<IteratorT>( Begin, End );
        }
        
        
                
        namespace range_detail
        {
             template< class Range >
             inline iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
             make_sub_range_impl( Range& r, 
                                  BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                                  BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end )
             {
                 BOOST_ASSERT( advance_begin >= 0 );
                 BOOST_ASSERT( advance_end >= 0 );
                 
                 BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type 
                     new_begin = begin( r ),
                     new_end   = end( r );
                 std::advance( new_begin, advance_begin );
                 std::advance( new_end, -advance_end );
                 return make_iterator_range( new_begin, new_end );
             }
             
             
             
            template< class Range >
            inline iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
            make_super_range_impl( Range& r, 
                                   BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                                   BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end )
            {
                BOOST_ASSERT( advance_begin >= 0 );
                BOOST_ASSERT( advance_end >= 0 );
            
                BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type 
                    new_begin = begin( r ),
                    new_end   = end( r );
                std::advance( new_begin, -advance_begin );
                std::advance( new_end, advance_end );
                return make_iterator_range( new_begin, new_end );
            }

        }

      
#ifdef BOOST_NO_FUNCTION_TEMPLATE_ORDERING

        template< typename Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
        make_iterator_range( Range& r ) 
        {   
            return iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
                ( begin( r ), end( r ) );
        }
        

        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
        make_sub_range( Range& r, 
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_sub_range_impl( r, advance_begin, advance_end );
        }
        
        
        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_result_iterator<Range>::type >
        make_super_range( Range& r, 
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_super_range_impl( r, advance_begin, advance_end );
        }

#else
        //! iterator_range construct helper
        /*!
            Construct an \c iterator_range from a \c Range containing the begin
            and end iterators.
        */
        template< class ForwardRange >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type >
        make_iterator_range( ForwardRange& r ) 
        {   
            return iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type >
                ( r );
        }

        template< class ForwardRange >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_const_iterator<ForwardRange>::type >
        make_iterator_range( const ForwardRange& r ) 
        {   
            return iterator_range< BOOST_DEDUCED_TYPENAME range_const_iterator<ForwardRange>::type >
                ( r );
        }
        
        
        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<Range>::type >
        make_sub_range( Range& r, 
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_sub_range_impl( r, advance_begin, advance_end );
        }
        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_const_iterator<Range>::type >
        make_sub_range( const Range& r, 
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                        BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_sub_range_impl( r, advance_begin, advance_end );
        }

        
        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<Range>::type >
        make_super_range( Range& r, 
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_super_range_impl( r, advance_begin, advance_end );
        }
        
        
        template< class Range >
        inline iterator_range< BOOST_DEDUCED_TYPENAME range_const_iterator<Range>::type >
        make_super_range( const Range& r, 
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_begin,
                          BOOST_DEDUCED_TYPENAME range_difference<Range>::type advance_end = 0 )
        {
            return range_detail::make_super_range_impl( r, advance_begin, advance_end );
        }


#endif // BOOST_NO_FUNCTION_TEMPLATE_ORDERING

        //! copy a range into a sequence
        /*!
            Construct a new sequence of the specified type from the elements
            in the given range

            \param Range An input range
            \return New sequence
        */
        template< typename SeqT, typename Range >
        inline SeqT copy_range( const Range& r )
        {
            return SeqT( begin( r ), end( r ) );
        }

        //! transform a range into a sequence
        /*!
            Create a new sequence from the elements in the range, transformed
            by a function

            \param Range An input range
            \param Func Transformation function
            \return New sequence
        */
        template< typename SeqT, typename Range, typename FuncT >
        inline SeqT transform_range( const Range& r, FuncT Func )
        {
            SeqT Seq;
            std::transform( begin( r ), end( r ), std::back_inserter(Seq), Func );
            return Seq;
        }

} // namespace 'boost'


#endif
