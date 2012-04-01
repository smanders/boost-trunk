//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/thread.hpp>

// class thread

// template <class F, class ...Args> thread(F f, Args... args);

#include <new>
#include <cstdlib>
#include <cassert>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>

unsigned throw_one = 0xFFFF;

void* operator new(std::size_t s) throw (std::bad_alloc)
{
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
  if (throw_one == 0) throw std::bad_alloc();
  --throw_one;
  return std::malloc(s);
}

void operator delete(void* p) throw ()
{
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
  std::free(p);
}

bool f_run = false;

void f(int i, double j)
{
  BOOST_TEST(i == 5);
  BOOST_TEST(j == 5.5);
  f_run = true;
}

class G
{
  int alive_;
public:
  static int n_alive;
  static bool op_run;

  G() :
    alive_(1)
  {
    ++n_alive;
  }
  G(const G& g) :
    alive_(g.alive_)
  {
    ++n_alive;
  }
  ~G()
  {
    alive_ = 0;
    --n_alive;
  }

  void operator()(int i, double j)
  {
    BOOST_TEST(alive_ == 1);
    BOOST_TEST(n_alive >= 1);
    BOOST_TEST(i == 5);
    BOOST_TEST(j == 5.5);
    op_run = true;
  }
};

int G::n_alive = 0;
bool G::op_run = false;


int main()
{
  {
    boost::thread t(f, 5, 5.5);
    t.join();
    BOOST_TEST(f_run == true);
  }
#ifndef BOOST_MSVC
  f_run = false;
  {
    try
    {
      throw_one = 0;
      boost::thread t(f, 5, 5.5);
      BOOST_TEST(false);
    }
    catch (...)
    {
      throw_one = 0xFFFF;
      BOOST_TEST(!f_run);
    }
  }
#endif
  {
    BOOST_TEST(G::n_alive == 0);
    std::cout << __FILE__ << ":" << __LINE__ <<" " << G::n_alive << std::endl;
    BOOST_TEST(!G::op_run);
    boost::thread t(G(), 5, 5.5);
    t.join();
    std::cout << __FILE__ << ":" << __LINE__ <<" " << G::n_alive << std::endl;
    BOOST_TEST(G::n_alive == 0);
    BOOST_TEST(G::op_run);
  }

  return boost::report_errors();
}
