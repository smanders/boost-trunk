// Copyright David Abrahams and Nikolay Mladenov 2003. Permission to
// copy, use, modify, sell and distribute this software is granted
// provided this copyright notice appears in all copies. This software
// is provided "as is" without express or implied warranty, and with
// no claim as to its suitability for any purpose.
#ifndef RETURN_ARG_DWA2003719_HPP
# define RETURN_ARG_DWA2003719_HPP
# include <boost/python/default_call_policies.hpp>
# include <boost/python/detail/none.hpp>

# include <boost/type_traits/add_reference.hpp>
# include <boost/type_traits/add_const.hpp>

# if 0 // argpkg
#  include <boost/mpl/int.hpp>
# endif

# include <boost/static_assert.hpp>
# include <boost/python/refcount.hpp>

# include <cstddef>

namespace boost { namespace python { 

namespace detail
{
  template <std::size_t>
  struct return_arg_pos_argument_must_be_positive
# if defined(__GNUC__) && __GNUC__ >= 3 || defined(__EDG__)
  {}
# endif
  ;

  struct return_none
  {
      template <class T> struct apply
      {
          struct type
          {
              static bool convertible()
              {
                  return true;
              }
              
              PyObject *operator()(
                  typename add_reference<
                      typename add_const<T>::type
                  >::type
              ) const
              {
                  return none();
              }
          };
      };
  };
}
    
template <
    std::size_t arg_pos=1
  , class Base = default_call_policies
> 
struct return_arg : Base
{
 private:
    BOOST_STATIC_CONSTANT(bool, legal = arg_pos > 0);

 public:
    typedef typename mpl::if_c<
        legal
        , detail::return_none
        , detail::return_arg_pos_argument_must_be_positive<arg_pos>
        // we could default to the base result_converter in case or
        // arg_pos==0 since return arg 0 means return result, but I
        // think it is better to issue an error instead, cause it can
        // lead to confusions
    >::type result_converter;

    template <class ArgumentPackage>
    static PyObject* postcall(ArgumentPackage const& args, PyObject* result)
    {
        // In case of arg_pos == 0 we could simply return Base::postcall,
        // but this is redundant
        BOOST_STATIC_ASSERT(arg_pos > 0);

        result = Base::postcall(args,result);
        if (!result)
            return 0;
        Py_DECREF(result);
# if 0 // argpkg
        return incref( detail::get(mpl::int_<arg_pos-1>(),args) );
# else
        return incref( detail::get<(arg_pos-1)>(args) );
# endif 
    }
};

template <
    class Base = default_call_policies
    >
struct return_self 
  : return_arg<1,Base>
{};

}} // namespace boost::python

#endif // RETURN_ARG_DWA2003719_HPP
