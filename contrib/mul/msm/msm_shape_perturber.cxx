#include "msm_shape_perturber.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

msm_shape_perturber::msm_shape_perturber()
{
  rel_gauss_ = 0.5;
  n_pose_    = 0;
  n_params_  = 0;
}

void msm_shape_perturber::set_model( const msm_shape_model& in )
{
  sm_ = in;
  sm_inst_.set_shape_model( sm_ );
  gt_inst_.set_shape_model( sm_ );

  n_pose_   = sm_inst_.pose().size();
  dpose_.set_size( n_pose_ );

  n_params_ = sm_inst_.params().size();
  dparams_.set_size( n_params_ );

  std::size_t np = n_pose_ + n_params_;

  all_.set_size( np );
  dall_.set_size( np );
  inv_dall_.set_size( np );
}

void msm_shape_perturber::perturb( const msm_points& pts )
{
  assert( max_dpose_.size() != 0 );
  assert( max_dpose_.size() == n_pose_ );

  gt_inst_.fit_to_points( pts );
  sm_inst_.fit_to_points( pts );

  const msm_aligner& aligner = sm_.aligner();

  // Generate random pose displacement
  for ( unsigned i=0; i<n_pose_; ++i )
    dpose_[i] = random_value( rand_, max_dpose_[i], rel_gauss_ );

  vnl_vector<double> pose = aligner.compose( sm_inst_.pose(), dpose_ );
  sm_inst_.set_pose( pose );

  if ( max_dparams_.size() != 0 )
  {
    // Generate a random parameter offset
    for ( unsigned i=0; i<n_params_; ++i )
      dparams_[i] = random_value(rand_,max_dparams_[i],rel_gauss_);

    vnl_vector<double> p = gt_inst_.params() + dparams_;

    // If any of the max_dp are zero, then assume
    // displacement is from zero
    for ( unsigned i=0; i<n_params_; ++i )
      if ( max_dparams_[i]==0 )
        p[i] = 0.0;

    sm_inst_.set_params(p);
  }
  else
  {
    // a fair perturbation should be from the mean shape
    if ( n_params_ > 0 )
    {
      dparams_ = vnl_vector<double>(n_params_,0);
      sm_inst_.set_params(dparams_);
    }
  }

  inv_dpose_ = aligner.inverse( sm_inst_.pose() );
  inv_dpose_ = aligner.compose( inv_dpose_, gt_inst_.pose() );

  for ( unsigned i=0; i<n_pose_; ++i )
  {
    all_[i]       = sm_inst_.pose()[i];
    dall_[i]      = dpose_[i];
    inv_dall_[i]  = inv_dpose_[i];
  }

  for ( unsigned i=0; i<n_params_; ++i )
  {
    all_[n_pose_+i]       = sm_inst_.params()[i];
    dall_[n_pose_+i]      = dparams_[i];
    inv_dall_[n_pose_+i]  = gt_inst_.params()[i] - sm_inst_.params()[i];
  }
}

void msm_shape_perturber::set_max_dp( const vnl_vector<double>& max_dpose, const vnl_vector<double>& max_dparams )
{
  max_dpose_   = max_dpose;
  max_dparams_ = max_dparams;
}

void msm_shape_perturber::set_seed( std::size_t s )
{
  rand_.reseed( s );
}

void msm_shape_perturber::set_rel_gauss( double val )
{
  rel_gauss_ = val;
}

const vnl_vector<double>& msm_shape_perturber::params() const
{
  return sm_inst_.params();
}

const vnl_vector<double>& msm_shape_perturber::pose() const
{
  return sm_inst_.pose();
}

const vnl_vector<double>& msm_shape_perturber::gt_params() const
{
  return gt_inst_.params();
}

const vnl_vector<double>& msm_shape_perturber::gt_pose() const
{
  return gt_inst_.pose();
}

const vnl_vector<double>& msm_shape_perturber::all() const
{
  return all_;
}

const vnl_vector<double>& msm_shape_perturber::inv_d_params() const
{
  return inv_dparams_;
}

const vnl_vector<double>& msm_shape_perturber::inv_d_pose() const
{
  return inv_dpose_;
}

const vnl_vector<double>& msm_shape_perturber::inv_d_all() const
{
  return inv_dall_;
}

const msm_points& msm_shape_perturber::points() const
{
  return const_cast<msm_shape_instance&>(sm_inst_).points();
}


double msm_shape_perturber::trunc_normal_sample( vnl_random& rand1,
                                                 double sd, double max_d )
{
  double s=max_d+1;
  while ( s < -max_d || s > max_d )
    s = sd * rand1.normal64();
  return s;
}

double msm_shape_perturber::random_value( vnl_random& rand,
                                          double max_v, double rel_gauss_sd )
{
  if ( max_v == 0 )
    return 0.0;
  else if ( rel_gauss_sd == 0.0 )
    return max_v * rand.drand64(-1,1);
  else
    return max_v * trunc_normal_sample( rand, rel_gauss_sd, 1 );
}
