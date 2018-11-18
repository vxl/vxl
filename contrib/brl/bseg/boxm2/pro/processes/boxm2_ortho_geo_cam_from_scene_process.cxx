// This is brl/bseg/boxm2/pro/processes/boxm2_ortho_geo_cam_from_scene_process.cxx
//:
// \file
// \brief A process for creating a orthogonal vpgl_geo camera given a scene
//
// \author Yi Dong
// \date Feb 13, 2014

#include <iostream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm2_ortho_geo_cam_from_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 3;
}

bool boxm2_ortho_geo_cam_from_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ortho_geo_cam_from_scene_process_globals;

  // process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  // process has 1 output, vpgl_geo_camera
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";    // return the ortho camera of the scene that can be used to generate height map from the scene
  output_types_[1] = "unsigned";                   // image size ni
  output_types_[2] = "unsigned";                   // image size nj

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ortho_geo_cam_from_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_ortho_geo_cam_from_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the input
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);

  vpgl_lvcs lvcs = scene->lvcs();
  vgl_box_3d<double> bbox = scene->bounding_box();

  // obtain the scene finest resolution
  // note that the sub block size is truncated to integer here
  std::map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();
  double res_x = 1E5, res_y = 1E5;
  for (auto & blk : blks)
  {
    double voxel_size_x = (blk.second.sub_block_dim_.x()) / (1 << (blk.second.max_level_ - blk.second.init_level_));
    double voxel_size_y = (blk.second.sub_block_dim_.y()) / (1 << (blk.second.max_level_ - blk.second.init_level_));
    if (res_x > voxel_size_x) res_x = voxel_size_x;
    if (res_y > voxel_size_y) res_y = voxel_size_y;
  }

  unsigned ni, nj;
  ni = (unsigned)std::ceil((bbox.max_x()-bbox.min_x())/res_x);
  nj = (unsigned)std::ceil((bbox.max_y()-bbox.min_y())/res_y);

  vgl_point_3d<double> upper_left(bbox.min_x(), bbox.max_y(), bbox.min_z());
  vgl_point_3d<double> lower_rght(bbox.max_x(), bbox.min_y(), bbox.min_z());
  double upper_left_lon, upper_left_lat, upper_left_elev;
  double lower_rght_lon, lower_rght_lat, lower_rght_elev;
  lvcs.local_to_global(upper_left.x(), upper_left.y(), upper_left.z(), vpgl_lvcs::wgs84, upper_left_lon, upper_left_lat, upper_left_elev);
  lvcs.local_to_global(lower_rght.x(), lower_rght.y(), lower_rght.z(), vpgl_lvcs::wgs84, lower_rght_lon, lower_rght_lat, lower_rght_elev);

  double scale_x = (lower_rght_lon - upper_left_lon)/(double)ni;
  double scale_y = (upper_left_lat - lower_rght_lat)/(double)nj;

  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = -scale_y;
  trans_matrix[0][3] = upper_left_lon;
  trans_matrix[1][3] = upper_left_lat;

  vpgl_lvcs_sptr lvcs_ptr = new vpgl_lvcs(lvcs);
  vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs_ptr);
  cam->set_scale_format(true);
  vpgl_camera_double_sptr camera = new vpgl_geo_camera(*cam);

  std::cout << " the bounding box of the scene is: " << bbox << std::endl;
  std::cout << " scene finest voxel resolution is: " << res_x << " x " << res_y << std::endl;
  std::cout << " length in meters: " << ni << " x " << nj << std::endl;
  std::cout << " upper_left:  " << std::setprecision(12) << upper_left_lon << " x " << upper_left_lat << std::endl;
  std::cout << " lower_right: " << std::setprecision(12) << lower_rght_lon << " x " << lower_rght_lat << std::endl;
  std::cout << " transformation matrix is: \n";
  trans_matrix.print(std::cout);
  std::cout << std::endl;

  pro.set_output_val<vpgl_camera_double_sptr>(0, camera);
  pro.set_output_val<unsigned>(1, ni);
  pro.set_output_val<unsigned>(2, nj);
  return true;
}
