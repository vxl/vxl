#include "rgrl_trans_homography2d.h"
//:
// \file
#include <cassert>
#include <vcl_compiler.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>

#include <rgrl/rgrl_util.h>

rgrl_trans_homography2d::
rgrl_trans_homography2d()
  : H_( 0.0 ),
    from_centre_( 0.0, 0.0 ),
    to_centre_( 0.0, 0.0 )
{}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H,
                         vnl_matrix<double> const& covar )
  : rgrl_transformation( covar ),
    H_( H ),
    from_centre_( 0.0, 0.0 ),
    to_centre_( 0.0, 0.0 )
{
  assert( H.rows() == 3 );
  assert( H.cols() == 3 );
  assert( covar_.rows() == covar_.cols() );
  // assert( covar_.rows() == 3 );
}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H )
  : H_( H ),
    from_centre_( 0.0, 0.0 ),
    to_centre_( 0.0, 0.0 )
{}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H,
                         vnl_matrix<double> const& covar,
                         vnl_vector<double> const& from_centre,
                         vnl_vector<double> const& to_centre )
  : rgrl_transformation( covar ),
    from_centre_( from_centre ),
    to_centre_( to_centre )
{
  assert( to_centre.size() == 2 );

  //Uncenter the H_ = to_matrix^-1 * H * from_matrix
  //
  //vnl_matrix<double> to_inv(3,3, vnl_matrix_identity);
  //to_inv(0,2) = to_centre[0];
  //to_inv(1,2) = to_centre[1];
  //
  //vnl_matrix<double> from_matrix(3,3, vnl_matrix_identity);
  //from_matrix(0,2) = -from_centre[0];
  //from_matrix(1,2) = -from_centre[1];
  //
  //H_ = to_inv * H * from_matrix;
  //from_centre_ = from_centre;

  H_ = H;
}

vnl_matrix_fixed<double, 3, 3>
rgrl_trans_homography2d::
H() const
{
  return uncenter_H_matrix();
}

vnl_matrix_fixed<double, 3, 3>
rgrl_trans_homography2d::
uncenter_H_matrix( ) const
{
  vnl_matrix_fixed<double, 3, 3> H;

  // uncenter To image
  vnl_matrix_fixed<double, 3, 3> to_inv;
  to_inv.set_identity();
  to_inv(0,2) = to_centre_[0];
  to_inv(1,2) = to_centre_[1];

  // uncenter From image
  vnl_matrix_fixed<double, 3, 3> from_matrix;
  from_matrix.set_identity();
  from_matrix(0,2) = -from_centre_[0];
  from_matrix(1,2) = -from_centre_[1];

  H = to_inv * H_ * from_matrix;

  return H;
}

vnl_matrix<double>
rgrl_trans_homography2d::
transfer_error_covar( vnl_vector<double> const& from_loc ) const
{
  assert ( is_covar_set() );
  assert ( from_loc.size() ==2 );

  vnl_matrix_fixed<double, 2, 9 > jac;
  vnl_matrix_fixed<double, 3, 9 > jf(0.0); // homogeneous coordinate
  vnl_matrix_fixed<double, 2, 3 > jg(0.0); // inhomo, [u/w, v/w]^T
  vnl_double_3 from_homo( from_loc[0]-from_centre_[0],
                          from_loc[1]-from_centre_[1],
                          1.0 );
  // transform coordinate
  vnl_double_3 mapped_homo = H_ * from_homo;

  // homogeneous coordinate w.r.t homography parameters
  jf(0,0) = jf(1,3) = jf(2,6) = from_homo[0]; // x
  jf(0,1) = jf(1,4) = jf(2,7) = from_homo[1]; // y
  jf(0,2) = jf(1,5) = jf(2,8) = 1.0;

  // derivatives w.r.t division
  jg(0,0) = 1.0/mapped_homo[2];
  jg(0,2) = -mapped_homo[0]/vnl_math::sqr(mapped_homo[2]);
  jg(1,1) = 1.0/mapped_homo[2];
  jg(1,2) = -mapped_homo[1]/vnl_math::sqr(mapped_homo[2]);

  // since Jab_g(f(p)) = Jac_g * Jac_f
  jac = jg * jf;

  return jac * covar_ * jac.transpose();
}

//: Inverse map using pseudo-inverse of H_.
void
rgrl_trans_homography2d::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  vnl_double_3 homo_to(to[0], to[1], 1.0);

  // apply inverse homography
  vnl_double_3 homo_from = vnl_inverse(uncenter_H_matrix()) * homo_to;

  from[0] = homo_from[0]/homo_from[2];
  from[1] = homo_from[1]/homo_from[2];

  return;
}

//:  Inverse map with an initial guess
void
rgrl_trans_homography2d::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  vnl_double_3 homo_to(to[0]-to_centre_[0], to[1]-to_centre_[1], 1.0);

  // apply inverse homography
  vnl_matrix_fixed<double, 3, 3> Hinv = vnl_inverse(H_);
  vnl_double_3 homo_from = Hinv * homo_to;

  from[0] = homo_from[0]/homo_from[2]+from_centre_[0];
  from[1] = homo_from[1]/homo_from[2]+from_centre_[1];

  if ( initialize_next ) {
    homo_to[0] += to_delta[0];
    homo_to[1] += to_delta[1];
    homo_from = Hinv * homo_to;
    from_next_est[0] = homo_from[0]/homo_from[2]+from_centre_[0];
    from_next_est[1] = homo_from[1]/homo_from[2]+from_centre_[1];
  }
}

