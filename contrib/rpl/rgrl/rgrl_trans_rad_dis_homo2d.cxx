//:
// \file
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_transpose.h>

#include "rgrl_trans_rad_dis_homo2d.h"
#include "rgrl_util.h"

// NOTE: the first parameter is always the destination in this file
//

// map homography
static
inline
void
map_inhomo_point( vnl_double_2& mapped, vnl_matrix_fixed<double, 3, 3> const& H, vnl_vector<double> const& loc )
{
  vnl_double_3 homo_from( loc[0], loc[1], 1 );
  vnl_double_3 homo_to = H * homo_from;
  mapped[0] = homo_to[0]/homo_to[2];
  mapped[1] = homo_to[1]/homo_to[2];
}

//: Return the jacobian of the transform.
static
void
homo_wrt_loc( vnl_matrix_fixed<double, 2, 2>&        jac_loc,
              vnl_matrix_fixed<double, 3, 3> const&  H,
              vnl_vector_fixed<double, 2>    const&  from_loc )
{
  // The jacobian is a 2x2 matrix with entries
  // [d(f_0)/dx   d(f_0)/dy;
  //  d(f_1)/dx   d(f_1)/dy]
  //
  const double mapped_w = H(2,0)*from_loc[0] + H(2,1)*from_loc[1] + H(2,2);

  // w/ respect to x
  jac_loc(0,0) = H(0,0)*( H(2,1)*from_loc[1]+H(2,2) ) - H(2,0)*( H(0,1)*from_loc[1] + H(0,2) );
  jac_loc(1,0) = H(1,0)*( H(2,1)*from_loc[1]+H(2,2) ) - H(2,0)*( H(1,1)*from_loc[1] + H(1,2) );
  // w/ respect to y
  jac_loc(0,1) = H(0,1)*( H(2,0)*from_loc[0]+H(2,2) ) - H(2,1)*( H(0,0)*from_loc[0] + H(0,2) );
  jac_loc(1,1) = H(1,1)*( H(2,0)*from_loc[0]+H(2,2) ) - H(2,1)*( H(1,0)*from_loc[0] + H(1,2) );

  jac_loc *= (1/(mapped_w*mapped_w));
}


static
void
homo_wrt_h( vnl_matrix_fixed<double, 2, 9>&        jac_h,
            vnl_matrix_fixed<double, 3, 3> const&  H,
            vnl_vector_fixed<double, 2>    const&  from_loc )
{
  vnl_matrix_fixed<double, 3, 9 > jf(0.0); // homogeneous coordinate
  vnl_matrix_fixed<double, 2, 3 > jg(0.0); // inhomo, [u/w, v/w]^T

  // transform coordinate
  vnl_double_3 from_homo( from_loc[0], from_loc[1], 1 );
  vnl_double_3 mapped_homo = H * from_homo;

  // homogeneous coordinate w.r.t homography parameters
  jf(0,0) = jf(1,3) = jf(2,6) = from_homo[0]; // x
  jf(0,1) = jf(1,4) = jf(2,7) = from_homo[1]; // y
  jf(0,2) = jf(1,5) = jf(2,8) = 1.0;

  // derivatives w.r.t division
  jg(0,0) = 1.0/mapped_homo[2];
  jg(0,2) = -mapped_homo[0]/vnl_math_sqr(mapped_homo[2]);
  jg(1,1) = 1.0/mapped_homo[2];
  jg(1,2) = -mapped_homo[1]/vnl_math_sqr(mapped_homo[2]);

  // Apply chain rule: Jab_g(f(p)) = Jac_g * Jac_f
  jac_h = jg * jf;
}

// distort image coordinate
static
inline
void
distort( vnl_double_2& dis_loc, vnl_double_2 const& true_loc, double k1 )
{
  const double c = 1 + k1 * true_loc.squared_magnitude();
  dis_loc = c * true_loc;
}

#if 0 // ***This is incorrect implementation***
// undistort image coordinate
static
inline
void
undistort( vnl_double_2& true_loc, vnl_double_2 const& dis_loc, double k1 )
{
  const double c = 1 + k1 * dis_loc.squared_magnitude();
  true_loc = dis_loc;
}
#endif // 0

