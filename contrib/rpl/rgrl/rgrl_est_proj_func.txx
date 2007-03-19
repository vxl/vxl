#ifndef rgrl_est_proj_func_txx_
#define rgrl_est_proj_func_txx_
//:
// \file
// \author Gehua Yang
// \date   March 2007
// \brief  a generic class to estimate a homogeneous projection matrix using L-M

#include <rgrl/rgrl_est_proj_func.h>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_set_of.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_cassert.h>

const static unsigned int maxval_unsigned = vnl_numeric_traits<unsigned int>::maxval;

template <unsigned int Tdim, unsigned int Fdim>
rgrl_est_proj_func<Tdim, Fdim>::
rgrl_est_proj_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                    bool with_grad )
: vnl_least_squares_function( (Fdim+1)*(Tdim+1)-1,
                              rgrl_est_matches_residual_number(matches),
                              with_grad ? use_gradient : no_gradient ),
  matches_ptr_( &matches ),
  from_centre_(double(0)), to_centre_(double(0)),
  index_row_(maxval_unsigned), index_col_(maxval_unsigned),
  max_num_iterations_(50),
  relative_threshold_(1e-7),
  zero_svd_thres_(1e-5)
{
}

template <unsigned int Tdim, unsigned int Fdim>
rgrl_est_proj_func<Tdim, Fdim>::
rgrl_est_proj_func( bool with_grad )
: vnl_least_squares_function( (Fdim+1)*(Tdim+1)-1,
                              1000 /*artificial number to avoid warning*/,
                              with_grad ? use_gradient : no_gradient ),
  matches_ptr_( 0 ),
  from_centre_(double(0)), to_centre_(double(0)),
  index_row_(maxval_unsigned), index_col_(maxval_unsigned),
  max_num_iterations_(50),
  relative_threshold_(1e-7),
  zero_svd_thres_(1e-5)
{
}

//: convert parameters
template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
convert_parameters( vnl_vector<double>& params,
                    vnl_matrix_fixed<double, Tdim+1, Fdim+1>  proj_matrix,
                    vnl_vector_fixed<double, Fdim> const& fc,
                    vnl_vector_fixed<double, Tdim> const& tc )
{
  // set centres
  from_centre_ = fc;
  to_centre_ = tc;

  // make the prejection matrix to centre around from_centre_
  vnl_matrix_fixed<double, Fdim+1, Fdim+1> from_centre_matrix;
  from_centre_matrix.set_identity();
  for ( unsigned i=0; i<Fdim; ++i )
    from_centre_matrix( i, Fdim ) = from_centre_[i];

  vnl_matrix_fixed<double, Tdim+1, Tdim+1> to_centre_matrix;
  to_centre_matrix.set_identity();
  for ( unsigned i=0; i<Tdim; ++i )
    to_centre_matrix( i, Tdim ) = -to_centre_[i];

  proj_matrix = to_centre_matrix * proj_matrix * from_centre_matrix;

  // find out which element in the projection matrix has largest value
  double max_val = 0;
  index_row_ = index_col_ = -1;  // init to bad value
  for ( unsigned i=0; i<Tdim+1; ++i )
    for ( unsigned j=0; j<Fdim+1; ++j )
      if ( vcl_abs( proj_matrix(i,j) ) > max_val ) {
        index_row_ = i;
        index_col_ = j;
        max_val = vcl_abs( proj_matrix(i,j) );
      }

  // normalize the proj_matrix to have the largest value as 1
  proj_matrix /= proj_matrix( index_row_, index_col_ );

  // fill in params
  params.set_size( proj_size_-1 );
  for ( unsigned k=0,i=0; i<Tdim+1; ++i )
    for ( unsigned j=0; j<Fdim+1; ++j ) {
      if ( i==index_row_ && j==index_col_ ) {
        continue;
      }
      // fill in elements in order
      params[k++] = proj_matrix(i,j);
    }
}

