//:
// \file
// \brief  Tests for change map update process
// \author Ozge C. Ozcanli
// \date   Oct 03, 2008
//
#include <testlib/testlib_test.h>
#include "../brec_update_changes_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/pro/bvxm_gen_synthetic_world_process.h>
#include <bvxm/pro/bvxm_render_expected_image_process.h>
#include <bvxm/pro/bvxm_detect_changes_process.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_parameters_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_vector_3d.h>

#include <brip/brip_vil_float_ops.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_box_2d.h>

#include <brec/brec_bg_pair_density.h>
#include <brec/pro/brec_create_mog_image_process.h>
#include <bbgm/pro/bbgm_save_image_of_process.h>
#include <bbgm/pro/bbgm_display_dist_image_process.h>

#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>

//: the following methods are defined in test_brec_update_changes_process.cxx
void create_a_synthetic_slab(bvxm_voxel_slab<float>& plane_img, unsigned nx, unsigned ny);
vpgl_camera_double_sptr create_camera();
vpgl_rational_camera<double> perspective_to_rational(vpgl_perspective_camera<double>& cam_pers);
vpgl_camera_double_sptr create_syn_world_camera(bvxm_voxel_world_sptr vox_world);
bvxm_voxel_slab_base_sptr create_mog_image_using_grey_processor(vcl_string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img);
bvxm_voxel_slab_base_sptr create_mog_image2_using_grey_processor(vcl_string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img);


MAIN( test_brec_create_mog_image_process )
{
  unsigned ni = 200;
  unsigned nj = 200;

  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  // call bvxmGenSyntheticWorldProcess process to generate a synthetic world with two boxes
  REG_PROCESS(bvxm_gen_synthetic_world_process, bprb_batch_process_manager);
  REG_PROCESS(brec_create_mog_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_save_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_display_dist_image_process, bprb_batch_process_manager);

  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE( bbgm_image_sptr );

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenSyntheticWorldProcess");
  bprb_parameters_sptr params = new bprb_parameters();
  params->add("size world x", "nx", 100U);
  params->add("size world y", "ny", 100U);
  params->add("size world z", "nz", 50U);
  params->add("box min x", "minx", 10U);
  params->add("box min y", "miny", 10U);
  params->add("box min z", "minz", 10U);
  params->add("box dim x", "dimx", 40U);
  params->add("box dim y", "dimy", 40U);
  params->add("box dim z", "dimz", 20U);
  params->add("generate 2 boxes", "gen2", true);
  params->add("generate images", "genImages", true);
  params->add("random texture on box1", "rand1", true);
  params->add("random texture on box2", "rand2", false);
  params->add("fixed appearance val", "appval", 0.7f);
  vcl_string world_dir("test_syn_world");
  params->add("world_dir", "worlddir", world_dir);

  // create an empty directory, or empty the directory if it exists
  if (vul_file::is_directory(world_dir))
    vul_file::delete_file_glob(world_dir+"/*");
  else {
    if (vul_file::exists(world_dir))
      vul_file::delete_file_glob(world_dir);
    vul_file::make_directory(world_dir);
  }

  good = good && bprb_batch_process_manager::instance()->set_params(params);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_world;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_world);
  TEST("run bvxmGenSyntheticWorldProcess", good ,true);

  brdb_query_aptr Q_w = brdb_query_comp_new("id", brdb_query::EQ, id_world);
  brdb_selection_sptr S_w = DATABASE->select("bvxm_voxel_world_sptr_data", Q_w);
  TEST("output world is in db", S_w->size(), 1);

  brdb_value_sptr value_w;
  TEST("output world is in db", S_w->get_value(vcl_string("value"), value_w), true);
  TEST("output world is non-null", (value_w != 0) ,true);

  brdb_value_t<bvxm_voxel_world_sptr>* result_w = static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_w.ptr());
  bvxm_voxel_world_sptr vox_world = result_w->value();
  vox_world->increment_observations<APM_MOG_GREY>(0);

  //vpgl_camera_double_sptr cam1 = create_camera();
  vpgl_camera_double_sptr cam1 = create_syn_world_camera(vox_world);

  // first run the detect changes process to get the change map
  vil_image_view<vxl_byte> input_img(ni, nj, 1);
  input_img.fill(100);
  vil_image_view_base_sptr input_img_sptr = new vil_image_view<vxl_byte>(input_img);

  // set the inputs
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(input_img_sptr);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  brdb_value_sptr v3 = new brdb_value_t<vcl_string>("apm_mog_grey");
  brdb_value_sptr v4 = new brdb_value_t<unsigned>(0U); // bin id
  brdb_value_sptr v5 = new brdb_value_t<unsigned>(0U); // scale id

  // run the create mog image process
  // inits
  good = bprb_batch_process_manager::instance()->init_process("brecCreateMOGImageProcess");
  bprb_parameters_sptr params2 = new bprb_parameters();
  params2->add("method to use to create a mog image", "mog_method", (unsigned)bvxm_mog_image_creation_methods::SAMPLING);   // otherwise uses expected values of the mixtures at voxels along the path of the rays
  params2->add("number of samples if using random sampling as mog_method", "n_samples", 10U);
  params2->add("ni for output mog", "ni", ni);  // should be set according to each test image
  params2->add("nj for output mog", "nj", nj);
  params2->add("nplanes for output expected view of output mog", "nplanes", 1U);
  params2->add("verbose", "verbose", false);
  good = good && bprb_batch_process_manager::instance()->set_params(params2);
  good = good && bprb_batch_process_manager::instance()->set_input(0, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v4);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v5);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img1;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img1);
  TEST("run create mog image process", good ,true);
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img1);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  TEST("output image is in db", S_img->size(), 1);
  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(vcl_string("value"), value_img), true);
  TEST("output image is non-null", (value_img != 0) ,true);
  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr out_exp_img = result->value();

