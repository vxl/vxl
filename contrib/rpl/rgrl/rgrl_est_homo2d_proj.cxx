//:
// \file
#include <rgrl/rgrl_est_homo2d_proj.h>
#include <rgrl/rgrl_est_homography2d.h>
#include <rgrl/rgrl_est_proj_func.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_internal_util.h>

#include <vnl/vnl_double_2.h>

// --------------------------------------------------------------------

rgrl_est_homo2d_proj::
rgrl_est_homo2d_proj( bool with_grad )
  : with_grad_( with_grad )
{
   rgrl_estimator::set_param_dof( 8 );

  // default value
  rgrl_nonlinear_estimator::set_max_num_iter( 50 );
  rgrl_nonlinear_estimator::set_rel_thres( 1e-5 );
}

rgrl_transformation_sptr
rgrl_est_homo2d_proj::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  // get initialization
  vnl_matrix_fixed<double, 3, 3> init_H;

  if ( !rgrl_internal_util_upgrade_to_homography2D( init_H, cur_transform ) )
  {
    // use normalized DLT to initialize
    DebugMacro( 0, "Use normalized DLT to initialize" );
    rgrl_est_homography2d est_homo;
    rgrl_transformation_sptr tmp_trans= est_homo.estimate( matches, cur_transform );
    if ( !tmp_trans )
      return 0;
    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( *tmp_trans );
    init_H = trans.H();
  }

  // construct least square cost function
  rgrl_est_proj_func<2,2> homo_func( matches, with_grad_ );
  homo_func.set_max_num_iter( max_num_iterations_ );
  homo_func.set_rel_thres( relative_threshold_ );


  // apply estimation
  vnl_double_2 from_centre, to_centre;
  vnl_matrix<double> covar;
  if ( !homo_func.projective_estimate( init_H, covar, from_centre, to_centre ) ) {
    WarningMacro( "L-M estimation failed." << vcl_endl );
    return 0;
  }

  return new rgrl_trans_homography2d( init_H, covar, from_centre, to_centre );
}


const vcl_type_info&
rgrl_est_homo2d_proj::
transformation_type() const
{
  return rgrl_trans_homography2d::type_id();
}
