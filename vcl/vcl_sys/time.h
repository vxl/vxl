#ifndef vcl_sys_time_h_
#define vcl_sys_time_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <sys/time.h>

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
