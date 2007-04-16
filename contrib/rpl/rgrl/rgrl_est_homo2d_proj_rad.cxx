//:
// \file
#include "rgrl_est_homo2d_proj_rad.h"

#include <rgrl/rgrl_est_homography2d.h>
#include <rgrl/rgrl_est_proj_rad_func.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_homo2d_proj_rad.h>
#include <rgrl/rgrl_trans_rad_dis_homo2d.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_internal_util.h>

#include <vnl/vnl_double_2.h>
#include <vul/vul_sprintf.h>

// --------------------------------------------------------------------

rgrl_est_homo2d_proj_rad::
rgrl_est_homo2d_proj_rad( unsigned int rad_order,
                          vnl_vector_fixed<double, 2> const& to_camera_centre,
                          bool with_grad )
  : to_camera_centre_( to_camera_centre ),
    camera_dof_( rad_order ),
    with_grad_( with_grad )
{
  rgrl_estimator::set_param_dof( 8+camera_dof_ );

  // default value
  rgrl_nonlinear_estimator::set_max_num_iter( 50 );
  rgrl_nonlinear_estimator::set_rel_thres( 1e-5 );

  // setting up transform name
  transform_name_ = vul_sprintf( "rgrl_trans_homo2d_proj_rad+radial-%d", camera_dof_ );
}

rgrl_transformation_sptr
rgrl_est_homo2d_proj_rad::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  // get initialization
  vnl_matrix_fixed<double, 3, 3> init_H;
  vcl_vector<double> radk( camera_dof_, 0.0 );

  if ( cur_transform.is_type( rgrl_trans_homo2d_proj_rad::type_id() ) )
  {
    rgrl_trans_homo2d_proj_rad const& trans = static_cast<rgrl_trans_homo2d_proj_rad const&>( cur_transform );
    init_H = trans.H();
    const vcl_vector<double> k = trans.radial_params();
    for ( unsigned int i=0; i<k.size()&&i<camera_dof_; ++i )
      radk[i] = k[i];
  }
  else if ( cur_transform.is_type( rgrl_trans_rad_dis_homo2d::type_id() ) )
  {
    rgrl_trans_rad_dis_homo2d const& trans = static_cast<rgrl_trans_rad_dis_homo2d const&>( cur_transform );
    init_H = trans.uncenter_H_matrix();
    const double k1_to   = trans.k1_to();
    radk[0] = k1_to;
  }
  else
  {
    if ( !rgrl_internal_util_upgrade_to_homography2D( init_H, cur_transform ) )
    {
      // use normalized DLT to initialize
      DebugMacro( 0, "Use normalized DLT to initialize" );
      rgrl_est_homography2d est_homo;
      rgrl_transformation_sptr tmp_trans= est_homo.estimate( matches, cur_transform );
      if ( !tmp_trans )
        return 0;
      rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( *tmp_trans );
      init_H = trans.uncenter_H_matrix();
    }
  }

  // construct least square cost function
  rgrl_est_proj_rad_func<2,2> homo_func( matches, camera_dof_, with_grad_ );
  homo_func.set_max_num_iter( max_num_iterations_ );
  homo_func.set_rel_thres( relative_threshold_ );

  // apply estimation
  vnl_double_2 from_centre, to_centre;
  vnl_matrix<double> covar;
  if ( !homo_func.projective_estimate( init_H, radk,
                                       covar,
                                       from_centre, to_centre,
                                       to_camera_centre_ ) ) {
    WarningMacro( "L-M estimation failed." << vcl_endl );
    return 0;
  }

  return new rgrl_trans_homo2d_proj_rad( init_H,
                                         radk,
                                         to_camera_centre_,
                                         covar,
                                         from_centre, to_centre );
}


const vcl_type_info&
rgrl_est_homo2d_proj_rad::
transformation_type() const
{
  return rgrl_trans_homo2d_proj_rad::type_id();
}
