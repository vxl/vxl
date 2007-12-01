#ifndef bwm_reg_matcher_h_
#define bwm_reg_matcher_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Find a match between sets of digital curves
//
// \verbatim
// Inputs: 1) two sets of digitial curves, model and search.
//            the goal is to find the translation that matches the model
//            curves with the search curves
//         2) the search image origin of the region enclosing the search curves
//         3) The number of search cols and rows to scan
//
// Modifications
// Original November 26, 2007
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vsol/vsol_digital_curve_2d.h>
#include "bwm_reg_edge_champher.h"

class bwm_reg_matcher
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bwm_reg_matcher(vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                  unsigned search_col_origin, unsigned search_row_origin,
                  unsigned search_cols, unsigned search_rows,
                  vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
                  );
  ~bwm_reg_matcher(){}

  //: Scan over entire search region, return the 
  //  offset from the upper left of the search region
  //  with the best match
  bool match(int& tcol, int& trow, double distance_threshold);

 protected:
  // INTERNALS-----------------------------------------------------------------
  double total_distance(int tc, int tr);

  // Data Members--------------------------------------------------------------

 private:

  //the origin of the search array in the search image
  unsigned search_col_origin_;
  unsigned search_row_origin_;

  //the dimensions of the search array
  unsigned search_cols_, search_rows_; 

  //the offset bounds of the model edges (min values are 0)
  unsigned model_cols_;
  unsigned model_rows_;
  // the model edges
  vcl_vector<vsol_digital_curve_2d_sptr> model_edges_;  

  // the edges to search for a match
  vcl_vector<vsol_digital_curve_2d_sptr> search_edges_;  
  
  // the index for efficient search
  bwm_reg_edge_champher champh_;
};

#endif
