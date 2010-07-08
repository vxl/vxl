#include <testlib/testlib_test.h>
//:
// \file
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>
#include <boxm/boxm_scene.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vgl/vgl_vector_3d.h>
#include <vul/vul_file.h>

boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > create_scene()
{
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short, data_type> tree_type;
  typedef boxm_scene<tree_type> scene_type;


  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(3,2,1);
  scene_type scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearance_model(BOXM_APM_MOG_GREY);
  
  vcl_string scene_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+"/contrib/brl/bseg/boxm/ocl/tests/boxm_scene1";
  vcl_string xml_path = scene_dir + "/scene1.xml";
  scene.set_paths(scene_dir, "block");
  vul_file::make_directory(scene_dir);
  vcl_ofstream os(xml_path.c_str());
  x_write(os, scene, "scene");
  os.close();

  unsigned max_level=5, init_level=3;
    
  // default model
  bsta_gauss_f1 simple_gauss_f1(0.0f,0.1f);
  bsta_num_obs<bsta_gauss_f1> simple_obs_gauss_val_f1(simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  simple_mix_gauss_val_f1;

  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_f1_3>  simple_obs_mix_gauss_val_f1(simple_mix_gauss_val_f1);

  data_type default_sample;
  default_sample.alpha=0.001f;
  default_sample.set_appearance(simple_obs_mix_gauss_val_f1);

  // sample 1
  bsta_gauss_f1 s1_simple_gauss_f1(0.5f,0.1f);
  bsta_num_obs<bsta_gauss_f1> s1_simple_obs_gauss_val_f1(s1_simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  s1_simple_mix_gauss_val_f1;

  s1_simple_mix_gauss_val_f1.insert(s1_simple_obs_gauss_val_f1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  s1_simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<s1_simple_bsta_mixture_fixed_f1_3>  s1_simple_obs_mix_gauss_val_f1(s1_simple_mix_gauss_val_f1);

  data_type s1_sample;
  s1_sample.alpha=0.6f;
  s1_sample.set_appearance(s1_simple_obs_mix_gauss_val_f1);

  // sample 2
  bsta_gauss_f1 s2_simple_gauss_f1(1.0f,0.1f);
  bsta_num_obs<bsta_gauss_f1> s2_simple_obs_gauss_val_f1(s2_simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  s2_simple_mix_gauss_val_f1;

  s2_simple_mix_gauss_val_f1.insert(s2_simple_obs_gauss_val_f1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  s2_simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<s2_simple_bsta_mixture_fixed_f1_3>  s2_simple_obs_mix_gauss_val_f1(s2_simple_mix_gauss_val_f1);

  data_type s2_sample;
  s2_sample.alpha=0.6f;
  s2_sample.set_appearance(s2_simple_obs_mix_gauss_val_f1);

  float count=0.1f;
  boxm_block_iterator<tree_type> iter(&scene);
  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<tree_type>* block=scene.get_active_block();
    tree_type* tree;
    if (iter.index().x()==0 && iter.index().y()==0 && iter.index().z()==0) {
      tree = new tree_type(max_level,init_level);
    } else {
      tree=new tree_type(max_level,init_level-1);
    }
    boct_tree_cell<short,data_type>* cel11 = tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.9));
    s2_sample.alpha=count;

    cel11->set_data(s2_sample);
    //boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell2=tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.51));
    //cell2->set_data(s2_sample);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
    count+=0.1f;
  }

  return scene;
}


static void test_scene_convert()
{
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short, data_type> tree_type;
  typedef boxm_scene<tree_type> scene_type;
  
  scene_type scene = create_scene();
  
  int num_buffers = 2, buff_size = 999;
  boxm_ocl_scene ocl_scene = boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::convert_scene(&scene, num_buffers, buff_size);

  vcl_cout<<ocl_scene<<vcl_endl;

}

TESTMAIN(test_scene_convert);
