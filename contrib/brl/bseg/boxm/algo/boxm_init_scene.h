#ifndef boxm_init_scene_h_
#define boxm_init_scene_h_
//:
// \file
#include <iostream>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: This method inits the blocks (octrees) of the scene of type boxm_sample
template <boxm_apm_type APM_TYPE>
void boxm_init_scene(boxm_scene<boct_tree<short, boxm_sample<APM_TYPE> > > &scene)
{
  typedef boct_tree<short, boxm_sample<APM_TYPE> > tree_type;
  boxm_block_iterator<tree_type> iter(&scene);
  float pinit=scene.pinit();
  float dimx=(float)scene.block_dim().x();
  float numbercells=(float)(1<<(scene.init_level()-1));

  float alpha_init=-std::log(1-pinit)*numbercells/dimx;
  for (; !iter.end(); iter++) {
       vgl_point_3d<int> index=iter.index();
    if (!scene.discover_block(index.x(),index.y(),index.z()))
    {
      boxm_sample<APM_TYPE> initsample(alpha_init);
      tree_type* tree= new tree_type(initsample,scene.max_level(),scene.init_level());
      boxm_block<tree_type> * block=new boxm_block<tree_type>(scene.get_block_bbox(index.x(),index.y(),index.z()),tree);
      scene.set_block(index,block);
      scene.write_active_block();
    }
  }
}

//: Initializes a general scene
template <class T_data>
void boxm_init_scene(boxm_scene<boct_tree<short, T_data> > &scene)
{
  typedef boct_tree<short, T_data> tree_type;
  boxm_block_iterator<tree_type> iter(&scene);
  float dimx=(float)scene.block_dim().x();

  for (; !iter.end(); iter++) {
    vgl_point_3d<int> index=iter.index();
    if (!scene.discover_block(index.x(),index.y(),index.z()))
    {
      T_data initdata;
      tree_type* tree= new tree_type(initdata,scene.max_level(),scene.init_level());
      boxm_block<tree_type> * block=new boxm_block<tree_type>(scene.get_block_bbox(index.x(),index.y(),index.z()),tree);
      scene.set_block(index,block);
      scene.write_active_block();
    }
  }
}

#endif // boxm_init_scene_h_
