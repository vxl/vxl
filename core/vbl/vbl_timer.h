#ifndef vbl_timer_h
#define vbl_timer_h
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
// .NAME vbl_timer - A timing facility for C++
// .LIBRARY vbl
// .HEADER vxl package
// .INCLUDE vbl/vbl_timer.h
// .FILE vbl_timer.cxx
// .EXAMPLE examples/vbl_timer.cxx
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

struct vbl_timer_data;

#include <vcl/vcl_iosfwd.h>

class vbl_timer {
public:
  vbl_timer();   // construct and mark
  ~vbl_timer();

  void mark();   // mark timer

  long real();   // real        time (ms) since last mark
  long user();   // user        time (ms) since last mark
  long system(); // system      time (ms) since last mark
  long all();    // user+system time (ms) since last mark

  void print(ostream& s);

private:
  vbl_timer_data *data;
  // disallow.
  vbl_timer(vbl_timer const &) { }
  void operator=(vbl_timer const &) { }
};

#endif // vbl_timer_h
