#include <testlib/testlib_test.h>
#include <vul/vul_file.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpl/vpl.h>


static void test_voxel_world_update()
{
  std::string model_dir("test_world_dir");
  if (vul_file::is_directory(model_dir))
    vpl_rmdir(model_dir.c_str());  //use this instead of vul since some MS versions hang on on Y/N?
  else if (vul_file::exists(model_dir))
    vpl_unlink(model_dir.c_str());
  vul_file::make_directory(model_dir);


  vgl_point_3d<float> grid_corner(0.f,0.f,0.f);
  vgl_vector_3d<unsigned> grid_size(5,5,1);
  float vox_len = 0.5f;

  // create the world
  bvxm_voxel_world  world;

  // initialize the data
  bvxm_mog_grey_processor::apm_datatype init_apm;

  bvxm_world_params_sptr params = new bvxm_world_params;

  // needs an lvcs, TO DO, instantiate lvcs with right parameters
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  params->set_params(model_dir,grid_corner,grid_size,vox_len,lvcs);

  world.set_params(params);
  world.clean_grids();

  //creating a synthetic projective camera
  vnl_matrix_fixed<double,3,4> camera_matrix;
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

  auto* camera = new vpgl_proj_camera<double>(camera_matrix);


  //create a synthetic image
  vil_image_view_base_sptr view_sptr = new vil_image_view<vxl_byte>(5,5,1,1);

  //create metadata:
  bvxm_image_metadata observation(view_sptr,camera);

  //update
  vil_image_view<float> prob_map(view_sptr->ni(),view_sptr->nj(),1);

  //testing the update on mog_grey,mg_rgb and also with different bin numbers
  vil_image_view<bool> mask(view_sptr->ni(),view_sptr->nj(),1);
  bool result  = world.update<APM_MOG_GREY>(observation, prob_map, mask, 0);
  result = result  && world.update<APM_MOG_GREY>(observation, prob_map, mask, 2);

  TEST("world update", result, true);

  //TO DO: check update for other processors
}

TESTMAIN( test_voxel_world_update );