//: Return an inverse transformation of the uncentered transform
rgrl_transformation_sptr
rgrl_trans_homography2d::
inverse_transform() const
{
  vnl_matrix_fixed<double,3,3> H_inv = vnl_inverse(H_);
  rgrl_transformation_sptr result =
    new rgrl_trans_homography2d( H_inv.as_ref(), vnl_matrix<double>(), to_centre_.as_ref(), from_centre_.as_ref() );

  const unsigned m = scaling_factors_.size();
  if ( m > 0 ) {
    vnl_vector<double> scaling( m );
    for ( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}

//: Return the jacobian of the transform.
vnl_matrix_fixed<double,2,3>
rgrl_trans_homography2d::
homo_jacobian( vnl_vector_fixed<double,2> const& from_loc ) const
{
  // Let h_i be the i_th row of H_, and p be the homogeneous vector of from_loc
  // f_0(p) = h_0.p/h_2.p
  // f_1(p) = h_1.p/h_2.p
  // The jacobian is a 2x3 matrix with entries
  // [d(f_0)/dx   d(f_0)/dy   d(f_0)/dw;
  //  d(f_1)/dx   d(f_1)/dy   d(f_1)/dw]
  //

  vnl_vector_fixed<double,3> p(from_loc[0]-from_centre_[0], from_loc[1]-from_centre_[1], 1.0);
  vnl_vector_fixed<double,3> h_0 = H_.get_row(0);
  vnl_vector_fixed<double,3> h_1 = H_.get_row(1);
  vnl_vector_fixed<double,3> h_2 = H_.get_row(2);
  double inv_mapped_w = 1.0/dot_product(h_2, p);
  double mapped_x = dot_product(h_0, p)*inv_mapped_w;
  double mapped_y = dot_product(h_1, p)*inv_mapped_w;

  vnl_matrix_fixed<double,2,3> jacobian;
  jacobian.set_row(0, h_0-mapped_x*h_2 );
  jacobian.set_row(1, h_1-mapped_y*h_2 );

  return inv_mapped_w*jacobian;
}

//: Return the jacobian of the transform.
void
rgrl_trans_homography2d::
jacobian_wrt_loc( vnl_matrix<double>& jacobian, vnl_vector<double> const& from_loc ) const
{
  // The jacobian is a 2x2 matrix with entries
  // [d(f_0)/dx   d(f_0)/dy;
  //  d(f_1)/dx   d(f_1)/dy]
  //
  vnl_double_2 centered_from = from_loc;
  centered_from -= from_centre_;

  double mapped_w = H_(2,0)*centered_from[0] + H_(2,1)*centered_from[1] + H_(2,2);

  jacobian.set_size(2, 2);
  // w/ respect to x
  jacobian(0,0) = H_(0,0)*( H_(2,1)*centered_from[1]+H_(2,2) ) - H_(2,0)*( H_(0,1)*centered_from[1] + H_(0,2) );
  jacobian(1,0) = H_(1,0)*( H_(2,1)*centered_from[1]+H_(2,2) ) - H_(2,0)*( H_(1,1)*centered_from[1] + H_(1,2) );
  // w/ respect to y
  jacobian(0,1) = H_(0,1)*( H_(2,0)*centered_from[0]+H_(2,2) ) - H_(2,1)*( H_(0,0)*centered_from[0] + H_(0,2) );
  jacobian(1,1) = H_(1,1)*( H_(2,0)*centered_from[0]+H_(2,2) ) - H_(2,1)*( H_(1,0)*centered_from[0] + H_(1,2) );

  jacobian *= (1/(mapped_w*mapped_w));
}

// for output CENTERED transformation
void
rgrl_trans_homography2d::
write(std::ostream& os ) const
{
  //vnl_vector<double> origin(from_centre_.size(), 0.0);

  // tag
  os << "HOMOGRAPHY2D\n"
  // parameters
     << 2 << '\n'
     << H_ << from_centre_ << "  " << to_centre_ << std::endl;

  // parent
  rgrl_transformation::write( os );
}

// for input
bool
rgrl_trans_homography2d::
read(std::istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  std::string str;
  std::getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "HOMOGRAPHY2D" ) != 0 ) {
    WarningMacro( "The tag is not HOMOGRAPHY2D. reading is aborted.\n" );
    return false;
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    is >> H_ >> from_centre_ >> to_centre_;
  }

  // parent
  return is.good() && rgrl_transformation::read( is );
}

void
rgrl_trans_homography2d::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to  ) const
{
  to.set_size(2);
  // convert "from" to homogeneous co-cord
  vnl_vector_fixed<double,3> h_from(from[0]-from_centre_[0], from[1]-from_centre_[1], 1);
  vnl_vector_fixed<double,3> h_to = H_*h_from;
  to[0] = h_to[0]/h_to[2] + to_centre_[0];
  to[1] = h_to[1]/h_to[2] + to_centre_[1];
}

void
rgrl_trans_homography2d::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir  ) const
{
  assert ( from_loc.size() == 2 );
  assert ( from_dir.size() == 2 );
  vnl_vector<double> to_loc_begin, to_loc_end;
  this->map_loc(from_loc, to_loc_begin);
  this->map_loc(from_loc+from_dir, to_loc_end);

  to_dir = to_loc_end - to_loc_begin;
  to_dir.normalize();
}

rgrl_transformation_sptr
rgrl_trans_homography2d::
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
  vnl_vector_fixed<double,2> from = from_centre_ * scale;
  vnl_vector_fixed<double,2> to = to_centre_ * scale;

  rgrl_transformation_sptr xform
    =  new rgrl_trans_homography2d( new_H.as_ref(),
                                    vnl_matrix<double>(),
                                    from.as_ref(), to.as_ref() );
  xform->set_scaling_factors( this->scaling_factors() );
  return xform;
}

//: make a clone copy
rgrl_transformation_sptr
rgrl_trans_homography2d::
clone() const
{
  return new rgrl_trans_homography2d( *this );
}
