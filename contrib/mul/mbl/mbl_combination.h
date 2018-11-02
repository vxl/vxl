#ifndef mbl_combination_h
#define mbl_combination_h
//:
// \file
// \author Tim Cootes
// \brief Functions to generate combinations of integers

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Generate first combination (n.size() zeros)
inline
std::vector<unsigned> mbl_combination_begin(const std::vector<unsigned>& n)
{
  std::vector<unsigned> x(n.size());
  std::fill(x.begin(),x.end(),0);
  return x;
}

//: Moves x to next combination of integers.
//  x[i] is a positive integer base n[i]
//
//  This allows stepping through all possible combinations of sets integers {x[i]}
//  using
// \code
//  std::vector<unsigned> x = mbl_combination_begin(n);
//  do { ...}  while (mbl_combination_next(x,n));
// \endcode
inline
bool mbl_combination_next(std::vector<unsigned>& x, const std::vector<unsigned>& n)
{
  assert(n.size()==x.size());
  for (unsigned i=0;i<n.size();++i)
  {
    x[i]=(x[i]+1)%n[i];
    if (x[i]>0) return true;
  }
  return false;
}

//: Number of possible combinations of n.size() values, drawing one from each of n[i] elements.
inline
unsigned mbl_n_combinations(const std::vector<unsigned>& n)
{
  unsigned nc = 1;
  for (unsigned int i : n) if (i>1) nc*=i;
  return nc;
}

//: Print combination as x3:x2:x1:x0
inline
void mbl_print_combination(std::ostream& os, const std::vector<unsigned>& x)
{
  for (int i=x.size()-1;i>0;--i) os<<x[i]<<':';
  os<<x[0];
}

#endif // mbl_combination_h
