//:
// \file
// \author Charlene Tsai

#include "rgrl_scale_est_null.h"

#include <rrel/rrel_objective.h>
#include <vnl/vnl_math.h>

#include "rgrl_scale.h"
#include "rgrl_match_set.h"
#include "rgrl_util.h"

#include <vcl_iostream.h>

rgrl_scale_est_null::
rgrl_scale_est_null( )
{  
  WarningMacro("No scale estimation will be performed \n");
}


rgrl_scale_est_null::
~rgrl_scale_est_null()
{
}


rgrl_scale_sptr
rgrl_scale_est_null::
estimate_unweighted( rgrl_match_set const& /*unused*/,
                     rgrl_scale_sptr const& /*unused*/,
                     bool /*unused*/ ) const
{
  rgrl_scale_sptr scales = new rgrl_scale;

  scales->set_geometric_scale( 1 );

  return scales;
}

rgrl_scale_sptr
rgrl_scale_est_null::
estimate_weighted( rgrl_match_set const& match_set,
                   rgrl_scale_sptr const& scales,
                   bool use_signature_only,
                   bool penalize_scaling ) const
{
  return estimate_unweighted( match_set, scales, penalize_scaling );
}
