#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <boxm/util/boxm_test_util.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree_cell.h>
#include <boxm/algo/sp/boxm_update_triangle.h>
#include <boxm/algo/sp/boxm_render_image_triangle.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_compiler.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#ifdef CLEANUP
#include <vpl/vpl.h>
#endif

std::vector<vpgl_camera_double_sptr > generate_cameras_z_multi_bin(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  double alpha = (vnl_math::pi_over_180)*65; // was: (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/36.;
  std::vector<vgl_point_3d<double> > centers;
  for (unsigned i=0; i<11; i++) {
    double z = boxm_camera_dist*std::sin(alpha);
    centers.emplace_back(centroid.x(), centroid.y(), centroid.z()+z);
    std::cout << centers[i] << std::endl;
    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  std::vector<vpgl_camera_double_sptr> rat_cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vpgl_camera_double_sptr rat_cam = generate_camera_top(world);
    rat_cameras.push_back(rat_cam);

    std::vector<vgl_point_3d<double> > corners = boxm_utils::corners_of_box_3d(world);
    for (auto c : corners) {
      double u,v, u2, v2;
      vpgl_perspective_camera<double> persp_cam;
      persp_cam.project(c.x(), c.y() ,c.z(), u, v);
      rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
      bb.add(vgl_point_2d<double> (u,v));

      if (verbose)
      {
        std::cout << "Perspective [" << u << ',' << v << "]\n"
                 << "Rational [" << u2 << ',' << v2 << "]\n\n";
      }
    }
    std::cout << bb << std::endl;
  }
  return rat_cameras;
}