//: uncenter projection matrix
template <unsigned int Tdim, unsigned int Fdim>
vnl_matrix_fixed<double, Tdim+1, Fdim+1>
rgrl_est_proj_func<Tdim, Fdim>::
uncentre_proj( vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj ) const
{
  // make the prejection matrix to centre around from_centre_
  vnl_matrix_fixed<double, Fdim+1, Fdim+1> from_centre_matrix;
  from_centre_matrix.set_identity();
  for ( unsigned i=0; i<Fdim; ++i )
    from_centre_matrix( i, Fdim ) = - from_centre_[i];

  vnl_matrix_fixed<double, Tdim+1, Tdim+1> to_centre_matrix;
  to_centre_matrix.set_identity();
  for ( unsigned i=0; i<Tdim; ++i )
    to_centre_matrix( i, Tdim ) = to_centre_[i];

  return to_centre_matrix * proj * from_centre_matrix;
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
full_proj_jacobian( vnl_matrix_fixed<double, Tdim, (Fdim+1)*(Tdim+1)>& complete_jac,
                    vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                    vnl_vector_fixed<double, Fdim>           const& from ) const
{
  // subtract centre
  const vnl_vector_fixed<double, Fdim> from_centred = from-from_centre_;

  vnl_vector_fixed<double, Tdim+1> homo;
  vnl_matrix_fixed<double, Tdim+1, proj_size_>   jf(0.0);    // grad in homogeneous coordinate
  vnl_matrix_fixed<double, Tdim,   Tdim+1>       jg(0.0);    // grad of division, [u/w, v/w]^T

  rgrl_est_proj_map_homo_point<Tdim, Fdim>( homo, proj, from_centred );

  // 1. linear gradient in homogeneous coordinate
  // fill jf (linear gradient) with 1.0 on elements corresponding to shift
  for ( unsigned i=0; i<Tdim+1; ++i )
    jf( i, i*(Fdim+1)+Fdim ) = 1.0;

  // skip the ones corresponding to shift
  for ( unsigned index=0,i=0; i<Tdim+1; ++i,index+=(Fdim+1) )
    for ( unsigned j=0; j<Fdim; ++j ) {
      jf( i, index+j ) = from_centred[j];
    }

  // 2. gradient of making division
  const double homo_last_neg_sqr = -vnl_math_sqr(homo[Tdim]);
  const double homo_last_div = 1.0/homo[Tdim];
  for ( unsigned i=0; i<Tdim; ++i )
    jg(i,i) = homo_last_div;
  for ( unsigned i=0; i<Tdim; ++i )
    jg(i,Tdim) = homo[i] / homo_last_neg_sqr;

  // 3. complete jacobian
  // since Jab_g(f(p)) = Jac_g * Jac_f
  complete_jac = jg * jf;
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
reduced_proj_jacobian( vnl_matrix_fixed<double, Tdim, (Fdim+1)*(Tdim+1)-1>& base_jac,
                       vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                       vnl_vector_fixed<double, Fdim>           const& from ) const
{
  vnl_matrix_fixed<double, Tdim,   proj_size_>   complete_jac;
  full_proj_jacobian( complete_jac, proj, from );

  // 4. remove the element being held as constant 1
  const unsigned index = (index_row_*(Fdim+1)+index_col_);
  for ( unsigned i=0,j=0; i<proj_size_; ++i )
    if ( i != index ) {
      for ( unsigned k=0; k<Tdim; ++k )
        base_jac(k, j) = complete_jac(k, i);
      ++j;
    }
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
proj_jac_wrt_loc( vnl_matrix_fixed<double, Tdim, Fdim>& jac_loc,
                  vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                  vnl_vector_fixed<double, Fdim>           const& from ) const
{
  // 1. linear gradient in homogeneous coordinate
  // fill jf (linear gradient) with 1.0 on elements corresponding to shift
  vnl_matrix_fixed<double, Tdim+1, Fdim>   jf;    // grad in homogeneous coordinate
  for ( unsigned i=0; i<Tdim+1; ++i )
    for ( unsigned j=0; j<Fdim; ++j )
      jf(i, j) = proj(i, j);

  // 2. gradient of making division
  vnl_vector_fixed<double, Tdim+1> homo;
  rgrl_est_proj_map_homo_point<Tdim, Fdim>( homo, proj, from-from_centre_ );

  vnl_matrix_fixed<double, Tdim,   Tdim+1>       jg(0.0);    // grad of division, [u/w, v/w]^T
  const double homo_last_neg_sqr = -vnl_math_sqr(homo[Tdim]);
  const double homo_last_div = 1.0/homo[Tdim];
  for ( unsigned i=0; i<Tdim; ++i )
    jg(i,i) = homo_last_div;
  for ( unsigned i=0; i<Tdim; ++i )
    jg(i,Tdim) = homo[i] / homo_last_neg_sqr;

  // 3. complete jacobian
  // since Jab_g(f(p)) = Jac_g * Jac_f
  jac_loc = jg * jf;
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
restored_centered_proj( vnl_matrix_fixed<double, Tdim+1, Fdim+1>& proj_matrix,
                        vnl_vector<double> const& params ) const
{
  assert( params.size()+1 >= proj_size_ );
  for ( unsigned k=0,i=0; i<Tdim+1; ++i )
    for ( unsigned j=0; j<Fdim+1; ++j ) {
      if ( i==index_row_ && j==index_col_ ) {
        proj_matrix(i,j) = 1.0;
        continue;
      }
      // fill in elements in order
      proj_matrix(i,j) = params[k++];
    }
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  vnl_vector_fixed<double, Tdim> mapped;
  vnl_matrix_fixed<double, Tdim,Tdim> error_proj_sqrt;
  unsigned int ind = 0;

  // retrieve the projection matrix
  vnl_matrix_fixed<double, Tdim+1, Fdim+1> proj;
  restored_centered_proj( proj, x );

  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )
    if ( (*matches_ptr_)[ms] != 0 ) // if pointer is valid
    {
      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi )
      {
        // map from point
        vnl_vector_fixed<double, Fdim> from = fi.from_feature()->location();
        map_loc( mapped, proj, from );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          vnl_vector_fixed<double, Tdim> to = ti.to_feature()->location();
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          vnl_vector_fixed<double, Tdim> diff = error_proj_sqrt * (mapped - to);

          // fill in
          for ( unsigned i=0; i<Tdim; ++i )
            fx(ind++) = wgt * diff[i];
        }
      }
  }

  // check
  assert( ind == get_number_of_residuals() );
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_func<Tdim, Fdim>::
gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  assert( jacobian.rows() == get_number_of_residuals() );
  assert( jacobian.cols()+1 == proj_size_ );

  vnl_matrix_fixed<double, Tdim,   Tdim>        error_proj_sqrt;
  vnl_matrix_fixed<double, Tdim,   proj_size_-1> base_jac, jac;

  // retrieve the projection matrix
  vnl_matrix_fixed<double, Tdim+1, Fdim+1> proj;
  restored_centered_proj( proj, x );

  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<matches_ptr_->size(); ++ms )
    if ( (*matches_ptr_)[ms] ) { // if pointer is valid

      rgrl_match_set const& one_set = *((*matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi )
      {
        // map from point
        vnl_vector_fixed<double, Fdim> from = fi.from_feature()->location();

        // jacobian computation
        reduced_proj_jacobian( base_jac, proj, from );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          //vnl_double_2 to = ti.to_feature()->location();
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = vcl_sqrt(ti.cumulative_weight());
          jac = error_proj_sqrt * base_jac;
          jac *= wgt;

          // fill in
          for ( unsigned i=0; i<Tdim; ++i,++ind ) {
            for ( unsigned j=0; j+1<proj_size_; ++j )
              jacobian(ind,j) = jac(i, j);
          }
        }
      }
  }
}

