#include "rgrl_trans_homo2d_proj_rad.h"
//:
// \file
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_homo2d_proj_rad::
rgrl_trans_homo2d_proj_rad()
  : rgrl_est_proj_rad_func<2,2>( 0, true )
  , H_( 0.0 )
  , rad_k_(0)
{
  const vnl_vector_fixed<double, 2> zeroc( 0, 0 );
  this->set_centres( zeroc, zeroc, zeroc );
}


rgrl_trans_homo2d_proj_rad::
rgrl_trans_homo2d_proj_rad( vnl_matrix_fixed<double, 3, 3> const& H,
                            vcl_vector<double>             const & k,
                            vnl_vector_fixed< double, 2 >  const & image_centre)
  : rgrl_est_proj_rad_func<2,2>( k.size(), true ),
    H_(H),
    rad_k_(k)
{
  const vnl_vector_fixed<double, 2> zeroc( 0, 0 );
  this->set_centres( zeroc, zeroc, image_centre );
}

rgrl_trans_homo2d_proj_rad::
rgrl_trans_homo2d_proj_rad( vnl_matrix_fixed<double, 3, 3> const& H,
                            vcl_vector<double>             const & k,
                            vnl_vector_fixed< double, 2 >  const & image_centre,
                            vnl_matrix<double> const& covar,
                            vnl_vector<double> const& from_centre,
                            vnl_vector<double> const& to_centre )
  : rgrl_transformation( covar ),
    rgrl_est_proj_rad_func<2,2>( k.size(), true ),
    H_(H),
    rad_k_(k)
{
  this->set_centres( from_centre, to_centre, image_centre );
}

void
rgrl_trans_homo2d_proj_rad::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to  ) const
{
  // use vnl_double_2 to reduce memory allocation
  vnl_double_2 pt = from;
  vnl_double_2 mapped;
  rgrl_est_proj_rad_func<2,2>::map_loc( mapped, H_, rad_k_, pt );
  to = mapped;
}

void
rgrl_trans_homo2d_proj_rad::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir  ) const
{
  assert ( from_loc.size() == 2 );
  assert ( from_dir.size() == 2 );

  const vnl_double_2 from_begin( from_loc );
  vnl_double_2 from_end( from_loc );
  from_end += from_dir;

  vnl_double_2 to_loc_begin, to_loc_end;
  rgrl_est_proj_rad_func<2,2>::map_loc( to_loc_begin, H_, rad_k_, from_begin );
  rgrl_est_proj_rad_func<2,2>::map_loc( to_loc_end,   H_, rad_k_, from_end );

  to_dir = to_loc_end - to_loc_begin;
  to_dir.normalize();
}

rgrl_transformation_sptr
rgrl_trans_homo2d_proj_rad::
scale_by( double scale ) const
{
  vnl_matrix_fixed<double,3,3> new_H( H_ );

  // scale
  new_H(0,2) *= scale;
  new_H(1,2) *= scale;

  // move the scale on the fixed coordinate,
  // and divide the 3rd row by this scale
  new_H(2,0) /= scale;
  new_H(2,1) /= scale;

  // normalize
  new_H /= new_H.fro_norm();

  // centers
  const vnl_vector_fixed<double,2> from = from_centre_ * scale;
  const vnl_vector_fixed<double,2> to = to_centre_ * scale;
  const vnl_vector_fixed<double,2> ic = image_centre_ * scale;

  // radial terms
  vcl_vector<double> radk = radial_params();
  const double sq_scale = scale*scale;

  double base = 1;
  for ( unsigned int i=0; i<radk.size(); ++i ) {
    base *= sq_scale;
    radk[i] /= base;
  }

  rgrl_transformation_sptr xform
    =  new rgrl_trans_homo2d_proj_rad( new_H,
                                       radk,
                                       ic,
                                       vnl_matrix<double>(),
                                       from, to );
  xform->set_scaling_factors( this->scaling_factors() );
  return xform;
}


vnl_matrix_fixed<double, 3, 3>
rgrl_trans_homo2d_proj_rad::
H( ) const
{
  return uncentre_proj( H_ );
}

vnl_matrix<double>
rgrl_trans_homo2d_proj_rad::
transfer_error_covar( vnl_vector<double> const& p ) const
{
  assert ( p.size() == 2 );
  vnl_matrix<double> jac;
  full_proj_rad_jacobian( jac, H_, rad_k_, p );
  return jac*covar_*vnl_transpose(jac);
}

//: Return the jacobian of the transform.
void
rgrl_trans_homo2d_proj_rad::
jacobian_wrt_loc( vnl_matrix<double>& jacobian, vnl_vector<double> const& from_loc ) const
{
  vnl_double_2x2 jac_loc;
  proj_jac_wrt_loc( jac_loc, H_, rad_k_, from_loc );
  jacobian = jac_loc;
}

// for output CENTERED transformation
void
rgrl_trans_homo2d_proj_rad::
write(vcl_ostream& os ) const
{
  // tag
  os << "HOMOGRAPHY2D+RADIAL\n"
  // parameters
     << H_ << vcl_endl;

  // radial terms
  os << rad_k_.size() << "  ";
  for ( unsigned i=0; i<rad_k_.size(); ++i )
    os << rad_k_[i] << ' ';

  os << '\n' << from_centre_ << "   " << to_centre_
     << '\n' << image_centre_ << "   " << centre_mag_norm_const_
     << vcl_endl;

  // parent
  rgrl_transformation::write( os );
}

// for input
bool
rgrl_trans_homo2d_proj_rad::
read(vcl_istream& is )
{
  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "HOMOGRAPHY2D+RADIAL" ) != 0 ) {
   return false;
  }

  // input global xform
  is >> H_;

  // input radial terms
  {
    int size = -1;
    is >> size;
    if ( size < 0 || !is.good() )
      return false;

    rad_k_.resize( size );
    for ( int i=0; i<size; ++i )
      is >> rad_k_[i];

    // set H_dof_
    camera_dof_ = size;
  }

  is >> from_centre_ >> to_centre_;
  is >> image_centre_;
  is >> centre_mag_norm_const_;

  // parent
  return is.good() && rgrl_transformation::read( is );
}

//: make a clone copy
rgrl_transformation_sptr
rgrl_trans_homo2d_proj_rad::
clone() const
{
  return new rgrl_trans_homo2d_proj_rad( *this );
}

//: return radial parameters
vcl_vector<double>
rgrl_trans_homo2d_proj_rad::
radial_params() const
{
  vcl_vector<double> ori_radk( rad_k_ );

  const double centre_mag_norm = centre_mag_norm_const();

  double base = 1;
  for ( unsigned i=0; i<ori_radk.size(); ++i ) {
    base *= centre_mag_norm;
    ori_radk[i] /= base;
  }
  return ori_radk;
}
