#ifndef bdpg_array_dynamic_prg_h_
#define bdpg_array_dynamic_prg_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Implements a dynamic program in a 2-d array
// Finds the mapping between two 1-d spaces (row and col).  Each element
// of the array is a bdpg_node_sptr.  The node maintains the path information
// as the dynamic program proceeds throught the array. Each element of a row
// of the array looks back to the previous row and finds the ancestor with the
// highest probability path. This choice is recorded in the node. At the last
// row, the highest probability column can be backtraced to find the full path.
//
// The choice of preceeding paths is restricted to prevent the same column
// appearing more than once in a path.  This restriction corresponds to 
// the semantics that a row element should be assigned to exactly one column 
// \verbatim
//  Modifications
//   Initial version February 5, 2005
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_2d.h>
#include <bdpg/bdpg_node_sptr.h>

class bdpg_array_dynamic_prg
{
 public:
  bdpg_array_dynamic_prg(const unsigned rows, const unsigned cols);
  ~bdpg_array_dynamic_prg();

  //: Return number of rows
  unsigned rows() const { return num_rows_; }

  //: Return number of columns
  unsigned cols() const { return num_cols_; }

  //:access to the dynamic program array
  bdpg_node_sptr const* operator[] (int row) const { return array_[row]; }
  bdpg_node_sptr * operator[] (int row) { return array_[row]; }
  bdpg_node_sptr const& operator() (int row, int col) const
    {return array_[row][col];}
  bdpg_node_sptr & operator() (int row, int col)
    {return array_[row][col];}

  //:set an element of the array
  void put(unsigned row, unsigned col, bdpg_node_sptr const &n)
    { array_[row][col] = n; }

  //: scan through the array generating the forward paths
  bool scan();

  //: retrieve the optimal assignment
  vcl_vector<unsigned> assignment();

  //:the best final assignment column
  unsigned best_assign_col();

  //utilities
  //: print the full array
  void print_array();

  //: print the path at (row, col)
  void print_path(unsigned row, unsigned col);

  // array search methods
  //:find if an old column exists in the path at (row, col).
  bool find(const unsigned row, const unsigned col, const unsigned old_col);

  //:find the column index in the previous row with maximum path probabilty
  bool maxp(unsigned row, unsigned col, int & best_col);

 private:
  unsigned num_rows_;//!< number of rows in the array
  unsigned num_cols_;//!< number of columns in the array
  vbl_array_2d<bdpg_node_sptr> array_;//!< the dynamic program array

};

#endif // bdpg_array_dynamic_prg_h_
