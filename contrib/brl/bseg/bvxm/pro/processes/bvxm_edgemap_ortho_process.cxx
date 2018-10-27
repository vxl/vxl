// This is brl/bseg/bvxm/pro/processes/bvxm_edgemap_ortho_process.cxx
#include "bvxm_edgemap_ortho_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_image_view.h>

bool bvxm_edgemap_ortho_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_edgemap_ortho_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "unsigned";  // scale
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";  // also return a z image - the z value of the largest prob edge along the ortho ray
  output_types_[3] = "vpgl_camera_double_sptr";  // return the ortho camera of the scene, as it may be needed for other processes
  return pro.set_output_types(output_types_);
}

// generates an edge map using an ortho camera constructed from the scene bounding box
bool bvxm_edgemap_ortho_process(bprb_func_process& pro)
{
  using namespace bvxm_edgemap_ortho_process_globals;

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

  // scale of image
  auto scale = pro.get_input<unsigned>(1);

  // generate vpgl_geo_camera for the scene
  bvxm_world_params_sptr params = world->get_params();
  vgl_box_3d<double> box = params->world_box_local();
  vgl_point_3d<float> corner = params->corner();
  vgl_point_3d<float> upper_left(corner.x(), (float)(corner.y() + box.height()), corner.z());
  vgl_point_3d<float> lower_right((float)(corner.x()+box.width()), corner.y(), corner.z());
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

   int num_observations = world->num_observations<EDGES>(0,scale);
  std::cout << "Number of observations in curren edge world: " << num_observations << '\n';

  float n_normal = world->get_params()->edges_n_normal();
  std::cout << "n_normal: " << n_normal << '\n';

  // render the expected edge image
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata_inp(dummy_img,camera);
  auto *img_eei_f = new vil_image_view<float>(ni,nj,1);
  vil_image_view_base_sptr img_eei_f_sptr = img_eei_f;

  bvxm_edge_ray_processor edge_proc(world);
  //edge_proc.expected_edge_image(camera_metadata_inp,img_eei_f_sptr,n_normal,scale);

  auto *img_height = new vil_image_view<float>(ni,nj,1);
  vil_image_view_base_sptr img_height_sptr = img_height;
  edge_proc.expected_edge_image_and_heights(camera_metadata_inp,img_eei_f_sptr,img_height_sptr,n_normal,scale);

  auto *img_eei_vb = new vil_image_view<vxl_byte>(ni,nj,1);
  brip_vil_float_ops::normalize_to_interval<float,vxl_byte>(*img_eei_f,*img_eei_vb,0.0f,255.0f);

  pro.set_output_val<vil_image_view_base_sptr>(0, img_eei_f);
  pro.set_output_val<vil_image_view_base_sptr>(1, img_eei_vb);
  pro.set_output_val<vil_image_view_base_sptr>(2, img_height);
  pro.set_output_val<vpgl_camera_double_sptr>(3, camera);

  return true;
}
