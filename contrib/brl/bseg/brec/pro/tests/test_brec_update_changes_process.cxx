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
#include <bvxm/pro/bvxm_processes.h>
#include <bvxm/pro/bvxm_register.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_parameters_sptr.h>
#include <bprb/bprb_parameters.h>

#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <brec/brec_bg_pair_density.h>
#include <brec/pro/brec_processes.h>
#include <brec/pro/brec_register.h>

#include <vil/vil_image_view.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
  vnl_double_3x4 cam_pers_matrix = cam_pers.get_matrix();
  std::vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0, x_off = 0.0,
         y_scale = 1.0, y_off = 0.0,
         z_scale = 1.0, z_off = 0.0,
         u_scale = 1.0, u_off = 0.0,
         v_scale = 1.0, v_off = 0.0;

  for (int i=0; i<20; i++) {
    neu_u.push_back(0.0);
    neu_v.push_back(0.0);
    den_u.push_back(0.0);
    den_v.push_back(0.0);
  }

  int vector_map[] = {9,15,18,19};

  for (int i=0; i<4; i++) {
    neu_u[vector_map[i]] = cam_pers_matrix(0,i);
    neu_v[vector_map[i]] = cam_pers_matrix(1,i);
    den_u[vector_map[i]] = cam_pers_matrix(2,i);
    den_v[vector_map[i]] = cam_pers_matrix(2,i);
  }

  vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
                                       x_scale,x_off,y_scale,y_off,z_scale,z_off,
                                       u_scale,u_off,v_scale,v_off);
  return cam_rat;
}


//:
vpgl_camera_double_sptr create_syn_world_camera(const bvxm_voxel_world_sptr& vox_world)
{
  vgl_point_3d<double> min_pt(vox_world->get_params()->corner().x(),
                              vox_world->get_params()->corner().y(),
                              vox_world->get_params()->corner().z());
  vgl_point_3d<double> max_pt(vox_world->get_params()->corner().x() + vox_world->get_params()->num_voxels().x(),
                              vox_world->get_params()->corner().y() + vox_world->get_params()->num_voxels().y(),
                              vox_world->get_params()->corner().z() + vox_world->get_params()->num_voxels().z());
  vgl_box_3d<double> world(min_pt, max_pt);

  constexpr int IMAGE_U = 200;
  constexpr int IMAGE_V = 200;
  constexpr double x_scale = 900;
  constexpr double y_scale = 900;
  constexpr double focal_length = 1.;
  constexpr double camera_dist = 200;

  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  std::cout << "centroid: " << centroid << std::endl;
  double x,y;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  alpha += 5*delta_alpha;

  x = camera_dist*std::cos(alpha);
  y = camera_dist*std::sin(alpha);

  vgl_point_3d<double> camera_center(x+centroid.x(), y+centroid.y(), 450+centroid.z());

  vpgl_perspective_camera<double> persp_cam;

  //generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
  vpgl_calibration_matrix<double> K(focal_length, principal_point, x_scale, y_scale);
  persp_cam.set_calibration(K);
  persp_cam.set_camera_center(camera_center);

  persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  vpgl_camera_double_sptr rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));

  std::vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
  vgl_box_2d<double> bb;
  for (auto c : corners) {
    double u,v, u2, v2;
    persp_cam.project(c.x(), c.y() ,c.z(), u, v);
    rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
    bb.add(vgl_point_2d<double> (u,v));
    std::cout << "Perspective [" << u << ',' << v << "]\n"
             << "Rational [" << u2 << ',' << v2 << "]\n" << std::endl;
  }
  std::cout << bb << std::endl;

  return rat_cam;
}

