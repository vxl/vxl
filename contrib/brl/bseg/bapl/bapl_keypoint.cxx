// This is brl/bseg/bapl/bapl_keypoint.cxx
//:
// \file

#include <iostream>
#include "bapl_keypoint.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Print a summary of the keypoint data to a stream
std::ostream& operator<< (std::ostream& os, bapl_keypoint const & k)
{
  k.print_summary(os);
  return os;
}
