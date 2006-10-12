//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_scale.h"


rgrl_scale::
rgrl_scale()
  : has_geometric_scale_( false ),
    has_signature_inv_covar_( false )
{
}


bool
rgrl_scale::
has_geometric_scale() const
{
  return has_geometric_scale_;
}


double
rgrl_scale::
geometric_scale( ) const
{
  return geometric_scale_;
}


bool
rgrl_scale::
has_signature_inv_covar() const
{
  return has_signature_inv_covar_;
}


vnl_matrix<double> const&
rgrl_scale::
signature_inv_covar() const
{
  return signature_inv_covar_;
}


void
rgrl_scale::
set_scales( double                     geometric_scale,
            vnl_matrix<double>  const& signature_inv_covar,
            type new_type )
{
  set_geometric_scale( geometric_scale );
  set_signature_inv_covar( signature_inv_covar );
  set_geo_scale_type( new_type );
}


void
rgrl_scale::
set_geometric_scale( double scale, type new_type )
{
  geometric_scale_ = scale;
  has_geometric_scale_ = true;
  set_geo_scale_type( new_type );
}


void
rgrl_scale::
set_signature_inv_covar( vnl_matrix<double> const& covar )
{
  signature_inv_covar_ = covar;
  has_signature_inv_covar_ = true;
}
