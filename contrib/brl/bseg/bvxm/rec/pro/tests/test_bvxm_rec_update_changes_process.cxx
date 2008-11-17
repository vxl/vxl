//:
// \file
// \brief  Tests for change map update process
// \author Ozge C. Ozcanli
// \date   10/03/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_rec_update_changes_process.h"
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

#include <rec/bvxm_bg_pair_density.h>

//: create a synthetic slab to fill layers with
void create_a_synthetic_slab(bvxm_voxel_slab<float>& plane_img, unsigned nx, unsigned ny)
{
  for (unsigned i=0; i<nx; ++i)
  {
    for (unsigned j=0; j<ny; ++j) {
      // mark the origin/x axis
      if ( (i < 20) && (j < 5) ) {
        plane_img(i,j) = 0.2f;
      }
      // just make some squares of constant color
      else if ( (i > 50) && (i < 90) && (j > 50) && (j < 90) ) {
        plane_img(i,j) = 0.7f;
      }
      else if ((i > 150) && (i < 190) && (j > 50) && (j < 90) ) {
        plane_img(i,j) = 0.5f;
      }
      else if ((i > 50) && (i < 90) && (j > 150) && (j < 190) ) {
        plane_img(i,j) = 0.3f;
      }
      else if ((i > 150) && (i < 190) && (j > 150) && (j < 190) ) {
        plane_img(i,j) = 0.1f;
      }
      else {
        plane_img(i,j) = 0.4f;
      }
      //plane_img(i,j) = plane_img(i,j) + ((0.2f*i)/(float)nx + (0.2f*j)/(float)ny);
    }
  }
}

//:
vpgl_camera_double_sptr create_camera()
{
// now create a couple of cameras and generate the expected images
  vnl_double_3x3 K(0.0);
  double f = 550.0;
  double offx = 320.0;
  double offy = 240.0;
  K(0,0) = f; K(1,1) = f;
  K(0,2) = offx; K(1,2) = offy;
  K(2,2) = 1.0;
  //vgl_point_3d<double> center1(100,100,150);
  vgl_point_3d<double> center1(100,-100,275);
  //vgl_rotation_3d<double> rot1(0.0,0.0,0.0);
  vgl_rotation_3d<double> rot1(5*vnl_math::pi/6,0.0,0.0);

  vpgl_camera_double_sptr cam1 = new vpgl_perspective_camera<double>(K,center1,rot1);
  return cam1;
}

//:
vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
  vcl_vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0,
         x_off = 0.0,
         y_scale = 1.0,
         y_off = 0.0,
         z_scale = 1.0,
         z_off = 0.0,
         u_scale = 1.0,
         u_off = 0.0,
         v_scale = 1.0,
         v_off = 0.0;

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
vpgl_camera_double_sptr create_syn_world_camera(bvxm_voxel_world_sptr vox_world)
{
  vgl_point_3d<double> min_pt(vox_world->get_params()->corner().x(),
                              vox_world->get_params()->corner().y(),
                              vox_world->get_params()->corner().z());
  vgl_point_3d<double> max_pt(vox_world->get_params()->corner().x() + vox_world->get_params()->num_voxels().x(),
                              vox_world->get_params()->corner().y() + vox_world->get_params()->num_voxels().y(),
                              vox_world->get_params()->corner().z() + vox_world->get_params()->num_voxels().z());
  vgl_box_3d<double> world(min_pt, max_pt);

  const int IMAGE_U = 200;
  const int IMAGE_V = 200;
  const double x_scale = 900;
  const double y_scale = 900;
  const double focal_length = 1.;
  const double camera_dist= 200;

  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  vcl_cout << "centroid: " << centroid << vcl_endl;
  double x,y;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  alpha += 5*delta_alpha;

  x = camera_dist*vcl_cos(alpha);
  y = camera_dist*vcl_sin(alpha);

  vgl_point_3d<double> camera_center(x+centroid.x(), y+centroid.y(), 450+centroid.z());

  vpgl_perspective_camera<double> persp_cam;

  //generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
  vpgl_calibration_matrix<double> K(focal_length, principal_point, x_scale, y_scale);
  persp_cam.set_calibration(K);
  persp_cam.set_camera_center(camera_center);

  persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  vpgl_camera_double_sptr rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));

  vcl_vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
  vgl_box_2d<double> bb;
  for (unsigned i=0; i<corners.size(); i++) {
    vgl_point_3d<double> c = corners[i];
    double u,v, u2, v2;
    persp_cam.project(c.x(), c.y() ,c.z(), u, v);
    rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
    bb.add(vgl_point_2d<double> (u,v));
    vcl_cout << "Perspective [" << u << ',' << v << "]\n"
             << "Rational [" << u2 << ',' << v2 << "]\n" << vcl_endl;
  }
  vcl_cout << bb << vcl_endl;

  return rat_cam;
}

