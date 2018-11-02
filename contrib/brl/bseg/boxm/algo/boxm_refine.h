#ifndef boxm_refine_h_
#define boxm_refine_h_
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

template <class T_loc, class T_data>
void boxm_refine_block(boxm_block<boct_tree<T_loc, T_data> > *block,
                       float occlusion_prob_thresh, unsigned & num_split, bool reset_appearance = true)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  tree_type* tree = block->get_tree();
  float max_alpha_int = (float)-std::log(1.0 - occlusion_prob_thresh);
  // get the leaf nodes of the block
  std::vector<boct_tree_cell<T_loc, T_data>*> leaf_nodes = tree->leaf_cells();
#ifdef DEBUG
  std::cout << "  NUMBER OF LEAF NODES=" << leaf_nodes.size();
#endif
  std::vector<boct_tree_cell<T_loc, T_data >*> split_list;
  for (unsigned i=0; i<leaf_nodes.size(); i++) {
    boct_tree_cell<T_loc, T_data>* cell = leaf_nodes[i];
    T_data data = cell->data();

    vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
    float side_len = (float) (cell_bb.max_z() - cell_bb.min_z());
    float alpha_int = data.alpha * side_len;

    if ((alpha_int > max_alpha_int) && (cell->level() > 0)) {
      split_list.push_back(cell);
      // make sure neighbor cells are not already coarser than this cell
      // get all the faces;
#if 0
      boct_face_idx faces = ALL;
      std::vector<boct_tree_cell<T_loc, T_data >*> neighbors;
      cell->find_neighbors(faces, neighbors, cell->level()+1);
      split_list.insert(split_list.end(), neighbors.begin(), neighbors.end());
#endif
    }
  }
  num_split+=(int)split_list.size();
#ifdef DEBUG
  std::cout<<" Splitting "<<split_list.size()<<" cells" << std::endl;
#endif
  // splitting
  for (unsigned i=0; i<split_list.size(); i++) {
    boct_tree_cell<T_loc, T_data>* cell = split_list[i];
    vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
    float side_len = (float) (cell_bb.max_z() - cell_bb.min_z());
    float new_alpha = max_alpha_int / side_len;

    T_data old_sample = cell->data();
    typename T_data::apm_datatype app;
    if (!reset_appearance) {
      // if it is not set, transfer the old appearance to the new sample
      T_data new_sample = old_sample;
      new_sample.alpha=new_alpha;
      cell->split(new_sample);
    }
    else {
      T_data new_sample = T_data();
      new_sample.alpha = new_alpha;
      cell->split(new_sample);
    }
  }
}

//: This method refines the blocks (octrees) of the scene
// If there are some cells with big values they are spitted into new child cells;
// the data of the cell is copied to the children's data. The purpose of this
// method is to elaborate the octree at the areas where more details exist.
// returns the number of leaf nodes in the refined octree
template <class T_loc, class T_data>
unsigned int boxm_refine_scene(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                               float occlusion_prob_thresh,
                               bool reset_appearance = true)
{
  typedef boct_tree<T_loc, T_data > tree_type;

  unsigned int nleaves = 0;
  boxm_block_iterator<tree_type> iter(&scene);
  //float max_alpha_int = (float)-std::log(1.0 - occlusion_prob_thresh);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boxm_refine_block(block, occlusion_prob_thresh,nleaves, reset_appearance);
    //nleaves += block->size();
    scene.write_active_block();
  }
  std::cout<<"Total No of leaves split "<<nleaves<<std::endl;
  return nleaves;
}

#endif // boxm_refine_h_
