//:
// \file
#include <rgrl/rgrl_est_dis_homo2d_lm.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_rad_dis_homo2d.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_match_set.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_fastops.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_cassert.h>

static
inline 
void
H2h( vnl_matrix_fixed<double, 3, 3>const& H, vnl_vector<double>& h )
{
  // h is already size 11
  // h.set_size( 9 );
  for ( unsigned i=0; i<H.rows(); ++i )
    for ( unsigned j=0; j<H.cols(); ++j )
      h( i*3+j ) = H(i,j);
}

static
inline 
void
h2H( vnl_vector<double>const& h, vnl_matrix_fixed<double, 3, 3>& H )
{
  for ( unsigned i=0; i<3; ++i )
    for ( unsigned j=0; j<3; ++j )
      H(i,j) = h( i*3+j );
}

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



class rgrl_rad_dis_homo2d_func
: public vnl_least_squares_function
{
 public:
  //: ctor
  rgrl_rad_dis_homo2d_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                    int num_res, bool with_grad = true )
  : vnl_least_squares_function( 11, num_res, with_grad ? use_gradient : no_gradient ),
    matches_ptr_( &matches ),
    from_centre_(2, 0.0), to_centre_(2, 0.0)
  {      }

  void set_centres( vnl_vector<double> const& fc, vnl_vector<double> const& tc )
  {
    assert( fc.size() == 2 && tc.size() == 2 );
    from_centre_ = fc;
    to_centre_ = tc;
  }

  //: obj func value
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Jacobian
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

 protected:
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  rgrl_set_of<rgrl_match_set_sptr> const* matches_ptr_;
  vnl_double_2                            from_centre_, to_centre_;
};

void
rgrl_rad_dis_homo2d_func::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  vnl_double_2 true_mapped, true_from, from, dis_mapped;
  vnl_matrix_fixed<double,2,2> error_proj;
  double k1_from = x[9];
  double k1_to   = x[10];
  vnl_double_3x3  H;
  h2H( x, H );
  
  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )
    if ( (*matches_ptr_)[ms] != 0 ) { // if pointer is valid

      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);

      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {

        // map from point
        from = fi.from_feature()->location();
        from -= from_centre_;
        
        // Step 1.
        distort( true_from, from, k1_from );
        // Step 2.
        map_inhomo_point( true_mapped, H, true_from );
        // Step 3. 
        distort( dis_mapped, true_mapped, k1_to );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {

          vnl_double_2 to = ti.to_feature()->location();
          to -= to_centre_;
          error_proj = ti.to_feature()->error_projector();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          vnl_double_2 diff = error_proj * (dis_mapped - to);

          // fill in
          fx(ind) = wgt*diff[0];
          fx(ind+1) = wgt*diff[1];
          ind+=2;
        }
      }
  }

  // check
  assert( ind == get_number_of_residuals() );
}

void
rgrl_rad_dis_homo2d_func::
gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
  assert( jacobian.rows() == get_number_of_residuals() && jacobian.cols() == 11 );

  double k1_from = x[9];
  double k1_to   = x[10];
  vnl_double_3x3  H;
  h2H( x, H );

  vnl_double_3 homo;
  vnl_double_2x2 pu_pd; 
  vnl_double_2 pu_k1_from;
  vnl_double_2x2 qu_pu;
  vnl_matrix_fixed<double, 2, 9> qu_h;
  vnl_double_2x2 qd_qu;
  vnl_double_2 qd_k1_to;
  vnl_matrix_fixed<double, 2, 9> qd_h; 
  vnl_double_2  qd_k1_from;
  
  vnl_matrix_fixed<double,2,2> error_proj;

  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )

    if ( (*matches_ptr_)[ms] ) { // if pointer is valid

      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);

      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {

        // Step 1. undistorted from coordinate and compute apu/apd
        vnl_double_2 dis_from_loc = fi.from_feature()->location();
        dis_from_loc -= from_centre_;
        
        vnl_double_2 true_from_loc;
        // make the trick: *distort* 
        distort( true_from_loc, dis_from_loc, k1_from );
        distort_wrt_loc( pu_pd, dis_from_loc, k1_from );
        distort_wrt_k1( pu_k1_from, dis_from_loc );
        
        // Step 2. homography transformation
        vnl_double_2 true_to_loc;
        map_inhomo_point( true_to_loc, H, true_from_loc );
        homo_wrt_loc( qu_pu, H, true_from_loc );
        homo_wrt_h( qu_h, H, true_from_loc );
        
        // Step 3. distorted To coodinates
        distort_wrt_loc( qd_qu, true_to_loc, k1_to );
        distort_wrt_k1( qd_k1_to, true_to_loc );
        
        // Steop 4. apply chain rule
        qd_h = qd_qu * qu_h;
        qd_k1_from = qd_qu * qu_pu * pu_k1_from;
        
        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          //vnl_double_2 to = ti.to_feature()->location();
          error_proj = ti.to_feature()->error_projector();
          double const wgt = vcl_sqrt(ti.cumulative_weight());

          qd_k1_from = wgt * error_proj * qd_k1_from;
          qd_k1_to   = wgt * error_proj * qd_k1_to;
          qd_h       = wgt * error_proj * qd_h;
          
          // fill in
          for ( unsigned i=0; i<9; i++ ) {
            jacobian(ind, i)   = qd_h(0, i);
            jacobian(ind+1, i) = qd_h(1, i);
          }
          // k1_from
          jacobian(ind, 9)   = qd_k1_from[0];
          jacobian(ind+1, 9) = qd_k1_from[1];
          // k1_to
          jacobian(ind, 10)   = qd_k1_to[0];
          jacobian(ind+1, 10) = qd_k1_to[1];
          
          ind+=2;
        }
      }
  }
}


