#ifndef mbl_combination_h
#define mbl_combination_h

/:
// \file
// \author Tim Cootes
// \brief Functions to generate combinations of integers

#include <vcl_cassert.h>
#include <vcl_vector.h>

//: Generate first combination (n.size() zeros)
inline
vcl_vector<unsigned> mbl_combination_begin(const vcl_vector<unsigned>& n)
{
  vcl_vector<unsigned> x(n.size());
  vcl_fill(x.begin(),x.end(),0);
  return x;
}

//: Moves x to next combination of integers
//  x[i] is a positive integer base n[i]
//
//  This allows stepping through all possible combinations of sets integers {x[i]}
//  using
// \verbatim
//  vcl_vector<unsigned> x = mbl_combination_begin(n);
//  do { ...}  while (mbl_combination_next(x,n));
// \endverbatim
bool mbl_combination_next(vcl_vector<unsigned>& x, const vcl_vector<unsigned>& n)
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
unsigned mbl_n_combinations(const vcl_vector<unsigned>& n)
{
  unsigned nc = 1;
  for (unsigned i=0;i<n.size();++i) if (n[i]>1) nc*=nc[i];
  return nc;
}


#endif
