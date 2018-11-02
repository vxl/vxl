#include "rgrl_feature.h"
//:
// \file
// \author Amitha Perera
// \date Feb 2002

#include "rgrl_transformation.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


double
rgrl_feature::
geometric_error( rgrl_feature const& other ) const
{
  return (this->location() - other.location()).pre_multiply( this->error_projector_sqrt() ).two_norm();
}

double
rgrl_feature::
geometric_error( rgrl_transformation const&  /*xform*/, rgrl_feature const& other ) const
{
  vnl_vector<double> mapped;
  return (this->location() - other.location()).pre_multiply( this->error_projector_sqrt() ).two_norm();
}


vnl_vector<double>
rgrl_feature::
signature_error_vector( rgrl_feature const& ) const
{
  assert( ! "This rgrl_feature doesn't implement signature_error_vector()" );
  return vnl_vector<double>(0); // to suppress compiler warnings
}


unsigned
rgrl_feature::signature_error_dimension( std::type_info const& ) const
{
  // This rgrl_feature doesn't implement signature_error_dimension()
  // By default, return 0;
  return 0;
}

vnl_matrix<double> const&
rgrl_feature::
error_projector_sqrt() const
{
  // by default, use error_projector
  return error_projector();
}
