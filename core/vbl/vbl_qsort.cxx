//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vbl_qsort.h"
#endif
//
// Class: vbl_qsort
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 19 Nov 97
// Modifications:
//   971119 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vbl_qsort.h"

//: Predicate that will sort doubles in ascending order.
int vbl_qsort_double_ascending(double const& a, double const& b)
{
  if (a < b)
    return -1;
   
  if (a == b)
    return 0;
  
  return 1;
}

//: Predicate that will sort in descending order.
int vbl_qsort_double_descending(const double& a, const double& b)
{
  if (a < b)
    return 1;
  
  if (a == b)
    return 0;
  
  return -1;
}

//: Ascending integers.
int vbl_qsort_int_ascending(int const& a, int const& b)
{
  if (a < b)
    return -1;
   
  if (a == b)
    return 0;
  
  return 1;
}

//: Descending integers.
int vbl_qsort_int_descending(const int& a, const int& b)
{
  if (a < b)
    return 1;
  
  if (a == b)
    return 0;
  
  return -1;
}

VBL_QSORT_INSTANTIATE(int);
VBL_QSORT_INSTANTIATE(double);
