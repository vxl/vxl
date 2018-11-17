#include "boxm2_vecf_ocl_transform_minimizer.h"
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include "boxm2_vecf_ocl_transform_scene.h"

boxm2_vecf_ocl_transform_minimizer::boxm2_vecf_ocl_transform_minimizer(boxm2_scene_sptr& source_scene,
                                                                       boxm2_scene_sptr& target_scene,
                                                                       const boxm2_opencl_cache_sptr& ocl_cache,
                                                                       vil_image_view<float> const& ref_img,
                                                                       vpgl_camera_double_sptr const& cam):
  boxm2_vecf_ocl_ssd_func(3, ref_img.ni(), ref_img.nj(), (new boxm2_vecf_ocl_transform_scene( source_scene, target_scene, ocl_cache )), target_scene, ocl_cache)
{

  boxm2_vecf_ocl_ssd_func::set_reference_image(ref_img);
  boxm2_vecf_ocl_ssd_func::set_reference_camera(cam);
}
bool boxm2_vecf_ocl_transform_minimizer::minimize(){
  vnl_vector<double> x(3, 1.0);
  x[0] = 0.995; x[2]=1.005;
  vnl_levenberg_marquardt lm(*this);
  lm.minimize(x);
  lm.diagnose_outcome(std::cout);
  std::cout << "x = " << x << std::endl;
  x_ = x;
  return true;
}

void boxm2_vecf_ocl_transform_minimizer::error_surface_1d(unsigned vindx, double smin, double smax, double sinc){
  vnl_vector<double> x(3, 1.0), fx(ni_*nj_);
  std::cout << '\n';
  for(double s = smin; s<=smax; s+=sinc){
    x[vindx]=s;
    f(x, fx);
    double error = fx.magnitude();
    std::cout << s << ' ' << error << '\n';
  }
}