// --------------------------------------------------------------------

rgrl_est_dis_homo2d_lm::
rgrl_est_dis_homo2d_lm( vnl_vector<double> const& from_centre,
                        vnl_vector<double> const& to_centre,
                        bool with_grad )
  : from_centre_( from_centre),
    to_centre_( to_centre ),
    with_grad_( with_grad )
{
  assert( from_centre.size() == 2 && to_centre.size() == 2 );

  rgrl_estimator::set_param_dof( 10 );
}

rgrl_transformation_sptr
rgrl_est_dis_homo2d_lm::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  // get initialization
  vnl_matrix_fixed<double, 3, 3> init_H;
  double k1_from = 0, k1_to = 0;
  
  if ( cur_transform.is_type( rgrl_trans_rad_dis_homo2d::type_id() ) ) {
    
    rgrl_trans_rad_dis_homo2d const& trans = static_cast<rgrl_trans_rad_dis_homo2d const&>( cur_transform );
    init_H = trans.H();
    k1_from = trans.k1_from();
    k1_to   = trans.k1_to();

    // check centre
    assert( from_centre_ == trans.from_centre() );
    assert( to_centre_ == trans.to_centre() );
  
  } else if ( cur_transform.is_type( rgrl_trans_homography2d::type_id() ) ) {
    
    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( cur_transform );
    init_H = trans.H();

    // make the init homography as a CENTERED one
    // centered H_ = to_matrix * H * from_matrix^-1
    //
    vnl_matrix_fixed<double, 3, 3> to_trans;
    to_trans.set_identity();
    to_trans(0,2) = -to_centre_[0];
    to_trans(1,2) = -to_centre_[1];

    vnl_matrix_fixed<double, 3, 3> from_inv;
    from_inv.set_identity();
    from_inv(0,2) = from_centre_[0];
    from_inv(1,2) = from_centre_[1];

    init_H = to_trans * init_H * from_inv;
  
  }else {
    
    // cannot get initial transform
    // return NULL
    return 0;
  }

  // count the number of constraints/residuals
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  unsigned int tot_num = 0;
  for ( unsigned ms = 0; ms<matches.size(); ++ms )
    if ( matches[ms] ) { // if pointer is valid
      rgrl_match_set const& one_set = *(matches[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        tot_num += fi.size() * 2;  // each point provides two constraints
      }
    }

  DebugMacro( 3, "From center: " << from_centre_
               <<"  To center: " << to_centre_ << vcl_endl );

  // normalize H
  init_H /= init_H.array_two_norm();
  
  // convert to vector form
  vnl_vector<double> p( 11, 0.0 );
  H2h( init_H, p );
  p[9] = k1_from;
  p[10] = k1_to;
  
  // construct least square cost function
  rgrl_rad_dis_homo2d_func dis_homo_func( matches, tot_num, with_grad_ );
  dis_homo_func.set_centres( from_centre_, to_centre_ );

  vnl_levenberg_marquardt lm( dis_homo_func );
  lm.set_trace( true );
  //lm.set_check_derivatives( 3 );
  bool ret;
  if ( with_grad_ )
    ret = lm.minimize_using_gradient(p);
  else
    ret = lm.minimize_without_gradient(p);
  if ( !ret ) {
    WarningMacro( "Levenberg-Marquatt failed" );
    return 0;
  }
  // lm.diagnose_outcome(vcl_cout);

  // convert parameters back into matrix form
  h2H( p, init_H );
  k1_from = p[9];
  k1_to   = p[10];
  // normalize H
  init_H /= init_H.array_two_norm();
  
  // compute covariance
  // JtJ is INVERSE of jacobian
  // vnl_svd<double> svd( lm.get_JtJ(), 1e-4 );
  // Cannot use get_JtJ() because it is affected by the
  // scale in homography parameter vector
  // Thus, use the nomalized p vector to compute Jacobian again
  vnl_matrix<double> jac(tot_num, 11), jtj(11, 11);
  dis_homo_func.gradf( p, jac );
  //
  vnl_fastops::AtA( jtj, jac );
  // vcl_cout << "Jacobian:\n" << jac << "\n\nJtJ:\n" << jtj << "\n\nReal JtJ:\n" << jac.transpose() * jac << vcl_endl;

  // compute inverse
  //
  vnl_svd<double> svd( jtj, 1e-6 );
  DebugMacro(3, "SVD of JtJ: " << svd << vcl_endl);
  // the second least singular value shall be greater than 0
  // or Rank 11-1 = 10
  if ( svd.rank() < 10 ) {
    WarningMacro( "The covariance of homography ranks less than 8! ");
  }
  // pseudo inverse only use first 10 singular values
  vnl_matrix<double> covar ( svd.pinverse(10) );
  DebugMacro(3, "Covariance: " << covar << vcl_endl );

  DebugMacro(2, "null vector: " << svd.nullvector() << "   estimate: " << p << vcl_endl );

  return new rgrl_trans_rad_dis_homo2d( init_H, k1_from, k1_to, covar, from_centre_, to_centre_ );
}


rgrl_transformation_sptr
rgrl_est_dis_homo2d_lm::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_dis_homo2d_lm::
transformation_type() const
{
  return rgrl_trans_rad_dis_homo2d::type_id();
}
