// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_transformation.h>

#include <vcl_cassert.h>


static
vnl_matrix<double> const&
identity_matrix( unsigned size )
{
  static vnl_matrix<double> matrices[5] = { vnl_matrix<double>(0, 0, vnl_matrix_identity ),
                                            vnl_matrix<double>(1, 1, vnl_matrix_identity ),
                                            vnl_matrix<double>(2, 2, vnl_matrix_identity ),
                                            vnl_matrix<double>(3, 3, vnl_matrix_identity ),
                                            vnl_matrix<double>(4, 4, vnl_matrix_identity ) };
  assert( size < 5 );
  return matrices[size];
}


rgrl_feature_point::
rgrl_feature_point( vnl_vector<double> const& loc )
  : location_( loc ),
    scale_( 0 )
{
}


rgrl_feature_point::
rgrl_feature_point( unsigned size )
  : location_( size ),
    scale_( 0 )
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

  // transform scale
  vnl_vector<double> const& scaling = xform.scaling_factors();
  const unsigned dim = this->location_.size();
  if( this->scale_ && scaling.size() == dim ) {
    // "average" them
    if( dim == 2 )
      result->scale_ = vcl_sqrt( scaling[0]*scaling[1] ) * this->scale_;
    else {
      double prod_scale=1;
      for( unsigned i=0; i < dim; ++i )
        prod_scale *= scaling[i];
      result->scale_ = vcl_exp( vcl_log(prod_scale) / double(dim) ) * this->scale_;
    }
  } else if( this-> scale_ ) {
    WarningMacro( "This feature has non-zero scale value, but transformation has no scaling factors." 
                  << "The scale of transformed features is NOT set. " );
  }

  return result_sptr;
};

//:  Compute the signature weight between two features.
double 
rgrl_feature_point ::
absolute_signature_weight( rgrl_feature_sptr other ) const
{
  rgrl_feature_point* pt_ptr = rgrl_cast<rgrl_feature_point*>(other);
  assert( pt_ptr );
 
  double scale_wgt = 1;
  if( this->scale_ && pt_ptr->scale_ ) {
    if( this->scale_ >= pt_ptr->scale_ )
      scale_wgt = pt_ptr->scale_ / this->scale_;
    else
      scale_wgt = this->scale_ / pt_ptr->scale_;
    // the weight change is too gradual, make it more steep
    scale_wgt = scale_wgt * scale_wgt;
  }
  
  return  scale_wgt;
}
