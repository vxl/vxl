#ifndef bwm_reg_matcher_h_
#define bwm_reg_matcher_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Find a match between sets of edgel chains
//
// \verbatim
//   November 26, 2007
// Modifications
//  None
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

  bwm_reg_matcher(unsigned model_cols, unsigned model_rows,
                  vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                  unsigned search_cols, unsigned search_rows,
                  vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
                  );
  ~bwm_reg_matcher(){}

  bool match(unsigned initial_col, unsigned initial_row, unsigned radius,
             unsigned& search_col, unsigned& search_row
             );

 protected:
  // INTERNALS-----------------------------------------------------------------
  double total_distance(unsigned scol, unsigned srow);

  // Data Members--------------------------------------------------------------

 private:

  //the dimensions of the model array
  unsigned model_cols_, model_rows_; 

  //the dimensions of the search array
  unsigned search_cols_, search_rows_; 

  // the model edges
  vcl_vector<vsol_digital_curve_2d_sptr> model_edges_;  

  // the edges to search for a match
  vcl_vector<vsol_digital_curve_2d_sptr> search_edges_;  
  
  // the index for efficient search
  bwm_reg_edge_champher champh_;
};

#endif
