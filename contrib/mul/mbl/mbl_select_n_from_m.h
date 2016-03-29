#ifndef mbl_select_n_from_m_h_
#define mbl_select_n_from_m_h_
//:
// \file
// \brief A class which returns an N element subset of the integers [0..M-1]
// \author Tim Cootes

#include <mbl/mbl_random_n_from_m.h>

//: A class which returns an N element subset of the integers [0..M-1]
// By default it systematically steps through all combinations of choosing
// N integers from a set of 0..M-1
// Thus when N=1 repeated calls to subset() return 1,2,3,4,...,M
// When N=2 calls to subset give (0,1), (0,1) .. (0,M-1), (1,2), (1,3),..(M-2,M-1)
// However, the use_random() function allows random examples to be
// returned (using the mbl_random_n_from_m class).
// When N > M, nothing is returned, ie, next() immediately returns false.
// When N == 0, the empty set is returned.
// The complement() function returns the integers not in the subset.
//
// The following code snippet runs process_data() on all possible subsets:
// \code
//  mbl_select_n_from_m selector(3,5);
//  if (selector.reset())
//  do
//  {
//    process_data(selector.subset());
//  } while selector.next();
// \endcode
class mbl_select_n_from_m
{
 private:
  unsigned int n_;
  unsigned int m_;
  bool is_done_;
  bool use_random_;

  std::vector<int> index_;
  std::vector<int> not_index_;

  mbl_random_n_from_m random_;

 public:

  //: Dflt ctor
  mbl_select_n_from_m();

  //: Construct to select n from m
  mbl_select_n_from_m(unsigned int n, unsigned int m);

  //: Construct to select n from m
  void set_n_m(unsigned int new_n, unsigned int new_m);

  //: If true then use random subsets
  void use_random(bool flag);

  //: Reseed randomiser
  void reseed(long);

  //: Reset to first example. Returns true if there is a valid first example.
  bool reset();

  //: Generate next set.  Return true until all combinations exhausted.
  bool next();

  //: Returns true when all sets enumerated
  //  Never returns true when random subsets being chosen
  bool is_done() const { return is_done_; }

  //: Current subset of n from m.
  //  is_done() should not be true when this function is being called
  const std::vector<int>& subset() const;

  //: Sub-set not chosen (m-n) from m.
  //  is_done() should not be true when this function is being called
  const std::vector<int>& complement();
};

#endif // mbl_select_n_from_m_h_
