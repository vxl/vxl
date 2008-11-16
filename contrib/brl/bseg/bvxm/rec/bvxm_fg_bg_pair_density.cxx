//:
// \file
// \brief bvxm recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//


#include "bvxm_fg_bg_pair_density.h"
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>

double bvxm_fg_bg_pair_density::operator()(const double y0, const double y1)
{
  double ss = vcl_pow(sigma_, 2.0);
  double B = vcl_exp(-vcl_pow(y0-y1, 2.0)/(2*ss));
  double ootss = 1.0/(2*ss);
  double oosts = 1.0/(vcl_sqrt(2.0)*sigma_);
  double A = 1+ss*(2-2*vcl_exp(-ootss)) - sigma_*vcl_sqrt(2*vnl_math::pi)*vnl_erf(oosts);
  return (1.0/A)*(1-B);
}

