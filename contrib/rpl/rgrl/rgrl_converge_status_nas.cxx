//:
// \file
// \author Gehua Yang
// \date   March 2005

#include "rgrl_converge_status_nas.h"

#include <vcl_cassert.h>
 
rgrl_converge_status_nas::
rgrl_converge_status_nas( bool     in_has_converged,
                      bool     in_has_stagnated,
                      bool     in_is_good_enough,
                      bool     in_is_failed,
                      double   in_error,
                      unsigned in_oscillation_count,
                      double   in_error_diff )
  : rgrl_converge_status( in_has_converged, in_has_stagnated, in_is_good_enough, in_is_failed,
                          in_error, in_oscillation_count, in_error_diff ),
    iteration_(0)
{
  
}

rgrl_converge_status_nas::
rgrl_converge_status_nas( rgrl_converge_status const& old )
  : rgrl_converge_status( old ),
    iteration_(0)
{
  
}

rgrl_converge_status_nas::
~rgrl_converge_status_nas()
{
}


void
rgrl_converge_status_nas::
set_init_scaling_factors( vnl_vector<double> const& scaling )
{
  init_scaling_factors_ = scaling;
}
