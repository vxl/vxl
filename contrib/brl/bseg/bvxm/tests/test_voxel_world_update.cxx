#include <testlib/testlib_test.h>
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_world.h"
#include "../bvxm_world_params.h"
#include "../bvxm_mog_grey_processor.h"
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vpgl/vpgl_proj_camera.h>


static void test_voxel_world_update()
{
  START("bvxm_voxel_world_update test");

  vcl_string model_dir("./test_world_dir");
  vul_file::make_directory(model_dir);

  vgl_point_3d<float> grid_corner(0,0,0);
  vgl_vector_3d<unsigned> grid_size(300,300,120);
  float vox_len = 0.5;

  //bvxm_world_params::appearance_model_type apm_type = bvxm_world_params::mog_grey;


  // create the world
  bvxm_voxel_world  world;

  // initialize the data
  bvxm_mog_grey_processor::apm_datatype init_apm;

  bvxm_world_params_sptr params = new bvxm_world_params;

  // needs an lvcs, TO DO, intantiate lvcs with right paramaters
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  params->set_params(model_dir,grid_corner,grid_size,vox_len,lvcs);

  world.set_params(params);
  world.clean_grids();

  //creating a synthetic projective camera
  vnl_matrix_fixed<double,3,4> camera_matrix ;
  camera_matrix.put(0,0,1);
  camera_matrix.put(0,1,0);
  camera_matrix.put(0,2,0);
  camera_matrix.put(0,3,1);
  camera_matrix.put(1,0,0);
  camera_matrix.put(1,1,1);
  camera_matrix.put(1,2,0);
  camera_matrix.put(1,3,1);
  camera_matrix.put(2,0,0);
  camera_matrix.put(2,1,0);
  camera_matrix.put(2,2,1);
  camera_matrix.put(2,3,1);

  vpgl_proj_camera<double>* camera = new vpgl_proj_camera<double>(camera_matrix);


  //create a synthetic image
  vil_image_view_base_sptr view_sptr = new vil_image_view<vxl_byte>(50,100,1,1);
  // img_view.fill((vxl_byte)1);

  //create metadata:
  bvxm_image_metadata observation(view_sptr,camera);

  //update
  vil_image_view<float> prob_map(view_sptr->ni(),view_sptr->nj(),1);

  //testing the update on mog_grey,mg_rgb and also with different bin numbers
  vil_image_view<bool> mask(view_sptr->ni(),view_sptr->nj(),1);
  world.update<APM_MOG_GREY>(observation, prob_map, mask, 0);
  world.update<APM_MOG_GREY>(observation, prob_map, mask, 2);

  //world.update<APM_MOG_RGB> (observation, prob_map, mask, 10);
  //world.update<APM_MOG_RGB> (observation, prob_map, mask, 15);

  return;
}

TESTMAIN( test_voxel_world_update );