std::vector<vpgl_camera_double_sptr > generate_cameras_yz_multi_bin(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  std::cout << "centroid: " << centroid << std::endl;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  std::vector<vgl_point_3d<double> > centers;

  for (unsigned i=0; i<num_train_images; i++) {
    double x = boxm_camera_dist*std::cos(alpha);
    double y = boxm_camera_dist*std::sin(alpha);
    centers.emplace_back(x+centroid.x(), y+centroid.y(), 450+centroid.z());
  if (verbose)
    std::cout << centers[i] << std::endl;

    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  std::vector<vpgl_camera_double_sptr> persp_cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vgl_point_3d<double> camera_center  = centers[i];
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(boxm_focal_length,principal_point, boxm_x_scale, boxm_y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
    persp_cameras.push_back(new vpgl_perspective_camera<double>(persp_cam));

    //save the camera to file
    std::stringstream cam_file;
    vul_file::make_directory("./cams");
    cam_file << "./cams/cam_" << i << ".txt";
    std::ofstream cam_out(cam_file.str().c_str());
    cam_out << persp_cam;

    if (verbose) {
      std::vector<vgl_point_3d<double> > corners = boxm_utils::corners_of_box_3d(world);
      for (auto c : corners) {
        double u,v;
        persp_cam.project(c.x(), c.y() ,c.z(), u, v);
        bb.add(vgl_point_2d<double> (u,v));
        std::cout << "Perspective [" << u << ',' << v << ']' << std::endl << std::endl;
      }
      std::cout << bb << std::endl;
    }
  }
  return persp_cameras;
}


static void test_update_multi_bin()
{
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(2,2,1);
  boxm_scene<boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > > scene(lvcs, origin, block_dim, world_dim);
#ifdef SET_APPEARENCE_MODEL_EXISTS
  scene.set_appearence_model(BOXM_APM_MOG_GREY);
#endif
  scene.set_paths("boxm_scene2_mb", "block");
  vul_file::make_directory("boxm_scene2_mb");
  std::ofstream os("scene2.xml");
  x_write(os, scene, "scene");
  os.close();

  boxm_block_iterator<boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > > iter(&scene);
  // default model
  bsta_gauss_sf1 simple_gauss_sf1(0.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> simple_obs_gauss_val_sf1(simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  simple_mix_gauss_val_sf1;

  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);
  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);
  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_sf1_3>  simple_obs_mix_gauss_val_sf1(simple_mix_gauss_val_sf1);


  boxm_sample_multi_bin<BOXM_APM_MOG_GREY> default_sample;
  default_sample.alpha=0.001f;
  default_sample.set_appearance(simple_obs_mix_gauss_val_sf1,5);

  // sample 1
  bsta_gauss_sf1 s1_simple_gauss_sf1(0.5f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> s1_simple_obs_gauss_val_sf1(s1_simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  s1_simple_mix_gauss_val_sf1;

  s1_simple_mix_gauss_val_sf1.insert(s1_simple_obs_gauss_val_sf1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  s1_simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<s1_simple_bsta_mixture_fixed_sf1_3>  s1_simple_obs_mix_gauss_val_sf1(s1_simple_mix_gauss_val_sf1);

  boxm_sample_multi_bin<BOXM_APM_MOG_GREY> s1_sample;
  s1_sample.alpha=0.6f;
  s1_sample.set_appearance(s1_simple_obs_mix_gauss_val_sf1,5);

  // sample 2
  bsta_gauss_sf1 s2_simple_gauss_sf1(1.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> s2_simple_obs_gauss_val_sf1(s2_simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  s2_simple_mix_gauss_val_sf1;

  s2_simple_mix_gauss_val_sf1.insert(s2_simple_obs_gauss_val_sf1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  s2_simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<s2_simple_bsta_mixture_fixed_sf1_3>  s2_simple_obs_mix_gauss_val_sf1(s2_simple_mix_gauss_val_sf1);

  boxm_sample_multi_bin<BOXM_APM_MOG_GREY> s2_sample;
  s2_sample.alpha=0.6f;
  s2_sample.set_appearance(s2_simple_obs_mix_gauss_val_sf1,3);


  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >  > * block=scene.get_active_block();
    boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > * tree=new boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(4,3);
    boct_tree_cell<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >* cel11=tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.01));
    cel11->set_data(s2_sample);
    boct_tree_cell<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >* cell2=tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.01));
    cell2->set_data(s1_sample);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
  }
  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+block_dim.x()*world_dim.x(),
                                 origin.y()+block_dim.y()*world_dim.y(),
                                 origin.z()+block_dim.z()*world_dim.z()));
#if 0
  vpgl_camera_double_sptr camera = generate_camera_top(world);
#endif
  std::vector<vpgl_camera_double_sptr > cameras = generate_cameras_z_multi_bin(world);

  vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> expected(IMAGE_U,IMAGE_V);
  vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> mask(IMAGE_U,IMAGE_V);

  for (unsigned i=0; i<cameras.size(); i++) {
    expected.fill(0.0);
    mask.fill(0.0);

    boxm_render_image_splatting<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(scene,cameras[i],expected,mask,3);
    std::stringstream ss;
    ss << "./boxm_scene2_mb/img3" << i << ".tif";
    vil_save(expected, ss.str().data());

    expected.fill(0.0);
    boxm_render_image_splatting_triangle<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(scene,cameras[i],expected,mask,5);
    std::stringstream ss1;
    ss1 << "./boxm_scene2_mb/img5" << i << ".tif";
    vil_save(expected, ss1.str().data());
  }
#ifdef CLEANUP
  vpl_rmdir("./boxm_scene1");
  vpl_unlink("./scene1.xml");
#endif
  boxm_scene<boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > > scene_new(lvcs, origin,
                                                                                    block_dim, world_dim, 4U, 3U);
  scene_new.set_appearance_model(BOXM_APM_MOG_GREY);
  scene_new.set_paths("./boxm_scene_update", "block");
  vul_file::make_directory("./boxm_scene_update");
  std::ofstream os1("./boxm_scene_update/scene.xml");
  x_write(os1, scene_new, "scene");
  os1.close();

  // update the world, with all the generated images and cameras
  for (unsigned i=0; i<cameras.size(); i++) {
    std::stringstream ss;
    ss << "./boxm_scene2_mb/img3" << i << ".tif";
    expected = vil_load(ss.str().data());
    boxm_update_triangle<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(scene_new, expected, cameras[i],3 );
    std::stringstream ss1;
    ss1 << "./boxm_scene2_mb/img5" << i << ".tif";
    expected = vil_load(ss1.str().data());
    boxm_update_triangle<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(scene_new, expected, cameras[i],5 );
  }

  // regenerate the images from world
  for (unsigned i=0; i<cameras.size(); i++) {
    std::stringstream ss; ss << "boxm_scene2_mb/img_new" << i << ".tif";
    vil_image_view<float> expected_new(IMAGE_U,IMAGE_V);
    boxm_render_image_splatting_triangle<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(scene_new,cameras[i],expected_new,mask,3);
    vil_image_view<unsigned char> expected_byte(expected_new.ni(),expected_new.nj(),expected_new.nplanes());
    vil_convert_stretch_range_limited(expected_new,expected_byte, 0.0f, 1.0f);

    vil_save(expected_byte, ss.str().data());
  }
}

TESTMAIN(test_update_multi_bin);
