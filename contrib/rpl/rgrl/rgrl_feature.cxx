//:
// \file
// \author Amitha Perera
// \date Feb 2002

#include "rgrl_feature.h"

#include "rgrl_transformation.h"

#include <vcl_cassert.h>


double
rgrl_feature::
geometric_error( rgrl_feature const& other ) const
{
  return (this->location() - other.location()).pre_multiply( this->error_projector() ).two_norm();
}


vnl_vector<double>
rgrl_feature::
signature_error_vector( rgrl_feature const& other ) const
{
  assert( "This feature doesn't implement signature_error_vector" == 0 );
  return vnl_vector<double>(0); // to supress compiler warnings
}


unsigned
rgrl_feature::signature_error_dimension( const vcl_type_info& other_feature_type ) const
{
  return 0;
}

double
rgrl_feature::absolute_signature_weight( rgrl_feature_sptr other ) const
{
  return 1;
}

