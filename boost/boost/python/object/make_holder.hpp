#if !defined(BOOST_PP_IS_ITERATING)

// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

# ifndef MAKE_HOLDER_DWA20011215_HPP
#  define MAKE_HOLDER_DWA20011215_HPP

#  include <boost/python/object/forward.hpp>
#  include <boost/python/object/class.hpp>
#  include <boost/python/detail/wrap_python.hpp>
#  include <boost/python/detail/preprocessor.hpp>

#  include <boost/mpl/at.hpp>

#  include <boost/preprocessor/comma_if.hpp>
#  include <boost/preprocessor/iterate.hpp>
#  include <boost/preprocessor/repeat.hpp>

namespace boost { namespace python { namespace objects {

template <int nargs> struct make_holder;

#  define BOOST_PYTHON_FORWARD_ARG(index, _)                \
    typedef typename mpl::at<index,ArgList>::type t##index; \
    typedef typename forward<t##index>::type f##index;

#  define BOOST_PYTHON_DO_FORWARD_ARG(index, _) , f##index(a##index)

// specializations...
#  define BOOST_PP_ITERATION_PARAMS_1 3, (0, BOOST_PYTHON_MAX_ARITY, <boost/python/object/make_holder.hpp>)
#  include BOOST_PP_ITERATE()

#  undef BOOST_PYTHON_FORWARD_ARG
#  undef BOOST_PYTHON_DO_FORWARD_ARG

}}} // namespace boost::python::objects

# endif // MAKE_HOLDER_DWA20011215_HPP

#elif BOOST_PP_ITERATION_DEPTH() == 1
# line BOOST_PP_LINE(__LINE__, make_holder.hpp)

# define N BOOST_PP_ITERATION()

template <>
struct make_holder<N>
{
    template <class Holder, class ArgList>
    struct apply
    {
        BOOST_PP_REPEAT(N, BOOST_PYTHON_FORWARD_ARG, nil)
        static void execute(
            PyObject* p
            BOOST_PP_COMMA_IF(N) BOOST_PYTHON_BINARY_ENUM(N, t, a))
        {
            (new Holder(
                p BOOST_PP_REPEAT(N, BOOST_PYTHON_DO_FORWARD_ARG, nil)))->install(p);
        }
    };
};

# undef N

#endif
