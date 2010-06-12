///////////////////////////////////////////////////////////////////////////////
/// \file as_expr.hpp
/// Contains definition of the as_expr\<\> and as_child\<\> helper class
/// templates used to implement proto::domain's as_expr\<\> and as_child\<\>
/// member templates.
//
//  Copyright 2010 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_DETAIL_AS_EXPR_HPP_EAN_06_09_2010
#define BOOST_PROTO_DETAIL_AS_EXPR_HPP_EAN_06_09_2010

#include <boost/proto/proto_fwd.hpp>
#include <boost/proto/args.hpp>

namespace boost { namespace proto { namespace detail
{

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Generator>
    struct base_generator
    {
        typedef Generator type;
    };

    template<typename Generator>
    struct base_generator<use_basic_expr<Generator> >
    {
        typedef Generator type;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator, bool WantsBasicExpr>
    struct as_expr;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator>
    struct as_expr<T, Generator, false>
    {
        typedef typename term_traits<T &>::value_type value_type;
        typedef proto::expr<proto::tag::terminal, term<value_type>, 0> expr_type;
        typedef typename Generator::template result<Generator(expr_type)>::type result_type;

        result_type operator()(T &t) const
        {
            return Generator()(expr_type::make(t));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator>
    struct as_expr<T, Generator, true>
    {
        typedef typename term_traits<T &>::value_type value_type;
        typedef proto::basic_expr<proto::tag::terminal, term<value_type>, 0> expr_type;
        typedef typename Generator::template result<Generator(expr_type)>::type result_type;

        result_type operator()(T &t) const
        {
            return Generator()(expr_type::make(t));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct as_expr<T, proto::default_generator, false>
    {
        typedef typename term_traits<T &>::value_type value_type;
        typedef proto::expr<proto::tag::terminal, term<value_type>, 0> result_type;

        result_type operator()(T &t) const
        {
            return result_type::make(t);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct as_expr<T, proto::default_generator, true>
    {
        typedef typename term_traits<T &>::value_type value_type;
        typedef proto::basic_expr<proto::tag::terminal, term<value_type>, 0> result_type;

        result_type operator()(T &t) const
        {
            return result_type::make(t);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr, typename Generator, bool SameGenerator>
    struct already_expr
    {
        typedef typename Expr::proto_derived_expr uncv_expr_type;
        typedef typename Generator::template result<Generator(uncv_expr_type)>::type result_type;

        result_type operator()(Expr &e) const
        {
            return Generator()(e);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr, typename Generator>
    struct already_expr<Expr, Generator, true>
    {
        typedef typename Expr::proto_derived_expr result_type; // remove cv

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr>
    struct already_expr<Expr, default_generator, false>
    {
        typedef typename Expr::proto_derived_expr result_type; // remove cv

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr>
    struct already_expr<Expr, default_generator, true>
    {
        typedef typename Expr::proto_derived_expr result_type; // remove cv

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator, bool WantsBasicExpr>
    struct as_child;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator>
    struct as_child<T, Generator, false>
    {
        typedef proto::expr<proto::tag::terminal, term<T &>, 0> expr_type;
        typedef typename Generator::template result<Generator(expr_type)>::type result_type;

        result_type operator()(T &t) const
        {
            return Generator()(expr_type::make(t));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T, typename Generator>
    struct as_child<T, Generator, true>
    {
        typedef proto::basic_expr<proto::tag::terminal, term<T &>, 0> expr_type;
        typedef typename Generator::template result<Generator(expr_type)>::type result_type;

        result_type operator()(T &t) const
        {
            return Generator()(expr_type::make(t));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct as_child<T, proto::default_generator, false>
    {
        typedef proto::expr<proto::tag::terminal, term<T &>, 0> result_type;

        result_type operator()(T &t) const
        {
            return result_type::make(t);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct as_child<T, proto::default_generator, true>
    {
        typedef proto::basic_expr<proto::tag::terminal, term<T &>, 0> result_type;

        result_type operator()(T &t) const
        {
            return result_type::make(t);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr, typename Generator, bool SameGenerator>
    struct already_child
    {
        typedef typename Expr::proto_derived_expr uncv_expr_type;
        typedef typename Generator::template result<Generator(uncv_expr_type)>::type result_type;

        result_type operator()(Expr &e) const
        {
            return Generator()(e);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr, typename Generator>
    struct already_child<Expr, Generator, true>
    {
        typedef Expr &result_type;

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr>
    struct already_child<Expr, default_generator, false>
    {
        typedef Expr &result_type;

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Expr>
    struct already_child<Expr, default_generator, true>
    {
        typedef Expr &result_type;

        result_type operator()(Expr &e) const
        {
            return e;
        }
    };

}}}

#endif
