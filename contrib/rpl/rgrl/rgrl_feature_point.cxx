// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_feature_point.h"

#include "rgrl_transformation.h"

#include <vcl_cassert.h>


static
vnl_matrix<double> const&
identity_matrix( unsigned size )
{
  static vnl_matrix<double>* matrices[5] = { 0, 0, 0, 0, 0 };
  assert( size < 5 );
  if( matrices[size] == 0 ) {
    matrices[size] = new vnl_matrix<double>( size, size, vnl_matrix_identity );
  }

  return *matrices[size];
}


rgrl_feature_point::
rgrl_feature_point( vnl_vector<double> const& loc )
  : location_( loc )
{
}


rgrl_feature_point::
rgrl_feature_point( unsigned size )
  : location_( size )
{
}


unsigned int
rgrl_feature_point::
num_constraints() const
{
  return location_.size();
}

vnl_vector<double> const&
rgrl_feature_point::
location() const
{
  return location_;
}


vnl_matrix<double> const&
rgrl_feature_point::
error_projector() const
{
  return identity_matrix( location_.size() );
}


rgrl_feature_sptr
rgrl_feature_point::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_point* result = new rgrl_feature_point( location_.size() );

  // capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = result;

  // Transform the location
  //
  xform.map_location( this->location_, result->location_ );

  return result_sptr;
};
