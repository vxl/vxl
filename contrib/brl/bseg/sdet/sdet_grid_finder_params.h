//----*-c++-*----tells emacs to use C++ mode----------
#ifndef sdet_grid_finder_params_h_
#define sdet_grid_finder_params_h_
//:
// \file
// \brief parameter mixin for sdet_grid_finder
//
// \author
//    Joseph L. Mundy - April 30, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class sdet_grid_finder_params : public gevd_param_mixin
{
 public:
  sdet_grid_finder_params(const int n_lines_x=11, const int n_lines_y=11, 
                          const double spacing=100.0, 
                          const int thresh=1, const float angle_tol=5);
                          


 sdet_grid_finder_params(const sdet_grid_finder_params& old_params);
 ~sdet_grid_finder_params(){}

  bool SanityCheck();
 friend
   vcl_ostream& operator<<(vcl_ostream& os, const sdet_grid_finder_params& gfp);
 protected:
  void InitParams(const int n_lines_x, const int n_lines_y, 
                  const double spacing, const int thresh,
                  const float angle_tol);
 public:
  //
  // Parameter blocks and parameters
  //
  int n_lines_x_;  //number of horizontal grid lines
  int n_lines_y_;  //number of vertical grid lines
  double spacing_; //spacing between lines
  int thresh_;     //bin threshold for dominant orientation groups
  float angle_tol_;//angle tolerance for a dominant group
};


#endif // sdet_grid_finder_params_h_
