#ifndef rgrl_est_proj_rad_func_txx_
#define rgrl_est_proj_rad_func_txx_
//:
// \file
// \author Gehua Yang
// \date   March 2007
// \brief  a generic class to estimate a homogeneous projection matrix with radial distortion parameter(s)  using L-M

#include <iostream>
#include <vector>
#include "rgrl_est_proj_rad_func.h"

#include <rgrl/rgrl_est_proj_func.hxx>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_set_of.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

template <unsigned int Tdim, unsigned int Fdim>
rgrl_est_proj_rad_func<Tdim, Fdim>::
rgrl_est_proj_rad_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                        unsigned int camera_dof,
                        bool with_grad )
: rgrl_est_proj_func<Tdim, Fdim>( matches, with_grad ),
  camera_dof_(camera_dof),
  image_centre_(double(0)),
  centre_mag_norm_const_(0)
{
  //modify the dof in vnl_least_squares_function
  vnl_least_squares_function::init(this->proj_size_-1+camera_dof_,
                                   this->get_number_of_residuals() );

  // temperary storage space
  temp_rad_k_.resize( camera_dof_ );
}

template <unsigned int Tdim, unsigned int Fdim>
rgrl_est_proj_rad_func<Tdim, Fdim>::
rgrl_est_proj_rad_func( unsigned int camera_dof,
                        bool with_grad )
: rgrl_est_proj_func<Tdim, Fdim>( with_grad ),
  camera_dof_(camera_dof),
  image_centre_(double(0)),
  centre_mag_norm_const_(0)
{
  //modify the dof in vnl_least_squares_function
  vnl_least_squares_function::init(this->proj_size_-1+camera_dof_,
                                   this->get_number_of_residuals() );

  // temperary storage space
  temp_rad_k_.resize( camera_dof_ );
}