// jacobian w.r.t k1 parameter
static
inline
void
distort_wrt_k1( vnl_double_2& jac_k1, vnl_double_2 const& true_loc )
{
  const double c = true_loc.squared_magnitude();
  jac_k1 = c * true_loc;
}


// jacobian w.r.t location
static
inline
void
distort_wrt_loc( vnl_double_2x2& jac_loc, vnl_double_2 const& true_loc, double k1 )
{
  const double c = 1 + k1 * true_loc.squared_magnitude();

  jac_loc(0,0) = c + 2*k1*vnl_math_sqr(true_loc[0]);
  jac_loc(1,1) = c + 2*k1*vnl_math_sqr(true_loc[1]);
  jac_loc(0,1) = jac_loc(1,0) = 2 * k1 * true_loc[0] * true_loc[1];
}


rgrl_trans_rad_dis_homo2d::
rgrl_trans_rad_dis_homo2d()
  : H_( 0.0 ),
    from_centre_( 0.0, 0.0 ),
    to_centre_( 0.0, 0.0 ),
    k1_from_( 0.0 ),
    k1_to_( 0.0 )
{}

rgrl_trans_rad_dis_homo2d::
rgrl_trans_rad_dis_homo2d( vnl_matrix<double> const& H,
                           double k1_from,
                           double k1_to,
                           vnl_vector<double> const& from_centre,
                           vnl_vector<double> const& to_centre )
 :  H_( H ),
    from_centre_( from_centre ),
    to_centre_( to_centre ),
    k1_from_( k1_from ),
    k1_to_( k1_to )
{}

rgrl_trans_rad_dis_homo2d::
rgrl_trans_rad_dis_homo2d( vnl_matrix<double> const& H,
                           double k1_from,
                           double k1_to,
                           vnl_matrix<double> const& covar,
                           vnl_vector<double> const& from_centre,
                           vnl_vector<double> const& to_centre )
 :  rgrl_transformation( covar ),
    H_( H ),
    from_centre_( from_centre ),
    to_centre_( to_centre ),
    k1_from_( k1_from ),
    k1_to_( k1_to )
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
}

vnl_matrix<double>
rgrl_trans_rad_dis_homo2d::
transfer_error_covar( vnl_vector<double> const& from  ) const
{
  assert ( is_covar_set() );
  assert ( from.size() ==2 );

  // Step 1. undistorted from coordinate and compute apu/apd
  vnl_double_2 dis_from_loc( from[0]-from_centre_[0], from[1]-from_centre_[1] );
  vnl_double_2 true_from_loc;
  // make the trick: *distort*
  distort( true_from_loc, dis_from_loc, k1_from_ );
  vnl_double_2x2 pu_pd;
  distort_wrt_loc( pu_pd, dis_from_loc, k1_from_ );
  vnl_double_2 pu_k1_from;
  distort_wrt_k1( pu_k1_from, dis_from_loc );

  // Step 2. homography transformation
  vnl_double_2 true_to_loc;
  map_inhomo_point( true_to_loc, H_, true_from_loc );
  vnl_double_2x2 qu_pu;
  homo_wrt_loc( qu_pu, H_, true_from_loc );
  vnl_matrix_fixed<double, 2, 9> qu_h;
  homo_wrt_h( qu_h, H_, true_from_loc );

  // Step 3. distorted To coodinates
  vnl_double_2x2 qd_qu;
  distort_wrt_loc( qd_qu, true_to_loc, k1_to_ );
  vnl_double_2 qd_k1_to;
  distort_wrt_k1( qd_k1_to, true_to_loc );

  // Steop 4. apply chain rule
  vnl_matrix_fixed<double, 2, 9> qd_h = qd_qu * qu_h;
  vnl_double_2  qd_k1_from = qd_qu * qu_pu * pu_k1_from;

  // assemble jacobian matrix
  vnl_matrix<double> jac(2, 11, 0.0);
  // wrt. k1_from
  jac(0,9) = qd_k1_from[0];
  jac(1,9) = qd_k1_from[1];

  // wrt. k1_to
  jac(0,10) = qd_k1_to[0];
  jac(1,10) = qd_k1_to[1];

  // wrt. H
  for ( unsigned i=0; i<2; ++i )
    for ( unsigned j=0; j<9; ++j )
      jac(i, j) = qd_h(i,j);

  return jac * covar_ * vnl_transpose( jac );
}

