#include "rgrl_estimator.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_set_of.h"
#include "rgrl_match_set_sptr.h"

rgrl_estimator::
rgrl_estimator( unsigned int param_dof )
  : dof_( param_dof )
{
}

rgrl_estimator::
rgrl_estimator()
  : dof_( 0 )
{
}

rgrl_estimator::
~rgrl_estimator()
{
}


rgrl_transformation_sptr
rgrl_estimator::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  rgrl_set_of<rgrl_match_set_sptr> set;
  set.push_back( matches );
  return estimate( set, cur_transform );
}
