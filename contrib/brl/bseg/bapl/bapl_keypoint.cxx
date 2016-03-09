// This is brl/bseg/bapl/bapl_keypoint.cxx
//:
// \file

#include "bapl_keypoint.h"
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

//: Print a summary of the keypoint data to a stream
std::ostream& operator<< (std::ostream& os, bapl_keypoint const & k)
{
  k.print_summary(os);
  return os;
}

