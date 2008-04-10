// This is brl/bseg/bvxm/pro/bvxm_gen_synthetic_world_process.h
#ifndef bvxm_gen_synthetic_world_process_h_
#define bvxm_gen_synthetic_world_process_h_
//:
// \file
// \brief A class for generating a synthetic bvxm_voxel_world.
//        Adapted form bvxm_test_gen_synthetic_world
//
// \author Isabel Restrepo
// \date 04/03/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include "../bvxm_voxel_grid.h"
#include "../bvxm_voxel_slab.h"
#include "../bvxm_voxel_world.h"
#include "../bvxm_world_params.h"
#include "../bvxm_mog_grey_processor.h"

typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype apm_datatype;

class bvxm_gen_synthetic_world_process : public bprb_process
{
 public:

  bvxm_gen_synthetic_world_process();

  //: Copy Constructor (no local data)
  bvxm_gen_synthetic_world_process(const bvxm_gen_synthetic_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_gen_synthetic_world_process(){};

  //: Clone the process
  virtual bvxm_gen_synthetic_world_process* clone() const {return new bvxm_gen_synthetic_world_process(*this);}

  vcl_string name(){return "bvxmGenSyntheticWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool test(bvxm_voxel_world_sptr r) { return test_reconstructed_ocp(r); }
  bool finish(){return true; }

 private:

  int on_box_surface(vgl_box_3d<double> box, vgl_point_3d<double> v);
  int in_box(vgl_box_3d<double> box, vgl_point_3d<double> v);

  void generate_persp_camera(double focal_length,
                             vgl_point_2d<double>& pp,  //principal point
                             double x_scale, double y_scale,
                             vgl_point_3d<double>& camera_center,
                             vpgl_perspective_camera<double>& cam);

  vpgl_rational_camera<double>
  perspective_to_rational(vpgl_perspective_camera<double>& cam_pers) ;

  vcl_vector<vpgl_camera_double_sptr >
  generate_cameras_yz(vgl_box_3d<double>& world);

  bool gen_images(vgl_vector_3d<unsigned> grid_size,
                  bvxm_voxel_world_sptr world,
                  bvxm_voxel_grid<float>* intensity_grid,
                  bvxm_voxel_grid<float>* ocp_grid,
                  bvxm_voxel_grid<apm_datatype>* apm_grid,
                  vcl_vector<vpgl_camera_double_sptr>& cameras,
                  vcl_vector <vil_image_view_base_sptr>& image_set,
                  unsigned int bin_num);

  bool reconstruct_world(bvxm_voxel_world_sptr recon_world, vcl_vector<vpgl_camera_double_sptr>& cameras,
                         vcl_vector <vil_image_view_base_sptr>& image_set,unsigned int bin_num);

  void gen_texture_map(vgl_box_3d<double> box,
                       vcl_vector<vcl_vector<float> >& intens_map_bt,
                       vcl_vector<vcl_vector<float> >& intens_map_side1,
                       vcl_vector<vcl_vector<float> >& intens_map_side2);

  void gen_voxel_world_2box(vgl_vector_3d<unsigned> grid_size,
                            vgl_box_3d<double> voxel_world,
                            bvxm_voxel_grid<float>* ocp_grid,
                            bvxm_voxel_grid<float>* intensity_grid);

  void gen_voxel_world_plane(vgl_vector_3d<unsigned> grid_size,
                             vgl_box_3d<double> voxel_world,
                             bvxm_voxel_grid<float>* ocp_grid,
                             bvxm_voxel_grid<float>* intensity_grid);

  bool test_reconstructed_ocp(bvxm_voxel_world_sptr recon_world);
};

#endif // bvxm_gen_synthetic_world_process_h_
