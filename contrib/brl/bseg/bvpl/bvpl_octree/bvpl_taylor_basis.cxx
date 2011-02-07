//:
// \file
// \author Isabel Restrepo
// \date 31-Jan-2011

#include "bvpl_taylor_basis.h"
#include <boxm/boxm_scene.h>

//: Applies all taylor kernels to the scene associated with this class. Writes response scenes indivudually
void bvpl_taylor_basis::apply_basis()
{
//  if (!scene_base_->appearence_model() == BOXM_FLOAT)
//    vcl_cerr << " In bvpl_taylor_basis::apply_basis(), unssuported scene type" << vcl_endl;
//  
//  typedef boct_tree<short, float > tree_type;
//  boxm_scene<tree_type> *scene_in = static_cast<boxm_scene<tree_type>* > (scene_base_.as_pointer());
//  
//  double finest_cell_length = scene_in->finest_cell_length();
//  
//  vcl_map<vcl_string, bvpl_kernel_sptr>::iterator map_it = kernels_map_.begin();
//    
//  for(; map_it!= kernels_map_.end(); map_it++)
//  {
//    vcl_string this_name = map_it->first;
//    bvpl_kernel_sptr kernel = map_it->second;
//    kernel->set_voxel_length(finest_cell_length);
//
//    //parameters of the output scene are the same as those of the input scene
//    boxm_scene<tree_type> *scene_out =
//    new boxm_scene<tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
//    scene_out->set_paths(path_out_, this_name);
//    scene_out->set_appearance_model(BOXM_FLOAT);
//    scene_out->write_scene(output_path + "/" + this_name + "_scene.xml");
//   
//    bvpl_algebraic_functor functor;
//    bvpl_scene_kernel_operator scene_oper;
//    //operate on scene
//    scene_oper.operate(*scene_in, functor, kernel, *scene_out);
//    
//    //make sure to unload output scene from memory
//    scene_out->unload_active_blocks();    
//    
//  }
 
}


//: Computes the sum of square errors between the scene and the taylor approximation for a percentage of voxels
// This function operates on a block specified by an index
//void bvpl_taylor_basis::compute_reconstruction_error(int block_i, int block_j, int block_k,
//                                                     vcl_string taylor_scene_path,
//                                                     float percentage,
//                                                     boxm_scene<boct_tree<short, float> > *scene_out)
//{
////  //Load I0 scene
////  //load scene
////  boxm_scene_parser parser;
////  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
////  scene_ptr->load_scene(filename, parser);
////  boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
////  scene->load_scene(parser);
//  
//}