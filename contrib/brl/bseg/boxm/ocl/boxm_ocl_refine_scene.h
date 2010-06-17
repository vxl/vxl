#ifndef boxm_ocl_refine_scene_h_
#define boxm_ocl_refine_scene_h_
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <vil/vil_transform.h>

#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_refine_manager.h>

#include <vcl_where_root_dir.h>
#include <vcl_iostream.h>


template <boxm_apm_type APM>
void boxm_ocl_refine_scene(boxm_scene<boct_tree<short, boxm_sample<APM> > >* scene,
                                     float prob_thresh)
{

  typedef boxm_sample<APM> data_type; 
  typedef boct_tree<short, data_type > tree_type;

  // render the image using the opencl raytrace manager
  boxm_refine_manager<data_type >* mgr = boxm_refine_manager<data_type >::instance();
  
  //loop through the blocks of scene, refine each tree
  boxm_block_iterator<tree_type > iter(scene);
  for(iter.begin(); !iter.end(); iter++) {
    scene->load_block(iter.index());
    boxm_block<tree_type >* block = scene->get_active_block();
    tree_type* tree = block->get_tree();    
  
    //initialize the manager
    if(!mgr->init(tree, prob_thresh)) {
      vcl_cout<<"ERROR : boxm_refine : mgr->init() failed"<<vcl_endl;
      return;
    }
    
    //run the refine method
    if (!mgr->run_tree()) {
      vcl_cout<<"ERROR : boxm_refine : mgr->run_tree() failed"<<vcl_endl;
      return;
    }
    
    
    // extract the output scene from the manager
    int* tree_array = mgr->get_tree();
    int  tree_size = mgr->get_tree_size();
    float* data = mgr->get_data();
    int  data_size = mgr->get_data_size();
    
    //////////////////////////////////////////
    //Do something with tree array here
    /////////////////////////////////////////
    vcl_cout<<"REFINED TREE! -> size:"<<tree_size<<vcl_endl;
      

    //clean up after each tree
    mgr->clean_refine();    
      
  }
}

#endif

