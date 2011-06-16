//:
// \file
// \author Isabel Restrepo
// \date Nov. 16, 2009.

#include <testlib/testlib_test.h>

#include "../bvpl_octree_neighbors.h"
#include "../bvpl_scene_kernel_operator.h"
#include "../bvpl_octree_vector_operator_impl.h"
#include "../bvpl_scene_vector_operator.h"
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>

#include <boct/boct_loc_code.h>

static bool debug = false;
typedef boct_tree<short, float> tree_type;
typedef boct_tree_cell<short, float> cell_type;


void fill_octree(boct_tree<short,float>* tree)
{
  vcl_cout << "Creating Octree" << vcl_endl;

  //Create tree
    //tree->split();

  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    tree->init_cells(0.1f);
    leaves[1]->set_data(0.5);
    if (debug)
      for (unsigned i=0; i<leaves.size(); i++) {
        vcl_cout<< leaves[i]->get_code().x_loc_ << ','
                << leaves[i]->get_code().y_loc_ << ','
                << leaves[i]->get_code().z_loc_ << ','
                << leaves[i]->data() << vcl_endl;
      }

    leaves[0]->split();
    //leaves[1]->set_data(0.6);
  }

#if 0
  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); i++) {
      vcl_cout<< leaves[i]->get_code().x_loc_ << ','
              << leaves[i]->get_code().y_loc_ << ','
              << leaves[i]->get_code().z_loc_ << ','
              << tree->local_origin(leaves[i]) << ','
              << leaves[i]->get_code().level << ','
              << tree->cell_size(leaves[i]) << ','
              << leaves[i]->data() << vcl_endl;
    }
  }
#endif
}

static void test_octree_neighbors()
{
  boct_tree<short,float>* tree = new boct_tree<short,float>(4,3);
  fill_octree(tree);
  bvpl_kernel_iterator iter;
  vgl_point_3d<int> min_pt(-1,-1,-1);
  vgl_point_3d<int> max_pt(1,1,1);
  for (int i=min_pt.x(); i<=max_pt.x(); i++) {
    for (int j=min_pt.y(); j<=max_pt.y(); j++) {
      for (int k=min_pt.z(); k<=max_pt.z(); k++) {
        iter.insert(vgl_point_3d<int>(i,j,k),bvpl_kernel_dispatch(1.0f));
      }
    }
  }
  bvpl_kernel_sptr kernel= new bvpl_kernel(iter, vnl_float_3(0,0,1), vnl_float_3(0,1,0),0.0f, vgl_vector_3d<int>(3,3,3), min_pt,max_pt);

  vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  bvpl_octree_neighbors<float> neighb(tree);
  for (unsigned i=0; i<leaves.size(); i++) {
    boct_tree_cell<short,float>* cell=leaves[i];
    vcl_cout << cell->code_
             << "THIS   " << tree->local_origin(cell) << vcl_endl;
    vcl_vector<boct_tree_cell<short,float> *> neighb_cells;
    neighb.neighbors(kernel, cell, neighb_cells);
    vcl_cout << "Number of neighbors: " << neighb_cells.size() << vcl_endl;
    for (unsigned n=0; n<neighb_cells.size(); n++) {
      vcl_cout << "  " << tree->local_origin(neighb_cells[n]) << vcl_endl;
    }
  }
}

TESTMAIN(test_octree_neighbors);
