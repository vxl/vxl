// This is brl/bseg/sdet/sdet_grid_finder_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_grid_finder_params.h"
//:
// \file
// See sdet_grid_finder_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

sdet_grid_finder_params::
sdet_grid_finder_params(const sdet_grid_finder_params& gfp)
  : gevd_param_mixin()
{
  InitParams(gfp.n_lines_x_,
             gfp.n_lines_y_,
             gfp.spacing_,
             gfp.thresh_,
             gfp.angle_tol_,
             gfp.verbose_,
             gfp.debug_state_);
}

sdet_grid_finder_params::
sdet_grid_finder_params(const int n_lines_x, const int n_lines_y,
                        const double spacing, const int thresh,
                        const float angle_tol, bool verbose,
                        int debug_state)
{
  InitParams(n_lines_x, n_lines_y, spacing, thresh, angle_tol, verbose,
             debug_state);
}

void
sdet_grid_finder_params::get_debug_choices(std::vector<std::string>& choices)
{
  choices.clear();
  choices.emplace_back("NO_DEBUG");
  choices.emplace_back("VANISHING_POINT");
  choices.emplace_back("AFFINE_GROUP_BEFORE_SKEW_SCALE");
  choices.emplace_back("AFFINE_GROUP_AFTER_SKEW_SCALE");
  choices.emplace_back("TRANS_PERIM_LINES");
  choices.emplace_back("AFFINE_GROUP_AFTER_TRANS");
}

void sdet_grid_finder_params::InitParams(const int n_lines_x,
                                         const int n_lines_y,
                                         const double spacing,
                                         const int thresh,
                                         const float angle_tol,
                                         bool verbose,
                                         int debug_state)
{
  n_lines_x_=n_lines_x;
  n_lines_y_=n_lines_y;
  spacing_=spacing;
  thresh_ = thresh;
  angle_tol_ = angle_tol;
  verbose_ = verbose;
  debug_state_ = debug_state;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool sdet_grid_finder_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (n_lines_x_<2)
  {
    msg << "ERROR: need at least 2 lines in the x direction";
    valid = false;
  }
  if (n_lines_x_<2)
  {
    msg << "ERROR: need at least 2 lines in the y direction";
    valid = false;
  }
  if (spacing_<=0)
  {
    msg << "ERROR: need finite spacing";
    valid = false;
  }
  if (thresh_<1)
  {
    msg << "ERROR: need at least 1 line in a bin ";
    valid = false;
  }
  if (angle_tol_<5)
  {
    msg << "ERROR: need at least 5 degrees tolerance ";
    valid = false;
  }
  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_grid_finder_params& gfp)
{
  return
  os << "sdet_grid_finder_params:\n[---\n"
     << "number of lines in x direction (y lines) " << gfp.n_lines_x_ << '\n'
     << "number of lines in y direction (x lines) " << gfp.n_lines_y_ << '\n'
     << "spacing between lines " << gfp.spacing_ << '\n'
     << "hough index bin count threshold " << gfp.thresh_ << '\n'
     << "hough angle tolerance " << gfp.thresh_ << '\n'
     << "---]" << std::endl;
}
