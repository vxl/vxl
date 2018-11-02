#ifndef mbl_screen_counter_h_
#define mbl_screen_counter_h_

//:
// \file
// \brief Printing dots to the screen to indicate events
// \author Ian Scott

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Prints items to the screen to represent events.
// \code
// // A rather trivial example
// mbl_screen_counter counter;
//
// int n = 10;
// for (int i=0;i<1000;i++)
// {
//   counter(std::cout);
// }
// \endcode

class mbl_screen_counter
{
  unsigned long count_;
  char symbol_;
  unsigned long skip_;
  std::ostream & os_;
public:
  mbl_screen_counter(std::ostream &) ;

//: Mark event
// This is a postfix increment operator
  mbl_screen_counter operator++ (int);
};

#endif // mbl_screen_counter_h_
