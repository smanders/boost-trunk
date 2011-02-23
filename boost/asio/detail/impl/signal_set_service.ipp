//
// detail/impl/signal_set_service.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_IMPL_SIGNAL_SET_SERVICE_IPP
#define BOOST_ASIO_DETAIL_IMPL_SIGNAL_SET_SERVICE_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#include <signal.h>
#include <cstring>
#include <boost/asio/detail/reactor.hpp>
#include <boost/asio/detail/signal_set_service.hpp>
#include <boost/asio/detail/static_mutex.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {

struct signal_state
{
  // Mutex used for protecting global state.
  static_mutex mutex_;

  // The read end of the pipe used for signal notifications.
  int read_descriptor_;

  // The write end of the pipe used for signal notifications.
  int write_descriptor_;

  // The head of a linked list of all signal_set_service instances.
  class signal_set_service* service_list_;

  // A count of the number of objects that are registered for each signal.
  std::size_t registration_count_[max_signal_number];
};

signal_state* get_signal_state()
{
  static signal_state state = {
    BOOST_ASIO_STATIC_MUTEX_INIT, -1, -1, 0, { 0 } };
  return &state;
}

void asio_signal_handler(int signal_number)
{
#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)
  signal_set_service::deliver_signal(signal_number);
#else // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
  signal_state* state = get_signal_state();
  (void)::write(state->write_descriptor_,
      &signal_number, sizeof(signal_number));
#endif // defined(BOOST_WINDOWS) || defined(__CYGWIN__)

#if !defined(BOOST_ASIO_HAS_SIGACTION)
  signal(signal_number, asio_signal_handler);
#endif // !defined(BOOST_ASIO_HAS_SIGACTION)
}

signal_set_service::signal_set_service(
    boost::asio::io_service& io_service)
  : io_service_(boost::asio::use_service<io_service_impl>(io_service)),
#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
    reactor_(boost::asio::use_service<reactor>(io_service)),
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
    next_(0),
    prev_(0)
{
  get_signal_state()->mutex_.init();

#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
  reactor_.init_task();
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)

  for (int i = 0; i < max_signal_number; ++i)
    registrations_[i] = 0;

  add_service(this);
}

signal_set_service::~signal_set_service()
{
  remove_service(this);
}

void signal_set_service::shutdown_service()
{
  remove_service(this);

  op_queue<operation> ops;

  for (int i = 0; i < max_signal_number; ++i)
  {
    registration* reg = registrations_[i];
    while (reg)
    {
      ops.push(*reg->queue_);
      reg = reg->next_in_table_;
    }
  }
}

void signal_set_service::construct(
    signal_set_service::implementation_type& impl)
{
  impl.signals_ = 0;
}

void signal_set_service::destroy(
    signal_set_service::implementation_type& impl)
{
  boost::system::error_code ignored_ec;
  clear(impl, ignored_ec);
  cancel(impl, ignored_ec);
}

boost::system::error_code signal_set_service::add(
    signal_set_service::implementation_type& impl,
    int signal_number, boost::system::error_code& ec)
{
  // Check that the signal number is valid.
  if (signal_number < 0 || signal_number > max_signal_number)
  {
    ec = boost::asio::error::invalid_argument;
    return ec;
  }

  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  // Find the appropriate place to insert the registration.
  registration** insertion_point = &impl.signals_;
  registration* next = impl.signals_;
  while (next && next->signal_number_ < signal_number)
  {
    insertion_point = &next->next_in_set_;
    next = next->next_in_set_;
  }

  // Only do something if the signal is not already registered.
  if (next == 0 || next->signal_number_ != signal_number)
  {
    registration* new_registration = new registration;

    // Register for the signal if we're the first.
    if (state->registration_count_[signal_number] == 0)
    {
#if defined(BOOST_ASIO_HAS_SIGACTION)
      using namespace std; // For memset.
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = asio_signal_handler;
      sigfillset(&sa.sa_mask);
      if (::sigaction(signal_number, &sa, 0) == -1)
#else // defined(BOOST_ASIO_HAS_SIGACTION)
      if (::signal(signal_number, asio_signal_handler) == SIG_ERR)
#endif // defined(BOOST_ASIO_HAS_SIGACTION)
      {
#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::asio::error::invalid_argument;
#else // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::system::error_code(errno,
            boost::asio::error::get_system_category());
#endif // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        delete new_registration;
        return ec;
      }
    }

    // Record the new registration in the set.
    new_registration->signal_number_ = signal_number;
    new_registration->queue_ = &impl.queue_;
    new_registration->next_in_set_ = next;
    *insertion_point = new_registration;

    // Insert registration into the registration table.
    new_registration->next_in_table_ = registrations_[signal_number];
    if (registrations_[signal_number])
      registrations_[signal_number]->prev_in_table_ = new_registration;
    registrations_[signal_number] = new_registration;

    ++state->registration_count_[signal_number];
  }

  ec = boost::system::error_code();
  return ec;
}

