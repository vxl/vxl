//:
// \file
#include <rgrl/rgrl_est_homo2d_lm.h>
#include <rgrl/rgrl_est_homography2d.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_internal_util.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_fastops.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_orthogonal_complement.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_cassert.h>

static
inline
void
H2h( vnl_matrix_fixed<double,3,3> const& H, vnl_vector<double>& h )
{
  for ( unsigned i=0; i<H.rows(); ++i )
    for ( unsigned j=0; j<H.cols(); ++j )
      h( i*3+j ) = H(i,j);
}

static
inline
void
h2H( vnl_vector<double> const& h, vnl_matrix_fixed<double,3,3>& H )
{
  for ( unsigned i=0; i<3; ++i )
    for ( unsigned j=0; j<3; ++j )
      H(i,j) = h( i*3+j );
}

inline
void
map_homo_point( vnl_double_3& mapped, vnl_vector<double> const& p, vnl_vector<double> const& loc )
{
  mapped[0] = p[0]*loc[0] + p[1]*loc[1] + p[2];
  mapped[1] = p[3]*loc[0] + p[4]*loc[1] + p[5];
  mapped[2] = p[6]*loc[0] + p[7]*loc[1] + p[8];
}

inline
void
map_inhomo_point( vnl_double_2& mapped, vnl_vector<double> const& p, vnl_vector<double> const& loc )
{
  vnl_double_3 tmp;
  map_homo_point( tmp, p, loc );
  mapped[0] = tmp[0]/tmp[2];
  mapped[1] = tmp[1]/tmp[2];
}

class rgrl_homo2d_func
: public vnl_least_squares_function
{
 public:
  //: ctor
  rgrl_homo2d_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                    int num_res, bool with_grad = true )
  : vnl_least_squares_function( 9, num_res, with_grad ? use_gradient : no_gradient ),
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
rgrl_homo2d_func::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  vnl_double_2 mapped;
  vnl_matrix_fixed<double,2,2> error_proj_sqrt;
  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )
    if ( (*matches_ptr_)[ms] != 0 ) { // if pointer is valid
      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        // map from point
        vnl_double_2 from = fi.from_feature()->location();
        from -= from_centre_;
        map_inhomo_point( mapped, x, from.as_ref() );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          vnl_double_2 to = ti.to_feature()->location();
          to -= to_centre_;
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          vnl_double_2 diff = error_proj_sqrt * (mapped - to);

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
rgrl_homo2d_func::
gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
  assert( jacobian.rows() == get_number_of_residuals() && jacobian.cols() == 9 );

  vnl_double_3 homo;
  vnl_matrix_fixed<double,2,2> error_proj_sqrt;
  vnl_matrix_fixed<double,2,9> base_jac, jac;
  vnl_matrix_fixed<double,3,9> jf(0.0); // homogeneous coordinate
  vnl_matrix_fixed<double,2,3> jg(0.0); // inhomo, [u/w, v/w]^T
  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )
    if ( (*matches_ptr_)[ms] ) { // if pointer is valid
      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        // map from point
        vnl_double_2 from = fi.from_feature()->location();
        from -= from_centre_;
        map_homo_point( homo, x, from.as_ref() );
        // homogeneous coordinate
        jf(0,0) = jf(1,3) = jf(2,6) = from[0]; // x
        jf(0,1) = jf(1,4) = jf(2,7) = from[1]; // y
        jf(0,2) = jf(1,5) = jf(2,8) = 1.0;
        // make division
        jg(0,0) = 1.0/homo[2];
        jg(0,2) = -homo[0]/vnl_math_sqr(homo[2]);
        jg(1,1) = 1.0/homo[2];
        jg(1,2) = -homo[1]/vnl_math_sqr(homo[2]);
        // since Jab_g(f(p)) = Jac_g * Jac_f
        base_jac = jg * jf;

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          //vnl_double_2 to = ti.to_feature()->location();
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          jac = error_proj_sqrt * base_jac;
          jac *= wgt;

          // fill in
          for ( unsigned i=0; i<9; i++ ) {
            jacobian(ind, i)   = jac(0, i);
            jacobian(ind+1, i) = jac(1, i);
          }
          ind+=2;
        }
      }
  }
}


// --------------------------------------------------------------------

rgrl_est_homo2d_lm::
rgrl_est_homo2d_lm( bool with_grad )
  : with_grad_( with_grad )
{
   rgrl_estimator::set_param_dof( 8 );

  // default value
  rgrl_nonlinear_estimator::set_max_num_iter( 50 );
  rgrl_nonlinear_estimator::set_rel_thres( 1e-5 );
}

