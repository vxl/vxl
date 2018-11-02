//:
// \file
// \brief  Tests for change map update process
// \author Ozge C. Ozcanli
// \date   Oct 03, 2008
//
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
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_parameters_sptr.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view.h>

#include <brip/brip_vil_float_ops.h>

#include <brec/brec_bg_pair_density.h>
#include <brec/pro/brec_processes.h>
#include <brec/pro/brec_register.h>

#include <bsta/bsta_basic_functors.h>

#ifdef EXTRA_TESTS
#include <bbgm/pro/bbgm_save_image_of_process.h>
#include <bbgm/pro/bbgm_display_dist_image_process.h>
#endif
#include <bbgm/pro/bbgm_processes.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_viewer.h>
#include <bbgm/bbgm_viewer_sptr.h>

static void test_brec_create_mog_image_process()
{
  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  // the appearance model processor
  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  vil_image_view<float> img(100, 100);
  img.fill(0.8f);
  vil_image_view_base_sptr img_sptr = new vil_image_view<float>(img);

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(img.ni(), img.nj(), 1);
  if (!bvxm_util::img_to_slab(img_sptr,image_slab)) {
    std::cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type: APM_MOG_GREY\n";
    TEST("converting image to voxel slab of observation type", true, false);
    return;
  }
  // create distribution slab
  float init_variance = 0.008f;
  bsta_gauss_sf1 this_gauss(0.0f, init_variance);
  bsta_num_obs<bsta_gauss_sf1> tg_o(this_gauss);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3> mix_gauss;
  mix_gauss.insert(tg_o, 0.3f);
  mix_gauss.insert(tg_o, 0.3f);
  mix_gauss.insert(tg_o, 0.4f);
  bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3> > tg_mg(mix_gauss);
  typedef bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3> > mix_type;

  bvxm_voxel_slab<mog_type> slab(100,100,1);
  slab.fill(tg_mg);

  bvxm_voxel_slab<float> w(100,100,1);
  w.fill(1.0f);

  // update once with the img
  apm_processor.update(slab, image_slab, w);

  //DECLARE_FUNC_CONS(brec_create_mog_image_process);

  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE(bbgm_image_sptr);
  REGISTER_DATATYPE(bvxm_voxel_slab_base_sptr);

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_create_mog_image_process, "brecCreateMOGImageProcess");
#ifdef EXTRA_TESTS
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_save_image_of_process, "bbgmSaveImageOfProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_display_dist_image_process, "bbgmDisplayDistImageProcess");
#endif

  // run the create mog image process
  // inits
  bvxm_voxel_slab_base_sptr s_ptr = new bvxm_voxel_slab<mog_type>(slab);
  brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_slab_base_sptr>(s_ptr);
  brdb_value_sptr v1 = new brdb_value_t<std::string>("apm_mog_grey");

  bool good = bprb_batch_process_manager::instance()->init_process("brecCreateMOGImageProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img1;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img1);
  TEST("run create mog image process", good ,true);
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img1);
  brdb_selection_sptr S_img = DATABASE->select("bbgm_image_sptr_data", std::move(Q_img));
  TEST("output image is in db", S_img->size(), 1);
  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(std::string("value"), value_img), true);
  TEST("output image is non-null", (value_img != nullptr) ,true);
  auto* result = static_cast<brdb_value_t<bbgm_image_sptr>* >(value_img.ptr());
  bbgm_image_sptr out_exp_img = result->value();
  TEST("output image is a valid bbgm image", !out_exp_img, false);

  bbgm_viewer_sptr viewer = new bbgm_mean_viewer();

  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<mix_type, bsta_mean_functor<mix_type> >);

  TEST("output image is a valid bbgm image", viewer->probe(out_exp_img), true);
  viewer->set_active_component(0);
  vil_image_view<double> d_image;
  TEST("mean image of bbgm image", viewer->apply(out_exp_img, d_image), true);

  vil_image_view<vxl_byte> byte_image;
  double dmin, dmax;
  vil_math_value_range(d_image, dmin, dmax);
  vil_convert_stretch_range_limited(d_image, byte_image, dmin, dmax);
  vil_save(byte_image, "mean.png");

