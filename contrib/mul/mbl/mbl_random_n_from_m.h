#ifndef mbl_random_n_from_m_h_
#define mbl_random_n_from_m_h_
//:
// \file
// \brief Randomly select n from m integers without replacement
// \author Tim Cootes

#include <vcl_vector.h>
#include <vnl/vnl_random.h>

//: Class to generate arrays of n integers chosen without replacement from [0,m-1]
class mbl_random_n_from_m
{
 private:
  vnl_random mz_random_;
  vcl_vector<bool> used_;
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
  void choose_n_from_m(vcl_vector<int>& chosen,
                       unsigned int n, unsigned int m);

  //: Select n integers from range [0,m-1], without replacement.
  //  ie all different
  //  n is required to be <= m; otherwise, the function abort()s.
  //  But n might be 0, in which case an empty list is returned.
  // \retval chosen  Array [0,n-1] of chosen elements
  // \retval not_chosen  Array[0,m-n-1] of not chosen elements
  void choose_n_from_m(vcl_vector<int>& chosen,
                       vcl_vector<int>& not_chosen,
                       unsigned int n, unsigned int m);
};

#endif // mbl_random_n_from_m_h_
