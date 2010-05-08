#ifndef boxm2_init_scene_h_
#define boxm2_init_scene_h_
//:
// \file
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm2/sample/boxm_sample.h>
#include <boxm2/boxm_scene.h>
#include <vcl_iostream.h>


//: This method inits the blocks (octrees) of the scene

template <boxm_apm_type APM_TYPE>
void boxm_init_scene(boxm_scene<boct_tree<short, boxm_sample<APM_TYPE> > > &scene)
{
  typedef boct_tree<short, boxm_sample<APM_TYPE> > tree_type;
  boxm_block_iterator<tree_type> iter(&scene);
  float pinit=scene.pinit();
  float dimx=(float)scene.block_dim().x();
  float numbercells=(float)(1<<(scene.init_level()-1));

  float alpha_init=-vcl_log(1-pinit)*numbercells/dimx;
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

#endif // boxm2_init_scene_h_
