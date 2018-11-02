#include <iostream>
#include "bwm_plane_fitting_lsf.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

void bwm_plane_fitting_lsf::f(vnl_vector<double> const &x, vnl_vector< double > &fx)
{
  std::cerr << "------ d_ == " << x[0] <<"   ";
  d_ = x[0];
  plane_.set(a_, b_, c_, d_);

  vsol_point_3d_sptr new_pt3d;
  master_obs_->backproj_point(master_img_pt_, new_pt3d, plane_);

  vgl_point_2d<double> new_img_pt;
  sec_obs_->proj_point(new_pt3d->get_p(), new_img_pt);
  vgl_vector_2d<double> v = new_pt_->get_p() - new_img_pt;
  fx[0] = v.length();
  std::cerr << "  fx = " << fx[0] << std::endl;
}
