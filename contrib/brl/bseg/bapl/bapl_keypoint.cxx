// This is algo/bapl/bapl_keypoint.cxx
//:
// \file

#include "bapl_keypoint.h"
#include <vcl_iostream.h>

//: Print a summary of the keypoint data to a stream
vcl_ostream& operator<< (vcl_ostream& os, bapl_keypoint const & k)
{
  k.print_summary(os);
  return os;
}