//: Inverse map using pseudo-inverse of H_.
void
rgrl_trans_rad_dis_homo2d::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  assert( ! "rgrl_trans_rad_dis_homo2d::inv_map( to, from ) is not implemented!!!" );
}

//: Return the jacobian of the transform.
void
rgrl_trans_rad_dis_homo2d::
jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from ) const
{
  // using chain rule:
  // aqd/apd = aqd/aqu * aqu/apu * apu/apd
  //

  jac.set_size(2, 2);

  // Step 1. undistorted from coordinate and compute apu/apd
  vnl_double_2 dis_from_loc( from[0]-from_centre_[0], from[1]-from_centre_[1] );
  vnl_double_2 true_from_loc;
  // make the trick: *distort*
  distort( true_from_loc, dis_from_loc, k1_from_ );
  vnl_double_2x2 pu_pd;
  distort_wrt_loc( pu_pd, dis_from_loc, k1_from_ );


  // Step 2. homography transformation
  vnl_double_2 true_to_loc;
  map_inhomo_point( true_to_loc, H_, true_from_loc );
  vnl_double_2x2 qu_pu;
  homo_wrt_loc( qu_pu, H_, true_from_loc );

  // Step 3. distorted To coodinates
  vnl_double_2x2 qd_qu;
  distort_wrt_loc( qd_qu, true_to_loc, k1_to_ );

  // Steop 4. put them together
  jac = qd_qu * qu_pu * pu_pd;
}

void
rgrl_trans_rad_dis_homo2d::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to ) const
{
  to.set_size(2);

  // Step 1. undistorted from coordinate
  vnl_double_2 dis_from_loc( from[0]-from_centre_[0], from[1]-from_centre_[1] );
  vnl_double_2 true_from_loc;
  // make the trick: *distort*
  distort( true_from_loc, dis_from_loc, k1_from_ );

  // Step 2. homography transformation
  vnl_double_2 true_to_loc;
  map_inhomo_point( true_to_loc, H_, true_from_loc );

  // Step 3. distorted To coodinates
  vnl_double_2 dis_to_loc;
  distort( dis_to_loc, true_to_loc, k1_to_ );

  // add center back
  to = dis_to_loc + to_centre_;
}

void
rgrl_trans_rad_dis_homo2d::
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
rgrl_trans_rad_dis_homo2d::
scale_by( double /*scale*/ ) const
{
  assert(!"rgrl_trans_rad_dis_homo2d::scale_by() is not implemented");
  return 0;
}

rgrl_transformation_sptr
rgrl_trans_rad_dis_homo2d::
inverse_transform( ) const
{
  assert ( ! "rgrl_trans_quadratic::inverse_transform() is not defined" );
  return 0;
}

// for output CENTERED transformation,
void
rgrl_trans_rad_dis_homo2d::
write(vcl_ostream& os ) const
{
  //vnl_vector<double> origin(from_centre_.size(), 0.0);

  // tag
  os << "HOMOGRAPHY2D_WITH_RADIAL_DISTORTION\n"
  // parameters
     << 2 << vcl_endl
     << H_
     << from_centre_ << "  " << to_centre_ << '\n'
     << k1_from_ << "  " <<  k1_to_
     << vcl_endl;

  // parent
  rgrl_transformation::write( os );
}

// for input
void
rgrl_trans_rad_dis_homo2d::
read(vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "HOMOGRAPHY2D_WITH_RADIAL_DISTORTION" ) != 0 ) {
    WarningMacro( "The tag is not HOMOGRAPHY2D. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    is >> H_ >> from_centre_ >> to_centre_ >> k1_from_ >> k1_to_;
  }

  // parent
  rgrl_transformation::read( is );
}

