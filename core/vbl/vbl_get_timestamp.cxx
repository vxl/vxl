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
#include <unistd.h>
#endif

#include <vcl/vcl_ctime.h>
#include <vcl/vcl_sys/time.h>

#ifndef WIN32
// POSIX
void vbl_get_timestamp(int &secs, int &msecs)
{
  struct timeval  timestamp;
  struct timezone dummy;
  gettimeofday(&timestamp, &dummy);

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