//: convert parameters
template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
convert_parameters( vnl_vector<double>& params,
                    vnl_matrix_fixed<double, Tdim+1, Fdim+1>const&  proj_matrix,
                    std::vector<double>             const& rad_dist,
                    vnl_vector_fixed<double, Fdim> const& fc,
                    vnl_vector_fixed<double, Tdim> const& tc,
                    vnl_vector_fixed<double, Tdim> const& camera_centre )
{
  // get params from the projection matrix
  vnl_vector<double> proj_params;
  rgrl_est_proj_func<Tdim, Fdim>::convert_parameters( proj_params, proj_matrix, fc, tc );

  // check camera dof
  assert( rad_dist.size() == camera_dof_ );

  // copy to new params
  params.set_size( proj_params.size() + rad_dist.size() );
  for ( unsigned i=0; i<proj_params.size(); ++i )
    params[i] = proj_params[i];
  for ( unsigned i=0; i<rad_dist.size(); ++i )
    params[i+proj_params.size()] = rad_dist[i];

  // set camera centre
  set_image_centre( camera_centre );
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
transfer_radial_params_into_temp_storage( vnl_vector<double> const& params ) const
{
  assert( params.size() + 1 == this->proj_size_ + camera_dof_ );
  const unsigned int index_shift = this->proj_size_-1;
  for ( unsigned i=0; i<camera_dof_; ++i )
    temp_rad_k_[i] = params[index_shift+i];
}

//: convert parameters
template <unsigned int Tdim, unsigned int Fdim>
inline
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
apply_radial_distortion( vnl_vector_fixed<double, Tdim>      & mapped,
                         vnl_vector_fixed<double, Tdim> const& p,
                         std::vector<double> const& radk ) const
{
  assert( radk.size() == camera_dof_ );

  vnl_vector_fixed<double, Tdim> centred = p-image_centre_;
  const double radial_dist = centred.squared_magnitude() / centre_mag_norm_const_;

  double base = 1;
  double coeff = 0;
  for (double i : radk) {
    base *= radial_dist;
    coeff += i * base;
  }
  mapped = p + coeff*centred;
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
reduced_proj_rad_jacobian( vnl_matrix<double>                            & base_jac,
                           vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                           std::vector<double>                       const& rad_k,
                           vnl_vector_fixed<double, Fdim>           const& from ) const
{
  assert( rad_k.size() == camera_dof_ );

  const unsigned proj_dof = this->proj_size_ -1;
  const unsigned param_size = proj_dof + camera_dof_;

  // 0. set size
  base_jac.set_size( Tdim, param_size );

  // 1. get projection matrix jacobian
  vnl_matrix_fixed<double, Tdim, (Tdim+1)*(Fdim+1)-1> dP_dp;
  base_type::reduced_proj_jacobian( dP_dp, proj, from );

  // 2. gradient w.r.t to mapped location
  vnl_matrix_fixed<double, Tdim, Tdim >  dD_dx;
  vnl_vector_fixed<double, Tdim> mapped;
  rgrl_est_proj_map_inhomo_point<Tdim, Fdim>( mapped, proj, from-this->from_centre_ );

  const vnl_vector_fixed<double, Tdim> centred = mapped-image_centre_;
  const double radial_dist = centred.squared_magnitude() / centre_mag_norm_const_;

  // compute radial distortion coefficient
  double base = 1;
  double coeff = 0;
  for (double i : rad_k) {
    base *= radial_dist;
    coeff += i * base;
  }

  // two part computation for dD_dx
  // first part
  dD_dx.set_identity();
  dD_dx *= (1+coeff);

  // second part, taking gradient on the squared radial distance
  base = 2 / centre_mag_norm_const_;
  for ( unsigned k=0; k<rad_k.size(); ++k )
  {
    const double base_coeff = double(k+1)*base*rad_k[k];

    //upper triangular
    for ( unsigned i=0; i<Tdim; ++i )
      for ( unsigned j=i; j<Tdim; ++j ) {
        dD_dx( i, j ) += base_coeff*centred[i]*centred[j];
      }

    // multiplication is at the end of loop,
    // because in gradient it is to the power of (k-1), not k
    base *= radial_dist;
  }

  // fill in lower triangular
  for ( unsigned i=0; i<Tdim; ++i )
    for ( unsigned j=i; j<Tdim; ++j )
      dD_dx( j, i ) = dD_dx( i, j );


  // 3. fill in base_jac
  dP_dp = dD_dx * dP_dp;

  for ( unsigned i=0; i<Tdim; ++i )
    for ( unsigned j=0; j<dP_dp.cols(); ++j )
      base_jac( i, j ) = dP_dp( i, j );

  // 3. gradient w.r.t to k
  base = 1;
  for ( unsigned k=0; k<camera_dof_; ++k ) {
    const unsigned index = k+proj_dof;
    base *= radial_dist;

    for ( unsigned i=0; i<Tdim; ++i )
      base_jac( i, index ) = centred[i] * base;
  }
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
full_proj_rad_jacobian( vnl_matrix<double>                            & base_jac,
                        vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                        std::vector<double>                       const& rad_k,
                        vnl_vector_fixed<double, Fdim>           const& from ) const
{
  assert( rad_k.size() == camera_dof_ );

  const unsigned param_size = this->proj_size_ + camera_dof_;

  // 0. set size
  base_jac.set_size( Tdim, param_size );

  // 1. get projection matrix jacobian
  vnl_matrix_fixed<double, Tdim, (Tdim+1)*(Fdim+1)> dP_dp;
  base_type::full_proj_jacobian( dP_dp, proj, from );

  // 2. gradient w.r.t to mapped location
  vnl_matrix_fixed<double, Tdim, Tdim >  dD_dx;
  vnl_vector_fixed<double, Tdim> mapped;
  rgrl_est_proj_map_inhomo_point<Tdim, Fdim>( mapped, proj, from-this->from_centre_ );

  const vnl_vector_fixed<double, Tdim> centred = mapped-image_centre_;
  const double radial_dist = centred.squared_magnitude() / centre_mag_norm_const_;

  // compute radial distortion coefficient
  double base = 1;
  double coeff = 0;
  for (double i : rad_k) {
    base *= radial_dist;
    coeff += i * base;
  }

  // two part computation for dD_dx
  // first part
  dD_dx.set_identity();
  dD_dx *= (1+coeff);

  // second part, taking gradient on the squared radial distance
  base = 2 / centre_mag_norm_const_;
  for ( unsigned k=0; k<rad_k.size(); ++k )
  {
    const double base_coeff = double(k+1)*base*rad_k[k];

    //upper triangular
    for ( unsigned i=0; i<Tdim; ++i )
      for ( unsigned j=i; j<Tdim; ++j ) {
        dD_dx( i, j ) += base_coeff*centred[i]*centred[j];
      }

    // multiplication is at the end of loop,
    // because in gradient it is to the power of (k-1), not k
    base *= radial_dist;
  }

  // fill in lower triangular
  for ( unsigned i=0; i<Tdim; ++i )
    for ( unsigned j=i; j<Tdim; ++j )
      dD_dx( j, i ) = dD_dx( i, j );


  // 3. fill in base_jac
  dP_dp = dD_dx * dP_dp;

  for ( unsigned i=0; i<Tdim; ++i )
    for ( unsigned j=0; j<dP_dp.cols(); ++j )
      base_jac( i, j ) = dP_dp( i, j );

  // 3. gradient w.r.t to k
  base = 1;
  for ( unsigned k=0; k<camera_dof_; ++k ) {
    const unsigned index = k+this->proj_size_;
    base *= radial_dist;

    for ( unsigned i=0; i<Tdim; ++i )
      base_jac( i, index ) = centred[i] * base;
  }
}

//: compute jacobian w.r.t. location
template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
proj_jac_wrt_loc( vnl_matrix_fixed<double, Tdim, Fdim>          & jac_loc,
                  vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                  std::vector<double>                       const& rad_k,
                  vnl_vector_fixed<double, Fdim>           const& from ) const
{
  // dP / dx
  vnl_matrix_fixed<double, Tdim, Fdim> dP_dx;
  rgrl_est_proj_func<Tdim, Fdim>::proj_jac_wrt_loc( dP_dx, proj, from );

  // 2. gradient w.r.t to mapped location
  vnl_matrix_fixed<double, Tdim, Tdim >  dD_dx;
  vnl_vector_fixed<double, Tdim> mapped;
  rgrl_est_proj_map_inhomo_point<Tdim, Fdim>( mapped, proj, from-this->from_centre_ );

  const vnl_vector_fixed<double, Tdim> centred = mapped-image_centre_;
  const double radial_dist = centred.squared_magnitude() / centre_mag_norm_const_;

  // compute radial distortion coefficient
  double base = 1;
  double coeff = 0;
  for (double i : rad_k) {
    base *= radial_dist;
    coeff += i * base;
  }

  // two part computation for dD_dx
  // first part
  dD_dx.set_identity();
  dD_dx *= (1+coeff);

  // second part, taking gradient on the squared radial distance
  base = 2 / centre_mag_norm_const_;
  for ( unsigned k=0; k<rad_k.size(); ++k )
  {
    const double base_coeff = double(k+1)*base*rad_k[k];

    //upper triangular
    for ( unsigned i=0; i<Tdim; ++i )
      for ( unsigned j=i; j<Tdim; ++j ) {
        dD_dx( i, j ) += base_coeff*centred[i]*centred[j];
      }

    // multiplication is at the end of loop,
    // because in gradient it is to the power of (k-1), not k
    base *= radial_dist;
  }

  // fill in lower triangular
  for ( unsigned i=0; i<Tdim; ++i )
    for ( unsigned j=i; j<Tdim; ++j )
      dD_dx( j, i ) = dD_dx( i, j );

  // final jac
  jac_loc = dD_dx * dP_dx;
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  vnl_vector_fixed<double, Tdim> distorted;
  vnl_matrix_fixed<double, Tdim,Tdim> error_proj_sqrt;
  unsigned int ind = 0;

  // retrieve the projection matrix
  vnl_matrix_fixed<double, Tdim+1, Fdim+1> proj;
  base_type::restored_centered_proj( proj, x );

  // retrieve the radial distortion parameters
  transfer_radial_params_into_temp_storage( x );

  for ( unsigned ms = 0; ms<this->matches_ptr_->size(); ++ms )
  {
    if ( (*this->matches_ptr_)[ms] != nullptr ) // if pointer is valid
    {
      rgrl_match_set const& one_set = *((*this->matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi ) {
        // map from point
        vnl_vector_fixed<double, Fdim> from = fi.from_feature()->location();
        map_loc( distorted, proj, temp_rad_k_, from );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          vnl_vector_fixed<double, Tdim> to = ti.to_feature()->location();
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = std::sqrt(ti.cumulative_weight());
          vnl_vector_fixed<double, Tdim> diff = error_proj_sqrt * (distorted - to);

          // fill in
          for ( unsigned i=0; i<Tdim; ++i )
            fx(ind++) = wgt * diff[i];
        }
      }
    }
  }

  // check
  assert( ind == this->get_number_of_residuals() );
}

template <unsigned int Tdim, unsigned int Fdim>
void
rgrl_est_proj_rad_func<Tdim, Fdim>::
gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  assert( jacobian.rows() == this->get_number_of_residuals() );
  assert( jacobian.cols()+1 == this->proj_size_+camera_dof_ );

  vnl_matrix_fixed<double, Tdim,   Tdim>        error_proj_sqrt;
  vnl_matrix<double> base_jac, jac;

  // retrieve the projection matrix
  vnl_matrix_fixed<double, Tdim+1, Fdim+1> proj;
  base_type::restored_centered_proj( proj, x );

  // retrieve the radial distortion parameters
  transfer_radial_params_into_temp_storage( x );

  unsigned int ind = 0;
  for ( unsigned ms = 0; ms<this->matches_ptr_->size(); ++ms )
  {
    if ( (*this->matches_ptr_)[ms] ) // if pointer is valid
    {
      rgrl_match_set const& one_set = *((*this->matches_ptr_)[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi )
      {
        // map from point
        vnl_vector_fixed<double, Fdim> from = fi.from_feature()->location();

        // jacobian computation
        reduced_proj_rad_jacobian( base_jac, proj, temp_rad_k_, from );

        for ( TIter ti=fi.begin(); ti!=fi.end(); ++ti ) {
          //vnl_double_2 to = ti.to_feature()->location();
          error_proj_sqrt = ti.to_feature()->error_projector_sqrt();
          double const wgt = std::sqrt(ti.cumulative_weight());
          jac = error_proj_sqrt * base_jac;
          jac *= wgt;

          // fill in
          for ( unsigned i=0; i<Tdim; ++i,++ind ) {
            for ( unsigned j=0; j<jac.cols(); ++j )
              jacobian(ind,j) = jac(i, j);
          }
        }
      }
    }
  }
}

template <unsigned int Tdim, unsigned int Fdim>
bool
rgrl_est_proj_rad_func<Tdim, Fdim>::
projective_estimate(  vnl_matrix_fixed<double, Tdim+1, Fdim+1>& proj,
                      std::vector<double>& rad_dist,
                      vnl_matrix<double>& full_covar,
                      vnl_vector_fixed<double, Fdim>& from_centre,
                      vnl_vector_fixed<double, Tdim>& to_centre,
                      vnl_vector_fixed<double, Tdim> const& camera_centre)
{
  // compute weighted centres
  // this function is going to resize the vnl_vector, use temporary ones instead
  vnl_vector<double> fc, tc;
  if ( !rgrl_est_compute_weighted_centres( *(this->matches_ptr_), fc, tc ) ) {
    return false;
  }
  assert( fc.size() == from_centre.size() );
  assert( tc.size() == to_centre.size() );
  assert( rad_dist.size() == camera_dof_ );
  from_centre = fc;
  to_centre = tc;

  // convert parameters
  vnl_vector<double> p;
  this->convert_parameters( p, proj, rad_dist, from_centre, to_centre, camera_centre );

  vnl_levenberg_marquardt lm( *this );
  //lm.set_trace( true );
  //lm.set_check_derivatives( 1 );
  // we don't need it to be super accurate
  lm.set_f_tolerance( this->relative_threshold_ );
  lm.set_max_function_evals( this->max_num_iterations_ );

  bool ret;
  if ( this->has_gradient() )
    ret = lm.minimize_using_gradient(p);
  else
    ret = lm.minimize_without_gradient(p);
  if ( !ret ) {
    std::cerr <<  "Levenberg-Marquardt failed\n";
    lm.diagnose_outcome(std::cerr);
    return false;
  }
  // lm.diagnose_outcome(std::cout);

  // convert parameters back into matrix form
  this->restored_centered_proj( proj, p );
  //std::cout << "Final params=" << proj << std::endl;

  // copy distortion parameters
  const unsigned int index_shift = this->proj_size_-1;
  for ( unsigned i=0; i<camera_dof_; ++i )
    rad_dist[i] = p[ i+index_shift ];

  // compute covariance
  // Jac^\top * Jac is INVERSE of the covariance
  //
  const unsigned int param_num = this->proj_size_+camera_dof_;
  vnl_matrix<double> jac( this->get_number_of_residuals(), param_num-1 );
  this->gradf( p, jac );
  //

  // SVD decomposition:
  // Jac = U W V^\top
  vnl_svd<double> svd( jac, this->zero_svd_thres_ );
  if ( svd.rank()+1 < param_num ) {
    std::cerr <<  "The covariance of projection matrix ranks less than "
             << param_num-1 << "!\n"
             << "  The singular values are " << svd.W() << std::endl;
    return false;
  }

  //invert the W matrix and square it
  vnl_diag_matrix<double> invW( param_num-1 );
  for ( unsigned i=0; i+1<param_num; ++i )
    invW[i] = vnl_math::sqr( 1.0/svd.W(i) );

  //compute inverse of Jac^\top * Jac
  const vnl_matrix<double>  covar( svd.V() * invW * vnl_transpose( svd.V() ) );

  // convert the covar to full dof+1 matrix
  // fill in the row and column of the fixed element
  // with 0
  full_covar.set_size( param_num, param_num );
  full_covar.fill( 0.0 );

  const unsigned int param_index
    = this->index_row_*(Fdim+1)
    + this->index_col_;
  for ( unsigned i=0,ic=0; i<param_num; ++i,++ic ) {
    if ( i==param_index ) { --ic; continue; }
    for ( unsigned j=0,jc=0; j<param_num; ++j,++jc ) {
      if ( j==param_index ) { --jc; continue; }
      full_covar( i, j ) = covar( ic, jc );
    }
  }

  return true;
}


#undef  RGRL_EST_PROJ_RAD_FUNC_INSTANTIATE
#define RGRL_EST_PROJ_RAD_FUNC_INSTANTIATE( tdim, fdim ) \
  template class rgrl_est_proj_rad_func< tdim, fdim >

#endif //rgrl_est_proj_rad_func_txx_
