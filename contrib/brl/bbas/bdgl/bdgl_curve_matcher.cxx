//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------
#include <bdgl/bdgl_curve_matcher.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_tracker.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <bdgl/bdgl_curve_description.h>


void bdgl_curve_matcher::match()
{
  // Transfer into a single curve
  vdgl_edgel_chain_sptr  edge1 = primitive_curve_.get_curve();

  // compute description
  bdgl_curve_description desc1(edge1);
  bdgl_curve_description desc2(image_curve_);

  //desc1.info();
  //desc2.info();

  // distance ...
  double dist;
  dist = 1.0*vcl_sqrt( vnl_math_sqr( (desc1.center_.x()-desc2.center_.x())/image_scale() )
                      +vnl_math_sqr( (desc1.center_.y()-desc2.center_.y())/image_scale() ) );
  dist+= 0.5*vnl_math_abs( (desc1.length_-desc2.length_)/image_scale() );
  dist+= 0.5*vnl_math_abs( (desc1.curvature_-desc2.curvature_)/image_scale() );

  dist+= 1.0*vnl_math_abs( (desc1.gradient_mean_val_-desc2.gradient_mean_val_)/grad_scale() );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_mean_dir_-desc2.gradient_mean_dir_)/angle_scale() );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_std_val_-desc2.gradient_std_val_)/grad_scale() );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_std_dir_-desc2.gradient_std_dir_)/angle_scale() );

  matching_score_ = dist;
  matching_flag_ = 1;

  return;
}
