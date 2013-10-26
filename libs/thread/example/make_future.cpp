// Copyright (C) 2012 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_THREAD_VERSION 4

#include <boost/thread/future.hpp>
#include <iostream>

namespace boost
{

  template <typename T>
  exception_ptr make_exception_ptr(T v)
  {
    return copy_exception(v);
  }
}

int p1() { return 5; }

void p() { }

#if defined BOOST_THREAD_USES_MOVE
boost::future<void> void_compute()
{
  return BOOST_THREAD_MAKE_RV_REF(boost::make_ready_future());
}
#endif

boost::future<int> compute(int x)
{
  if (x == 0) return boost::make_ready_future(0);
  //if (x < 0) return boost::make_ready_future<int>(boost::make_exception_ptr(std::logic_error("Error")));
  if (x < 0) return boost::make_ready_future<int>(std::logic_error("Error"));
  //boost::future<int> f1 = boost::async([]() { return x+1; });
  //boost::future<int> f1 = boost::async(boost::launch::async, &p1);
  boost::future<int> f1 = boost::async(p1);
  return boost::move(f1);
}
boost::shared_future<int> shared_compute(int x)
{
  if (x == 0) return boost::make_ready_future(0).share();
  if (x < 0) return boost::make_ready_future<int>(std::logic_error("Error")).share();
  //boost::future<int> f1 = boost::async([]() { return x+1; });
  boost::shared_future<int> f1 = boost::async(&p1).share();
  return f1;
}


int main()
{
  for (int i=0; i< 10; i++)
  try
  {
#if defined BOOST_THREAD_USES_MOVE
  {
    std::cout << __FILE__ << " "<< __LINE__ << std::endl;
    boost::future<void> f = void_compute();
    f.get();
  }
#endif
  {
    std::cout << __FILE__ << " "<< __LINE__ << std::endl;
    boost::future<int> f = compute(0);
    std::cout << f.get() << std::endl;
  }
  {
    std::cout << __FILE__ << " "<< __LINE__ << std::endl;
    boost::future<int> f = compute(0);
    std::cout << f.get() << std::endl;
  }
  {
    std::cout << __FILE__ << " "<< __LINE__ << std::endl;
    boost::shared_future<int> f = shared_compute(0);
    std::cout << f.get() << std::endl;
  }
  }
  catch (std::exception& ex)
  {
    std::cout << "ERRORRRRR "<<ex.what() << "" << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << "ERRORRRRR "<<"ERRORRRRR exception thrown" << std::endl;
    return 2;
  }
  return 0;
}
