#ifndef mbl_dyn_prog_h_
#define mbl_dyn_prog_h_

//:
// \file
// \brief Class to solve simple dynamic programming problems
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_iosfwd.h>

#include <vsl/vsl_fwd.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_vector.h>

//: Class to solve simple dynamic programming problems
//  Assume n values x(i) to be chosen, x(i) in [0,k-1], i=0..n-1
//  Cost of each is W(i,x(i))
//  Total cost = sum_i W(i,x(i)) + C_i(x(i),x(i-1))
//  The algorithm finds the path (x(0),x(1)..x(n-1)) through which minimises
//  the total cost, with a variety of different ways of defining C_i(x(i),x(i-1))
//
class mbl_dyn_prog {
private:
     //: After search, links_(i,j) shows the best prior state 
     // (ie at i) leading to state j at time i+1
  vbl_array_2d<int> links_;

    //: Workspace for running cost.  
    //  After search gives cost to get to each state in last row
  vnl_vector<double> running_cost_;

    //: Workspace for previous running cost.
  vnl_vector<double> next_cost_;

    //: Construct path from links_, assuming it ends at end_state
  void construct_path(vcl_vector<int>& x, int end_state);

    //: Compute running costs for DP problem with costs W
    //  Pair cost term:  C_i(x1,x2) = c(|x1-x2|)
    //  Size of c indicates maximum displacement between neighbouring
    //  states.
    //  If first_state>=0 then the first is constrained to that index value
  void running_costs(const vnl_matrix<double>& W, 
               const vnl_vector<double>& pair_cost,
               int first_state);

public:

    //: Dflt ctor
  mbl_dyn_prog();

    //: Destructor
  virtual ~mbl_dyn_prog();

    //: Solve the dynamic programming problem with costs W
    //  Pair cost term:  C_i(x1,x2) = pair_cost(|x1-x2|)
    //  Size of c indicates maximum displacement between neighbouring
    //  states.
    //  If first_state>=0 then the first is constrained to that index value
    // \retval x  Optimal path
    // \return Total cost of given path
  double solve(vcl_vector<int>& x, 
               const vnl_matrix<double>& W, 
               const vnl_vector<double>& pair_cost,
               int first_state=-1);

    //: Solve the loop dynamic programming problem with costs W
    //  Pair cost term:  C_i(x1,x2) = pair_cost(|x1-x2|)
    //  Size of c indicates maximum displacement between neighbouring
    //  states.
    //  As solve(x,W,c), but includes constraint between first and
    //  last states.
    // \retval x  Optimal path
    // \return Total cost of given path
  double solve_loop(vcl_vector<int>& x, 
               const vnl_matrix<double>& W, 
               const vnl_vector<double>& pair_cost);


    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_dyn_prog& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_dyn_prog& b);

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mbl_dyn_prog& b);

//=======================================================================
#endif