#if 1
  vil_image_view<vxl_byte> exp_img_v(out_exp_img);
  bool saved = vil_save(exp_img_v, "expected_image_of_mog.png");
  TEST("saved", saved, true);
#endif // 0

  unsigned id_mog;
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_mog);
  TEST("run create mog image process", good ,true);
  brdb_query_aptr Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mog);
  brdb_selection_sptr S_img1 = DATABASE->select("bbgm_image_sptr_data", Q_img1);
  TEST("output image is in db", S_img1->size(), 1);
  brdb_value_sptr value_img1;
  TEST("output image is in db", S_img1->get_value(vcl_string("value"), value_img1), true);
  TEST("output image is non-null", (value_img1 != 0) ,true);
  brdb_value_t<bbgm_image_sptr>* result1 = static_cast<brdb_value_t<bbgm_image_sptr>* >(value_img1.ptr());
  bbgm_image_sptr out_mog_img = result1->value();
  TEST("run create mog image process - mog image ptr", !out_mog_img , false);

  //: save the output mog image
  brdb_value_sptr v6 = new brdb_value_t<vcl_string>("out_mog.bin");
  brdb_value_sptr v7 = new brdb_value_t<bbgm_image_sptr>(out_mog_img);
  good = bprb_batch_process_manager::instance()->init_process("SaveImageOfProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v6);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v7);
  good = good && bprb_batch_process_manager::instance()->run_process();
  TEST("save mog image process", good , true);

  for (int component = 0; component < 3; component++) {
    vcl_stringstream ss; ss << component;
    //: display the output mog image
    brdb_value_sptr v8 = new brdb_value_t<vcl_string>("mean");
    brdb_value_sptr v9 = new brdb_value_t<int>(component);  // the component to display
    good = bprb_batch_process_manager::instance()->init_process("DisplayDistImageProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v7);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v8);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v9);
    good = good && bprb_batch_process_manager::instance()->run_process();
    TEST("display mog image process", good , true);

    unsigned id_mean; bprb_batch_process_manager::instance()->commit_output(0, id_mean);
    Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mean);
    S_img1 = DATABASE->select("vil_image_view_base_sptr_data", Q_img1);
    S_img1->get_value(vcl_string("value"), value_img1);
    result = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img1.ptr());

  #if 1
    vil_image_view<vxl_byte> mean_img_v(result->value());
    vcl_string name = "mean_image_of_mog_component_" + ss.str() + ".png";
    saved = vil_save(mean_img_v, name.c_str());
    TEST("saved", saved, true);
  #endif // 0

    //: display the output mog image's std deviation
    brdb_value_sptr v10 = new brdb_value_t<vcl_string>("variance");
    good = bprb_batch_process_manager::instance()->init_process("DisplayDistImageProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v7);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v10);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v9);
    good = good && bprb_batch_process_manager::instance()->run_process();
    TEST("display mog image process", good , true);

    bprb_batch_process_manager::instance()->commit_output(0, id_mean);
    Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mean);
    S_img1 = DATABASE->select("vil_image_view_base_sptr_data", Q_img1);
    S_img1->get_value(vcl_string("value"), value_img1);
    result = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img1.ptr());

  #if 1
    vil_image_view<vxl_byte> var_img_v(result->value());
    name = "variance_image_of_mog_component_" + ss.str() + ".png";
    saved = vil_save(var_img_v, name.c_str());
    TEST("saved", saved, true);
  #endif // 0
  }

  SUMMARY();
}
