/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vbl_get_timestamp.h"

#ifdef WIN32
#include <direct.h>
#include <sys/timeb.h>
#else
#include <vcl/vcl_unistd.h> // for struct timeval
#endif

//#include <vcl/vcl_ctime.h> // for struct timezone
#include <vcl/vcl_sys/time.h> // for gettimeofday()

#ifndef WIN32
// POSIX
void vbl_get_timestamp(int &secs, int &msecs)
{
  struct timeval  timestamp;
  void* dummy = 0;
  gettimeofday(&timestamp, dummy);

  secs = timestamp.tv_sec;
  msecs = timestamp.tv_usec/1000;
}
#else
// WIN32
void vbl_get_timestamp(int &secs, int &msecs)
{
  struct _timeb real;
  _ftime(&real);

  secs = real.time;
  msecs = real.millitm;
}
#endif
