#ifndef boxm2_vecf_ocl_ssd_func_h_
#define boxm2_vecf_ocl_ssd_func_h_
//:
// \file
// \brief A cost function for deforming volumes based on ssd
// \author J.L. Mundy
// \date October 12, 2014
//
#include <vnl/vnl_vector.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include "boxm2_vecf_ocl_transform_scene_sptr.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_least_squares_function.h>
#include <vil/vil_image_view.h>
#include "boxm2_vecf_ocl_ssd_func.h"
#include "boxm2_vecf_ocl_transform_scene.h"
#include <boxm2/ocl/algo/boxm2_ocl_expected_image_renderer.h>
#include <boxm2/ocl/algo/boxm2_ocl_depth_renderer.h>


//: A cost function for deforming a volume
//  minimizing square difference in intensities while varying anisotropic scale
class boxm2_vecf_ocl_ssd_func : public vnl_least_squares_function
{
 public:
 // boxm2_vecf_ocl_ssd_func():vnl_least_squares_function(0, 0) {}
 // boxm2_vecf_ocl_ssd_func(unsigned nx, unsigned ni, unsigned nj):
 //  vnl_least_squares_function(nx, ni*nj,vnl_least_squares_function::no_gradient), ni_(ni), nj_(nj) {}
  boxm2_vecf_ocl_ssd_func(unsigned nx, unsigned ni, unsigned nj, const boxm2_vecf_ocl_transform_scene_sptr& tscn, const boxm2_scene_sptr& target_scene,const boxm2_opencl_cache_sptr& ocl_cache);
  void set_reference_image(vil_image_view<float> const& ref_img)
  { ref_img_ = ref_img;}
  void set_reference_camera(vpgl_camera_double_sptr const& cam)
  { ref_cam_ = cam;}
  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;
  vil_image_view<float> diff_img(vnl_vector<double> const& x) ;
  void finish();
 protected:
  void scale_to_x(vgl_vector_3d<double>const& scale, vnl_vector<double>& x) const{
    x[0]=scale.x();    x[1]=scale.y();   x[2]=scale.z();
  }
  void x_to_scale(vnl_vector<double> const& x, vgl_vector_3d<double>& scale ) const{
    scale.set(x[0], x[1], x[2]);
  }

  bool render_scene_appearance(vpgl_camera_double_sptr const & cam,
                               vil_image_view<float>& expected_img, vil_image_view<float>& vis_img, unsigned ni, unsigned nj) ;
  bool render_scene_depth(vpgl_camera_double_sptr const & cam,
                               vil_image_view<float>& expected_depth,  vil_image_view<float>& vis_img, unsigned ni, unsigned nj);

  boxm2_vecf_ocl_transform_scene_sptr tscn_;
  boxm2_scene_sptr target_scene_;

  boxm2_ocl_expected_image_renderer renderer_;
  boxm2_ocl_depth_renderer depth_renderer_;

  vgl_vector_3d<double> trans_;
  vgl_rotation_3d<double> rot_;
  vil_image_view<float> ref_img_;
  vpgl_camera_double_sptr ref_cam_;
  unsigned ni_;
  unsigned nj_;
};


#endif // boxm2_vecf_ocl_ssd_func_h_
