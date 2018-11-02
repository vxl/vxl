// This is oul/oufgl/asyncio.cxx
#include <iostream>
#include <cerrno>
#include "asyncio.h"
//:
// \file
#include <vcl_sys/types.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Initialise shared state to "no operation in progress"

volatile sig_atomic_t AsyncIO_Shared_State::complete = 1;

// Set complete flag when completion signal arrives. Currently ignores
// the signal number (see the warning at the start of the header)

void AsyncIO_Shared_State::signal_handler(int)
{
  complete = 1;
}

//: Constructor - perform I/O on file descriptor fd, using sig as completion signal.
// Note that SIGUSR1,2 may be used by the linuxthreads library.

AsyncIO::AsyncIO(int fd, int sig)
{
  cb.aio_fildes = fd;
  cb.aio_reqprio = 0;
  cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  cb.aio_sigevent.sigev_signo = sig;
  cb.aio_lio_opcode = LIO_NOP;
  // Remaining cb fields are filled in by read() or write()
  signal(sig, &AsyncIO_Shared_State::signal_handler);
}

//: Destructor - disconnect signal handler

AsyncIO::~AsyncIO()
{
  signal(cb.aio_sigevent.sigev_signo, SIG_DFL);
}

//: Begin reading n bytes into buf starting at current file position

int AsyncIO::read(volatile void *buf, std::size_t n)
{
  // Only one op allowed at a time. Note that aio_read() itself never
  // returns EBUSY.

  if (!complete)
    return EBUSY;

  off_t pos = lseek(cb.aio_fildes, 0, SEEK_CUR);
  if (pos == -1)
    return errno;
  else
    return read(buf, n, pos);
}

//: Begin reading n bytes into buf starting at absolute file position pos

int AsyncIO::read(volatile void *buf, std::size_t n, off_t pos)
{
  // Only one op allowed at a time. Note that aio_read() itself never
  // returns EBUSY.

  if (!complete)
    return EBUSY;

  complete = 0;
  cb.aio_buf = buf;
  cb.aio_nbytes = n;
  cb.aio_offset = pos;

  // Initiate read - mark as complete if initiation fails

  if (aio_read(&cb) != 0)
  {
    complete = 1;
    return errno;
  }
  else
    return 0;
}

//: Begin writing n bytes from buf starting at current file position

int AsyncIO::write(volatile void *buf, std::size_t n)
{
  // Only one op allowed at a time. Note that aio_write() itself never
  // returns EBUSY.

  if (!complete)
    return EBUSY;

  off_t pos = lseek(cb.aio_fildes, 0, SEEK_CUR);
  if (pos == -1)
    return errno;
  else
    return write(buf, n, pos);
}

//: Begin writing n bytes from buf starting at absolute file position pos

int AsyncIO::write(volatile void *buf, std::size_t n, off_t pos)
{
  // Only one op allowed at a time. Note that aio_write() itself never
  // returns EBUSY.

  if (!complete)
    return EBUSY;

  complete = 0;
  cb.aio_buf = buf;
  cb.aio_nbytes = n;
  cb.aio_offset = pos;

  // Initiate write - mark as complete if initiation fails

  if (aio_write(&cb) != 0)
  {
    complete = 1;
    return errno;
  }
  else
    return 0;
}

//: Wait for I/O to complete, then return status.
// If suspend is true (the default), block the calling process while waiting,
// otherwise continuously poll for completion
// (not recommended, but may be more reliable).

int AsyncIO::wait_for_completion(bool suspend)
{
  int status;

  if (!complete)
  {
    if (suspend)
    {
      const struct aiocb *aio_list[1] = { &cb };
      // Delivery of the completion signal might cause EINTR, so wait for
      // either success or some other status
      while (aio_suspend(aio_list, 1, NULL) == -1)
        if (errno != EINTR)
          return errno;
    }
    else
      while (aio_error(&cb) == EINPROGRESS)
        /* wait */ ;
    assert(complete);      // Just in case...
  }
  if ((status = aio_error(&cb)) != 0)
    return status;
  else if (aio_return(&cb) == -1)
    return errno;
  else
    return 0;
}