template <unsigned int Tdim, unsigned int Fdim>
bool
rgrl_est_proj_func<Tdim, Fdim>::
projective_estimate( vnl_matrix_fixed<double, Tdim+1, Fdim+1>& proj,
                     vnl_matrix<double>& full_covar,
                     vnl_vector_fixed<double, Fdim>& from_centre,
                     vnl_vector_fixed<double, Tdim>& to_centre )
{
  // compute weighted centres
  // this function is going to resize the vnl_vector, use temporary ones instead
  vnl_vector<double> fc, tc;
  if ( !rgrl_est_compute_weighted_centres( *matches_ptr_, fc, tc ) ) {
    return false;
  }
  assert( fc.size() == from_centre.size() );
  assert( tc.size() == to_centre.size() );
  from_centre = fc;
  to_centre = tc;

  // convert parameters
  vnl_vector<double> p;
  this->convert_parameters( p, proj, from_centre, to_centre );

  vnl_levenberg_marquardt lm( *this );
  //lm.set_trace( true );
  //lm.set_check_derivatives( 1 );
  // we don't need it to be super accurate
  lm.set_f_tolerance( relative_threshold_ );
  lm.set_max_function_evals( max_num_iterations_ );

  bool ret;
  if ( has_gradient() )
    ret = lm.minimize_using_gradient(p);
  else
    ret = lm.minimize_without_gradient(p);
  if ( !ret ) {
    vcl_cerr <<  "Levenberg-Marquatt failed\n";
    lm.diagnose_outcome(vcl_cerr);
    return false;
  }
  //lm.diagnose_outcome(vcl_cout);

  // convert parameters back into matrix form
  this->restored_centered_proj( proj, p );
  //vcl_cout << "Final params=" << proj << vcl_endl;

  // compute covariance
  // Jac^\top * Jac is INVERSE of the covariance
  //
  const unsigned int proj_size_ = (Fdim+1)*(Tdim+1);
  vnl_matrix<double> jac(get_number_of_residuals(), proj_size_-1);
  this->gradf( p, jac );
  //

  // SVD decomposition:
  // Jac = U W V^\top
  vnl_svd<double> svd( jac, zero_svd_thres_ );
  if ( svd.rank()+1 < proj_size_ ) {
    vcl_cerr <<  "The covariance of projection matrix ranks less than "
             << proj_size_-1 << "!\n";
    vcl_cerr << "  The singular values are " << svd.W() << vcl_endl;
    return false;
  }

  //invert the W matrix and square it
  vnl_diag_matrix<double> invW( proj_size_-1 );
  for ( unsigned i=0; i+1 < proj_size_; ++i )
    invW[i] = vnl_math_sqr( 1.0/svd.W(i) );

  //compute inverse of Jac^\top * Jac
  const vnl_matrix<double>  covar( svd.V() * invW * vnl_transpose( svd.V() ) );

  // convert the covar to full dof+1 matrix
  // fill in the row and column of the fixed element
  // with 0
  full_covar.set_size( proj_size_, proj_size_ );
  full_covar.fill( 0.0 );

  const unsigned int param_index = index_row_*(Fdim+1) + index_col_;
  for ( unsigned i=0,ic=0; i<proj_size_; ++i,++ic ) {
    if ( i==param_index ) { --ic; continue; }
    for ( unsigned j=0,jc=0; j<proj_size_; ++j,++jc ) {
      if ( j==param_index ) { --jc; continue; }
      full_covar( i, j ) = covar( ic, jc );
    }
  }

  return true;
}

