//  Copyright (c) 2001-2008 Hartmut Kaiser
//  Copyright (c) 2001-2007 Joel de Guzman
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  This example shows how to create a simple lexer recognizing a couple of 
//  different tokens and how to use this with a grammar. This example has a 
//  heavily backtracking grammar which makes it a candidate for lexer based 
//  parsing (all tokens are scanned and generated only once, even if 
//  backtracking is required) which speeds up the overall parsing process 
//  considerably, out-weighting the overhead needed for setting up the lexer.
//  Additionally it demonstrates how to use one of the defined tokens as a 
//  parser component in the grammar.
//
//  The grammar recognizes a simple input structure: any number of English 
//  simple sentences (statements, questions and commands) are recognized and
//  are being counted separately.

// #define BOOST_SPIRIT_DEBUG 
// #define BOOST_SPIRIT_LEXERTL_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexer_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "example.hpp"

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::lex;
using boost::phoenix::ref;

///////////////////////////////////////////////////////////////////////////////
//  Token definition
///////////////////////////////////////////////////////////////////////////////
template <typename Lexer>
struct example2_tokens : lexer_def<Lexer>
{
    template <typename Self>
    void def (Self& self)
    {
        //  A 'word' is comprised of one or more letters and an optional 
        //  apostrophe. If it contains an apostrophe, there may only be one and 
        //  the apostrophe must be preceded and succeeded by at least 1 letter.  
        //  For example, "I'm" and "doesn't" meet the definition of 'word' we 
        //  define below.
        word = "[a-zA-Z]+('[a-zA-Z]+)?";
        
        // associate the tokens and the token set with the lexer
        self = token_def<>(',') | '!' | '.' | '?' | ' ' | '\n' | word;
    }
    
    token_def<> word;
};

///////////////////////////////////////////////////////////////////////////////
//  Grammar definition
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct example2_grammar : grammar_def<Iterator>
{
    template <typename TokenDef>
    example2_grammar(TokenDef const& tok)
      : paragraphs(0), commands(0), questions(0), statements(0)
    {
        story 
            =  +paragraph
            ;

        paragraph
            =   (  +(   command [ ++ref(commands) ] 
                    |   question [ ++ref(questions) ]
                    |   statement [ ++ref(statements) ]
                    ) 
                    >> *char_(' ') >> +char_('\n')
                ) 
                [ ++ref(paragraphs) ]
            ;

        command 
            =  +(tok.word | ' ' | ',') >> '!' 
            ;

        question 
            =  +(tok.word | ' ' | ',') >> '?' 
            ;

        statement 
            =  +(tok.word | ' ' | ',') >> '.' 
            ;

        BOOST_SPIRIT_DEBUG_NODE(story);
        BOOST_SPIRIT_DEBUG_NODE(paragraph);
        BOOST_SPIRIT_DEBUG_NODE(command);
        BOOST_SPIRIT_DEBUG_NODE(question);
        BOOST_SPIRIT_DEBUG_NODE(statement);
    }

    rule<Iterator> story, paragraph, command, question, statement;
    int paragraphs, commands, questions, statements;
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    // iterator type used to expose the underlying input stream
    typedef std::string::iterator base_iterator_type;
    
    // This is the token type to return from the lexer iterator
    typedef lexertl_token<base_iterator_type> token_type;
    
    // This is the lexer type to use to tokenize the input.
    // Here we use the lexertl based lexer engine.
    typedef lexertl_lexer<token_type> lexer_type;
    
    // This is the token definition type (derived from the given lexer type).
    typedef example2_tokens<lexer_type> example2_tokens;
    
    // this is the iterator type exposed by the lexer 
    typedef lexer<example2_tokens>::iterator_type iterator_type;

    // this is the type of the grammar to parse
    typedef example2_grammar<iterator_type> example2_grammar;

    // now we use the types defined above to create the lexer and grammar
    // object instances needed to invoke the parsing process
    example2_tokens tokens;                         // Our token definition
    example2_grammar def (tokens);                  // Our grammar definition

    lexer<example2_tokens> lex(tokens);             // Our lexer
    grammar<example2_grammar> calc(def, def.story); // Our grammar

    std::string str (read_from_file("example2.input"));

    // At this point we generate the iterator pair used to expose the
    // tokenized input stream.
    std::string::iterator it = str.begin();
    iterator_type iter = lex.begin(it, str.end());
    iterator_type end = lex.end();
        
    // Parsing is done based on the the token stream, not the character 
    // stream read from the input.
    bool r = parse(iter, end, calc);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "There were " 
                  << def.commands << " commands, " 
                  << def.questions << " questions, and " 
                  << def.statements << " statements.\n";
        std::cout << "-------------------------\n";
    }
    else
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