rgrl_transformation_sptr
rgrl_est_homo2d_lm::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  // get initialization
  vnl_matrix_fixed<double, 3, 3> init_H;
  
  if( !rgrl_internal_util_upgrade_to_homography2D( init_H, cur_transform ) ) {

    // use normalized DLT to initialize
    DebugMacro( 0, "Use normalized DLT to initialize" );
    rgrl_est_homography2d est_homo;
    rgrl_transformation_sptr tmp_trans= est_homo.estimate( matches, cur_transform );
    if ( !tmp_trans )
      return 0;
    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( *tmp_trans );
    init_H = trans.H();
  }

  // count the number of constraints/residuals
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  unsigned int tot_num = 0;
  for ( unsigned ms = 0; ms<matches.size(); ++ms )
    if ( matches[ms] ) { // if pointer is valid

      rgrl_match_set const& one_set = *(matches[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) 
        if( fi.size() ) {
          tot_num += fi.size() * fi.begin().to_feature()->dim();  // each point provides two constraints
        }
    }

  // Determine the weighted centres for computing the more stable
  // covariance matrix of homography parameters
  //
  vnl_vector<double> from_centre;
  vnl_vector<double> to_centre;
  if( !compute_weighted_centres( matches, from_centre, to_centre ) )
    return 0;
   DebugMacro( 3, "From center: " << from_centre
               <<"  To center: " << to_centre << vcl_endl );

  // make the init homography as a CENTERED one
  {
    // centered H_ = to_matrix * H * from_matrix^-1
    //
    vnl_matrix<double> to_trans( 3, 3, vnl_matrix_identity );
    to_trans(0,2) = -to_centre[0];
    to_trans(1,2) = -to_centre[1];

    vnl_matrix<double> from_inv( 3, 3, vnl_matrix_identity );
    from_inv(0,2) = from_centre[0];
    from_inv(1,2) = from_centre[1];

    init_H = to_trans * init_H * from_inv;
  }
  // convert to vector form
  vnl_vector<double> p(9);
  H2h( init_H, p );

  // construct least square cost function
  rgrl_homo2d_func homo_func( matches, tot_num, with_grad_ );
  homo_func.set_centres( from_centre, to_centre );

  vnl_levenberg_marquardt lm( homo_func );
  //lm.set_trace( true );
  //lm.set_check_derivatives( 10 );
  // we don't need it to be super accurate
  lm.set_f_tolerance( relative_threshold_ );
  lm.set_max_function_evals( max_num_iterations_ );
  
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

  // normalize H
  p /= p.two_norm();
  // convert parameters back into matrix form
  h2H( p, init_H );
  vnl_vector<double> centre(2,0.0);

  // check rank of H
  vnl_double_3x3 tmpH(init_H);
  if( vcl_abs(vnl_det(tmpH)) < 1e-8 ) 
    return 0;


  // compute covariance
  // JtJ is INVERSE of jacobian
  //
  // vnl_svd<double> svd( lm.get_JtJ(), 1e-4 );
  // Cannot use get_JtJ() because it is affected by the
  // scale in homography parameter vector
  // Thus, use the nomalized p vector to compute Jacobian again
  vnl_matrix<double> jac(tot_num, 9), jtj(9, 9);
  homo_func.gradf( p, jac );
  //
  //vnl_matrix<double>  proj(9, 9, vnl_matrix_identity);
  //proj -= outer_product( p, p );
  //jac *= proj;
  vnl_fastops::AtA( jtj, jac );
  // vcl_cout << "Jacobian:\n" << jac << "\n\nJtJ:\n" << jtj << "\n\nReal JtJ:\n" << jac.transpose() * jac << vcl_endl;

  // compute inverse
  //
#if 1
  const vnl_matrix<double> compliment = vnl_orthogonal_complement( p );
 
  vnl_svd<double> svd( vnl_transpose(compliment) * jtj *compliment, 1e-6 );
  if ( svd.rank() < 8 ) {
    WarningMacro( "The covariance of homography ranks less than 8! ");
    return 0;
  }
  
  vnl_matrix<double>covar = compliment * svd.inverse() * compliment.transpose();
  
  

#else
  vnl_svd<double> svd( jtj, 1e-6 );
  DebugMacro(3, "SVD of JtJ: " << svd << vcl_endl);
  // the second least singular value shall be greater than 0
  // or Rank 8
  if ( svd.rank() < 8 ) {
    WarningMacro( "The covariance of homography ranks less than 8! ");
    return 0;
  }
  // pseudo inverse only use first 8 singular values
  vnl_matrix<double> covar ( svd.pinverse(8) );
  DebugMacro(3, "Covariance: " << covar << vcl_endl );

  //vnl_vector<double> tmp_f(tot_num,-1.0);
  //homo_func.f( svd.nullvector(), tmp_f );
  //vcl_cout << "using null vector: " << tmp_f.two_norm() << vcl_endl;
  //homo_func.f( p, tmp_f );
  //vcl_cout << "using estimate   : " << tmp_f.two_norm() << vcl_endl;
  double abs_dot = vcl_abs( dot_product( p, svd.nullvector() ) );
  if ( abs_dot < 0.9 ) {
    WarningMacro("The null vector of covariance matrix of homography is too DIFFERENT\n"
                 << "compared to estimate of homography: dot_product=" << abs_dot << vcl_endl );
    // it is considered as failure
    return 0;
  }
#endif

  DebugMacro(2, "null vector: " << svd.nullvector() << "   estimate: " << p << vcl_endl );

  return new rgrl_trans_homography2d( init_H, covar, from_centre, to_centre );
}


const vcl_type_info&
rgrl_est_homo2d_lm::
transformation_type() const
{
  return rgrl_trans_homography2d::type_id();
}
