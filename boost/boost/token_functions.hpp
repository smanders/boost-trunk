// Boost token_functions.hpp  ------------------------------------------------//

// Copyright John R. Bandela 2001. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// See http://www.boost.org for updates, documentation, and revision history.

// Revision History:

// 02 Feb 2002   Jeremy Siek
//      Removed tabs and a little cleanup.


#ifndef BOOST_TOKEN_FUNCTIONS_JRB051801_HPP_
#define BOOST_TOKEN_FUNCTIONS_JRB051801_HPP_

#include <vector>
#include <stdexcept>
#include <cassert>
#include <string>
#include <cctype>

namespace boost{

  //===========================================================================
  // The escaped_list_separator class. Which is a model of TokenizerFunction
  // An escaped list is a super-set of what is commonly known as a comma 
  // separated value (csv) list.It is separated into fields by a comma or 
  // other character. If the delimiting character is inside quotes, then it is
  // counted as a regular character.To allow for embedded quotes in a field,
  // there can be escape sequences using the \ much like C. 
  // The role of the comma, the quotation mark, and the escape 
  // character (backslash \), can be assigned to other characters.

  struct escaped_list_error : public std::runtime_error{
    escaped_list_error(const std::string& what):std::runtime_error(what) { }
  };
  

// The out of the box GCC 2.95 on cygwin does not have a char_traits class.
// MSVC does not like the following typename
#ifndef BOOST_MSVC
  template <class Char, 
    class Traits = typename std::basic_string<Char>::traits_type >
#else
  template <class Char, 
    class Traits = std::basic_string<Char>::traits_type >
#endif
  class escaped_list_separator {

  private:
    typedef std::basic_string<Char,Traits> string_type;
    struct char_eq {
      Char e_;
      char_eq(Char e):e_(e) { }
      bool operator()(Char c) {
        return Traits::eq(e_,c);
      }
    };
    string_type  escape_;
    string_type  c_;
    string_type  quote_;      
    bool last_;

    bool is_escape(Char e) {
      char_eq f(e);
      return std::find_if(escape_.begin(),escape_.end(),f)!=escape_.end();
    }
    bool is_c(Char e) {
      char_eq f(e);
      return std::find_if(c_.begin(),c_.end(),f)!=c_.end();
    }
    bool is_quote(Char e) {
      char_eq f(e);
      return std::find_if(quote_.begin(),quote_.end(),f)!=quote_.end();
    }
    template <typename iterator, typename Token>
    void do_escape(iterator& next,iterator end,Token& tok) {
      if (++next == end)
        throw escaped_list_error(std::string("cannot end with escape"));
      if (Traits::eq(*next,'n')) {
        tok+='\n';
        return;
      }
      else if (is_quote(*next)) {
        tok+=*next;
        return;
      }
      else if (is_c(*next)) {
        tok+=*next;
        return;
      }
      else if (is_escape(*next)) {
        tok+=*next;
        return;
      }
      else
        throw escaped_list_error(std::string("unknown escape sequence"));
    }

    public:
    
    explicit escaped_list_separator(Char  e = '\\',
                                    Char c = ',',Char  q = '\"')
      : escape_(1,e), c_(1,c), quote_(1,q), last_(false) { }
    
    escaped_list_separator(string_type e, string_type c, string_type q)
      : escape_(e), c_(c), quote_(q), last_(false) { }
    
    void reset() {last_=false;}

    template <typename InputIterator, typename Token>
    bool operator()(InputIterator& next,InputIterator end,Token& tok) {
      bool bInQuote = false;
      tok = Token();
      
      if (next == end) {
        if (last_) {
          last_ = false;
          return true;
        }
        else
          return false;
      }
      last_ = false;
      for (;next != end;++next) {
        if (is_escape(*next)) {
          do_escape(next,end,tok);
        }
        else if (is_c(*next)) {
          if (!bInQuote) {
            // If we are not in quote, then we are done
            ++next;
            // The last character was a c, that means there is
            // 1 more blank field
            last_ = true; 
            return true;
          }
          else tok+=*next;
        }
        else if (is_quote(*next)) {
          bInQuote=!bInQuote;
        }
        else {
          tok += *next;
        }
      }
      return true;
    }
  };

   
  //===========================================================================
  // The offset_separator class, which is a model of TokenizerFunction.
  // Offset breaks a string into tokens based on a range of offsets