#if 0
  unsigned id_mog;
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_mog);
  TEST("run create mog image process", good ,true);
  brdb_query_aptr Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mog);
  brdb_selection_sptr S_img1 = DATABASE->select("bbgm_image_sptr_data", Q_img1);
  TEST("output image is in db", S_img1->size(), 1);
  brdb_value_sptr value_img1;
  TEST("output image is in db", S_img1->get_value(std::string("value"), value_img1), true);
  TEST("output image is non-null", (value_img1 != 0) ,true);
  brdb_value_t<bbgm_image_sptr>* result1 = static_cast<brdb_value_t<bbgm_image_sptr>* >(value_img1.ptr());
  bbgm_image_sptr out_mog_img = result1->value();
  TEST("run create mog image process - mog image ptr", !out_mog_img , false);

  // save the output mog image
  brdb_value_sptr v6 = new brdb_value_t<std::string>("out_mog.bin");
  brdb_value_sptr v7 = new brdb_value_t<bbgm_image_sptr>(out_mog_img);
  good = bprb_batch_process_manager::instance()->init_process("bbgmSaveImageOfProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v6);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v7);
  good = good && bprb_batch_process_manager::instance()->run_process();
  TEST("save mog image process", good , true);

  for (int component = 0; component < 3; component++) {
    std::stringstream ss; ss << component;
    // display the output mog image
    brdb_value_sptr v8 = new brdb_value_t<std::string>("mean");
    brdb_value_sptr v9 = new brdb_value_t<int>(component);  // the component to display
    brdb_value_sptr v11 = new brdb_value_t<bool>(true);  // scale result to a byte image
    good = bprb_batch_process_manager::instance()->init_process("bbgmDisplayDistImageProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v7);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v8);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v9);
    good = good && bprb_batch_process_manager::instance()->set_input(3, v11);
    good = good && bprb_batch_process_manager::instance()->run_process();
    TEST("display mog image process", good , true);

    unsigned id_mean; bprb_batch_process_manager::instance()->commit_output(0, id_mean);
    Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mean);
    S_img1 = DATABASE->select("vil_image_view_base_sptr_data", Q_img1);
    S_img1->get_value(std::string("value"), value_img1);
    result = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img1.ptr());

  #if 1
    vil_image_view<vxl_byte> mean_img_v(result->value());
    std::string name = "mean_image_of_mog_component_" + ss.str() + ".png";
    saved = vil_save(mean_img_v, name.c_str());
    TEST("saved", saved, true);
  #endif // 0

    // display the output mog image's std deviation
    brdb_value_sptr v10 = new brdb_value_t<std::string>("variance");

    good = bprb_batch_process_manager::instance()->init_process("bbgmDisplayDistImageProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v7);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v10);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v9);
    good = good && bprb_batch_process_manager::instance()->set_input(3, v11);
    good = good && bprb_batch_process_manager::instance()->run_process();
    TEST("display mog image process", good , true);

    bprb_batch_process_manager::instance()->commit_output(0, id_mean);
    Q_img1 = brdb_query_comp_new("id", brdb_query::EQ, id_mean);
    S_img1 = DATABASE->select("vil_image_view_base_sptr_data", Q_img1);
    S_img1->get_value(std::string("value"), value_img1);
    result = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img1.ptr());

  #if 1
    vil_image_view<vxl_byte> var_img_v(result->value());
    name = "variance_image_of_mog_component_" + ss.str() + ".png";
    saved = vil_save(var_img_v, name.c_str());
    TEST("saved", saved, true);
  #endif // 0
  }
#endif
}

TESTMAIN(test_brec_create_mog_image_process);
