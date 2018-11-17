//This is brl/bseg/bvxm/pro/processes/bvxm_synth_world_generator.h
#ifndef bvxm_synth_world_generator_h_
#define bvxm_synth_world_generator_h_
//:
// \file
// \brief A class for generating a synthetic bvxm_voxel_world.
//
// \author Isabel Restrepo
// \date March 7, 2009
// \verbatim
//  Modifications
//   10 Mar, 2009 - Isabel Restrepo: This class is based on the old bvxm_gen_synthetic_world_process.
//                                   Some of the methods for reconstruction have been left out
// \endverbatim

#include <iostream>
#include <string>
#include <vector>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <bvxm/bvxm_util.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_mog_grey_processor.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvxm_synth_world_generator
{
 public:

  bvxm_synth_world_generator();
  bvxm_voxel_world_sptr generate_world();
  void set_dim_parmas( unsigned nx, unsigned ny, unsigned nz);
  void set_min_dim_params( unsigned minx, unsigned miny, unsigned minz);
  void set_bool_params ( bool gen_out_imgs, bool gen2box, bool rand1, bool rand2);
  void set_world_dir(std::string world_dir);
  void set_app_val (float app_val);
  void set_world_length(float len);

 private:

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype apm_datatype;

  //store parameters as global variables to be used across functions
  unsigned nx_;
  unsigned ny_;
  unsigned nz_;
  unsigned minx_;
  unsigned miny_;
  unsigned minz_;
  unsigned dimx_;
  unsigned dimy_;
  unsigned dimz_;
  unsigned num_train_images_;
  bool gen_images_;
  bool gen2_box_;
  bool texture_map_;
  std::string world_dir_;
  std::string recon_world_dir_;
  bool rand1_;
  bool rand2_;
  float app_val_;

 //hard-coded values ... should be removed and added as a parameter class
  int IMAGE_U;
  int IMAGE_V;
  double x_scale;
  double y_scale;
  double focal_length;
  double camera_dist;

  //size of the world
  float vox_length;

  //for debugging purposes
  bool verbose;

  std::vector<vgl_box_3d<double> > boxes_vector;

  //this process functions
  int on_box_surface(vgl_box_3d<double> box, vgl_point_3d<double> v);
  int in_box(vgl_box_3d<double> box, vgl_point_3d<double> v);
  void generate_persp_camera(double focal_length,
                             vgl_point_2d<double>& pp,  //principal point
                             double x_scale, double y_scale,
                             vgl_point_3d<double>& camera_center,
                             vpgl_perspective_camera<double>& cam);
  vpgl_rational_camera<double> perspective_to_rational(vpgl_perspective_camera<double>& cam_pers);
  std::vector<vpgl_camera_double_sptr > generate_cameras_yz(vgl_box_3d<double>& world);
  std::vector<vpgl_camera_double_sptr > generate_cameras_z(vgl_box_3d<double>& world);
  bool update(vgl_vector_3d<unsigned> grid_size,
              const bvxm_voxel_world_sptr& world,
              bvxm_voxel_grid<float>* intensity_grid,
              bvxm_voxel_grid<float>* ocp_grid,
              bvxm_voxel_grid<apm_datatype>* apm_grid,
              std::vector<vpgl_camera_double_sptr>& cameras,
              std::vector <vil_image_view_base_sptr>& image_set,
              unsigned int bin_num);
  void gen_texture_map(vgl_box_3d<double> box,
                       std::vector<std::vector<float> >& intens_map_bt,
                       std::vector<std::vector<float> >& intens_map_side1,
                       std::vector<std::vector<float> >& intens_map_side2,
                       bool gen_rand, float app_val);

  void gen_voxel_world_2box(vgl_vector_3d<unsigned> grid_size,
                            vgl_box_3d<double> voxel_world,
                            bvxm_voxel_grid<float>* ocp_grid,
                            bvxm_voxel_grid<float>* intensity_grid,
                            unsigned minx, unsigned miny, unsigned minz,
                            unsigned dimx, unsigned dimy, unsigned dimz,
                            unsigned nx, unsigned ny, unsigned nz,
                            bool gen_2box);

  void gen_voxel_world_plane(vgl_vector_3d<unsigned> grid_size,
                             vgl_box_3d<double> voxel_world,
                             bvxm_voxel_grid<float>* ocp_grid,
                             bvxm_voxel_grid<float>* intensity_grid,
                             unsigned nx, unsigned ny, unsigned nz);

  bool gen_lidar_2box( vgl_vector_3d<unsigned> grid_size,const bvxm_voxel_world_sptr& world);
};

#endif