static void test_brec_update_changes_process()
{
  unsigned ni = 200, nj = 200;

  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  //DECLARE_FUNC_CONS(bvxm_gen_synthetic_world_process);
  //DECLARE_FUNC_CONS(brec_update_changes_process);
  //DECLARE_FUNC_CONS(bvxm_detect_changes_process);

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_gen_synthetic_world_process, "bvxmGenSyntheticWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_update_changes_process, "brecUpdateChangesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_detect_changes_process, "bvxmDetectChangesProcess");

  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenSyntheticWorldProcess");
  std::string world_dir("test_syn_world");

  // create an empty directory, or empty the directory if it exists
  std::string delete_str = world_dir+"/*.vox";
  if (vul_file::is_directory(world_dir))
    vul_file::delete_file_glob(delete_str.c_str());
  else {
    if (vul_file::exists(world_dir))
      vul_file::delete_file_glob(world_dir.c_str());
    vul_file::make_directory(world_dir);
  }

  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_world;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_world);
  TEST("run bvxmGenSyntheticWorldProcess", good ,true);

  brdb_query_aptr Q_w = brdb_query_comp_new("id", brdb_query::EQ, id_world);
  brdb_selection_sptr S_w = DATABASE->select("bvxm_voxel_world_sptr_data", std::move(Q_w));
  TEST("output world is in db", S_w->size(), 1);

  brdb_value_sptr value_w;
  TEST("output world is in db", S_w->get_value(std::string("value"), value_w), true);
  TEST("output world is non-null", (value_w != nullptr) ,true);

  auto* result_w = static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value_w.ptr());
  bvxm_voxel_world_sptr vox_world = result_w->value();
  vox_world->increment_observations<APM_MOG_GREY>(0);

  //vpgl_camera_double_sptr cam1 = create_camera();
  vpgl_camera_double_sptr cam1 = create_syn_world_camera(vox_world);

#if 0
  // test bvxm_bg_pair_density.h
  bvxm_bg_pair_density bgd(vox_world, cam1, "apm_mog_grey", 0, 0, ni, nj);
  bgd.verbose = true;
  TEST("testing bacground model generation", bgd.generate_mixture_image(), true);
  bgd.set_image_coords(0,0);
  std::cout << "prob returned: " << bgd(0.1, 0.1) << std::endl;
#endif // 0

  // first run the detect changes process to get the change map
  vil_image_view<vxl_byte> input_img(ni, nj, 1);
  input_img.fill(100);
  vil_image_view_base_sptr input_img_sptr = new vil_image_view<vxl_byte>(input_img);

  // set the inputs
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(input_img_sptr);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  brdb_value_sptr v3 = new brdb_value_t<std::string>("apm_mog_grey");
  brdb_value_sptr v4 = new brdb_value_t<unsigned>(0);
  brdb_value_sptr v5 = new brdb_value_t<unsigned>(0);

  // inits
  good = bprb_batch_process_manager::instance()->init_process("bvxmDetectChangesProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v4);
  good = good && bprb_batch_process_manager::instance()->set_input(5, v5);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img1;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img1);
  TEST("run bvxm detect instance process", good ,true);
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img1);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));
  TEST("output image is in db", S_img->size(), 1);
  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(std::string("value"), value_img), true);
  TEST("output image is non-null", (value_img != nullptr) ,true);
  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr out_change_map = result->value();

#if 0
  vil_image_view<vxl_byte> exp_img_overlayed_v(exp_img_overlayed);
  bool saved = vil_save(exp_img_overlayed_v, "expected_output.png");
  TEST("saved", saved, true);
#endif // 0

  // run the update changes process
  // inits
  brdb_value_sptr v6 = new brdb_value_t<vil_image_view_base_sptr>(out_change_map);
  brdb_value_sptr v7 = new brdb_value_t<float>(0.2f);
  brdb_value_sptr v8 = new brdb_value_t<unsigned>(1U);
  good = bprb_batch_process_manager::instance()->init_process("brecUpdateChangesProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v6);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v8);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v7);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned id_img2;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img2);
  TEST("run brec update changes process", good ,true);
}

TESTMAIN(test_brec_update_changes_process);
