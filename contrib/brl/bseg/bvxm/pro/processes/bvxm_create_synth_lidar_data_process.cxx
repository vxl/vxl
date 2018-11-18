#include <iostream>
#include "bvxm_create_synth_lidar_data_process.h"
//:
// \file
#include <bvxm/bvxm_util.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:sets input and output types
bool bvxm_create_synth_lidar_data_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_synth_lidar_data_process_globals;
  std::vector<std::string> output_types_(n_outputs_);
  // this process takes no input
  // output:
  // 1. lidar image
  // 2. camera related to that image
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vpgl_camera_double_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  return true;
}

bool gen_lidar_view(int x, int y, int z,
                    std::vector<vgl_box_3d<double> > boxes,
                    vil_image_view_base_sptr& lidar,
                    vpgl_camera_double_sptr& cam,
                    vpgl_lvcs_sptr& lvcs)
{
  vil_image_view<float> lv(x, y);
  lv.fill(0);

  // Place the heights of boxes on lidar
  for (auto box : boxes) {
    double maxz = box.max_z(); // top face of the box
    if (maxz > z) maxz = z;
    for (int i=0; i<x; i++)
      for (int j=0; j<y; j++) {
        vgl_point_3d<double> p(i,j,maxz);
        if (box.contains(p)) {
          // check if there is already a higher d there
          if (lv(i,j) < (float)maxz)
            lv(i,j) = (float)maxz;
        }
      }
  }

  lidar = new vil_image_view<float>(lv);

  // generate the camera, which is a one to one mapping between
  // lidar image and voxel slabs
  auto* geo_cam = new vpgl_geo_camera();
  geo_cam->set_lvcs(lvcs);
  cam = geo_cam;
  return true;
}

//: Execute the process
bool bvxm_create_synth_lidar_data_process(bprb_func_process& pro)
{
  using namespace bvxm_create_synth_lidar_data_process_globals;
  // define and read in the parameters
  // voxel world dimensions
  unsigned int v_dimx = 100, v_dimy=100, v_dimz=100;
  pro.parameters()->get_value(param_voxel_dim_x_, v_dimx);
  pro.parameters()->get_value(param_voxel_dim_y_, v_dimy);
  pro.parameters()->get_value(param_voxel_dim_z_, v_dimz);

  // box dimensions
  unsigned int dimx=50, dimy=50, dimz=50;
  pro.parameters()->get_value(param_box_dim_x_, dimx);
  pro.parameters()->get_value(param_box_dim_y_, dimy);
  pro.parameters()->get_value(param_box_dim_z_, dimz);

  // box min point
  unsigned int minx=20, miny=20, minz=20;
  pro.parameters()->get_value(param_box_min_x_, minx);
  pro.parameters()->get_value(param_box_min_y_, miny);
  pro.parameters()->get_value(param_box_min_z_, minz);

  // lvcs parameters
  std::string lvcs_path;
  if (!pro.parameters()->get_value(param_lvcs_, lvcs_path)) {
    std::cout << "bvxm_create_voxel_world_process::execute() -- problem in retrieving parameter lvcs_path\n";
    return false;
  }

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  if (lvcs_path != "") {
    std::ifstream is(lvcs_path.c_str());
    if (!is)
    {
      std::cerr << " Error opening file  " << lvcs_path << std::endl;
      return false;
    }
    lvcs->read(is);
  }

  // generate boxes, lidar image
  vil_image_view_base_sptr lidar_img;
  vpgl_camera_double_sptr lidar_cam;
  std::vector<vgl_box_3d<double> > boxes;
  bvxm_util::generate_test_boxes((double)minx, (double)miny, (double)minz,
                                 (double)dimx, (double)dimy, (double)dimz,
                                 (double)v_dimx, (double)v_dimy, (double)v_dimz, boxes);
  gen_lidar_view(v_dimx, v_dimy, v_dimz, boxes, lidar_img, lidar_cam, lvcs);

  pro.set_output_val<vil_image_view_base_sptr>(0, lidar_img);
  pro.set_output_val<vpgl_camera_double_sptr>(1, lidar_cam);

  return true;
}
