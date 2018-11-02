// This is brl/bseg/boxm/algo/boxm_crop_scene.h
#ifndef boxm_crop_scene_h
#define boxm_crop_scene_h

//:
// \file
// \brief  Template functions to create a new scene that is a cropped portion of another
// \author Isabel Restrepo mir@lems.brown.edu
// \date  January 11, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vgl/vgl_box_3d.h>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T_data>
boxm_scene<boct_tree<short, T_data> > *  boxm_crop_scene(boxm_scene<boct_tree<short,T_data> > *scene_in,vgl_box_3d<double> bbox)
{
  //only one-block scenes are supported for now
  if (!(scene_in->world_dim().x() == 1 && scene_in->world_dim().y()==1 && scene_in->world_dim().z()==1))
  {
    std::cerr << "In bvxm_crop_scene -- Input scene has more than one block, which is not supported\n";
    return nullptr;
  }

  //get the tree inside the block and locate region
  boxm_block_iterator<boct_tree<short, T_data> > iter_in = scene_in->iterator();
  iter_in.begin();

  //there is no need to increase iterator, since there should only be one block in the scene
  scene_in->load_block(iter_in.index());
  boct_tree<short, T_data>  *tree_in= (*iter_in)->get_tree();

  boct_tree_cell<short,T_data>* root = tree_in->locate_region_global(bbox, true);

  //most parameters of the output scene are the same as those of the input scene,
  //except the ROI which is determined by the root node
  double cell_length = 1.0/(double)(1<<(tree_in->root_level() - root->level()));
  double cell_origin_x = (double)(root->get_code().x_loc_)/(double)(1<<(tree_in->root_level()));
  double cell_origin_y = (double)(root->get_code().y_loc_)/(double)(1<<(tree_in->root_level()));
  double cell_origin_z = (double)(root->get_code().z_loc_)/(double)(1<<(tree_in->root_level()));

  vgl_box_3d<double> global_bbox = tree_in->bounding_box();

  vgl_point_3d<double> origin(cell_origin_x, cell_origin_y, cell_origin_z);

  vgl_point_3d<double> min_point((global_bbox.min_x() + global_bbox.width()*origin.x()),
                                 (global_bbox.min_y() + global_bbox.height()*origin.y()),
                                 (global_bbox.min_z() + global_bbox.depth()*origin.z()));

  vgl_vector_3d<double> block_dim(cell_length*global_bbox.width(),
                                  cell_length*global_bbox.height(),
                                  cell_length*global_bbox.depth());

  boxm_scene<boct_tree<short, T_data> > *scene_out =
  new boxm_scene<boct_tree<short, T_data> >(scene_in->lvcs(),min_point,block_dim,scene_in->world_dim(), scene_in->save_internal_nodes());

  scene_out->set_paths(scene_in->path(), "cropped");
  scene_out->set_appearance_model(scene_in->appearence_model());
  scene_out->clean_scene();
  boxm_block_iterator<boct_tree<short, T_data> > iter_out = scene_out->iterator();
  iter_out.begin();
  scene_out->load_block(iter_out.index());


  //Region of interest in [0,1)x[0,1)x[0,1) coordinates
  vgl_point_3d<double> norm_bbox_min((bbox.min_x()-global_bbox.min_x())/global_bbox.width(),
                                     (bbox.min_y()-global_bbox.min_y())/global_bbox.height(),
                                     (bbox.min_z()-global_bbox.min_z())/global_bbox.depth());
  vgl_point_3d<double> norm_bbox_max((bbox.max_x()-global_bbox.min_x())/global_bbox.width(),
                                     (bbox.max_y()-global_bbox.min_y())/global_bbox.height(),
                                     (bbox.max_z()-global_bbox.min_z())/global_bbox.depth());


  //output tree
  boct_tree<short, T_data >  *tree_out = tree_in->clone_and_intersect(root, tree_in->root_level(), vgl_box_3d<double>(norm_bbox_min,norm_bbox_max));

  //write the output tree
  (*iter_out)->init_tree(tree_out);
  scene_out->write_active_block();
  scene_out->write_scene("/cropped_scene.xml");

  return scene_out;
}


#endif // boxm_crop_scene_h