  class offset_separator {
  private:

    std::vector<int> offsets_;
    unsigned int current_offset_;
    bool wrap_offsets_;
    bool return_partial_last_;
    
  public:
    template <typename Iter>
    offset_separator(Iter begin, Iter end, bool wrap_offsets = true,
                     bool return_partial_last = true)
      : offsets_(begin,end), current_offset_(0),
        wrap_offsets_(wrap_offsets),
        return_partial_last_(return_partial_last) { }
   
    offset_separator()
      : offsets_(1,1), current_offset_(),
        wrap_offsets_(true), return_partial_last_(true) { }

    void reset() {
      current_offset_ = 0;
    }

    template <typename InputIterator, typename Token>
    bool operator()(InputIterator& next, InputIterator end, Token& tok)
    {
      assert(!offsets_.empty());
    
      tok = Token();
      
      if (next == end)
        return false;

      if (current_offset_ == offsets_.size())
        if (wrap_offsets_)
          current_offset_=0;
        else
          return false;
      
      int c = offsets_[current_offset_];
      int i = 0;
      for (; i < c; ++i) {
        if (next == end)break;
        tok+=*next++;
      }
    
      if (!return_partial_last_)
        if (i < (c-1) )
          return false;
      
      ++current_offset_;
      return true;
    }
  };


  //===========================================================================
  // The char_separator class breaks a sequence of characters into
  // tokens based on the character delimiters (very much like bad old
  // strtok). A delimiter character can either be kept or dropped. A
  // kept delimiter shows up as an output token, whereas a dropped
  // delimiter does not.

  // This class replaces the char_delimiters_separator class. The
  // constructor for the char_delimiters_separator class was too
  // confusing and needed to be deprecated. However, because of the
  // default arguments to the constructor, adding the new constructor
  // would cause ambiguity, so instead I deprecated the whole class.
  // The implementation of the class was also simplified considerably.

  enum empty_token_policy { drop_empty_tokens, keep_empty_tokens };

  // The out of the box GCC 2.95 on cygwin does not have a char_traits class.
#ifndef BOOST_MSVC
  template <typename Char, 
    typename Traits = typename std::basic_string<Char>::traits_type >
#else
  template <typename Char, 
    typename Traits = std::basic_string<Char>::traits_type >
#endif
  class char_separator
  {
    typedef std::basic_string<Char,Traits> string_type;
  public:
    explicit 
    char_separator(const Char* dropped_delims,
                   const Char* kept_delims = "",
                   empty_token_policy empty_tokens = drop_empty_tokens)
      : m_kept_delims(kept_delims),
        m_dropped_delims(dropped_delims),
        m_use_ispunct(false),
        m_use_isspace(false),
        m_empty_tokens(empty_tokens),
        m_output_done(false) { }

 		// use ispunct() for kept delimiters and isspace for dropped.
    explicit
    char_separator()
      : m_use_ispunct(true), 
        m_use_isspace(true), 
        m_empty_tokens(drop_empty_tokens) { }

    void reset() { }

    template <typename InputIterator, typename Token>
    bool operator()(InputIterator& next, InputIterator end, Token& tok)
    {
      tok = Token();

      // skip past all dropped_delims
      if (m_empty_tokens == drop_empty_tokens)
        for (; next != end  && is_dropped(*next); ++next)
          { }
      
      if (m_empty_tokens == drop_empty_tokens) {

        if (next == end)
          return false;

        // if we are on a kept_delims move past it and stop
        if (is_kept(*next)) {
          tok += *next;
          ++next;
        } else
          // append all the non delim characters
          for (; next != end && !is_dropped(*next) && !is_kept(*next); ++next)
            tok += *next;
      } 
      else { // m_empty_tokens == keep_empty_tokens
        
        // Handle empty token at the end
        if (next == end)
          if (m_output_done == false) {
            m_output_done = true;
            return true;
          } else
            return false;

        if (is_kept(*next)) {
          if (m_output_done == false)
            m_output_done = true;
          else {
            tok += *next;
            ++next;
            m_output_done = false;
          }
        } 
        else if (m_output_done == false && is_dropped(*next)) {
          m_output_done = true;
        } 
        else {
          if (is_dropped(*next))
            ++next;
          for (; next != end && !is_dropped(*next) && !is_kept(*next); ++next)
            tok += *next;
          m_output_done = true;
        }
      }
      return true;
    }

