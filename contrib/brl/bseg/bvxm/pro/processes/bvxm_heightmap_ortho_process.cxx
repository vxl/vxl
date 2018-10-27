// This is brl/bseg/bvxm/pro/processes/bvxm_heightmap_ortho_process.cxx
#include "bvxm_heightmap_ortho_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <vil/vil_image_view.h>

bool bvxm_heightmap_ortho_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_ortho_process_globals;
  // This process has 4 inputs:
  //input[0]: The voxel world
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // this one is the depth map from top
  output_types_[1] = "vil_image_view_base_sptr";  // this one is negated depth map, so height map from scene floor + scene floor height --> eventual output is an absolute height map
  output_types_[2] = "vil_image_view_base_sptr";  // confidence map: max prob value along the ortho ray that generated the height value at that pixel, can be used to filter noise from the height map
  return pro.set_output_types(output_types_);
}

// generates a height map from a given camera viewpoint
bool bvxm_heightmap_ortho_process(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_ortho_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  unsigned i = 0;
  //voxel_world
  bvxm_voxel_world_sptr world =  pro.get_input<bvxm_voxel_world_sptr>(i++);
  if (!world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }

  // generate vpgl_geo_camera for the scene
  bvxm_world_params_sptr params = world->get_params();
  vgl_box_3d<double> box = params->world_box_local();
  vgl_point_3d<float> corner = params->corner();
  vgl_point_3d<float> upper_left(corner.x(), (float)(corner.y() + box.height()), corner.z());
  vgl_point_3d<float> lower_right((float)(corner.x()+box.width()), corner.y(), corner.z());
  vpgl_lvcs_sptr lvcs = params->lvcs();

  double lat, lon, elev;
  /*lvcs->get_origin(lat, lon, elev);
  std::cout << " lvcs origin: " << lat << " " << lon << " " << elev << std::endl;*/
  // find the elevation of the point where the ray tracing stops which is one slab below the lowest world slab
  vgl_point_3d<float> ray_trace_end = world->voxel_index_to_xyz(0, 0, params->num_voxels().z()+1, 0);
  lvcs->local_to_global(ray_trace_end.x(), ray_trace_end.y(), ray_trace_end.z(), vpgl_lvcs::wgs84, lon, lat, elev);
  auto base_elev = (float)elev;

  // determine the upper left corner to use a vpgl_geo_cam, WARNING: assumes that the world is compass-alinged
  double upper_left_lon, upper_left_lat, upper_left_elev;
  lvcs->local_to_global(upper_left.x(), upper_left.y(), upper_left.z(), vpgl_lvcs::wgs84, upper_left_lon, upper_left_lat, upper_left_elev);
  std::cout << "upper left corner in the image is: " << upper_left_lon << " lat: " << upper_left_lat << std::endl;

  double lower_right_lon, lower_right_lat, lower_right_elev;
  lvcs->local_to_global(lower_right.x(), lower_right.y(), lower_right.z(), vpgl_lvcs::wgs84, lower_right_lon, lower_right_lat, lower_right_elev);
  std::cout << "lower right corner in the image is: " << lower_right_lon << " lat: " << lower_right_lat << std::endl;

  int ni = box.width();
  int nj = box.height();
  vnl_matrix<double> trans_matrix(4,4,0.0);
  //trans_matrix[0][0] = (lower_right_lon-lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lat)/nj;
  // lvcs origin is not necessarily one of the corners of the scene
  trans_matrix[0][0] = (lower_right_lon-upper_left_lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lower_right_lat)/nj;
  trans_matrix[0][3] = upper_left_lon; trans_matrix[1][3] = upper_left_lat;
  vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
  cam->set_scale_format(true);
  vpgl_camera_double_sptr camera = new vpgl_geo_camera(*cam);

  auto *dmap = new vil_image_view<unsigned>(ni, nj, 1);
  auto *conf_map = new vil_image_view<float>(ni, nj, 1);
  world->heightmap(camera,*dmap, *conf_map);  // this method actually generates a depth not a height map

  // subtract from the scene height to get the height from scene floor
  float h = box.depth();

  std::cout << "Using scene height: " << h << " to negate the depth map!\n";
  auto *hmap = new vil_image_view<float>(ni, nj, 1);
  hmap->fill(0.0f);
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++)
      (*hmap)(i,j) = h-(*dmap)(i,j)+base_elev;

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, dmap);
  pro.set_output_val<vil_image_view_base_sptr>(1, hmap);
  pro.set_output_val<vil_image_view_base_sptr>(2, conf_map);

  return true;
}


// compute the expected height map and its variance along each ray
bool bvxm_heightmap_exp_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_exp_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "unsigned"; // ni
  input_types_[i++] = "unsigned"; // nj
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // expected height map
  output_types_[1] = "vil_image_view_base_sptr";  // variance of height map
  return pro.set_output_types(output_types_);
}

// generates a height map from a given camera viewpoint
bool bvxm_heightmap_exp_process(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_exp_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  unsigned i = 0;
  //voxel_world
  bvxm_voxel_world_sptr world =  pro.get_input<bvxm_voxel_world_sptr>(i++);
  if (!world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni = pro.get_input<unsigned>(i++);
  auto nj = pro.get_input<unsigned>(i++);

   // generate vpgl_geo_camera for the scene
  bvxm_world_params_sptr params = world->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();

  double lat, lon, elev;
  vgl_point_3d<float> ray_trace_end = world->voxel_index_to_xyz(0, 0, params->num_voxels().z()+1, 0);
  lvcs->local_to_global(ray_trace_end.x(), ray_trace_end.y(), ray_trace_end.z(), vpgl_lvcs::wgs84, lon, lat, elev);
  auto base_elev = (float)elev;
  std::cout << "!!!!!!!!!!!!!! base_elev: " << base_elev << std::endl;

  auto *hmap = new vil_image_view<float>(ni, nj, 1);
  auto *var_map = new vil_image_view<float>(ni, nj, 1);
  float max_depth;
  world->heightmap_exp(cam,*hmap, *var_map, max_depth);

  float max_val, min_val;
  vil_math_value_range(*hmap, min_val, max_val);
  std::cout << "!!!!!!!!!!! in height map min_val: " << min_val << " max_val: " << max_val << "!\n";

  std::cout << "!!!!!!!!!!! Using max_depth: " << max_depth << " to negate the depth map!\n";
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      (*hmap)(i,j) = max_depth-(*hmap)(i,j)+base_elev;
      (*var_map)(i,j) = std::sqrt((*var_map)(i,j));
    }

  vil_math_value_range(*hmap, min_val, max_val);
  std::cout << "!!!!!!!!!!! in height map min_val: " << min_val << " max_val: " << max_val << "!\n";

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, hmap);
  pro.set_output_val<vil_image_view_base_sptr>(1, var_map);
  return true;
}



// compute the average uncertainty along each ray
bool bvxm_uncertainty_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_uncertainty_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "unsigned"; // ni
  input_types_[i++] = "unsigned"; // nj
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // uncertainty map
  return pro.set_output_types(output_types_);
}

// generates a height map from a given camera viewpoint
bool bvxm_uncertainty_process(bprb_func_process& pro)
{
  using namespace bvxm_uncertainty_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  unsigned i = 0;
  //voxel_world
  bvxm_voxel_world_sptr world =  pro.get_input<bvxm_voxel_world_sptr>(i++);
  if (!world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni = pro.get_input<unsigned>(i++);
  auto nj = pro.get_input<unsigned>(i++);

  auto *dmap = new vil_image_view<float>(ni, nj, 1);
  world->uncertainty(cam,*dmap);

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, dmap);
  return true;
}
