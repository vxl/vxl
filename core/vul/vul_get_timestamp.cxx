// This is vxl/vul/vul_get_timestamp.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vul_get_timestamp.h"

#include <vcl_compiler.h>

#ifdef VCL_WIN32
#include <direct.h>
#include <sys/timeb.h>
#else
#include <unistd.h> // for struct timeval
#endif

//#include <vcl_ctime.h> // for struct timezone
#include <vcl_sys/time.h> // for gettimeofday()

#ifndef VCL_WIN32
// POSIX
void vul_get_timestamp(int &secs, int &msecs)
{
  struct timeval  timestamp;
  struct timezone* dummy = 0;
  gettimeofday(&timestamp, dummy);

  secs = timestamp.tv_sec;
  msecs = timestamp.tv_usec/1000;
}
#else
// VCL_WIN32
void vul_get_timestamp(int &secs, int &msecs)
{
  struct _timeb real;
  _ftime(&real);

  secs = real.time;
  msecs = real.millitm;
}
#endif
