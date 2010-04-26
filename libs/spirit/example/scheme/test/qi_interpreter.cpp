/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/config/warning_disable.hpp>

#include <input/sexpr.hpp>
#include <input/parse_sexpr_impl.hpp>
#include <scheme/compiler.hpp>
#include <utree/io.hpp>
#include <boost/spirit/include/qi.hpp>
#include <map>

#include "../../../test/qi/test.hpp"

#define SCHEME_QI_COMPILER_LIMIT 20

namespace scheme { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    // parser compiler
    ///////////////////////////////////////////////////////////////////////////
    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;

    typedef qi::rule<char const*> skipper_type;
    typedef qi::rule<char const*, utree(), skipper_type> rule_type;

    ///////////////////////////////////////////////////////////////////////////
    // All rule are stored here. Rules are held in the utree by its id;
    // i.e. its index in the vector.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Rule>
    class rule_fragments
    {
    public:

        rule_fragments()
          : index(0)
        {
        };

        template <typename Expr>
        int new_rule(Expr const& expr)
        {
            rules[index] = expr;
            return index++;
        }

        Rule const& operator[](int id) const
        {
            typename std::map<int, Rule>::const_iterator
                iter = rules.find(id);
            BOOST_ASSERT(iter != rules.end());
            return iter->second;
        }

        Rule const& operator[](utree const& id) const
        {
            return (*this)[id.get<int>()];
        }

    private:

        int index;
        std::map<int, Rule> rules;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Composes primitive parsers held by index. Handles the compilation of
    // primitive (nullary) parsers such as int_, double_, alpha, space and all
    // those that require no arguments.
    ///////////////////////////////////////////////////////////////////////////
    struct primitive_parser_composite : composite<primitive_parser_composite>
    {
        int id;
        primitive_parser_composite(int id)
          : id(id)
        {
        }

        function compose(actor_list const& elements) const
        {
            return val(id);
        }
    };

    template <typename Fragments, typename Expr>
    inline primitive_parser_composite
    make_primitive_parser_composite(Fragments& fragments, Expr const& expr)
    {
        return primitive_parser_composite(fragments.new_rule(expr));
    }

    ///////////////////////////////////////////////////////////////////////////
    // Handles the compilation of char_
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    struct char_function : actor<char_function<Fragments> >
    {
        Fragments& fragments;
        function a;
        function b;
        char_function(
            Fragments& fragments, function const& a, function const& b)
          : a(a), b(b), fragments(fragments)
        {
        }

        utree eval(utree const& a) const
        {
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a.which() == utree_type::string_type);

            utf8_string_range a_ = a.get<utf8_string_range>();
            if (a_.size() == 1)
            {
                // char_('x')
                return fragments.new_rule(qi::char_(a_[0]));
            }
            else
            {
                // char_("some-regex")
                return fragments.new_rule(
                    qi::char_(std::string(a_.begin(), a_.end())));
            }
        }

        utree eval(utree const& a, utree const& b) const
        {
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a.which() == utree_type::string_type);
            BOOST_ASSERT(b.which() == utree_type::string_type);

            utf8_string_range a_ = a.get<utf8_string_range>();
            utf8_string_range b_ = b.get<utf8_string_range>();
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a_.size() == 1);
            BOOST_ASSERT(b_.size() == 1);

