// This is core/vul/vul_get_timestamp.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vul_get_timestamp.h"

#include <vcl_compiler.h>

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#include <sys/timeb.h>
#else
#include <unistd.h> // for struct timeval
#ifdef __CYGWIN__
#include <sys/time.h>
#endif
#endif

//#include <vcl_ctime.h> // for struct timezone
#include <vcl_sys/time.h> // for gettimeofday()

#if !defined(VCL_WIN32) || defined(__CYGWIN__)
// POSIX
void vul_get_timestamp(int &secs, int &msecs)
{
  struct timeval  timestamp;
  struct timezone* dummy = 0;
  gettimeofday(&timestamp, dummy);

  secs = timestamp.tv_sec;
  msecs = timestamp.tv_usec/1000;
}
#elif defined(VCL_WIN32) && defined(VCL_BORLAND)
// VCL_WIN32 and not __CYGWIN__ and VCL_BORLAND
void vul_get_timestamp(int &secs, int &msecs)
{
  struct timeb real;
  ftime(&real);

  secs = real.time;
  msecs = real.millitm;
}
#else
// VCL_WIN32 and not __CYGWIN__ and not VCL_BORLAND
void vul_get_timestamp(int &secs, int &msecs)
{
  struct _timeb real;
  _ftime(&real);

  secs = real.time;
  msecs = real.millitm;
}
#endif
