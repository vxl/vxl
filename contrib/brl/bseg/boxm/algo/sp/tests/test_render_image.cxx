#include <testlib/testlib_test.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <boxm/util/boxm_test_util.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree_cell.h>
#include <vpl/vpl.h>
#include <boxm/algo/sp/boxm_render_image.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_camera_double_sptr.h>

static void test_render_image()
{
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(2,2,3);
  boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearance_model(BOXM_APM_MOG_GREY);
  scene.set_paths("./boxm_scene1", "block");
  vul_file::make_directory("./boxm_scene1");
  std::ofstream os("scene1.xml");
  x_write(os, scene, "scene");
  os.close();

  boxm_block_iterator<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);
  // default model
  bsta_gauss_sf1 simple_gauss_sf1(0.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> simple_obs_gauss_val_sf1(simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  simple_mix_gauss_val_sf1;

  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);
  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);
  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,0.1f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_sf1_3>  simple_obs_mix_gauss_val_sf1(simple_mix_gauss_val_sf1);


  boxm_sample<BOXM_APM_MOG_GREY> default_sample;
  default_sample.alpha=0.001f;
  default_sample.set_appearance(simple_obs_mix_gauss_val_sf1);

  // sample 1
  bsta_gauss_sf1 s1_simple_gauss_sf1(0.5f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> s1_simple_obs_gauss_val_sf1(s1_simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  s1_simple_mix_gauss_val_sf1;

  s1_simple_mix_gauss_val_sf1.insert(s1_simple_obs_gauss_val_sf1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  s1_simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<s1_simple_bsta_mixture_fixed_sf1_3>  s1_simple_obs_mix_gauss_val_sf1(s1_simple_mix_gauss_val_sf1);

  boxm_sample<BOXM_APM_MOG_GREY> s1_sample;
  s1_sample.alpha=0.6f;
  s1_sample.set_appearance(s1_simple_obs_mix_gauss_val_sf1);

  // sample 2
  bsta_gauss_sf1 s2_simple_gauss_sf1(1.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> s2_simple_obs_gauss_val_sf1(s2_simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  s2_simple_mix_gauss_val_sf1;

  s2_simple_mix_gauss_val_sf1.insert(s2_simple_obs_gauss_val_sf1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  s2_simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<s2_simple_bsta_mixture_fixed_sf1_3>  s2_simple_obs_mix_gauss_val_sf1(s2_simple_mix_gauss_val_sf1);

  boxm_sample<BOXM_APM_MOG_GREY> s2_sample;
  s2_sample.alpha=0.6f;
  s2_sample.set_appearance(s2_simple_obs_mix_gauss_val_sf1);


  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >  > * block=scene.get_active_block();
    auto * tree=new boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >(3,2);
    boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cel11=tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.01));
    cel11->set_data(s2_sample);
    boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell2=tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.01));
    cell2->set_data(s1_sample);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
  }
  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+world_dim.x(), origin.y()+world_dim.y(), origin.z()+world_dim.z()));
  vpgl_camera_double_sptr camera = generate_camera_top(world);

  vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> expected(IMAGE_U,IMAGE_V);
  expected.fill(0.0);
  vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> mask(IMAGE_U,IMAGE_V);
  mask.fill(0.0);

  boxm_render_image_splatting<short,boxm_sample<BOXM_APM_MOG_GREY> >(scene,camera,expected,mask);
  vpl_rmdir("boxm_scene1");
  vpl_unlink("scene1.xml");
}

TESTMAIN(test_render_image);