//:
bvxm_voxel_slab_base_sptr create_mog_image_using_grey_processor(vcl_string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img)
{
  vul_file::make_directory(model_dir);

  unsigned nx = 200;
  unsigned ny = 200;
  unsigned nz = 4;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 1.0f;

  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0,0);
  bvxm_voxel_grid<float> *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;

  bvxm_voxel_slab<float> plane_img(nx,ny,1);
  create_a_synthetic_slab(plane_img, nx, ny);

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  // create a slab of constant weights for update
  bvxm_voxel_slab<float> ones(nx,ny,1);
  ones.fill(1.0f);

  // iterate through layers of apm grid and update each level with the same synthetic image
  // if you want different levels to look different youll have to create a different image for each level
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0,0);
  bvxm_voxel_grid<mog_type> *apm_grid = dynamic_cast<bvxm_voxel_grid<mog_type>*>(apm_base.ptr());
  // initialize the appearance model data to get rid of any previous data on disk
  apm_grid->initialize_data(bvxm_voxel_traits<APM_MOG_GREY>::initial_val());

  bvxm_voxel_grid<mog_type>::iterator apm_it = apm_grid->begin();
  for (; apm_it != apm_grid->end(); ++apm_it) {
    apm_processor.update(*apm_it, plane_img, ones);
  }

  vpgl_camera_double_sptr cam1 = create_camera();

  vox_world->increment_observations<APM_MOG_GREY>(0);

  expected_img = new vil_image_view<unsigned char>(640,480);
  vil_image_view<float> mask(640,480);
  bvxm_image_metadata meta1(expected_img,cam1);
  vox_world->expected_image<APM_MOG_GREY>(meta1,expected_img, mask);

  bvxm_image_metadata meta2(expected_img,cam1);  // image is passed to get ni and nj basically

  bvxm_voxel_slab_base_sptr mog_image;
  vox_world->mixture_of_gaussians_image<APM_MOG_GREY>(meta2, mog_image, 0);
  return mog_image;
}

//: create a synthetic slab to fill layers with
void create_a_synthetic_slab2(bvxm_voxel_slab<float>& plane_img, unsigned nx, unsigned ny)
{
  for (unsigned i=0; i<nx; ++i)
  {
    for (unsigned j=0; j<ny; ++j) {
        plane_img(i,j) = 0.7f;
    }
  }
}

//:
bvxm_voxel_slab_base_sptr create_mog_image2_using_grey_processor(vcl_string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img)
{
  vul_file::make_directory(model_dir);

  unsigned nx = 40;
  unsigned ny = 40;
  unsigned nz = 4;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 1.0f;

  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0,0);
  bvxm_voxel_grid<float> *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;

  bvxm_voxel_slab<float> plane_img(nx,ny,1);
  create_a_synthetic_slab2(plane_img, nx, ny);

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  // create a slab of constant weights for update
  bvxm_voxel_slab<float> ones(nx,ny,1);
  ones.fill(1.0f);

  // iterate through layers of apm grid and update each level with the same synthetic image
  // if you want different levels to look different youll have to create a different image for each level
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0,0);
  bvxm_voxel_grid<mog_type> *apm_grid = dynamic_cast<bvxm_voxel_grid<mog_type>*>(apm_base.ptr());
  // initialize the appearance model data to get rid of any previous data on disk
  apm_grid->initialize_data(bvxm_voxel_traits<APM_MOG_GREY>::initial_val());

  bvxm_voxel_grid<mog_type>::iterator apm_it = apm_grid->begin();
  for (; apm_it != apm_grid->end(); ++apm_it) {
    apm_processor.update(*apm_it, plane_img, ones);
  }

  vpgl_camera_double_sptr cam1 = create_camera();

  vox_world->increment_observations<APM_MOG_GREY>(0);

  expected_img = new vil_image_view<unsigned char>(640,480);
  vil_image_view<float> mask(640,480);
  bvxm_image_metadata meta1(expected_img,cam1);
  vox_world->expected_image<APM_MOG_GREY>(meta1,expected_img, mask);

  bvxm_image_metadata meta2(expected_img,cam1);  // image is passed to get ni and nj basically

  bvxm_voxel_slab_base_sptr mog_image;
  vox_world->mixture_of_gaussians_image<APM_MOG_GREY>(meta2, mog_image, 0);
  return mog_image;
}

