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
// However, the useRandom() function allows random examples to be
// returned (using the SM_RandomNfromM class).
// The complement() function returns the integers not in the subset.
// \verbatim
// mbl_select_n_from_m selector(3,5);
// selector.reset();
// do
// {
//   process_data(selector.subset());
// } while selector.next();
// \endverbatim
class mbl_select_n_from_m {
private:
  int n_;
  int m_;
  bool is_done_;
  bool use_random_;

  vcl_vector<int> index_;
  vcl_vector<int> not_index_;

  mbl_random_n_from_m random_;

public:

    //: Dflt ctor
  mbl_select_n_from_m();

    //: Construct to select n from m
  mbl_select_n_from_m(int n, int m);

    //: Construct to select n from m  
  void set_n_m(int new_n, int new_m);
  
    //: If true then use random subsets
  void use_random(bool flag);
  
    //: Reseed randomiser
  void reseed(long);

    //: Reset to first example
  void reset();
  
    //: Generate next set.  Return true until all combinations exhausted.
  bool next();
  
    //: Returns true when all sets enumerated
    //  Never returns true when random subsets being chosen
  bool is_done();

    //: Current subset of n from m
  const vcl_vector<int>& subset();

    //: Sub-set not chosen (m-n) from m
  const vcl_vector<int>& complement();

};

#endif


