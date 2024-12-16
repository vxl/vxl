// This is core/vbl/vbl_sort.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   19 Nov 97
//
//-----------------------------------------------------------------------------

#include "vbl_sort.h"

//: Predicate that will sort doubles in ascending order.
int
vbl_sort_double_ascending(const double & a, const double & b)
{
  if (a < b)
    return -1;

  if (a == b)
    return 0;

  return 1;
}

//: Predicate that will sort in descending order.
int
vbl_sort_double_descending(const double & a, const double & b)
{
  if (a < b)
    return 1;

  if (a == b)
    return 0;

  return -1;
}

//: Ascending integers.
int
vbl_sort_int_ascending(const int & a, const int & b)
{
  if (a < b)
    return -1;

  if (a == b)
    return 0;

  return 1;
}

//: Descending integers.
int
vbl_sort_int_descending(const int & a, const int & b)
{
  if (a < b)
    return 1;

  if (a == b)
    return 0;

  return -1;
}

VBL_SORT_INSTANTIATE(int);
VBL_SORT_INSTANTIATE(double);
