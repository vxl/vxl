// This is core/vpl/vpl_mutex.h
#ifndef vpl_mutex_h_
#define vpl_mutex_h_
//:
// \file
// \author fsm, Oxford RRG
// \date   08 Dec 2001
//
// \verbatim
//  Modifications
//   08 Dec 2001 first version.
// \endverbatim

#include <vxl_config.h>

#if VXL_HAS_PTHREAD_H
# include <pthread.h>
# include <vcl_cerrno.h> // for EBUSY, I think
struct vpl_mutex
{
  vpl_mutex() { pthread_mutex_init(&mutex_, 0); }

  void lock() { pthread_mutex_lock(&mutex_); }

  //: returns `true' if lock was acquired.
  bool trylock() { return pthread_mutex_trylock(&mutex_) != EBUSY; }

  void unlock() { pthread_mutex_unlock(&mutex_); }

  ~vpl_mutex() { pthread_mutex_destroy(&mutex_); }

 private:
  pthread_mutex_t mutex_;

  // disallow assignment.
  vpl_mutex(vpl_mutex const &) { }
  void operator=(vpl_mutex const &) { }
};

#else
# error "only works with pthreads for now"
#endif

#endif // vpl_mutex_h_
