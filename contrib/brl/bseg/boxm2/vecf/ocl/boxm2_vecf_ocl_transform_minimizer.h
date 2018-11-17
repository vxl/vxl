#ifndef boxm2_vecf_ocl_transform_minimizer_h
#define boxm2_vecf_ocl_transform_minimizer_h
//:
// \file
// \brief A class to find the minimizing transform of a scene
// \author J.L. Mundy
// \date October 5, 2014
//
#include "boxm2_vecf_ocl_transform_scene.h"
#include "boxm2_vecf_ocl_ssd_func.h"
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
class boxm2_vecf_ocl_transform_minimizer : public boxm2_vecf_ocl_ssd_func
{
 public:
  boxm2_vecf_ocl_transform_minimizer(boxm2_scene_sptr& source_scene,
                                     boxm2_scene_sptr& target_scene,
                                     const boxm2_opencl_cache_sptr& ocl_cache,
                                     vil_image_view<float> const& ref_img,
                                     vpgl_camera_double_sptr const& cam);
  bool minimize();
  void finish(){boxm2_vecf_ocl_ssd_func::finish();}
  vil_image_view<float> opt_diff() {return boxm2_vecf_ocl_ssd_func::diff_img(x_);}
  void error_surface_1d(unsigned vindx, double smin, double smax, double sinc);
  vnl_vector<double> minimizer() const{return x_;}
 private:
  vnl_vector<double> x_;
};
#endif