#if 0 // commented out
// --------------------------------------------------------------------
template <unsigned int Tdim, unsigned int Fdim>
rgrl_est_proj_lm::
rgrl_est_proj_lm( bool with_grad )
  : with_grad_( with_grad )
{
   rgrl_estimator::set_param_dof( Fdim*Tdim-1 );

  // default value
  rgrl_nonlinear_estimator::set_max_num_iter( 50 );
  rgrl_nonlinear_estimator::set_rel_thres( 1e-5 );
}

template <unsigned int Tdim, unsigned int Fdim>
bool
rgrl_est_proj_lm::
projective_estimate( vnl_matrix_fixed<double, Tdim+1, Fdim+1>& proj,
                     vnl_matrix<double>& full_covar,
                     vnl_vector_fixed<double, Fdim>& from_centre,
                     vnl_vector_fixed<double, Tdim>& to_centre,
                     rgrl_set_of<rgrl_match_set_sptr> const& matches ) const
{
  // count the number of constraints/residuals
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  unsigned int tot_num = 0;
  for ( unsigned ms = 0; ms<matches.size(); ++ms )
    if ( matches[ms] ) { // if pointer is valid

      rgrl_match_set const& one_set = *(matches[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi )
        if ( fi.size() ) {
          tot_num += fi.size() * fi.begin().to_feature()->dim();  // each point provides two constraints
        }
    }

  // Determine the weighted centres for computing the more stable
  // covariance matrix of homography parameters
  //
  if ( !compute_weighted_centres( matches, from_centre, to_centre ) )
    return 0;
   DebugMacro( 3, "From center: " << from_centre
               <<"  To center: " << to_centre << vcl_endl );

  // construct least square cost function
  rgrl_est_proj_func<Tdim, Fdim> proj_func( matches, tot_num, with_grad_ );

  // convert parameters
  vnl_vector<double> p;
  proj_func.convert_parameters( p, proj, from_centre, to_centre );

  vnl_levenberg_marquardt lm( proj_func );
  //lm.set_trace( true );
  //lm.set_check_derivatives( 2 );
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
  //lm.diagnose_outcome(vcl_cout);

  // convert parameters back into matrix form
  restored_centered_proj( proj, p );

  // compute covariance
  // Jac^\top * Jac is INVERSE of the covariance
  //
  const unsigned int proj_size_ = (Fdim+1)*(Tdim+1);
  vnl_matrix<double> jac(tot_num, proj_size_-1);
  proj_func.gradf( p, jac );
  //

  // SVD decomposition:
  // Jac = U W V^\top
  vnl_svd<double> svd( jac, 1e-4 );
  if ( svd.rank()+1 < proj_size_ ) {
    WarningMacro( "The covariance of homography ranks less than 8! ");
    return 0;
  }

  //invert the W matrix and square it
  vnl_diag_matrix<double> invW( proj_size_-1 );
  for ( unsigned i=0; i+1<proj_size_; ++i )
    invW[i] = vnl_math_sqr( 1.0/svd.W(i) );

  //compute inverse of Jac^\top * Jac
  const vnl_matrix<double>  covar( svd.V() * invW * vnl_transpose( svd.V() ) );

  // convert the covar to full dof+1 matrix
  // fill in the row and column of the fixed element
  // with 0
  full_covar.set_size( proj_size_, proj_size_ );

  const unsigned int param_index = index_row_*(Fdim+1) + index_col_;
  for ( unsigned i=0,ic=0; i<proj_size_; ++i,++ic ) {
    if ( i==param_index ) { --ic; continue; }
    for ( unsigned j=0;jc=0; j<proj_size_; ++j,++jc ) {
      if ( j==param_index ) { --jc; continue; }
      full_covar( i, j ) = covar( ic, jc );
    }
  }
}
#endif // 0

#undef  RGRL_EST_PROJ_FUNC_INSTANTIATE
#define RGRL_EST_PROJ_FUNC_INSTANTIATE( tdim, fdim ) \
  template class rgrl_est_proj_func< tdim, fdim >

#endif //rgrl_est_proj_func_txx_
