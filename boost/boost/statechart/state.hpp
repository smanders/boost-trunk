#ifndef BOOST_FSM_STATE_HPP_INCLUDED
#define BOOST_FSM_STATE_HPP_INCLUDED
//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2002-2003 Andreas Huber Doenni, Switzerland
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//////////////////////////////////////////////////////////////////////////////



#include <boost/fsm/simple_state.hpp>



namespace boost
{
namespace fsm
{



template< class Derived,
          class Context, // either an outer state or a state_machine
          class Reactions = no_reactions,
          class InnerInitial = detail::empty_list > // initial inner state
class state :
  public simple_state< Derived, Context, Reactions, InnerInitial >
{
  typedef simple_state< Derived, Context, Reactions, InnerInitial >
    base_type;

  protected:
    //////////////////////////////////////////////////////////////////////////
    typedef typename base_type::top_context_type top_context_type;
    typedef typename base_type::context_ptr_type context_ptr_type;
    typedef typename base_type::inner_context_ptr_type inner_context_ptr_type;
    typedef typename base_type::inner_initial_list inner_initial_list;

    struct my_context
    {
      my_context( context_ptr_type pContext ) :
        pContext_( pContext )
      {
      }

      context_ptr_type pContext_;
    };

    typedef state my_base;

    state( my_context ctx )
    {
      base_type::set_context( ctx.pContext_ );
    }

  public:
    //////////////////////////////////////////////////////////////////////////
    // CAUTION: The following declarations should be private.
    // They are only public because many compilers lack template friends.
    //////////////////////////////////////////////////////////////////////////
    // See base class for documentation
    static void deep_construct( top_context_type & topContext )
    {
      base_type::deep_construct( topContext );
    }

    // See base class for documentation
    static void deep_construct(
      const context_ptr_type & pContext,
      top_context_type & topContext )
    {
      const inner_context_ptr_type pInnerContext(
        shallow_construct( pContext ) );
      base_type::deep_construct_inner< inner_initial_list >(
        pInnerContext, topContext );
    }

    static inner_context_ptr_type shallow_construct(
      const context_ptr_type & pContext )
    {
      return inner_context_ptr_type( new Derived( my_context( pContext ) ) );
    }
};



} // namespace fsm
} // namespace boost



#endif
