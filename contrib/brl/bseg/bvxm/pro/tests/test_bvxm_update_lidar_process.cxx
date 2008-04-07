#include <testlib/testlib_test.h>
#include "../bvxm_update_lidar_process.h"
#include "../bvxm_gen_synthetic_world_process.h"
#include "../bvxm_create_voxel_world_process.h"
#include "../bvxm_create_synth_lidar_data_process.h"
#include "../bvxm_save_occupancy_raw_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <vil/vil_image_view.h>

#include <vul/vul_file.h>

MAIN( test_bvxm_update_lidar_process )
{
  REG_PROCESS(bvxm_create_synth_lidar_data_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_voxel_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_raw_process, bprb_batch_process_manager);
  //REG_PROCESS(bvxm_gen_synthetic_world_process, bprb_batch_process_manager);
  // REG_PROCESS(bvxm_lidar_init_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_lidar_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(unsigned);

  //global variables
  vil_image_view_base_sptr lidar_img_;
  vil_image_view_base_sptr prob_map_;
  bvxm_voxel_world_sptr world_;
  vpgl_camera_double_sptr lidar_cam_;

  // 1. Set inputs for bvxm_create_synth_lidar_data_process

  {
    bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateSynthLidarDataProcess");
    good = bprb_batch_process_manager::instance()->set_params("synth_test_params.xml");
    good = good && bprb_batch_process_manager::instance()->run_process();

    unsigned id_lidar_img;
    unsigned id_cam;
  
    good = good && bprb_batch_process_manager::instance()->commit_output(0, id_lidar_img);
    good = good && bprb_batch_process_manager::instance()->commit_output(1, id_cam);

    TEST("run create syhtnetic lidar data process", good ,true);

    //retrieve lidar image
    brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_lidar_img);
    brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);

    if (S_img->size()!=1){
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " no selections\n";
    }

    brdb_value_sptr value_img;
    if (!S_img->get_value(vcl_string("value"), value_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " didn't get value\n";
    }
    bool non_null = (value_img != 0);
    TEST("display output non-null", non_null ,true);

    brdb_value_t<vil_image_view_base_sptr>* result =
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
    vil_image_view_base_sptr lidar_img = result->value();

    lidar_img_ = lidar_img; 

    
   //vil_image_view<vxl_byte> lidar2 = static_cast<vil_image_view<vxl_byte>>(*lidar_img);
   //vil_save(lidar2, "./lidar_img.png");

    bool saved = vil_save(*lidar_img, "lidar_image.tif");
    TEST("lidar image saved", saved ,true);

    //retrieve lidar camera
    brdb_query_aptr Q_cam = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
    brdb_selection_sptr S_cam = DATABASE->select("vpgl_camera_double_sptr_data", Q_cam);

    if (S_cam->size()!=1){
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " no selections\n";
    }

    brdb_value_sptr value_cam;
    if (!S_cam->get_value(vcl_string("value"), value_cam)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " didn't get value\n";
    }
   
    non_null = (value_cam != 0);
    TEST("display output non-null", non_null ,true);

    brdb_value_t<vpgl_camera_double_sptr>* result2 =
      static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(value_cam.ptr());
    vpgl_camera_double_sptr lidar_cam = result2->value();

    lidar_cam_ = lidar_cam;
  }

  ///***********************Generate Synthetic World*********************************/
  //{
  //  // Set output for bvxm_gen_synthetic_world_process

  //  bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenSyntheticWorldProcess");
  //  //good = bprb_batch_process_manager::instance()->set_params("change_display_params.xml");
  //  good = good && bprb_batch_process_manager::instance()->run_process();

  //  unsigned id_world;

  //  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_world);

  //  TEST("run gen synthetic world process", good ,true);

  //  //retrieve world 
  //  brdb_query_aptr Q_world = brdb_query_comp_new("id", brdb_query::EQ, id_world);
  //  brdb_selection_sptr S_world = DATABASE->select("bvxm_voxel_world_sptr_data", Q_world);
  //  if (S_world->size()!=1){
  //    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
  //      << " no selections\n";
  //  }

  //  brdb_value_sptr value_world;
  //  if (!S_world->get_value(vcl_string("value"), value_world)) {
  //    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
  //      << " didn't get value\n";
  //  }
  //  bool non_null = (value_world != 0);
  //  TEST("display output non-null", non_null ,true);

  //  brdb_value_t<bvxm_voxel_world_sptr>* result =
  //    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_world.ptr());
  //  bvxm_voxel_world_sptr world = result->value();

  //  world_ = world;
  //}

    /***********************Generate Empty World*********************************/
  {
    // Set output for bvxm_gen_synthetic_world_process

    vcl_string test_dir("./create_world_test");
    vul_file::make_directory(test_dir);
    	
    bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateVoxelWorldProcess");
    good = bprb_batch_process_manager::instance()->set_params("world_model_params.xml");
    good = good && bprb_batch_process_manager::instance()->run_process();

    unsigned id_world;

    good = good && bprb_batch_process_manager::instance()->commit_output(0, id_world);

    TEST("run gen synthetic world process", good ,true);

    //retrieve world 
    brdb_query_aptr Q_world = brdb_query_comp_new("id", brdb_query::EQ, id_world);
    brdb_selection_sptr S_world = DATABASE->select("bvxm_voxel_world_sptr_data", Q_world);
    if (S_world->size()!=1){
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " no selections\n";
    }

    brdb_value_sptr value_world;
    if (!S_world->get_value(vcl_string("value"), value_world)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " didn't get value\n";
    }
    bool non_null = (value_world != 0);
    TEST("display output non-null", non_null ,true);

    brdb_value_t<bvxm_voxel_world_sptr>* result =
      static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_world.ptr());
    bvxm_voxel_world_sptr world = result->value();

    world_ = world;
  }
  
   /***************************** Save Raw data**************************************/

  {
    // 3. Run save raw process
     vcl_string empty_world("empty.raw");

    // set the inputs 
    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);
    brdb_value_sptr v1 = new brdb_value_t<vcl_string> (empty_world);
    
    
    bool good = bprb_batch_process_manager::instance()->init_process("bvxmSaveOccupancyRaw");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
    good = good && bprb_batch_process_manager::instance()->run_process();

   
    TEST("run save occupancy empty wolrd process", good ,true);

  
  }
  
  /***************************** Update World with lidar data**************************************/

  {
    // 3. Run update lidar process

    // set the inputs 
    brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(lidar_img_);
    brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr> (lidar_cam_);
    brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);


    bool good = bprb_batch_process_manager::instance()->init_process("bvxmUpdateLidarProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
    good = good && bprb_batch_process_manager::instance()->run_process();

    unsigned id_prob_map;
    unsigned id_mask;


    good = good && bprb_batch_process_manager::instance()->commit_output(0, id_prob_map);
    good = good && bprb_batch_process_manager::instance()->commit_output(1, id_mask);
    TEST("run init lidar process", good ,true);


    //retrieve prob_map 
    brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_prob_map);
    brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
    if (S_img->size()!=1){
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " no selections\n";
    }

    brdb_value_sptr value_img;
    if (!S_img->get_value(vcl_string("value"), value_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
        << " didn't get value\n";
    }
    bool non_null = (value_img != 0);
    TEST("display output non-null", non_null ,true);

    brdb_value_t<vil_image_view_base_sptr>* result =
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
    vil_image_view_base_sptr prob_map = result->value();


    bool saved = vil_save(*prob_map, "voxel_image.tif");
    TEST("image saved", saved ,true);


  }
  
  
     /***************************** Save Raw data**************************************/

  {
    // 3. Run save raw process
    vcl_string updated_world("updated_world.raw");

    // set the inputs 
    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);
    brdb_value_sptr v1 = new brdb_value_t<vcl_string> (updated_world);
    

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmSaveOccupancyRaw");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
    good = good && bprb_batch_process_manager::instance()->run_process();

   
    TEST("run save occupancy empty wolrd process", good ,true);

  
  }

