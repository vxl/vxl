//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_scale.h"


rgrl_scale::
rgrl_scale()
  : has_geometric_scale_( false ),
    has_signature_covar_( false )
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
has_signature_covar() const
{
  return has_signature_covar_;
}


vnl_matrix<double> const&
rgrl_scale::
signature_covar() const
{
  return signature_covar_;
}


void
rgrl_scale::
set_scales( double                     geometric_scale,
            vnl_matrix<double>  const& signature_covar )
{
  set_geometric_scale( geometric_scale );
  set_signature_covar( signature_covar );
}


void
rgrl_scale::
set_geometric_scale( double scale )
{
  geometric_scale_ = scale;
  has_geometric_scale_ = true;
}


void
rgrl_scale::
set_signature_covar( vnl_matrix<double> const& covar )
{
  signature_covar_ = covar;
  has_signature_covar_ = true;
}
