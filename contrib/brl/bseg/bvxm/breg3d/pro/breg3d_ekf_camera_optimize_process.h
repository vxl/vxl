// This is brl/bseg/bvxm/breg3d/pro/breg3d_ekf_camera_optimize_process.h
#ifndef breg3d_ekf_camera_optimize_process_h_
#define breg3d_ekf_camera_optimize_process_h_
//:
// \file
// \brief A process that optimizes camera parameters based on image, voxel_world, and GPS/INS measurements
//
// \author Daniel Crispell
// \date Feb 26, 2008
// \verbatim
//  Modifications
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d/pro
// \endverbatim
#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>
#include <ihog/ihog_transform_2d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bvxm/bvxm_voxel_world.h>

class breg3d_ekf_camera_optimize_process : public bprb_process
{
 public:

  breg3d_ekf_camera_optimize_process();

  //: Copy Constructor (no local data)
  breg3d_ekf_camera_optimize_process(const breg3d_ekf_camera_optimize_process& other): bprb_process(*static_cast<const bprb_process*>(&other)) {}

  ~breg3d_ekf_camera_optimize_process() {}

  //: Clone the process
  virtual breg3d_ekf_camera_optimize_process* clone() const {return new breg3d_ekf_camera_optimize_process(*this);}

  virtual vcl_string name() const {return "breg3dEkfCameraOptimize";}

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }

 private:

  //: calculates the Jacobian of the homography wrt the rotation parameters as a function of K
  vnl_matrix<double> homography_jacobian_r(vnl_matrix_fixed<double,3,3> K);

  //: calculates the Jacobian of the homography wrt the translation parameters using finite differences
  vnl_matrix<double> homography_jacobian_t(bvxm_voxel_world_sptr world, vpgl_perspective_camera<double>* cam, vil_image_view_base_sptr &expected_view);

  //: calculate a homography between two images
  ihog_transform_2d calculate_homography(vil_image_view_base_sptr &base_img, vil_image_view_base_sptr &img, vil_image_view_base_sptr &mask);
};


#endif // breg3d_ekf_camera_optimize_process_h_
