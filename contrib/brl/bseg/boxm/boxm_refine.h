#ifndef boxm_refine_h_
#define boxm_refine_h_
//:
// \file
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_scene.h>


//: This method refines the blocks (octrees) of the scene
// If there are some cells with big values they are spitted into new child cells;
// the data of the cell is copied to the children's data. The purpose of this
// method is to elaborate the octree at the areas where more details exist.

template <class T_loc, boxm_apm_type APM>
void boxm_refine(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                 float occlusion_prob_thresh,
                 bool reset_appearance = true)
{
  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;

  boxm_block_iterator<tree_type> iter(&scene);
  float max_alpha_int = (float)-vcl_log(1.0 - occlusion_prob_thresh);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    tree_type* tree = block->get_tree();

    // get the leaf nodes of the block
    vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> >*> leaf_nodes = tree->leaf_cells();
    vcl_cout << "*****************************************" << vcl_endl;
    vcl_cout << "BLOCK " << iter.index() << vcl_endl;
    vcl_cout << "NUMBER OF LEAF NODES=" << leaf_nodes.size() << vcl_endl;
    vcl_cout << "*****************************************" << vcl_endl;
    vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> >*> split_list;
    for (unsigned i=0; i<leaf_nodes.size(); i++) {
      boct_tree_cell<T_loc, boxm_sample<APM> >* cell = leaf_nodes[i];
      boxm_sample<APM> data = cell->data();

      vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
      float side_len = (float) (cell_bb.max_z() - cell_bb.min_z());
      float alpha_int = data.alpha * side_len;

      if ((alpha_int > max_alpha_int) && (cell->level() > 0)) {
        split_list.push_back(cell);
        // make sure neighbor cells are not already coarser than this cell
        // get all the faces;
       /* boct_face_idx faces = ALL;
        vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> >*> neighbors;
        cell->find_neighbors(faces, neighbors, cell->level()+1);
        split_list.insert(split_list.end(), neighbors.begin(), neighbors.end());*/
      }
    }

    // splitting
    for (unsigned i=0; i<split_list.size(); i++) {
      boct_tree_cell<T_loc, boxm_sample<APM> >* cell = split_list[i];
      vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
      float side_len = (float) (cell_bb.max_z() - cell_bb.min_z());
      float new_alpha = max_alpha_int / side_len;

      boxm_sample<APM> old_sample = cell->data();
      typename boxm_sample<APM>::apm_datatype app;
      boxm_sample<APM> new_sample = boxm_sample<APM>(new_alpha, app);
      if (!reset_appearance) {
        // if it is not set, transfer the old appearance to the new sample
        new_sample.appearance = old_sample.appearance;
      }
      //cell->split(new_sample);
    }
  }
}

#endif
