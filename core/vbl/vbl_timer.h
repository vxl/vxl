#ifndef vbl_timer_h
#define vbl_timer_h
// This is vxl/vbl/vbl_timer.h
//
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
//:
// \file
// \brief vbl_timer - A timing facility for C++.
//
// \verbatim
// Modifications:
// Created: BMK 07/14/89  Initial design and implementation.
// Updated: LGO 09/23/89  Conform to COOL coding style.
// Updated: AFM 12/31/89  OS/2 port.
// Updated: DLS 03/22/91  New lite version.
// Updated: VDN 10/14/93  ANSI C does not have user/system time.
// \endverbatim
//
// Description
// The Timer class and provides an interface to system timing.
// It allows a C++ program to record the time between  a  reference
// point (mark) and now. This class uses the system
// time(2) interface to provide time resolution at either mil-
// lisecond  or microsecond granularity, depending upon operat-
// ing system support and features. Since the time duration  is
// stored  in  a  32-bit  word,  the maximum time period before
// rollover occurs is about 71 minutes.
//
// Due to operating system dependencies, the  accuracy  of  all
// member  function results may not be as documented. For exam-
// ple, some operating  systems  do  not  support  timers  with
// microsecond  resolution. In those cases, the values returned
// are provided to the nearest millisecond  or  other  unit  of
// time  as  appropriate. See the Timer header file for system-
// specific notes.
//
// The Timer class provides timing code  for performance evaluation.
// This code
// was originally written by Joe Rahmeh at UT Austin.
// \verbatim
//  User time:
//    time cpu spends in user mode on behalf of the program.
//  System time:
//    time cpu spends in system mode on behalf of the program.
//  Real time:
//    what you get from a stop watch timer.
// \endverbatim

//: struct containing timer data
struct vbl_timer_data;

#include <vcl_iosfwd.h>

//: The Timer class provides timing code  for performance evaluation.
class vbl_timer {
public:
  //: construct and mark
  vbl_timer();
  ~vbl_timer();
  //: mark timer
  void mark();
  //: real        time (ms) since last mark
  long real();
  //: user        time (ms) since last mark
  long user();
  //: system      time (ms) since last mark
  long system();
  //: user+system time (ms) since last mark
  long all();

  //: Display user and real time since the last mark.
  void print(vcl_ostream& s);

private:
  //: struct containing timer data
  vbl_timer_data *data;
  //: disallow.
  vbl_timer(vbl_timer const &) { }
  //: NYI
  void operator=(vbl_timer const &) { }
};

#endif // vbl_timer_h
