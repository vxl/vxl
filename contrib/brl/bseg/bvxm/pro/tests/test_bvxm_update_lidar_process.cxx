#include <string>
#include <iostream>
#include <testlib/testlib_test.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view.h>

#include <vul/vul_file.h>
#include <bvxm/pro/processes/bvxm_create_synth_lidar_data_process.h>
#include <bvxm/pro/processes/bvxm_create_voxel_world_process.h>
#include <bvxm/pro/processes/bvxm_update_lidar_process.h>
#include <bvxm/pro/processes/bvxm_save_occupancy_raw_process.h>

static void test_bvxm_update_lidar_process()
{
  //DECLARE_FUNC_CONS(bvxm_update_lidar_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_lidar_process, "bvxmUpdateLidarProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_synth_lidar_data_process, "bvxmCreateSynthLidarDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_voxel_world_process, "bvxmCreateVoxelWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_save_occupancy_raw_process,"bvxmSaveOccupancyRaw");
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);

  //global variables
  vil_image_view_base_sptr lidar_img_;
  vil_image_view_base_sptr prob_map_;
  bvxm_voxel_world_sptr world_;
  vpgl_camera_double_sptr lidar_cam_;

  // 1. Set inputs for bvxm_create_synth_lidar_data_process
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateSynthLidarDataProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    std::string xml = "./synth_test_params.xml";
    good = bprb_batch_process_manager::instance()->set_params(xml);
    TEST("bprb_batch_process_manager::instance()->set_params()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run create synthetic lidar data process", good ,true);
    if (!good) break;

    unsigned id_lidar_img;
    unsigned id_cam;

    good = bprb_batch_process_manager::instance()->commit_output(0, id_lidar_img);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;
    good = bprb_batch_process_manager::instance()->commit_output(1, id_cam);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    //retrieve lidar image
    brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_lidar_img);
    brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));

    if (S_img->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_img;
    if (!S_img->get_value(std::string("value"), value_img)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    bool non_null = (value_img != nullptr);
    TEST("display output non-null", non_null ,true);

    auto* result =
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
    vil_image_view_base_sptr lidar_img = result->value();

    lidar_img_ = lidar_img;
#ifdef DEBUG
   vil_image_view<vxl_byte> lidar2 = static_cast<vil_image_view<vxl_byte>>(*lidar_img);
   vil_save(lidar2, "./lidar_img.png");
#endif // DEBUG
    bool saved = vil_save(*lidar_img, "lidar_image.tif");
    TEST("lidar image saved", saved ,true);

    //retrieve lidar camera
    brdb_query_aptr Q_cam = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
    brdb_selection_sptr S_cam = DATABASE->select("vpgl_camera_double_sptr_data", std::move(Q_cam));

    if (S_cam->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_cam;
    if (!S_cam->get_value(std::string("value"), value_cam)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }

    non_null = (value_cam != nullptr);
    TEST("display output non-null", non_null ,true);

    auto* result2 =
      static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(value_cam.ptr());
    vpgl_camera_double_sptr lidar_cam = result2->value();

    lidar_cam_ = lidar_cam;
  }

#if 0
  /***********************Generate Synthetic World*********************************/
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    // Set output for bvxm_gen_synthetic_world_process

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenSyntheticWorldProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    std::string xml = std::string(argv[1]) + "/" + "change_display_params.xml";
//  good = bprb_batch_process_manager::instance()->set_params(xml);
    TEST("bprb_batch_process_manager::instance()->set_params()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run gen synthetic world process", good ,true);
    if (!good) break;

    unsigned id_world;

    good = bprb_batch_process_manager::instance()->commit_output(0, id_world);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    //retrieve world
    brdb_query_aptr Q_world = brdb_query_comp_new("id", brdb_query::EQ, id_world);
    brdb_selection_sptr S_world = DATABASE->select("bvxm_voxel_world_sptr_data", Q_world);
    if (S_world->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_world;
    if (!S_world->get_value(std::string("value"), value_world)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    bool non_null = (value_world != 0);
    TEST("display output non-null", non_null ,true);

    brdb_value_t<bvxm_voxel_world_sptr>* result =
      static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_world.ptr());
    bvxm_voxel_world_sptr world = result->value();

    world_ = world;
  }
#endif // 0

  /***********************Generate Empty World*********************************/
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    // Set output for bvxm_gen_synthetic_world_process
    std::string test_dir("./create_world_test");
    vul_file::make_directory(test_dir);

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateVoxelWorldProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    std::string xml = "./world_model_params.xml";
    good = bprb_batch_process_manager::instance()->set_params(xml);
    TEST("bprb_batch_process_manager::instance()->set_params()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run gen synthetic world process", good, true);
    if (!good) break;

    unsigned id_world;

    good = bprb_batch_process_manager::instance()->commit_output(0, id_world);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    //retrieve world
    brdb_query_aptr Q_world = brdb_query_comp_new("id", brdb_query::EQ, id_world);
    brdb_selection_sptr S_world = DATABASE->select("bvxm_voxel_world_sptr_data", std::move(Q_world));
    if (S_world->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_world;
    if (!S_world->get_value(std::string("value"), value_world)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    bool non_null = (value_world != nullptr);
    TEST("display output non-null", non_null ,true);

    auto* result =
      static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_world.ptr());
    bvxm_voxel_world_sptr world = result->value();

    world_ = world;
  }

  /***************************** Save Raw data**************************************/
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    // 3. Run save raw process
     std::string empty_world("empty.raw");

    // set the inputs
    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);
    brdb_value_sptr v1 = new brdb_value_t<std::string> (empty_world);
    brdb_value_sptr v2 = new brdb_value_t<unsigned> (0);
    brdb_value_sptr v3 = new brdb_value_t<std::string> ("float");

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmSaveOccupancyRaw");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    good = bprb_batch_process_manager::instance()->set_input(0, v0)
        && bprb_batch_process_manager::instance()->set_input(1, v1)
        && bprb_batch_process_manager::instance()->set_input(2, v2)
        && bprb_batch_process_manager::instance()->set_input(3, v3);
    TEST("bprb_batch_process_manager::instance()->set_input()", good, true);

    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run save occupancy empty wolrd process", good, true);
    if (!good) break;
  }

  /***************************** Update World with lidar data**************************************/
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    // 3. Run update lidar process

    // set the inputs
    brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(lidar_img_);
    brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr> (lidar_cam_);
    brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);
    brdb_value_sptr v3 = new brdb_value_t<unsigned>(0);
    brdb_value_sptr v4 = new brdb_value_t<bool>(false);

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmUpdateLidarProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    good = bprb_batch_process_manager::instance()->set_input(0, v0)
        && bprb_batch_process_manager::instance()->set_input(1, v1)
        && bprb_batch_process_manager::instance()->set_input(2, v2)
        && bprb_batch_process_manager::instance()->set_input(3, v3)
        && bprb_batch_process_manager::instance()->set_input(4, v4);
    TEST("bprb_batch_process_manager::instance()->set_input()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run init lidar process", good, true);
    if (!good) break;

    unsigned id_prob_map;
    unsigned id_mask;

    good = bprb_batch_process_manager::instance()->commit_output(0, id_prob_map);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;
    good = bprb_batch_process_manager::instance()->commit_output(1, id_mask);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    //retrieve prob_map
    brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_prob_map);
    brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));
    if (S_img->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_img;
    if (!S_img->get_value(std::string("value"), value_img)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    bool non_null = (value_img != nullptr);
    TEST("display output non-null", non_null ,true);

    auto* result =
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
    vil_image_view_base_sptr prob_map = result->value();

    bool saved = vil_save(*prob_map, "voxel_image.tif");
    TEST("image saved", saved ,true);
  }

  /***************************** Save Raw data**************************************/
  for (int dummy = 0; dummy == 0; ++dummy)
  {
    // 3. Run save raw process
    std::string updated_world("updated_world.raw");

    // set the inputs
    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(world_);
    brdb_value_sptr v1 = new brdb_value_t<std::string> (updated_world);
    brdb_value_sptr v2 = new brdb_value_t<unsigned>(0);
    brdb_value_sptr v3 = new brdb_value_t<std::string>("float");

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmSaveOccupancyRaw");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    good = bprb_batch_process_manager::instance()->set_input(0, v0)
        && bprb_batch_process_manager::instance()->set_input(1, v1)
        && bprb_batch_process_manager::instance()->set_input(2, v2)
        && bprb_batch_process_manager::instance()->set_input(3, v3);
    TEST("bprb_batch_process_manager::instance()->set_input()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run save occupancy empty world process", good ,true);
    if (!good) break;
  }

#if 0
  // 2. Set inputs for bvxm_lidar_init_process
  //    - world
  //    - 1st return path
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
  brdb_value_sptr v0 = new brdb_value_t<std::string> ("1st return path");
  brdb_value_sptr v1 = new brdb_value_t<std::string> ("2st return path");
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(new voxel_world_sptr(wolrd));

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmLidarInitProcess");
  TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
  std::string xml = std::string(argv[1]) + "/" + "change_display_params.xml";
  good = bprb_batch_process_manager::instance()->set_params(xml);
    TEST("bprb_batch_process_manager::instance()->set_params()", good, true);
  good = bprb_batch_process_manager::instance()->set_input(0, v0)
      && bprb_batch_process_manager::instance()->set_input(1, v1)
      && bprb_batch_process_manager::instance()->set_input(2, v2);
  TEST("bprb_batch_process_manager::instance()->set_input()", good, true);
  good = bprb_batch_process_manager::instance()->run_process();
  TEST("run init lidar process", good, true);

  unsigned id_cam;
  unsigned id_1ret;
  unsigned id_2ret;
  unsigned id_mask;

  good = bprb_batch_process_manager::instance()->commit_output(0, id_cam)
      && bprb_batch_process_manager::instance()->commit_output(1, id_1ret)
      && bprb_batch_process_manager::instance()->commit_output(2, id_2ret)
      && bprb_batch_process_manager::instance()->commit_output(3, id_mask);
  TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);

  //retrieve camera
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  if (S_img->size()!=1) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  //retrieve 1st return
  brdb_value_sptr value_img;
  if (!S_img->get_value(std::string("value"), value_img)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value_img != 0);
  TEST("display output non-null", non_null ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr nitf_roi = result->value();

//#endif // 0
#else
  // do nothing ...
#endif // full function body commented out
}

TESTMAIN(test_bvxm_update_lidar_process);
