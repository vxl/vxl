//:
// \file
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>

#include "rgrl_trans_homography2d.h"
#include "rgrl_util.h"

rgrl_trans_homography2d::
rgrl_trans_homography2d()
  : H_( vnl_matrix<double>( 3, 3, vnl_matrix_identity ) ),
    from_centre_( 2, 0.0 )
{}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H,
                        vnl_matrix<double> const& covar )
  : rgrl_transformation( covar ),
    H_( H ),
    from_centre_( 2, 0.0 )
{
  assert( H_.rows() == 3 );
  assert( H_.cols() == 3 );
  assert( covar_.rows() == covar_.cols() );
  assert( covar_.rows() == 3 );
}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H )
  :  H_( H ),
     from_centre_( 2, 0.0 )
{}

rgrl_trans_homography2d::
rgrl_trans_homography2d( vnl_matrix<double> const& H,
                        vnl_matrix<double> const& covar,
                        vnl_vector<double> const& from_centre,
                        vnl_vector<double> const& to_centre )
  : rgrl_transformation( covar )
{
  //Uncenter the H_ = to_matrix^-1 * H * from_matrix
  //
  vnl_matrix<double> to_inv(3,3, vnl_matrix_identity);
  to_inv(0,2) = to_centre[0];
  to_inv(1,2) = to_centre[1];

  vnl_matrix<double> from_matrix(3,3, vnl_matrix_identity);
  from_matrix(0,2) = -from_centre[0];
  from_matrix(1,2) = -from_centre[1];

  H_ = to_inv * H * from_matrix;
  from_centre_ = from_centre;
}


vnl_matrix<double>
rgrl_trans_homography2d::
transfer_error_covar( vnl_vector<double> const& from_loc  ) const
{
  assert ( is_covar_set() );
  assert ( from_loc.size() ==2 );

  vnl_matrix<double> J_wrt_h( 2, 9, 0.0 );
  vnl_vector<double> h_0 = H_.get_row(0);
  vnl_vector<double> h_1 = H_.get_row(1);
  vnl_vector<double> h_2 = H_.get_row(2);
  vnl_vector_fixed<double,3> p(from_loc[0]-from_centre_[0], from_loc[1]-from_centre_[1], 1);
  double h_0Tp = dot_product(h_0, p);
  double h_1Tp = dot_product(h_1, p);
  double h_2Tp_inv = 1/dot_product(h_2, p);

  J_wrt_h(0,0) = J_wrt_h(1,3) = p(0);
  J_wrt_h(0,1) = J_wrt_h(1,4) = p(1);
  J_wrt_h(0,2) = J_wrt_h(1,5) = p(2);

  J_wrt_h(0,6) = -1*p(0)*h_0Tp*h_2Tp_inv;
  J_wrt_h(0,7) = -1*p(1)*h_0Tp*h_2Tp_inv;
  J_wrt_h(0,8) = -1*p(2)*h_0Tp*h_2Tp_inv;

  J_wrt_h(1,6) = -1*p(0)*h_1Tp*h_2Tp_inv;
  J_wrt_h(1,7) = -1*p(1)*h_1Tp*h_2Tp_inv;
  J_wrt_h(1,8) = -1*p(2)*h_1Tp*h_2Tp_inv;

  J_wrt_h(1,8) *= h_2Tp_inv;

  return J_wrt_h * covar_ * J_wrt_h.transpose();
}

//: Inverse map using pseudo-inverse of H_.
void
rgrl_trans_homography2d::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  vnl_svd<double> svd_h(H_);
  vnl_matrix<double> H_inv = svd_h.inverse();
  vnl_vector<double> homo_to(3,1), homo_from(3,1);
  homo_to[0] = to[0];
  homo_to[1] = to[1];
  homo_from = H_inv*homo_to;
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
  const double epsilon = 0.01;
  const double eps_squared = epsilon*epsilon;
  int t=0;
  const int max_t = 50;  //  Generally, only one or two iterations should be needed.
  assert (to.size() == from.size());
  vnl_vector<double> to_est = this->map_location(from);
  vnl_matrix<double> approx_A_inv;
  vnl_matrix<double> J;
  vnl_vector<double> homo_from_delta(3,1);
  vnl_vector<double> from_delta(2,1);

  while ( vnl_vector_ssd(to, to_est) > eps_squared && t<max_t )
  {
    ++t;

    // compute the inverse of the approximated affine from the jacobian
    J = homo_jacobian( from );
    vnl_svd<double> svd(J);
    approx_A_inv = svd.inverse();

    // Increase "from" by approx_A^-1*(to-to_est).  "homo_from_delta"
    // provides the correct direction, but its magnitude is
    // arbitrary. To get around the problem, we take (to -
    // to_est).two_norm() as an indication as the magnitude.

    homo_from_delta = approx_A_inv * (to - to_est);
    if (homo_from_delta[2] < 0)
      homo_from_delta *= -1;
    from_delta[0] = homo_from_delta[0];
    from_delta[1] = homo_from_delta[1];
    from += from_delta.normalize()*(to - to_est).two_norm()*0.95;
    to_est = this->map_location(from);
  }
  if ( t > max_t )
    DebugMacro( 0, " rgrl_trans_homography2d::inv_map()--  no convergence\n");

  if ( initialize_next ) {
    if ( t == 0 ) { //approx_A_inv not yet calculated
      J = homo_jacobian( from );
      vnl_svd<double> svd(J);
      approx_A_inv = svd.inverse();
    }
    vnl_vector<double> homo_from_delta(3,1);
    homo_from_delta = approx_A_inv * to_delta;
    if (homo_from_delta[2] < 0)
      homo_from_delta *= -1;
    from_next_est[0] = from[0] + homo_from_delta[0]*to_delta.two_norm();
    from_next_est[1] = from[1] + homo_from_delta[1]*to_delta.two_norm();
  }
}

