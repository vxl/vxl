#include "rgrl_feature_face_pt.h"
#include <rgrl/rgrl_transformation.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_cassert.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_cast.h>

rgrl_feature_face_pt ::
rgrl_feature_face_pt( vnl_vector< double > const& location,
                      vnl_vector< double > const& normal )
  : location_( location ), normal_( normal ),
    error_proj_( normal.size(), normal.size() ),
    scale_( 0 ),
    subspace_cached_( false )
{
  normal_.normalize();
  error_proj_ = outer_product( normal, normal ) ;
}

//  private constructor for transformed face points
rgrl_feature_face_pt ::
rgrl_feature_face_pt()
  : scale_( 0 ), subspace_cached_( false )
{
}


vnl_vector<double> const&
rgrl_feature_face_pt ::
location() const
{
  return location_;
}

vnl_matrix<double> const&
rgrl_feature_face_pt ::
error_projector() const
{
  return error_proj_;
}

unsigned int
rgrl_feature_face_pt::
num_constraints() const
{
  return 1;
}

vnl_vector<double> const&
rgrl_feature_face_pt :: normal() const
{
  return normal_;
}

vnl_matrix<double> const&
rgrl_feature_face_pt ::
tangent_subspace()
{
  if ( subspace_cached_ )
    return tangent_subspace_;

  vnl_matrix<double> one_row( 1, this -> normal_.size() );
  one_row.set_row( 0, this -> normal_ );
  vnl_svd<double> normal_svd( one_row );
  tangent_subspace_ = normal_svd.nullspace();
  assert( tangent_subspace_ . columns() == this -> normal_ . size() - 1 );
  subspace_cached_ = true;
  return tangent_subspace_;
}

rgrl_feature_sptr
rgrl_feature_face_pt ::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_face_pt* face_ptr = new rgrl_feature_face_pt();

  // Capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = face_ptr;

  xform.map_location( this->location_, face_ptr->location_ );
  xform.map_normal( this->location_, this->normal_, face_ptr->normal_ );

  face_ptr->error_proj_ = outer_product( face_ptr->normal_, face_ptr->normal_ );

  // transform scale
  vnl_vector<double> const& scaling = xform.scaling_factors();
  const unsigned dim = this->location_.size();
  if( this->scale_ && scaling.size() == dim ) {
    // average them
    if( dim == 2 )
      face_ptr->scale_ = vcl_sqrt( scaling[0]*scaling[1] ) * this->scale_;
    else {
      double prod_scale=1;
      for( unsigned i=0; i < dim; ++i )
        prod_scale *= scaling[i];
      face_ptr->scale_ = vcl_exp( vcl_log(prod_scale) / double(dim) ) * this->scale_;
    }
  } else if( this-> scale_ ) {
    WarningMacro( "This feature has non-zero scale value, but transformation has no scaling factors." 
                  << "The scale of transformed features is NOT set. " );
  }

  return result_sptr;
}

//:  Compute the signature weight between two features.
double 
rgrl_feature_face_pt ::
absolute_signature_weight( rgrl_feature_sptr other ) const
{
  rgrl_feature_face_pt* face_ptr = rgrl_cast<rgrl_feature_face_pt*>(other);
  assert( face_ptr );
  double dir_wgt = vcl_abs( dot_product( this->normal_, face_ptr->normal_ ) );
 
  double scale_wgt = 1;
  if( this->scale_ && face_ptr->scale_ ) {
    if( this->scale_ >= face_ptr->scale_ )
      scale_wgt = face_ptr->scale_ / this->scale_;
    else
      scale_wgt = this->scale_ / face_ptr->scale_;
    // the weight change is too gradual, make it more steep
    scale_wgt = scale_wgt * scale_wgt;
  }
  
  return  dir_wgt* scale_wgt;
}
