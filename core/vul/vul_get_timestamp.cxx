// This is core/vul/vul_get_timestamp.cxx
//:
// \file
// \author fsm

#include <ctime>
#include <sstream>
#include <iomanip>
#include "vul_get_timestamp.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#else
#include <unistd.h> // for struct timeval
#endif

#include <vcl_sys/time.h> // for gettimeofday()

// for vul_get_time_string()
#include <vul/vul_string.h>
//

#if !defined(_WIN32) || defined(__CYGWIN__)
// POSIX
void vul_get_timestamp(int &secs, int &msecs)
{
  struct timeval  timestamp;
  struct timezone* dummy = nullptr;
  gettimeofday(&timestamp, dummy);

  secs = timestamp.tv_sec;
  msecs = timestamp.tv_usec/1000;
}
#else
// _WIN32 and not __CYGWIN__
void vul_get_timestamp(int &secs, int &msecs)
{
  struct _timeb real;
  _ftime(&real);

  secs = static_cast<int>(real.time);
  msecs = real.millitm;
}
#endif


// Get the present time and date as a string, e.g. "Fri Dec 8 14:54:17 2006"
std::string vul_get_time_as_string(vul_time_style style/*default=vul_asc*/)
{
  std::string timestr;

  // Get time in seconds since Jan 1 1970
  std::time_t time_secs;
  std::time(&time_secs);

  // Convert time to struct tm form
  struct std::tm *time;
  time = std::localtime(&time_secs);

  switch (style)
  {
    case vul_numeric_msf:
    {
      // Express as a series of space-separated numbers, most significant first
      // e.g. yyyy mm dd hh mm ss
      // NB Month, day start at 1. Hour, minute, second start at 0.
      // Leading zeros are used for single-digit month,day,hour,min,sec.
      std::ostringstream oss;
      oss.fill('0');
      oss << std::setw(4) << 1900+time->tm_year << ' '
          << std::setw(2) << 1 + time->tm_mon << ' '
          << std::setw(2) << time->tm_mday << ' '
          << std::setw(2) << time->tm_hour << ' '
          << std::setw(2) << time->tm_min << ' '
          << std::setw(2) << time->tm_sec;
      timestr = oss.str();
    }
    break;

    default:
    {
      // Get local time & date using standard asctime() function,
      // Removes the trailing eol that asctime() inserts.
      timestr = std::asctime(time);
      vul_string_right_trim(timestr, "\n");
    }
    break;
  }

  return timestr;
}
