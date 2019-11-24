#include "testlib/testlib_test.h"
#include "vul/vul_file.h"

#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_vector_3d.h"

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include "vil/vil_load.h"
#include "vil/vil_image_resource_sptr.h"
#include "vil/vil_convert.h"
#include "vpgl/vpgl_proj_camera.h"


static void test_voxel_world()
{
  std::string model_dir("test_world_dir");
  if (vul_file::is_directory(model_dir))
    vul_file::delete_file_glob(model_dir+"/*");
  else {
    if (vul_file::exists(model_dir))
      vul_file::delete_file_glob(model_dir);
    vul_file::make_directory(model_dir);
  }

  vgl_point_3d<float> grid_corner(2170.0f, -1480.0f, -30.0f);
  vgl_vector_3d<unsigned> grid_size(300,300,120);
  float vox_len = 0.5f;

 //bvxm_world_params::appearance_model_type apm_type = bvxm_world_params::mog_grey;
  // setup global parameters

  bvxm_world_params_sptr params = new bvxm_world_params();

  // NEEDS AN LVCS, TODO: initialize lvcs
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  params->set_params(model_dir,grid_corner,grid_size,vox_len,lvcs);

  // create the world
  bvxm_voxel_world world;
  world.set_params(params);

  // initialize the data
  bvxm_mog_grey_processor::apm_datatype init_apm;


#if 0
  float init_ocp = 1.0f/120;
  world.init_world(init_ocp,init_apm);

  //camera for frame 0
  //std::string cam_file = "./cameras/frame_00000.txt";
  // run the executable for the test file from lemsvxl/build/contrib/bvxm/tests/  to find the following cam file
  std::string cam_file = "frame_00000.txt";

  std::ifstream cam_ifstream(cam_file.c_str());
  vpgl_proj_camera<double>* camera = new vpgl_proj_camera<double>();
  cam_ifstream >> *camera;

  //frame 0
  //vil_image_resource_sptr img_res_sptr = vil_load_image_resource("./test_data/frame_00000.png");
  // run the executable for the test file from lemsvxl/build/contrib/bvxm/tests/
  vil_image_resource_sptr img_res_sptr = vil_load_image_resource("frame_00000.png");
  vil_image_view_base_sptr view_sptr = vil_convert_cast(vxl_byte(),img_res_sptr->get_view());

  //create metadata:
  bvxm_image_metadata observation(view_sptr,camera);

  //update
  vil_image_view<float> prob_map(view_sptr->ni(),view_sptr->nj(),1);
  world.update<APM_MOG_GREY>(observation, prob_map);
  // TODO: cleanup files
#endif // 0
}

TESTMAIN( test_voxel_world );
