// This is core/vul/vul_timer.cxx
#include <ctime>
#include <iostream>
#include "vul_timer.h"
//:
// \file
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// Created: BMK 07/14/89  Initial design and implementation
// Updated: LGO 09/23/89  Conform to COOL coding style
// Updated: AFM 12/31/89  OS/2 port
// Updated: DLS 03/22/91  New lite version
// Updated: VDN 10/14/93  ANSI C does not have user/system time.
//
// The Timer class provides timing code  for performance evaluation.  This code
// was originally written by Joe Rahmeh at UT Austin.
//
//  User time:
//    time cpu spends in user mode on behalf of the program.
//  System time:
//    time cpu spends in system mode on behalf of the program.
//  Real time:
//    what you get from a stop watch timer.
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vcl_sys/time.h>
# undef __USE_BSD

struct vul_timer_data
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  tms usage0;                    // usage mark.
  struct timeval real0;          // wall clock mark.
#else
 std::clock_t usage0;
 struct _timeb real0;
#endif
};

// for CLK_TCK


//#define CLK_TCK _sysconf(3) in <climits> has error

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h> // for sysconf()
#else
#include <unistd.h>
#endif
#undef CLK_TCK
#define CLK_TCK sysconf(_SC_CLK_TCK)

vul_timer::vul_timer()
  : data(new vul_timer_data)
{
  mark();
}

vul_timer::~vul_timer()
{
  delete data;
  data = nullptr;
}

//: Sets the reference time to now.

void vul_timer::mark()
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  times(&data->usage0);  // user/system time
#ifndef SYSV
  struct timezone tz;
  gettimeofday(&data->real0, &tz);  // wall clock time
#else
#if VXL_TWO_ARG_GETTIME
  gettimeofday(&data->real0, (struct timezone*)0);
#else
  gettimeofday(&data->real0);
#endif
#endif
#else
  // Win32 section
  data->usage0 = std::clock();
  _ftime(&data->real0);
#endif
}

//: Returns the number of milliseconds of wall clock time, since last mark().

long vul_timer::real()
{
  long s;

#if !defined(_WIN32) || defined(__CYGWIN__)
  struct timeval  real_time;    // new real time
#ifndef SYSV
  struct timezone tz;
  gettimeofday(&real_time, &tz);  // wall clock time
#else
#if VXL_TWO_ARG_GETTIME
  gettimeofday(&real_time, (struct timezone*)0);
#else
  gettimeofday(&real_time);
#endif
#endif
  s  = real_time.tv_sec    - data->real0.tv_sec;
  long us = real_time.tv_usec - data->real0.tv_usec;

  if (us < 0) { us += 1000000; --s; }
  return long(1000.0*s + us / 1000.0 + 0.5);

#else
  // Win32 section
  struct _timeb real_time;
  _ftime(&real_time);
  s = long(real_time.time - data->real0.time);
  long ms = real_time.millitm - data->real0.millitm;

  if (ms < 0) { ms += 1000; --s; }
  return 1000*s + ms;
#endif
}


long vul_timer::user()
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  tms usage;
  times(&usage);  // new user/system time
  return (usage.tms_utime - data->usage0.tms_utime) * 1000 / CLK_TCK;
#else
  std::clock_t usage = std::clock();
  return (usage - data->usage0) / (CLOCKS_PER_SEC/1000);
#endif
}

//: Returns the number of milliseconds spent in user-process or operating system respectively, since last mark().

long vul_timer::system()
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  tms usage;
  times(&usage);  // new user/system time
  return (usage.tms_stime - data->usage0.tms_stime) * 1000 / CLK_TCK;
#else
  return 0L;
#endif
}

// Returns the number of milliseconds spent in user-process AND
// operating system, since last mark().

long vul_timer::all()
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  tms usage;
  times(&usage);  // new user/system time
  return (usage.tms_utime + usage.tms_stime -
          data->usage0.tms_utime - data->usage0.tms_stime)  * 1000 / CLK_TCK;
#else
  std::clock_t usage = std::clock();
  return (usage - data->usage0) / (CLOCKS_PER_SEC/1000);
#endif
}

//: Display user and real time since the last mark.
void vul_timer::print(std::ostream& s)
{
  s << "Time: user " << user() / 1000.0 << ", real " << this->real() / 1000.0 << std::endl;
}
