/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_WHAT_FUNCTION_APR_22_2007_0236PM)
#define SPIRIT_WHAT_FUNCTION_APR_22_2007_0236PM

#include <string>

namespace boost { namespace spirit { namespace detail
{
    struct what_function
    {
        what_function(std::string& str)
          : str(str), first(true)
        {
        }

        template <typename Component>
        void operator()(Component const& component) const
        {
            if (first)
                first = false;
            else
                str += ", ";
            typedef typename Component::director director;
            str += director::what(component);
        }

        std::string& str;
        mutable bool first;
    };
}}}

#endif
