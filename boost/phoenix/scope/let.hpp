/*==============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2004 Daniel Wallin
    Copyright (c) 2010 Thomas Heller

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_PHOENIX_SCOPE_LET_HPP
#define BOOST_PHOENIX_SCOPE_LET_HPP

#include <boost/phoenix/core/limits.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/phoenix/core/call.hpp>
#include <boost/phoenix/core/expression.hpp>
#include <boost/phoenix/core/meta_grammar.hpp>
#include <boost/phoenix/scope/scoped_environment.hpp>
#include <boost/phoenix/scope/local_variable.hpp>
#include <boost/phoenix/support/iterate.hpp>
#include <boost/phoenix/support/vector.hpp>

BOOST_PHOENIX_DEFINE_EXPRESSION(
    (boost)(phoenix)(let)
  , (proto::terminal<proto::_>)
    (meta_grammar)
)

namespace boost { namespace phoenix
{
    struct let_eval
    {
        template <typename Sig>
        struct result;

        template <typename This, typename Context, typename Locals, typename Let>
        struct result<This(Context, Locals const &, Let const &)>
        {
            typedef
                typename result_of::actions<Context>::type
                actions_type;

            typedef
                typename 
                    boost::result_of<
                        detail::local_var_def_eval(
                            typename proto::result_of::value<
                                Locals const &
                            >::type
                          , Context
                        )
                    >::type
                locals_type;

            typedef
                typename evaluator::impl<
                    Let const &
                  , typename result_of::context<
                        scoped_environment<
                            typename result_of::env<Context>::type
                          , typename result_of::env<Context>::type
                          , locals_type
                        >
                      , actions_type
                    >::type
                  , int
                >::result_type
                type;
        };

        template <typename Context, typename Locals, typename Let>
        typename result<let_eval(Context const&, Locals const &, Let const &)>::type
        operator()(Context const& ctx, Locals const & locals, Let const & let) const
        {
            typedef
                typename result_of::env<Context>::type
                env_type;

            typedef
                typename result_of::actions<Context>::type
                actions_type;

            typedef
                typename 
                    boost::result_of<
                        detail::local_var_def_eval(
                            typename proto::result_of::value<
                                Locals const &
                            >::type
                          , Context const &
                        )
                    >::type
                locals_type;
            
            typedef scoped_environment<env_type, env_type, locals_type> scoped_env_type;

            locals_type l
                = detail::local_var_def_eval()(proto::value(locals), ctx);

            scoped_env_type
                scoped_env(
                    env(ctx)
                  , env(ctx)
                  , l
                );

            return eval(let, context(scoped_env, actions(ctx)));
        }
    };

    template <typename Dummy>
    struct default_actions::when<rule::let, Dummy>
        : call<let_eval, Dummy>
    {};

    template <typename Locals = void, typename Dummy = void>
    struct let_actor_gen;

    template <>
    struct let_actor_gen<void, void>
    {
        template <typename Expr>
        Expr const &
        operator[](Expr const & expr) const
        {
            return expr;
        }
    };

    template <typename Locals>
    struct let_actor_gen<Locals>
    {
        let_actor_gen(Locals const & locals)
            : locals(locals)
        {}

        template <typename Expr>
        typename expression::let<
            Locals
          , Expr
        >::type const
        operator[](Expr const & expr) const
        {
            return expression::let<Locals, Expr>::make(locals, expr);
        }

        Locals locals;
    };


    struct let_local_gen
    {
        let_actor_gen<> const
        operator()() const
        {
            return let_actor_gen<>();
        }

        #include <boost/phoenix/scope/detail/let_local_gen.hpp>
    };

    let_local_gen const let = {};
    
    template <typename Dummy>
    struct is_nullary::when<rule::let, Dummy>
        : proto::make<
            mpl::and_<
                detail::local_var_def_is_nullary<proto::_value(proto::_child_c<0>), _context>()
              , evaluator(
                    proto::_child_c<1>
                  , vector2<
                        mpl::true_
                      , detail::scope_is_nullary_actions
                    >()
                  , int()
                )
            >()
        >
    {};

}}

#endif