boost::system::error_code signal_set_service::remove(
    signal_set_service::implementation_type& impl,
    int signal_number, boost::system::error_code& ec)
{
  // Check that the signal number is valid.
  if (signal_number < 0 || signal_number > max_signal_number)
  {
    ec = boost::asio::error::invalid_argument;
    return ec;
  }

  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  // Find the signal number in the list of registrations.
  registration** deletion_point = &impl.signals_;
  registration* reg = impl.signals_;
  while (reg && reg->signal_number_ < signal_number)
  {
    deletion_point = &reg->next_in_set_;
    reg = reg->next_in_set_;
  }

  if (reg != 0 && reg->signal_number_ == signal_number)
  {
    // Set signal handler back to the default if we're the last.
    if (state->registration_count_[signal_number] == 1)
    {
#if defined(BOOST_ASIO_HAS_SIGACTION)
      using namespace std; // For memset.
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = SIG_DFL;
      if (::sigaction(signal_number, &sa, 0) == -1)
#else // defined(BOOST_ASIO_HAS_SIGACTION)
      if (::signal(signal_number, SIG_DFL) == SIG_ERR)
#endif // defined(BOOST_ASIO_HAS_SIGACTION)
      {
#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::asio::error::invalid_argument;
#else // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::system::error_code(errno,
            boost::asio::error::get_system_category());
#endif // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        return ec;
      }
    }

    // Remove the registration from the set.
    *deletion_point = reg->next_in_set_;

    // Remove the registration from the registration table.
    if (registrations_[signal_number] == reg)
      registrations_[signal_number] = reg->next_in_table_;
    if (reg->prev_in_table_)
      reg->prev_in_table_->next_in_table_ = reg->next_in_table_;
    if (reg->next_in_table_)
      reg->next_in_table_->prev_in_table_ = reg->prev_in_table_;

    --state->registration_count_[signal_number];

    delete reg;
  }

  ec = boost::system::error_code();
  return ec;
}

boost::system::error_code signal_set_service::clear(
    signal_set_service::implementation_type& impl,
    boost::system::error_code& ec)
{
  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  while (registration* reg = impl.signals_)
  {
    // Set signal handler back to the default if we're the last.
    if (state->registration_count_[reg->signal_number_] == 1)
    {
#if defined(BOOST_ASIO_HAS_SIGACTION)
      using namespace std; // For memset.
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = SIG_DFL;
      if (::sigaction(reg->signal_number_, &sa, 0) == -1)
#else // defined(BOOST_ASIO_HAS_SIGACTION)
      if (::signal(reg->signal_number_, SIG_DFL) == SIG_ERR)
#endif // defined(BOOST_ASIO_HAS_SIGACTION)
      {
#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::asio::error::invalid_argument;
#else // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        ec = boost::system::error_code(errno,
            boost::asio::error::get_system_category());
#endif // defined(BOOST_WINDOWS) || defined(__CYGWIN__)
        return ec;
      }
    }

    // Remove the registration from the registration table.
    if (registrations_[reg->signal_number_] == reg)
      registrations_[reg->signal_number_] = reg->next_in_table_;
    if (reg->prev_in_table_)
      reg->prev_in_table_->next_in_table_ = reg->next_in_table_;
    if (reg->next_in_table_)
      reg->next_in_table_->prev_in_table_ = reg->prev_in_table_;

    --state->registration_count_[reg->signal_number_];

    impl.signals_ = reg->next_in_set_;
    delete reg;
  }

  ec = boost::system::error_code();
  return ec;
}

boost::system::error_code signal_set_service::cancel(
    signal_set_service::implementation_type& impl,
    boost::system::error_code& ec)
{
  BOOST_ASIO_HANDLER_OPERATION(("signal_set", &impl, "cancel"));

  op_queue<operation> ops;
  while (signal_op* op = impl.queue_.front())
  {
    op->ec_ = boost::asio::error::operation_aborted;
    impl.queue_.pop();
    ops.push(op);
  }

  io_service_.post_deferred_completions(ops);

  ec = boost::system::error_code();
  return ec;
}

