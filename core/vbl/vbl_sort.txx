//-*- c++ -*-------------------------------------------------------------------
//
// Class: vbl_sort
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 19 Nov 97
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_sort.h>

// -- Predicate that will sort doubles in ascending order.
int vbl_sort::double_ascending(double const& a, double const& b)
{
  if (a < b)
    return -1;
   
  if (a == b)
    return 0;
  
  return 1;
}

// -- Predicate that will sort in descending order.
int vbl_sort::double_descending(const double& a, const double& b)
{
  if (a < b)
    return 1;
  
  if (a == b)
    return 0;
  
  return -1;
}

// -- Ascending integers.
int vbl_sort::int_ascending(int const& a, int const& b)
{
  if (a < b)
    return -1;
   
  if (a == b)
    return 0;
  
  return 1;
}

// -- Descending integers.
int vbl_sort::int_descending(const int& a, const int& b)
{
  if (a < b)
    return 1;
  
  if (a == b)
    return 0;
  
  return -1;
}


#define VBL_SORT_INSTANTIATE(T)\
template struct vbl_sort_helper<T>;\
template void vbl_qsort_ascending(T*,int);\
template void vbl_qsort_descending(T*,int)


VBL_SORT_INSTANTIATE(int);
VBL_SORT_INSTANTIATE(double);
