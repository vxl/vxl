#ifndef vcl_sys_time_h_
#define vcl_sys_time_h_

#include <vcl/vcl_compiler.h>

#if defined(VCL_WIN32)
#include <sys/timeb.h>
extern "C" int gettimeofday(struct timeval*, struct timezone*);
#else
#if !defined(VCL_NO_SYS_TIME_H)
#include <sys/time.h>
#else
#ifdef SYSV
extern "C" int gettimeofday(struct timeval *tp);
#else
extern "C" int gettimeofday(struct timeval*, struct timezone*);
#endif
#endif
#endif

//struct timeval:
// time_t         tv_sec      seconds
// suseconds_t    tv_usec     microseconds

//struct itimerval:
// struct timeval it_interval timer interval
// struct timeval it_value    current value

// int   getitimer(int, struct itimerval *);
// int   setitimer(int, const struct itimerval *, struct itimerval *);
// int   gettimeofday(struct timeval *, void *);
// int   select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
// int   utimes(const char *, const struct timeval [2]);

#endif
