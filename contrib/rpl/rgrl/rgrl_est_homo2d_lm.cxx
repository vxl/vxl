#include <rgrl/rgrl_est_homo2d_lm.h>
#include <rgrl/rgrl_est_homography2d.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_match_set.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

static 
void 
copy_matrix_at( vnl_matrix<double>& dest, unsigned int rp, unsigned int cp, 
                vnl_matrix<double>const& src )
{
  for( unsigned i=0; i<src.rows(); ++i )
    for( unsigned j=0; j<src.cols(); ++j )
      dest(rp+i, cp+j) = src(i,j);
}

static 
void 
copy_vector_at( vnl_matrix<double>& dest, unsigned int rp, unsigned int cp, 
                vnl_vector<double>const& src )
{
  for( unsigned i=0; i<src.size(); ++i )
      dest(rp+i, cp) = src(i);
}

static 
void 
H2h( vnl_matrix<double>const& H, vnl_vector<double>& h )
{
  h.set_size( 9 );
  for( unsigned i=0; i<H.rows(); ++i )
    for( unsigned j=0; j<H.cols(); ++j )
      h( i*H.cols()+j ) = H(i,j);
}

static 
void 
h2H( vnl_vector<double>const& h, vnl_matrix<double>& H )
{
  H.set_size( 3, 3 );
  for( unsigned i=0; i<3; ++i )
    for( unsigned j=0; j<3; ++j )
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
    matches_ptr_( &matches )
  {      }
  
  //: obj func value
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx);
  
  //: Jacobian
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

protected:
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  rgrl_set_of<rgrl_match_set_sptr> const* matches_ptr_;
};

void 
rgrl_homo2d_func::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  vnl_double_2 mapped, from, to, diff;
  vnl_matrix_fixed<double, 2, 2 > error_proj;
  unsigned int ind = 0;
  for( unsigned ms = 0; ms<matches_ptr_->size(); ++ms ) 
    if( (*matches_ptr_)[ms] ) { // if pointer is valid
      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        // map from point
        from = fi.from_feature()->location();
        map_inhomo_point( mapped, x, from );

        for( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          to = ti.to_feature()->location();
          error_proj = ti.to_feature()->error_projector();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          diff = error_proj * (mapped - to);
          
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
  
  vnl_double_2 from, to, diff;
  vnl_double_3 homo;
  vnl_matrix_fixed<double, 2, 2 > error_proj;
  vnl_matrix_fixed<double, 2, 9 > base_jac, jac;
  vnl_matrix_fixed<double, 3, 9 > jf(0.0); // homogenous coordinate
  vnl_matrix_fixed<double, 2, 3 > jg(0.0); // inhomo, [u/w, v/w]^T
  unsigned int ind = 0;
  for( unsigned ms = 0; ms<matches_ptr_->size(); ++ms ) 
    if( (*matches_ptr_)[ms] ) { // if pointer is valid
      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        // map from point
        from = fi.from_feature()->location();
        map_homo_point( homo, x, from );
        // homogenous coordinate
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
        
        for( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          to = ti.to_feature()->location();
          error_proj = ti.to_feature()->error_projector();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          jac = error_proj * base_jac;
          jac *= wgt;
          
          // fill in
          for( unsigned i=0; i<9; i++ ) {
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
}

rgrl_transformation_sptr 
rgrl_est_homo2d_lm::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  // get initialization
  vnl_matrix<double> init_H( 3, 3, vnl_matrix_identity );
  if( cur_transform.is_type( rgrl_trans_homography2d::type_id() ) ) {
    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( cur_transform );
    init_H = trans.H();
  } else if( cur_transform.is_type( rgrl_trans_affine::type_id() ) ) {
    rgrl_trans_affine const& trans = static_cast<rgrl_trans_affine const&>( cur_transform );
    assert( trans.t().size() == 2 );
    copy_matrix_at( init_H, 0, 0, trans.A() );
    copy_vector_at( init_H, 0, 2, trans.t() );
  } else if( cur_transform.is_type( rgrl_trans_similarity::type_id() ) ) {
    rgrl_trans_similarity const& trans = static_cast<rgrl_trans_similarity const&>( cur_transform );
    assert( trans.t().size() == 2 );
    copy_matrix_at( init_H, 0, 0, trans.A() );
    copy_vector_at( init_H, 0, 2, trans.t() );
  } else if( cur_transform.is_type( rgrl_trans_rigid::type_id() ) ) {
    rgrl_trans_rigid const& trans = static_cast<rgrl_trans_rigid const&>( cur_transform );
    assert( trans.t().size() == 2 );
    copy_matrix_at( init_H, 0, 0, trans.R() );
    copy_vector_at( init_H, 0, 2, trans.t() );
  } else if( cur_transform.is_type( rgrl_trans_translation::type_id() ) ) {
    rgrl_trans_translation const& trans = static_cast<rgrl_trans_translation const&>( cur_transform );
    assert( trans.t().size() == 2 );
    copy_vector_at( init_H, 0, 2, trans.t() );
  } else {
    // use normalized DLT to initialize 
    DebugMacro( 1, "Use normalized DLT to initialize" );
    rgrl_est_homography2d est_homo;
    rgrl_transformation_sptr tmp_trans= est_homo.estimate( matches, cur_transform );
    if( !tmp_trans ) 
      return 0;
    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( *tmp_trans );
    init_H = trans.H();
  }
  
  // convert to vector form
  vnl_vector<double> initp;
  H2h( init_H, initp );
  // count the number of constraints/residuals
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  unsigned int tot_num = 0;
  for( unsigned ms = 0; ms<matches.size(); ++ms ) 
    if( matches[ms] ) { // if pointer is valid
      rgrl_match_set const& one_set = *(matches[ms]);
      for( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        tot_num += fi.size() * 2;  // each point provides two constraints
      }
    }
    
  // construct least square cost function
  rgrl_homo2d_func homo_func( matches, tot_num, with_grad_ );
  vnl_levenberg_marquardt lm( homo_func );
  // lm.set_trace( true );
  // lm.set_check_derivatives( 10 );
  bool ret;
  if( with_grad_ )
    ret = lm.minimize_using_gradient(initp);
  else
    ret = lm.minimize_without_gradient(initp);
  if( !ret ) {
    WarningMacro( "Levenberg-Marquatt failed" );
    return 0;
  }
  // lm.diagnose_outcome(vcl_cout);
  
  // normalize H
  initp /= initp.two_norm();
  // convert parameters back into matrix form
  h2H( initp, init_H );
  vnl_vector<double> center(2,0.0);
  vnl_matrix<double> covar ( lm.get_JtJ() );
  return new rgrl_trans_homography2d( init_H, covar, center, center );
  
//    return new rgrl_trans_homography2d( H, covar, from_center, to_center );
}


rgrl_transformation_sptr
rgrl_est_homo2d_lm::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_homo2d_lm::
transformation_type() const
{
  return rgrl_trans_homography2d::type_id();
}
