//:
// \file
// \author Amitha Perera
// \date   Feb 2003
#include <vnl/vnl_math.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_util.h>

#include <vcl_cassert.h>

#if 0 // unused static function
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
#endif // 0

rgrl_feature_point::
rgrl_feature_point( vnl_vector<double> const& loc )
  : rgrl_feature( loc )
{
}

rgrl_feature_point::
rgrl_feature_point( vnl_vector<double> const& loc, double scale )
  : rgrl_feature( loc, scale )
{
}

rgrl_feature_point::
rgrl_feature_point( unsigned size )
: rgrl_feature()
{
  location_.set_size( size );
}


unsigned int
rgrl_feature_point::
num_constraints() const
{
  return location_.size();
}

vnl_matrix<double> const&
rgrl_feature_point::
error_projector() const
{
  if ( !err_proj_.size() )
  {
    const unsigned m = location_.size();
    err_proj_.set_size( m, m );
    err_proj_.set_identity();
    err_proj_ /= vnl_math_sqr( scale_ );
  }

  return err_proj_;
}

vnl_matrix<double> const&
rgrl_feature_point::
error_projector_sqrt() const
{
  if ( !err_proj_sqrt_.size() )
  {
    const unsigned m = location_.size();
    err_proj_sqrt_.set_size( m, m );
    err_proj_sqrt_.set_identity();
    err_proj_sqrt_ /= scale_;
  }

  return err_proj_sqrt_;
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
  if ( this->scale_ > 0.0 )
    result->scale_ = this->transform_scale( xform );

  return result_sptr;
}

double
rgrl_feature_point::
transform_scale( rgrl_transformation const& xform ) const
{
#if 1 /*old method, rely on scaling factors*/
  // transform scale
  vnl_vector<double> const& scaling = xform.scaling_factors();
  const unsigned dim = this->location_.size();
  // use default value of 1.0
  double scale = 1.0;

  if ( this->scale_ > 0.0 && scaling.size() == dim )
  {
    // "average" them
    if ( dim == 2 )
      scale = vcl_sqrt( scaling[0]*scaling[1] ) * this->scale_;
    else {
      double prod_scale=1;
      for ( unsigned i=0; i < dim; ++i )
        prod_scale *= scaling[i];
      scale = vcl_exp( vcl_log(prod_scale) / double(dim) ) * this->scale_;
    }
  } else if ( this-> scale_ != 1.0 ) {
    WarningMacro( "This feature has non-zero scale value, but transformation has no scaling factors."
                  << "The scale of transformed features is NOT set." );
  }
  return scale;

#else  /*new method, though Jacobian of the transformation*/

  const unsigned dim = this->location_.size();

  // get jacobian
  vnl_matrix<double> jac;
  xform.jacobian_wrt_loc( jac, this->location() );
  assert( jac.cols() == dim );

  // each column in this jac matrix represents the change 
  // on fixed image coordindate given one change on one of the axes of
  // the moving image. 
  // Thus, the magnitude of this column vector represents the scale change 
  // on this particular axis. 
  // Take the average of magnitude on all the axes
  double cumulative_scale_change = 0.0;
  for( unsigned i=0; i<jac.cols(); ++i ) {
    
    double sqr_mag = 0.0;
    for( unsigned j=0; j<jac.rows(); ++j )
      sqr_mag += vnl_math_sqr( jac(j,i) );
    
    cumulative_scale_change += vcl_sqrt( sqr_mag );
  }
  cumulative_scale_change /= jac.cols();

  return cumulative_scale_change * this->scale();
#endif
}

//:  Compute the signature weight between two features.
double
rgrl_feature_point::
absolute_signature_weight( rgrl_feature_sptr other ) const
{
  //if other is invalid
  if ( !other )  return 0.0;

  rgrl_feature_point* pt_ptr = rgrl_cast<rgrl_feature_point*>(other);
  assert( pt_ptr );

  double scale_wgt = 1;
  if ( this->scale_ && pt_ptr->scale_ ) {
    if ( this->scale_ >= pt_ptr->scale_ )
      scale_wgt = pt_ptr->scale_ / this->scale_;
    else
      scale_wgt = this->scale_ / pt_ptr->scale_;
    // the weight change is too gradual, make it more steep
    // scale_wgt = scale_wgt * scale_wgt;
  }

  return  vcl_sqrt(scale_wgt);
}

//: write out feature
void
rgrl_feature_point::
write( vcl_ostream& os ) const
{
  // tag
  os << "POINT" << vcl_endl;

  // dim
  os << location_.size() << vcl_endl;

  // atributes
  os << location_ << "    " << scale_ << vcl_endl;
}

//: read in feature
bool
rgrl_feature_point::
read( vcl_istream& is, bool skip_tag )
{
  if ( !skip_tag )
  {
    // skip empty lines
    rgrl_util_skip_empty_lines( is );

    vcl_string str;
    vcl_getline( is, str );

    // The token should appear at the beginning of line
    if ( str.find( "POINT" ) != 0 ) {
      WarningMacro( "The tag is not POINT. reading is aborted.\n" );
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

  return true;
}
