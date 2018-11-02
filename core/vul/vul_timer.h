// This is core/vul/vul_timer.h
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
#ifndef vul_timer_h
#define vul_timer_h
//:
// \file
// \brief A timing facility for C++
// \author This code was originally written by Joe Rahmeh at UT Austin.
//
// The vul_timer class provides an interface to system timing.
// It allows a C++ program to record the time between  a  reference
// point (mark) and now. This class uses the system
// time(2) interface to provide time resolution at either
// millisecond  or microsecond granularity, depending upon
// operating system support and features. Since the time duration  is
// stored  in  a  32-bit  word,  the maximum time period before
// rollover occurs is about 71 minutes.
//
// Due to operating system dependencies, the  accuracy  of  all
// member  function results may not be as documented. For  example,
// some operating  systems  do  not  support  timers  with
// microsecond  resolution. In those cases, the values returned
// are provided to the nearest millisecond  or  other  unit  of
// time  as  appropriate. See the Timer header file for system
// specific notes.
//
// The Timer class provides timing code for performance evaluation.
//  - User time:   time cpu spends in user mode on behalf of the program.
//  - System time: time cpu spends in system mode on behalf of the program.
//  - Real time:   what you get from a stop watch timer.
//
// \verbatim
//  Modifications
//   Created: BMK 07/14/89  Initial design and implementation.
//   Updated: LGO 09/23/89  Conform to COOL coding style.
//   Updated: AFM 12/31/89  OS/2 port.
//   Updated: DLS 03/22/91  New lite version.
//   Updated: VDN 10/14/93  ANSI C does not have user/system time.
//   Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim

//: struct containing timer data
struct vul_timer_data;

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: The Timer class provides timing code for performance evaluation.
class vul_timer
{
  //: struct containing timer data
  vul_timer_data *data;
 public:
  //: construct and reset counter to now.
  vul_timer();
  ~vul_timer();
  //: Reset the counted to now
  void mark();
  //: Real        time (ms) since last mark
  long real();
  //: User        time (ms) since last mark
  long user();
  //: System      time (ms) since last mark
  long system();
  //: User+system time (ms) since last mark
  long all();

  //: Display user and real time since the last mark.
  void print(std::ostream& s);

 private:
  // disallow assigning to objects of this class:
  vul_timer(vul_timer const &) { }
  vul_timer& operator=(vul_timer const &) { return *this; }
};

#endif // vul_timer_h
