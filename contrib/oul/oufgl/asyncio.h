// This is oul/oufgl/asyncio.h
#ifndef OTAGO_Asyncio_IO__h_INCLUDED
#define OTAGO_Asyncio_IO__h_INCLUDED
//:
// \file
// \brief A simple wrapper around POSIX.1b asynchronous I/O
//
// Copyright (c) 2001 Simon Brady
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------
//
//: Asynchronous I/O wrapper
// Simple wrapper around POSIX.1b asynchronous I/O (as documented in GNU Info
// under Libc -> Low-Level I/O -> Asynchronous I/O). This should be portable
// across most Unix-like systems. Win32 has its own async I/O facilities which
// are similar to these (see Jeffrey Richter's "Advanced Windows" for details).
//
// An AsyncIO object represents I/O on a particular file descriptor. The read()
// and write() methods attempt to initiate I/O, returning error status if
// unsuccessful. If they succeed then I/O is underway and you can't touch the
// buffer until completion. You can either poll by calling is_complete(), or
// (better) do everything else you need to do then call wait_for_completion()
// which will block in a multiprocessing-friendly way. Even if is_complete()
// has returned true you must still call wait_for_completion() to determine
// the completion status of the operation.
//
// WARNING: Although you can instantiate multiple AsyncIO objects, only one
// I/O op can be in progress at once within a process. This is because we get
// notified of completion by a signal, whose handler has to be a non-member
// function. The handler could be extended to inspect a list of (signal,
// request) pairs to determine which object's request has completed, but I
// haven't done this since protecting such a list from race conditions is
// probably non-trivial and I don't need the capability. Note the use of
// class AsyncIO_Shared_State to clarify the distinction between shared
// and per-object state.
//
// \verbatim
//  Modifications
//   06-Nov-00  SJB  Initial version
//   13-Dec-00  SJB  Use SIGIO as default completion signal
//                   Fix up wait_for_completion, add suspend flag
// \endverbatim
// Status: Complete
// \author Simon Brady
//----------------------------------------------------------------------

#include <iostream>
#include <csignal>
#include <cstddef>
#include <unistd.h>
#ifndef _POSIX_ASYNCHRONOUS_IO
#error Your system does not support POSIX asynchronous I/O
#endif
#include <aio.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class AsyncIO_Shared_State
{
 protected:
  static volatile std::sig_atomic_t complete;   // 0 - in progress, 1 - complete

  // Set complete flag when completion signal arrives. Currently ignores
  // the signal number (see the warning at the start of this header)
  static void signal_handler(int);
};

class AsyncIO : protected AsyncIO_Shared_State
{
  struct aiocb cb;                         // Control block

 public:

  // Constructor - perform I/O on file descriptor fd, using sig as completion
  // signal. Note that SIGUSR1,2 may be used by the linuxthreads library.
  AsyncIO(int fd, int sig = SIGIO);

  // Destructor - disconnect signal handler
 ~AsyncIO();

  // NB: All methods returning int return zero on success, or an errno value
  // on failure

  // Begin reading n bytes into buf starting at current file position
  int read(volatile void *buf, std::size_t n);

  // Begin reading n bytes into buf starting at absolute file position pos
  int read(volatile void *buf, std::size_t n, off_t pos);

  // Begin writing n bytes from buf starting at current file position
  int write(volatile void *buf, std::size_t nbytes);

  // Begin writing n bytes from buf starting at absolute file position pos
  int write(volatile void *buf, std::size_t nbytes, off_t pos);

  // Wait for I/O to complete, then return status. If suspend is true, block
  // the calling process while waiting, otherwise continuously poll for
  // completion (not recommended, but may be more reliable).
  int wait_for_completion(bool suspend = true);

  // Test if operation complete. You still need to call wait_for_completion()
  // to tidy up and get final status
  bool is_complete() const { return complete; }
};

#endif // OTAGO_Asyncio_IO__h_INCLUDED
