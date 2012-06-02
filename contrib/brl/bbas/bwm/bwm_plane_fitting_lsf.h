// This is brl/bbas/bwm/bwm_plane_fitting_lsf.h
#ifndef bwm_plane_fitting_lsf_h_
#define bwm_plane_fitting_lsf_h_
//:
// \file
// \brief  Least Square Function Definition to solve for the right projection plane while moving a polygon to the specified point while keeping the plane in the camera direction of the master camera observer
//
// \author Gamze Tunali
// \date   August 20, 2007
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bwm_observer_cam.h"

#include <vnl/vnl_least_squares_function.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vsol/vsol_point_2d_sptr.h>

class bwm_plane_fitting_lsf : public vnl_least_squares_function
{
 public:

  //: 1 unknown (d of projection plane ax+by+cx=d), 1 residual (distance between the current and the destination point)
  bwm_plane_fitting_lsf(double a, double b, double c, double d,
                        vsol_point_2d_sptr master_img_pt, vsol_point_2d_sptr new_pt,
                        bwm_observer_cam* master_obs, bwm_observer_cam* sec_obs)
  : vnl_least_squares_function(1, 1, no_gradient),
    a_(a), b_(b), c_(c), d_(d),
    master_obs_(master_obs), sec_obs_(sec_obs),
    master_img_pt_(master_img_pt), new_pt_(new_pt)
  {}

  virtual ~bwm_plane_fitting_lsf() {}

  virtual void f(vnl_vector<double> const &x, vnl_vector< double > &fx);

  vgl_homg_plane_3d<double> plane() const { return plane_; }

 private:
  double a_, b_, c_, d_;              //parameters of the projection plane
  vgl_homg_plane_3d<double> plane_;
  bwm_observer_cam* master_obs_;
  bwm_observer_cam* sec_obs_;
  vsol_point_2d_sptr master_img_pt_;
  vsol_point_2d_sptr new_pt_;
};

#endif
