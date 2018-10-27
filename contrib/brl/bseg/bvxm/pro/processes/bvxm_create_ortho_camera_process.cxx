// This is brl/bseg/bvxm/pro/processes/bvxm_create_ortho_camera_process.cxx
#include "bvxm_create_ortho_camera_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_utm.h>

bool bvxm_create_ortho_camera_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_ortho_camera_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "bool";
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // return the ortho camera of the scene, as it may be needed for other processes
  return pro.set_output_types(output_types_);
}

// generates an ortho camera from the scene bounding box, GSD of the image is 1 meter
bool bvxm_create_ortho_camera_process(bprb_func_process& pro)
{
  using namespace bvxm_create_ortho_camera_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //voxel_world
  bvxm_voxel_world_sptr world =  pro.get_input<bvxm_voxel_world_sptr>(0);
  if (!world) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  bool is_utm = pro.get_input<bool>(1);

  // generate vpgl_geo_camera for the scene
  bvxm_world_params_sptr params = world->get_params();
  vgl_box_3d<double> box = params->world_box_local();
  vgl_point_3d<float> corner = params->corner();
  vgl_point_3d<float> upper_left(corner.x(), (float)(corner.y() + box.height()), corner.z());
  vgl_point_3d<float> lower_right((float)(corner.x()+box.width()), corner.y(), corner.z());
  float voxel_length = params->voxel_length();

  vpgl_lvcs_sptr lvcs = params->lvcs();
  double lat, lon, elev;
  lvcs->get_origin(lat, lon, elev);
  std::cout << " lvcs origin: " << lat << " " << lon << " " << elev << std::endl;

  // determine the upper left corner to use a vpgl_geo_cam, WARNING: assumes that the world is compass-alinged
  double upper_left_lon, upper_left_lat, upper_left_elev;
  lvcs->local_to_global(upper_left.x(), upper_left.y(), upper_left.z(), vpgl_lvcs::wgs84, upper_left_lon, upper_left_lat, upper_left_elev);
  std::cout << "upper left corner in the image is: " << upper_left_lon << " lat: " << upper_left_lat << std::endl;

  double lower_right_lon, lower_right_lat, lower_right_elev;
  lvcs->local_to_global(lower_right.x(), lower_right.y(), lower_right.z(), vpgl_lvcs::wgs84, lower_right_lon, lower_right_lat, lower_right_elev);
  std::cout << "lower right corner in the image is: " << lower_right_lon << " lat: " << lower_right_lat << std::endl;

  vnl_matrix<double> trans_matrix(4,4,0.0);

  if (is_utm) {
    double scale_x = voxel_length;
    double scale_y = -1*voxel_length;
    // transfer upper left corner to utm system
    vpgl_utm utm;
    double upper_left_x, upper_left_y;
    int utm_zone;
    utm.transform(upper_left_lat, upper_left_lon, upper_left_x, upper_left_y, utm_zone);
    std::cout << "upper left in utm = " << upper_left_x << " x " << upper_left_y << std::endl;
    std::cout << "scale_x = " << scale_x << " scale_y = " << scale_y << std::endl;
    trans_matrix[0][0] = scale_x;
    trans_matrix[1][1] = scale_y;
    trans_matrix[0][3] = upper_left_x;
    trans_matrix[1][3] = upper_left_y;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    unsigned northing = 1;
    if (upper_left_lat < 0 && lower_right_lat < 0)
      northing = 0;
    if (upper_left_lat*lower_right_lat < 0)
      std::cout << "warning: scene world crosses the Equator" << std::endl;
    cam->set_utm(utm_zone,northing);
    cam->set_scale_format(true);
    vpgl_camera_double_sptr camera = new vpgl_geo_camera(*cam);
    pro.set_output_val<vpgl_camera_double_sptr>(0, camera);
    return true;
  }
  else {
    auto ni = (unsigned)std::ceil(box.width() / voxel_length);
    auto nj = (unsigned)std::ceil(box.height()/ voxel_length);
    //trans_matrix[0][0] = (lower_right_lon-lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lat)/nj;
    // lvcs origin is not necessarily one of the corners of the scene
    trans_matrix[0][0] = (lower_right_lon-upper_left_lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lower_right_lat)/nj;
    trans_matrix[0][3] = upper_left_lon; trans_matrix[1][3] = upper_left_lat;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    cam->set_scale_format(true);
    vpgl_camera_double_sptr camera = new vpgl_geo_camera(*cam);

    pro.set_output_val<vpgl_camera_double_sptr>(0, camera);
    return true;
  }


}