MAIN( test_bvxm_rec_update_changes_process )
{
  unsigned ni = 200;
  unsigned nj = 200;

  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  // call bvxmGenSyntheticWorldProcess process to generate a synthetic world with two boxes
  REG_PROCESS(bvxm_gen_synthetic_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_rec_update_changes_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_detect_changes_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenSyntheticWorldProcess");
  bprb_parameters_sptr params = new bprb_parameters();
  params->add("size world x", "nx", (unsigned)100);
  params->add("size world y", "ny", (unsigned)100);
  params->add("size world z", "nz", (unsigned)50);
  params->add("box min x", "minx", (unsigned)10);
  params->add("box min y", "miny", (unsigned)10);
  params->add("box min z", "minz", (unsigned)10);
  params->add("box dim x", "dimx", (unsigned)40);
  params->add("box dim y", "dimy", (unsigned)40);
  params->add("box dim z", "dimz", (unsigned)20);
  params->add("generate 2 boxes", "gen2", true);
  params->add("generate images", "genImages", true);
  params->add("random texture on box1", "rand1", true);
  params->add("random texture on box2", "rand2", false);
  params->add("fixed appearance val", "appval", 0.7f);

  params->add("world_dir", "worlddir", vcl_string("./test_syn_world"));
  vcl_string command = "rm -rf ./test_syn_world";
  system(command.c_str());
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

#if 0
  // test bvxm_bg_pair_density.h
  bvxm_bg_pair_density bgd(vox_world, cam1, "apm_mog_grey", 0, 0, ni, nj);
  bgd.verbose = true;
  TEST("testing bacground model generation", bgd.generate_mixture_image(), true);
  bgd.set_image_coords(0,0);
  vcl_cout << "prob returned: " << bgd(0.1, 0.1) << vcl_endl;
#endif // 0

  // first run the detect changes process to get the change map
  vil_image_view<vxl_byte> input_img(ni, nj, 1);
  input_img.fill(100);
  vil_image_view_base_sptr input_img_sptr = new vil_image_view<vxl_byte>(input_img);

  // set the inputs
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(input_img_sptr);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  brdb_value_sptr v3 = new brdb_value_t<vcl_string>("apm_mog_grey");
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
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  TEST("output image is in db", S_img->size(), 1);
  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(vcl_string("value"), value_img), true);
  TEST("output image is non-null", (value_img != 0) ,true);
  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr out_change_map = result->value();

#if 0
  vil_image_view<vxl_byte> exp_img_overlayed_v(exp_img_overlayed);
  bool saved = vil_save(exp_img_overlayed_v, "./expected_output.png");
  TEST("saved", saved, true);
#endif // 0

  // run the update changes process
  // inits
  brdb_value_sptr v6 = new brdb_value_t<vil_image_view_base_sptr>(out_change_map);
  brdb_value_sptr v7 = new brdb_value_t<float>(0.2f);
  brdb_value_sptr v8 = new brdb_value_t<unsigned>(1);
  good = bprb_batch_process_manager::instance()->init_process("bvxmUpdateChangesProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v6);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v8);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v7);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned id_img2;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img2);
  TEST("run bvxm update changes process", good ,true);

  SUMMARY();
}

#if 0
  bprb_parameters_sptr det_params = new bprb_parameters();
  det_params->add("ni for output image", "ni", (int)ni);
  det_params->add("nj for output image", "nj", (int)nj);
  det_params->add("x interval", "x_int", (unsigned)26);
  det_params->add("y interval", "y_int", (unsigned)26);
  det_params->add("z interval", "z_int", (unsigned)30);
  det_params->add("angle intervals for rotational search (in degrees)", "angle_int", 20.0f);
  det_params->add("verbose", "verbose", true);

  good = good && bprb_batch_process_manager::instance()->set_params(det_params);
#endif // 0

