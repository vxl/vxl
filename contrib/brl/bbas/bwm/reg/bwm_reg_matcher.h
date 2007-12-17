#ifndef bwm_reg_matcher_h_
#define bwm_reg_matcher_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Find a match between sets of digital curves
//
// Inputs:
// 1) two sets of digitial curves, model and search.
//    the goal is to find the translation that matches the model
//    curves with the search curves
// 2) the search image origin of the region enclosing the search curves
// 3) The number of search cols and rows to scan
//
// \verbatim
//  Modifications
//   Original November 26, 2007
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <bsta/bsta_histogram.h>
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
  //  with the best match. min_probability is the threshold on
  //  the  cumulative probability of distances <= distance_threshold
  bool match(int& tcol, int& trow, double distance_threshold,
             double angle_threshold,
             double min_probability);

  //: Filter out model edges whose samples  don't lie within the 
  //  specified distance of some search edge point
  bool close_edges(double filter_distance, double angle_threshold,
                   vcl_vector<vsol_digital_curve_2d_sptr>& close_edges);

  //: Debug 
  void print_hist(){min_hist_.print();}
 protected:
  // INTERNALS-----------------------------------------------------------------
  void distance_histogram(int tc, int tr);

  // Data Members--------------------------------------------------------------

 private:

  //: Tolerance on angle match
  double angle_threshold_;

  //: the origin of the search array in the search image
  unsigned search_col_origin_;
  unsigned search_row_origin_;


  //: the dimensions of the search array
  unsigned search_cols_, search_rows_; 

  //: the origin of the model edges bounding box
  unsigned model_col_origin_;
  unsigned model_row_origin_;

  //: the dimensions of the model edges bounding_box
  unsigned model_cols_;
  unsigned model_rows_;

  //: the model edges
  vcl_vector<vsol_digital_curve_2d_sptr> model_edges_;  


  //: the edges to search for a match
  vcl_vector<vsol_digital_curve_2d_sptr> search_edges_;  
  
  //: the index for efficient search
  bwm_reg_edge_champher champh_;

  //: a histogram of match distances for the best case
  bsta_histogram<double> min_hist_;

  //: a histogram of match distances (for finding min histogram)
  bsta_histogram<double> hist_;

};

#endif