void signal_set_service::deliver_signal(int signal_number)
{
  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  signal_set_service* service = state->service_list_;
  while (service)
  {
    op_queue<operation> ops;

    registration* reg = service->registrations_[signal_number];
    while (reg)
    {
      if (reg->queue_->empty())
      {
        ++reg->undelivered_;
      }
      else
      {
        while (signal_op* op = reg->queue_->front())
        {
          op->signal_number_ = signal_number;
          reg->queue_->pop();
          ops.push(op);
        }
      }

      reg = reg->next_in_table_;
    }

    service->io_service_.post_deferred_completions(ops);

    service = service->next_;
  }
}

#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
class signal_set_service::pipe_read_op : public reactor_op
{
public:
  pipe_read_op()
    : reactor_op(&pipe_read_op::do_perform, pipe_read_op::do_complete)
  {
  }

  static bool do_perform(reactor_op*)
  {
    signal_state* state = get_signal_state();

    int fd = state->read_descriptor_;
    int signal_number = 0;
    while (::read(fd, &signal_number, sizeof(int)) == sizeof(int))
      if (signal_number >= 0 && signal_number < max_signal_number)
        signal_set_service::deliver_signal(signal_number);

    return false;
  }

  static void do_complete(io_service_impl* /*owner*/, operation* base,
      boost::system::error_code /*ec*/, std::size_t /*bytes_transferred*/)
  {
    pipe_read_op* o(static_cast<pipe_read_op*>(base));
    delete o;
  }
};
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)

void signal_set_service::add_service(signal_set_service* service)
{
  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
  // If this is the first service to be created, open a new pipe.
  if (state->service_list_ == 0)
  {
    int pipe_fds[2];
    if (::pipe(pipe_fds) == 0)
    {
      state->read_descriptor_ = pipe_fds[0];
      ::fcntl(state->read_descriptor_, F_SETFL, O_NONBLOCK);

      state->write_descriptor_ = pipe_fds[1];
      ::fcntl(state->write_descriptor_, F_SETFL, O_NONBLOCK);

#if defined(FD_CLOEXEC)
      ::fcntl(state->read_descriptor_, F_SETFD, FD_CLOEXEC);
      ::fcntl(state->write_descriptor_, F_SETFD, FD_CLOEXEC);
#endif // defined(FD_CLOEXEC)
    }
    else
    {
      boost::system::error_code ec(errno,
          boost::asio::error::get_system_category());
      boost::asio::detail::throw_error(ec, "signal_set_service pipe");
    }
  }
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)

  // Insert service into linked list of all services.
  service->next_ = state->service_list_;
  service->prev_ = 0;
  if (state->service_list_)
    state->service_list_->prev_ = service;
  state->service_list_ = service;

#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
  // Register for pipe readiness notifications.
  service->reactor_.register_internal_descriptor(reactor::read_op,
      state->read_descriptor_, service->reactor_data_, new pipe_read_op);
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
}

void signal_set_service::remove_service(signal_set_service* service)
{
  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  if (service->next_ || service->prev_ || state->service_list_ == service)
  {
#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
    // Disable the pipe readiness notifications.
    service->reactor_.deregister_descriptor(
        state->read_descriptor_, service->reactor_data_, false);
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)

    // Remove service from linked list of all services.
    if (state->service_list_ == service)
      state->service_list_ = service->next_;
    if (service->prev_)
      service->prev_->next_ = service->next_;
    if (service->next_)
      service->next_->prev_= service->prev_;
    service->next_ = 0;
    service->prev_ = 0;

#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
    // If this is the last service to be removed, close the pipe.
    if (state->service_list_ == 0)
    {
      ::close(state->read_descriptor_);
      state->read_descriptor_ = -1;
      ::close(state->write_descriptor_);
      state->write_descriptor_ = -1;
    }
#endif // !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
  }
}

void signal_set_service::start_wait_op(
    signal_set_service::implementation_type& impl, signal_op* op)
{
  io_service_.work_started();

  signal_state* state = get_signal_state();
  static_mutex::scoped_lock lock(state->mutex_);

  registration* reg = impl.signals_;
  while (reg)
  {
    if (reg->undelivered_ > 0)
    {
      --reg->undelivered_;
      io_service_.post_deferred_completion(op);
      return;
    }

    reg = reg->next_in_set_;
  }

  impl.queue_.push(op);
}

} // namespace detail
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_IMPL_SIGNAL_SET_SERVICE_IPP
