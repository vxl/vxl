#include "rgrl_feature.h"
//:
// \file
// \author Amitha Perera
// \date Feb 2002

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
signature_error_vector( rgrl_feature const& ) const
{
  assert( ! "This rgrl_feature doesn't implement signature_error_vector()" );
  return vnl_vector<double>(0); // to suppress compiler warnings
}


unsigned
rgrl_feature::signature_error_dimension( vcl_type_info const& ) const
{
  assert( ! "This rgrl_feature doesn't implement signature_error_dimension()" );
  return 0;
}

template <class CastTo>
rgrl_feature_caster<CastTo>::rgrl_feature_caster( rgrl_feature_sptr f )
    : data_( dynamic_cast<CastTo*>( f.as_pointer() ) )
  {
    assert( data_ );
  }
