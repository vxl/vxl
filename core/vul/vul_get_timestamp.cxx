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

// for vul_get_time_string()
#include <vcl_ctime.h>
#include <vul/vul_string.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
//


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


// Get the present time and date as a string, e.g. "Fri Dec 8 14:54:17 2006"
vcl_string vul_get_time_as_string(vul_time_style style/*default=vul_asc*/)
{
  vcl_string timestr;

  // Get time in seconds since Jan 1 1970
  vcl_time_t time_secs;
  vcl_time(&time_secs);

  // Convert time to struct tm form
  struct vcl_tm *time;
  time = vcl_localtime(&time_secs);

  switch (style)
  {
    case vul_numeric_msf:
    {
      // Express as a series of space-separated numbers, most significant first
      // e.g. yyyy mm dd hh mm ss
      // NB Month, day start at 1. Hour, minute, second start at 0.
      // Leading zeros are used for single-digit month,day,hour,min,sec.
      vcl_ostringstream oss;
      oss.fill('0');
      oss << vcl_setw(4) << 1900+time->tm_year << ' '
          << vcl_setw(2) << 1 + time->tm_mon << ' '
          << vcl_setw(2) << time->tm_mday << ' '
          << vcl_setw(2) << time->tm_hour << ' '
          << vcl_setw(2) << time->tm_min << ' '
          << vcl_setw(2) << time->tm_sec;
      timestr = oss.str();
    }
    break;

    default:
    {
      // Get local time & date using standard asctime() function,
      // Removes the trailing eol that asctime() inserts.
      timestr = vcl_asctime(time);
      vul_string_right_trim(timestr, "\n");
    }
    break;
  }

  return timestr;
}

