//:
// \file
#include "rgrl_feature_face_pt.h"
#include <rgrl/rgrl_transformation.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>

#include <cassert>
#include <vcl_compiler.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_cast.h>


rgrl_feature_face_pt ::
rgrl_feature_face_pt( vnl_vector< double > const& location,
                      vnl_vector< double > const& normal )
  : rgrl_feature( location ), normal_( normal ), subspace_cached_( false )
{
  normal_.normalize();
}

//  private constructor for transformed face points
rgrl_feature_face_pt ::
rgrl_feature_face_pt()
  : subspace_cached_( false )
{
}

vnl_matrix<double> const&
rgrl_feature_face_pt ::
error_projector() const
{
  if (scale_ == 0) {
    WarningMacro( "The scale is zero." );
  }
  if ( !err_proj_.size() ) {
    err_proj_ = outer_product( normal_, normal_ ) ;
    err_proj_ /= vnl_math::sqr( scale_ );
  }

  return err_proj_;
}

vnl_matrix<double> const&
rgrl_feature_face_pt ::
error_projector_sqrt() const
{
  if (scale_ == 0) {
    WarningMacro( "The scale is zero." );
  }
  if ( !err_proj_sqrt_.size() ) {
    err_proj_sqrt_ = outer_product( normal_, normal_ ) ;
    err_proj_sqrt_ /= scale_;
  }

  return err_proj_sqrt_;
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
  auto* face_ptr = new rgrl_feature_face_pt();

  // Capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = face_ptr;

  xform.map_location( this->location_, face_ptr->location_ );
  xform.map_normal( this->location_, this->normal_, face_ptr->normal_ );

  // it is not clear what's meaning of the scale of corner and face points
  // It certainly does not represent the physical scale and is related to
  // the sharpness of the edge.
#if 0
  // transform scale
  if ( this->scale_ > 0.0 )
    face_ptr->scale_ = this->transform_scale( xform );
#else
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

  vnl_vector<double> loc(normal_), xformed_loc(face_ptr->location_.size());
  loc *= scale_;
  loc += location_;

  // map the end point
  xform.map_location( loc, xformed_loc );

  // subtract the starting point
  xformed_loc -= face_ptr->location_;

  face_ptr->scale_ = xformed_loc.magnitude();
#endif

  return result_sptr;
}

double
rgrl_feature_face_pt::
transform_scale( rgrl_transformation const& xform ) const
{
  assert( !"should not use this function. The reason is above in the transform() function." );

  // transform scale
  vnl_vector<double> const& scaling = xform.scaling_factors();
  const unsigned dim = this->location_.size();
  double scale = 0.0;

  if ( this->scale_ > 0.0 && scaling.size() == dim ) {
    //use the scaling factor projected onto normal direction
    // CAUTION: it can become negative if use dot product
    // SOLUTION: use the sum of product of ABSOLUTE value of normal

    scale = 0.0;
    for ( unsigned i=0; i<normal_.size(); ++i )
      scale += std::abs(normal_[i]) * scaling[i];
    scale *= this->scale_;
  }
  else if ( this-> scale_ > 0.0 ) {
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
  if ( !other )  return 0.0;

  rgrl_feature_face_pt* face_ptr = rgrl_cast<rgrl_feature_face_pt*>(other);
  assert( face_ptr );
  double dir_wgt = std::abs( dot_product( this->normal_, face_ptr->normal_ ) );

  double scale_wgt = 1;
  if ( this->scale_ && face_ptr->scale_ ) {
    if ( this->scale_ >= face_ptr->scale_ )
      scale_wgt = face_ptr->scale_ / this->scale_;
    else
      scale_wgt = this->scale_ / face_ptr->scale_;
    // the weight change is too gradual, make it more steep
    // scale_wgt = scale_wgt * scale_wgt;
  }

  return  dir_wgt* std::sqrt(scale_wgt);
}

//:  Compute the signature error vector between two features.
vnl_vector<double>
rgrl_feature_face_pt::
signature_error_vector( rgrl_feature const& other ) const
{
  if ( !other.is_type( rgrl_feature_face_pt::type_id() ) )
    return vnl_vector<double>();

  // cast it to face point type
  auto const& other_face_pt = static_cast<rgrl_feature_face_pt const&>(other);

  // compute cos between normals
  const double dot = dot_product( this->normal_, other_face_pt.normal_ );
  const double half_pi = vnl_math::pi / 2.0;
  double ang = std::acos( dot );

  // make it between [-pi/2, pi/2]
  if ( ang > half_pi )  ang -= vnl_math::pi;

  vnl_vector<double> error_vec(1, ang);
  return error_vec;
}

//:  the dimensions of the signature error vector.
unsigned
rgrl_feature_face_pt::
signature_error_dimension( const std::type_info& other_feature_type ) const
{
  if ( other_feature_type == rgrl_feature_face_pt::type_id() )
    return 1;
  else
    return 0;
}

//: write out feature
void
rgrl_feature_face_pt::
write( std::ostream& os ) const
{
  // tag
  os << "FACE\n"
  // dimension
     << location_.size() << '\n'
  // atributes
     << location_ << "    " << scale_ << '\n'
     << normal_  << std::endl;
}

//: read in feature
bool
rgrl_feature_face_pt::
read( std::istream& is, bool skip_tag )
{
  if ( !skip_tag ) {

    // skip empty lines
    rgrl_util_skip_empty_lines( is );

    std::string str;
    std::getline( is, str );

    // The token should appear at the beginning of line
    if ( str.find( "FACE" ) != 0 ) {
      WarningMacro( "The tag is not FACE. reading is aborted.\n" );
      return false;
    }
  }

  // get dim
  int dim=-1;
  is >> dim;

  if ( !is || dim<=0 )
    return false;    // cannot get dimension

  // get location
  location_.set_size( dim );
  is >> location_;
  if ( !is )
    return false;   // cannot read location

  // get scale
  is >> scale_;
  if ( !is )
    return false;   // cannot read scale

  // get normal
  normal_.set_size( dim );
  is >> normal_;
  if ( !is )
    return false;

  //reset flag
  subspace_cached_ = false;

  return true;
}

rgrl_feature_sptr
rgrl_feature_face_pt::
clone() const
{
  return new rgrl_feature_face_pt(*this);
}
