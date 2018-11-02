#ifndef mbl_random_n_from_m_h_
#define mbl_random_n_from_m_h_
//:
// \file
// \brief Randomly select n from m integers without replacement
// \author Tim Cootes

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>

//: Class to generate arrays of n integers chosen without replacement from [0,m-1]
class mbl_random_n_from_m
{
 private:
  vnl_random mz_random_;
  std::vector<bool> used_;
 public:

  //: Dflt ctor
  mbl_random_n_from_m();

  //: Set seed of random number generator
  void reseed(long new_seed);

  //: Select n integers from range [0,m-1], without replacement.
  //  ie all different
  //  n is required to be <= m; otherwise, the function abort()s.
  //  But n might be 0, in which case an empty list is returned.
  // \retval chosen  Array [0,n-1] of chosen elements
  void choose_n_from_m(std::vector<unsigned>& chosen,
                       unsigned int n, unsigned int m);

  //: Select n integers from range [0,m-1], without replacement.
  //  ie all different
  //  n is required to be <= m; otherwise, the function abort()s.
  //  But n might be 0, in which case an empty list is returned.
  // \retval chosen  Array [0,n-1] of chosen elements
  // \retval not_chosen  Array[0,m-n-1] of not chosen elements
  void choose_n_from_m(std::vector<unsigned>& chosen,
                       std::vector<unsigned>& not_chosen,
                       unsigned int n, unsigned int m);

  //: Select n integers from range [0,m-1], without replacement.
  //  ie all different
  //  n is required to be <= m; otherwise, the function abort()s.
  //  But n might be 0, in which case an empty list is returned.
  // \retval chosen  Array [0,n-1] of chosen elements
  void choose_n_from_m(std::vector<int>& chosen,
                       unsigned int n, unsigned int m);

  //: Select n integers from range [0,m-1], without replacement.
  //  ie all different
  //  n is required to be <= m; otherwise, the function abort()s.
  //  But n might be 0, in which case an empty list is returned.
  // \retval chosen  Array [0,n-1] of chosen elements
  // \retval not_chosen  Array[0,m-n-1] of not chosen elements
  void choose_n_from_m(std::vector<int>& chosen,
                       std::vector<int>& not_chosen,
                       unsigned int n, unsigned int m);
};

#endif // mbl_random_n_from_m_h_