#if 0
  // 2. Set inputs for bvxm_lidar_init_process
  //    - world
  // 		- 1st return path
  //    - 2nd return path

  vil_image_view<unsigned char> input_img(5,5,1);
  input_img.fill(255);

  vil_image_view<float> prob_img(5,5,1);

  prob_img.fill(0.3f);
  prob_img(2,2)=0.6f;
  prob_img(3,2)=0.6f;
  prob_img(2,3)=0.6f;
  prob_img(3,3)=0.6f;

  // set the inputs 
  brdb_value_sptr v0 = new brdb_value_t<vcl_string> ("1st return path");
  brdb_value_sptr v1 = new brdb_value_t<vcl_string> ("2st return path");
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(new voxel_world_sptr(wolrd));

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmLidarInitProcess");
  good = bprb_batch_process_manager::instance()->set_params("change_display_params.xml");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_cam;
  unsigned id_1ret;
  unsigned id_2ret;
  unsigned id_mask;

  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_1ret);
  good = good && bprb_batch_process_manager::instance()->commit_output(2, id_2ret);
  good = good && bprb_batch_process_manager::instance()->commit_output(3, id_mask);
  TEST("run init lidar process", good ,true);

  //retreive camera 
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  if (S_img->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
      << " no selections\n";
  }

  //retreive 1st return
  brdb_value_sptr value_img;
  if (!S_img->get_value(vcl_string("value"), value_img)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
      << " didn't get value\n";
  }
  bool non_null = (value_img != 0);
  TEST("display output non-null", non_null ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr nitf_roi = result->value();

#endif 


  SUMMARY();
}