            // char_('x', 'y')
            return fragments.new_rule(qi::char_(a_[0], b_[0]));
        }

        utree eval(scope const& env) const
        {
            if (b.empty())
                return eval(a(env));
            else
                return eval(a(env), b(env));
        }
    };

    template <typename Fragments>
    struct char_composite
      : composite<char_composite<Fragments> >
    {
        Fragments& fragments;
        char_composite(Fragments& fragments)
          : fragments(fragments) {}

        function compose(actor_list const& elements) const
        {
            typedef char_function<Fragments> function_type;
            actor_list::const_iterator i = elements.begin();

            function empty;
            function const& a = *i++;
            function const& b = (i == elements.end())? empty : *i;
            return function(function_type(fragments, a, b));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Handles the compilation of kleene *a
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    struct kleene_function : actor<kleene_function<Fragments> >
    {
        Fragments& fragments;
        function a;
        kleene_function(
            Fragments& fragments, function const& a)
          : a(a), fragments(fragments)
        {
        }

        utree eval(utree const& a) const
        {
            return fragments.new_rule(*fragments[a]); // *a
        }

        utree eval(scope const& env) const
        {
            return eval(a(env));
        }
    };

    template <typename Fragments>
    struct kleene_composite
      : composite<kleene_composite<Fragments> >
    {
        Fragments& fragments;
        kleene_composite(Fragments& fragments)
          : fragments(fragments) {}

        function compose(actor_list const& elements) const
        {
            typedef kleene_function<Fragments> function_type;
            return function(function_type(fragments, elements.front()));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Handles the compilation of difference a - b
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    struct difference_function : actor<difference_function<Fragments> >
    {
        Fragments& fragments;
        function a;
        function b;
        difference_function(
            Fragments& fragments, function const& a, function const& b)
          : a(a), b(b), fragments(fragments)
        {
        }

        utree eval(utree const& a, utree const& b) const
        {
            return fragments.new_rule(
                fragments[a] - fragments[b]); // a - b
        }

        utree eval(scope const& env) const
        {
            return eval(a(env), b(env));
        }
    };

    template <typename Fragments>
    struct difference_composite
      : composite<difference_composite<Fragments> >
    {
        Fragments& fragments;
        difference_composite(Fragments& fragments)
          : fragments(fragments) {}

        function compose(actor_list const& elements) const
        {
            typedef difference_function<Fragments> function_type;
            actor_list::const_iterator i = elements.begin();

            function const& a = *i++;
            function const& b = *i;
            return function(function_type(fragments, a, b));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Build our scheme compiler environment.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    void build_environment(Fragments& fragments, environment& env)
    {
        build_basic_environment(env);

        env.define("qi:space",
            make_primitive_parser_composite(fragments, qi::space), 0, true);

        env.define("qi:alpha",
            make_primitive_parser_composite(fragments, qi::alpha), 0, true);

        env.define("qi:int_",
            make_primitive_parser_composite(fragments, qi::int_), 0, true);

        env.define("qi:char_",
            char_composite<Fragments>(fragments), 1, false);

        env.define("qi:*",
            kleene_composite<Fragments>(fragments), 1, true);

        env.define("qi:-",
            difference_composite<Fragments>(fragments), 2, true);
    }
}}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    using scheme::utree;
    using scheme::interpreter;
    using scheme::environment;
    using scheme::qi::build_environment;
    using scheme::qi::rule_fragments;
    using scheme::qi::rule_type;
    using spirit_test::test;

    environment env;
    rule_fragments<rule_type> fragments;
    build_environment(fragments, env);

    scheme::qi::skipper_type space = boost::spirit::qi::space;

    {
        utree src =
            "(define charx (qi:char_ \"x\"))"
            "(define integer (qi:int_))"
            "(define int_not_0 (qi:- (qi:int_) (qi:char_ \"0\")))"
            "(define integers (qi:* (qi:int_)))";
        interpreter parser(src, "parse.scm", &env);

        BOOST_TEST(test("x",        fragments[parser["charx"]()],       space));
        BOOST_TEST(!test("y",       fragments[parser["charx"]()],       space));
        BOOST_TEST(test("1234",     fragments[parser["integer"]()],     space));
        BOOST_TEST(!test("x1234",   fragments[parser["integer"]()],     space));
        BOOST_TEST(test("1 2 3 4",  fragments[parser["integers"]()],    space));
        BOOST_TEST(test("1",        fragments[parser["int_not_0"]()],   space));
        BOOST_TEST(!test("0",       fragments[parser["int_not_0"]()],   space));
    }

    return boost::report_errors();
}