//: Return an inverse transformation of the uncentered transform
rgrl_transformation_sptr 
rgrl_trans_homography2d::
inverse_transform() const
{
  vnl_svd<double> svd_h(H_);
  vnl_matrix<double> H_inv = svd_h.inverse();
  rgrl_transformation_sptr result = new rgrl_trans_homography2d( H_inv );

  const unsigned m = scaling_factors_.size();
  if( m > 0 ) {
    vnl_vector<double> scaling( m );
    for( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}

//: Return the jacobian of the transform.
vnl_matrix<double>
rgrl_trans_homography2d::
homo_jacobian( vnl_vector<double> const& from_loc ) const
{
  // Let h_i be the i_th row of H_, and p be the homogeneous vector of from_loc
  // f_0(p) = h_0.p/h_2.p
  // f_1(p) = h_1.p/h_2.p
  // The jacobian is a 2x3 matrix with entries
  // [d(f_0)/dx   d(f_0)/dy   d(f_0)/dw;
  //  d(f_1)/dx   d(f_1)/dy   d(f_1)/dw]
  //

  vnl_vector_fixed<double,3> p(from_loc[0], from_loc[1], 1);
  vnl_vector<double> h_0 = H_.get_row(0);
  vnl_vector<double> h_1 = H_.get_row(1);
  vnl_vector<double> h_2 = H_.get_row(2);
  double inv_mapped_w = 1/dot_product(h_2, p);
  double mapped_x = dot_product(h_0, p)*inv_mapped_w;
  double mapped_y = dot_product(h_1, p)*inv_mapped_w;

  vnl_matrix<double> jacobian(2, 3);
  jacobian.set_row(0, h_0-mapped_x*h_2 );
  jacobian.set_row(1, h_1-mapped_y*h_2 );

  return inv_mapped_w*jacobian;
}

//: Return the jacobian of the transform.
vnl_matrix<double>
rgrl_trans_homography2d::
jacobian( vnl_vector<double> const& from_loc ) const
{
  // The jacobian is a 2x3 matrix with entries
  // [d(f_0)/dx   d(f_0)/dy;
  //  d(f_1)/dx   d(f_1)/dy]
  //

  double mapped_w = H_(2,0)*from_loc[0] + H_(2,1)*from_loc[1] + H_(2,2);

  vnl_matrix<double> jacobian(2, 2);
  // w/ respect to x
  jacobian(0,0) = H_(0,0)*( H_(2,1)*from_loc[1]+H_(2,2) ) - H_(2,0)*( H_(0,1)*from_loc[1] + H_(0,2) );
  jacobian(1,0) = H_(1,0)*( H_(2,1)*from_loc[1]+H_(2,2) ) - H_(2,0)*( H_(1,1)*from_loc[1] + H_(1,2) );
  // w/ respect to y
  jacobian(0,1) = H_(0,1)*( H_(2,0)*from_loc[0]+H_(2,2) ) - H_(2,1)*( H_(0,0)*from_loc[0] + H_(0,2) );
  jacobian(1,1) = H_(1,1)*( H_(2,0)*from_loc[0]+H_(2,2) ) - H_(2,1)*( H_(1,0)*from_loc[0] + H_(1,2) );
  
  jacobian *= (1/(mapped_w*mapped_w));
  return jacobian;
}

// for output UNCENTERED transformation, with the origin as the center.
void
rgrl_trans_homography2d::
write(vcl_ostream& os ) const
{
  vnl_vector<double> origin(from_centre_.size(), 0.0);
  // tag
  os << "HOMOGRAPHY2D\n"
  // parameters
     << 2 << vcl_endl
     << H_<< origin << vcl_endl;
}

// for input
void
rgrl_trans_homography2d::
read(vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "HOMOGRAPHY2D" ) != 0 ) {
    WarningMacro( "The tag is not HOMOGRAPHY2D. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    H_.set_size( 3,3 );
    from_centre_.set_size( dim );
    is >> H_ >> from_centre_;
  }
}

void
rgrl_trans_homography2d::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to ) const
{
  to.set_size(2);
  // convert "from" to homogeneous co-cord
  vnl_vector_fixed<double,3> h_from(from[0], from[1], 1);
  vnl_vector<double> h_to = H_*h_from.as_ref();
  to[0] = h_to[0]/h_to[2];
  to[1] = h_to[1]/h_to[2];
}

void
rgrl_trans_homography2d::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
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
scale_by( double /*scale*/ ) const
{
  assert(!"rgrl_trans_homography2d::scale_by() is not implemented");
  return 0;
}

