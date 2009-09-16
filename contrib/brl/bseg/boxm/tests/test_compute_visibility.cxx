#include <testlib/testlib_test.h>
#include <boxm/boxm_utils.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <boxm/boxm_compute_volume_visibility.h>
#include <boxm/boxm_scene.h>
#include "test_utils.h"

#include <vpl/vpl.h>
#include <vul/vul_file.h>

MAIN( test_compute_visibility )
{
  START ("TEST VISIBILITY");

  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(2,2,3);
  boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearence_model(BOXM_APM_MOG_GREY);
  scene.set_paths("./boxm_scene1", "block");
  vul_file::make_directory("./boxm_scene1");
  vcl_ofstream os("scene1.xml");
  x_write(os, scene, "scene");
  os.close();

  // sample 1
  bsta_gauss_f1 s1_simple_gauss_f1(0.5f,0.1f);
  bsta_num_obs<bsta_gauss_f1> s1_simple_obs_gauss_val_f1(s1_simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3> s1_simple_mix_gauss_val_f1;

  s1_simple_mix_gauss_val_f1.insert(s1_simple_obs_gauss_val_f1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3> s1_simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<s1_simple_bsta_mixture_fixed_f1_3> s1_simple_obs_mix_gauss_val_f1(s1_simple_mix_gauss_val_f1);

  boxm_sample<BOXM_APM_MOG_GREY> s1_sample;
  s1_sample.alpha=0.6f;
  s1_sample.set_appearance(s1_simple_obs_mix_gauss_val_f1);

  boxm_block_iterator<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);


  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
    boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=new boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >(3,2);
    if (iter.index().z()==1)
    {
      vcl_vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> > * > leaf_cells=tree->leaf_cells();
      s1_sample.alpha=0.6f;

      for (unsigned i=0;i<leaf_cells.size();i++)
      {
        leaf_cells[i]->set_data(s1_sample);
      }
      block->init_tree(tree);
      scene.write_active_block();
    }
    else
    {
      vcl_vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> > * > leaf_cells= tree->leaf_cells();
      s1_sample.alpha=0.01f;

      for (unsigned i=0;i<leaf_cells.size();i++)
      {
        leaf_cells[i]->set_data(s1_sample);
      }
      block->init_tree(tree);
      scene.write_active_block();
    }

    iter++;
  }
  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+block_dim.x()*world_dim.x(), 
                                 origin.y()+block_dim.y()*world_dim.y(), 
                                 origin.z()+block_dim.z()*world_dim.z()));
  vpgl_camera_double_sptr camera = generate_camera_top(world);

  double X=5,Y=5,Z;
  for (Z=40; Z>-10; --Z)
  {
    boxm_compute_point_visibility<short,boxm_sample<BOXM_APM_MOG_GREY> >(vgl_point_3d<double>(X,Y,Z),scene, camera);
  }
  vpl_rmdir("./boxm_scene1");
  vpl_unlink("./scene1.xml");

  SUMMARY();
}
