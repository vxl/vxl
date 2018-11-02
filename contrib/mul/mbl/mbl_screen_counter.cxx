// This is mul/mbl/mbl_screen_counter.cxx
#include <iostream>
#include "mbl_screen_counter.h"
//:
// \file
// \brief Printing dots to the screen to indicate events
// \author Ian Scott

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

mbl_screen_counter::mbl_screen_counter(std::ostream & os):
count_(0), symbol_('a'), skip_(1), os_(os)
{
}

//: Mark event
// This is a postfix increment operator
mbl_screen_counter mbl_screen_counter::operator++ (int)
{
  count_++;
  if (count_ % skip_ == 0) os_ << symbol_ << std::flush;

  if (count_ / skip_ == 100)
  {
    skip_ *= 10;
    symbol_++;
    os_ << "\nReached " << count_ << ". Each '" << symbol_ << "' = " << skip_ << ":";
  }
  return *this;
}
