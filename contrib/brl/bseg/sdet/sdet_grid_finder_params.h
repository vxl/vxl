// This is brl/bseg/sdet/sdet_grid_finder_params.h
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
#include <iostream>
#include <iosfwd>
#include <vector>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class sdet_grid_finder_params : public gevd_param_mixin
{
 public:
  enum debug {NO_DEBUG=0,
              VANISHING_POINT,
              AFFINE_GROUP_BEFORE_SKEW_SCALE,
              AFFINE_GROUP_AFTER_SKEW_SCALE,
              TRANS_PERIM_LINES,
              AFFINE_GROUP_AFTER_TRANS};

  sdet_grid_finder_params(const int n_lines_x=11, const int n_lines_y=11,
                          const double spacing=53.34,
                          const int thresh=1, const float angle_tol=5,
                          bool verbose = false,
                          int debug_state = false);

  sdet_grid_finder_params(const sdet_grid_finder_params& old_params);
 ~sdet_grid_finder_params() override= default;

  bool SanityCheck() override;
  void get_debug_choices(std::vector<std::string>& choices);
  friend
    std::ostream& operator<<(std::ostream&, const sdet_grid_finder_params& gfp);
 protected:
  void InitParams(const int n_lines_x, const int n_lines_y,
                  const double spacing, const int thresh,
                  const float angle_tol,
                  bool verbose,
                  int debug_state);
 public:
  //
  // Parameter blocks and parameters
  //

  // Variables n_lines_x_ and n_lines_y_ appear to be backwards here,
  // with n_lines_x_ representing the number of grid lines you have as
  // you move in the Y direction, and vice versa.  This results in
  // many statements in the implementation of sdet_grid_finder that
  // appear to have an x/y mismatch.  Any development of these classes
  // should be tested on a non-square grid.  There may still be bugs in
  // sdet_grid_finder related to this issue.

  int n_lines_x_;  //!< number of horizontal grid lines
  int n_lines_y_;  //!< number of vertical grid lines
  double spacing_; //!< spacing between lines
  int thresh_;     //!< bin threshold for dominant orientation groups
  float angle_tol_;//!< angle tolerance for a dominant group
  bool verbose_;   //!< print informative debug output
  int debug_state_;//!< general purpose debug state
};

#endif // sdet_grid_finder_params_h_
