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
// .NAME vbl_timer - A timing facility for C++.
// .LIBRARY vbl
// .HEADER System Interface Classes
// .INCLUDE vbl/vbl_timer.h
// .FILE vbl/vbl_timer.C
// .FILE vbl/vbl_timer.h
// .EXAMPLE examples/ex_Timer.C
//
//
// .SECTION Description
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

#ifndef vbl_timerh
#define vbl_timerh

#ifndef WIN32
#include <sys/times.h>                // system/user times with times()
#include <sys/time.h>                // real time through ftime() system call
#else
#include <time.h>
#include <sys/timeb.h>
#endif

#include <vcl/vcl_iosfwd.h>

class vbl_timer {
public:
  vbl_timer () {mark();}	// constructor & mark
  
  void mark ();			// mark timer
  long real ();			// real        time (ms) since last Mark
  
  long user ();			// user        time (ms) since last Mark
  long system ();		// system      time (ms) since last Mark
  long all ();			// user+system time (ms) since last Mark

  void print(ostream& s);
  
private:

#ifndef WIN32
  tms usage0;			          // usage mark. 
  struct timeval real0;			// wall clock mark.
#else
 clock_t usage0;
 struct _timeb real0;
#endif
};

#endif