  private:
    string_type m_kept_delims;
    string_type m_dropped_delims;
    bool m_use_ispunct;
    bool m_use_isspace;
    empty_token_policy m_empty_tokens;
    bool m_output_done;
    
    bool is_kept(Char E) const
    {  
      if (m_kept_delims.length())
        return m_kept_delims.find(E) != string_type::npos;
      else if (m_use_ispunct) {
        using namespace std;
        return ispunct(E) != 0;
      } else
        return false;
    }
    bool is_dropped(Char E) const
    {
      if (m_dropped_delims.length())
        return m_dropped_delims.find(E) != string_type::npos;
      else if (m_use_isspace) {
        using namespace std; 
        return isspace(E) != 0;
      } else
        return false;
    }
  };

  //===========================================================================
  // The following class is DEPRECATED, use class char_separators instead.
  //
  // The char_delimiters_separator class, which is a model of
  // TokenizerFunction.  char_delimiters_separator breaks a string
  // into tokens based on character delimiters. There are 2 types of
  // delimiters. returnable delimiters can be returned as
  // tokens. These are often punctuation. nonreturnable delimiters
  // cannot be returned as tokens. These are often whitespace

  // The out of the box GCC 2.95 on cygwin does not have a char_traits class.
#ifndef BOOST_MSVC
  template <class Char, 
    class Traits = typename std::basic_string<Char>::traits_type >
#else
  template <class Char, 
    class Traits = std::basic_string<Char>::traits_type >
#endif
  class char_delimiters_separator {
  private:  

    typedef std::basic_string<Char,Traits> string_type;
    string_type returnable_;
    string_type nonreturnable_;
    bool return_delims_;
    bool no_ispunct_;
    bool no_isspace_;
    
    bool is_ret(Char E)const
    {  
      if (returnable_.length())
        return  returnable_.find(E) != string_type::npos;
      else{
        if (no_ispunct_) {return false;}
        else{
          using namespace std;
          int r = ispunct(E);
          return r != 0;
        }
      }
    }
    bool is_nonret(Char E)const
    {
      if (nonreturnable_.length())
        return  nonreturnable_.find(E) != string_type::npos;
      else{
        if (no_isspace_) {return false;}
        else{
          using namespace std; 
          int r = isspace(E);
          return r != 0;
        }
      }
    }
    
  public:
    explicit char_delimiters_separator(bool return_delims = false, 
                                       const Char* returnable = 0,
                                       const Char* nonreturnable = 0)
      : returnable_(returnable ? returnable : string_type().c_str()),
        nonreturnable_(nonreturnable ? nonreturnable:string_type().c_str()),
        return_delims_(return_delims), no_ispunct_(returnable!=0),
        no_isspace_(nonreturnable!=0) { }
    
    void reset() { }

  public:

     template <typename InputIterator, typename Token>
     bool operator()(InputIterator& next, InputIterator end,Token& tok) {
     tok = Token();
     
     // skip past all nonreturnable delims
     // skip past the returnable only if we are not returning delims
     for (;next!=end && ( is_nonret(*next) || (is_ret(*next) 
       && !return_delims_ ) );++next) { }
     
     if (next == end) {
       return false;
     }
     
     // if we are to return delims and we are one a returnable one
     // move past it and stop
     if (is_ret(*next) && return_delims_) {
       tok+=*next;
       ++next;
     }
     else
       // append all the non delim characters
       for (;next!=end && !is_nonret(*next) && !is_ret(*next);++next)
         tok+=*next;
       
       
     return true;
   }
  };


} //namespace boost


#endif 





