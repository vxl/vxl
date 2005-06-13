//:
// \file
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
  //if( this->scale_ > 0.0 )
  //  face_ptr->scale_ = this->transform_scale( xform );

  // NOTE:
  // Because the scaling factors is computed using PCA,
  // it is invariant to rotation.  As a result, 
  // the scaling factors are not with respect to x, y, or z anymore. 
  // If there is different scaling on x, y, (or z) axis, 
  // the scale of the mapped face point can be  different
  // depending on the orientation. 

  // Current solution is to approximate this by transforming 
  // a vector whose magnitude is the original scale. 
  // the after scale is the magnitude of the transformed vector as 
  
  vnl_vector<double> loc(normal_), xformed_loc(location_.size());
  loc *= scale_;
  loc += location_;
  
  // map the end point
  xform.map_location( loc, xformed_loc );
  
  // substract the starting point
  xformed_loc -= face_ptr->location_;
  
  face_ptr->scale_ = xformed_loc.magnitude();

  return result_sptr;
}

double
rgrl_feature_face_pt::
transform_scale( rgrl_transformation const& xform ) const
{
  assert( !"should not use this function. The reason is above in the tranform() function." );
  
  // transform scale
  vnl_vector<double> const& scaling = xform.scaling_factors();
  const unsigned dim = this->location_.size();
  double scale = 0.0;
  
  if ( this->scale_ > 0.0 && scaling.size() == dim ) {
    //use the scaling factor projected onto normal direction
    // CAUTION: it can become negative if use dot product
    // SOLUTION: use the sum of product of ABSOLUTE value of normal
    
    scale = 0.0;
    for( unsigned i=0; i<normal_.size(); ++i )
      scale += vcl_abs(normal_[i]) * scaling[i];
    scale *= this->scale_;
    
  } else if ( this-> scale_ > 0.0 ) {
    WarningMacro( "This feature has non-zero scale value, but transformation has no scaling factors."
                  << "The scale of transformed features is NOT set." );
  }

  return scale;
}


//:  Compute the signature weight between two features.
double
rgrl_feature_face_pt ::
absolute_signature_weight( rgrl_feature_sptr other ) const
{
  //if other is invalid
  if( !other )  return 0.0;

  rgrl_feature_face_pt* face_ptr = rgrl_cast<rgrl_feature_face_pt*>(other);
  assert( face_ptr );
  double dir_wgt = vcl_abs( dot_product( this->normal_, face_ptr->normal_ ) );

  double scale_wgt = 1;
  if ( this->scale_ && face_ptr->scale_ ) {
    if ( this->scale_ >= face_ptr->scale_ )
      scale_wgt = face_ptr->scale_ / this->scale_;
    else
      scale_wgt = this->scale_ / face_ptr->scale_;
    // the weight change is too gradual, make it more steep
    // scale_wgt = scale_wgt * scale_wgt;
  }

  return  dir_wgt* scale_wgt;
}

//: write out feature
void
rgrl_feature_face_pt::
write( vcl_ostream& os ) const
{
  // tag
  os << "FACE" << vcl_endl;
  
  // dim
  os << location_.size() << vcl_endl;
  
  // atributes
  os << location_ << "    " << scale_ << "\n"
     << normal_ << "\n" 
     << error_proj_ << vcl_endl;
}

//: read in feature
bool 
rgrl_feature_face_pt::
read( vcl_istream& is, bool skip_tag )
{
  if( !skip_tag ) {

    // skip empty lines
    rgrl_util_skip_empty_lines( is );
    
    vcl_string str;
    vcl_getline( is, str );
    
    // The token should appear at the beginning of line
    if ( str.find( "FACE" ) != 0 ) {
      WarningMacro( "The tag is not FACE. reading is aborted.\n" );
      return false;
    }
  }

  // get dim
  int dim=-1;
  is >> dim;
  
  if( !is || dim<=0 ) 
    return false;    // cannot get dimension
    
  // get location
  location_.set_size( dim );
  is >> location_;
  if( !is )
    return false;   // cannot read location
    
  // get scale
  is >> scale_; 
  if( !is )
    return false;   // cannot read scale

  // get normal
  normal_.set_size( dim );
  is >> normal_;
  if( !is ) 
    return false; 
    
  // get error projector
  error_proj_.set_size( dim, dim );
  is >> error_proj_;
  if( !is ) 
    return false; 
  
  //reset flag
  subspace_cached_ = false;
  
  return true;
}
