#include <sstream>
#include <iostream>
#include "brip_para_cvrg_params.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------
//
//: Checks that parameters are within acceptable bounds
//
bool brip_para_cvrg_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (sigma_ <= 0)  // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << std::ends;
    valid = false;
  }

  if (thresh_ <= 0)  // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be >0" << std::ends;
    valid = false;
  }

  if (gauss_tail_ <= 0) // Cutoff for gaussian kernel radius
  {
    msg << "ERROR: Value of gauss tail fraction is too low <= 0" << std::ends;
    valid = false;
  }

  if (proj_width_ <= 0)  //Projection kernel width
  {
    msg << "ERROR: Value of projection kernel width must be >0" << std::ends;
    valid = false;
  }

  if (proj_height_ <= 0)  //Projection kernel height
  {
    msg << "ERROR: Value of projection kernel height must be >0" << std::ends;
    valid = false;
  }

  if (sup_radius_ <= 0)  //Max suppression radius
  {
    msg << "ERROR: Value of max suppression radius must be >0" << std::ends;
    valid = false;
  }

  msg << std::ends;
  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator<<(std::ostream& os, const brip_para_cvrg_params& pcp)
{
  return
  os << "brip_para_coverage_params:\n[---\n"
     << "sigma " << pcp.sigma_ << std::endl
     << "thresh " << pcp.thresh_ << std::endl
     << "proj half width " << pcp.proj_width_ << std::endl
     << "proj half height " << pcp.proj_height_ << std::endl
     << "non maximum suppression radius " << pcp.sup_radius_ << std::endl
     << "verbose " << pcp.verbose_ << std::endl
     << "---]" << std::endl;
}
