//
#include <testlib/testlib_test.h>
//:
//\file
//\author Isabel Restrepo
//\date November 16, 2009.

#include "../bvpl_octree_kernel_operator.h"
#include "../bvpl_octree_vector_operator.h"
#include <bvpl/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_edge3d_kernel_factory.h>
#include <bvpl/bvpl_create_directions.h>

#include <boct/boct_loc_code.h>

bool debug =0;

void fill_sample_octree(boct_tree<short,float>* tree)
{
  vcl_cout << "Creating Octree" << vcl_endl;

  //Create tree
  tree->split();

  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    tree->init_cells(0.1f);
    leaves[1]->set_data(0.5);
#if debug
    for (unsigned i=0; i<leaves.size(); ++i) {
          vcl_cout << leaves[i]->get_code().x_loc_ << ','
                   << leaves[i]->get_code().y_loc_ << ','
                   << leaves[i]->get_code().z_loc_ << ','
                   << leaves[i]->data() << vcl_endl;
        }
#endif

    leaves[1]->split();
    //leaves[1]->set_data(0.6);
  }

#if 0
  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); ++i) {
      vcl_cout << leaves[i]->get_code().x_loc_ << ','
               << leaves[i]->get_code().y_loc_ << ','
               << leaves[i]->get_code().z_loc_ << ','
               << tree->local_origin(leaves[i]) << ','
               << leaves[i]->get_code().level << ','
               << tree->cell_size(leaves[i]) << ','
               << leaves[i]->data() << vcl_endl;
    }
  }
#endif
  return;
}

void fill_edge3d_tree(boct_tree<short,float>* tree)
{
  tree-> split();
  vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  for (unsigned i= 0; i<4; ++i)
  {
    leaves[i]->set_data(0.2f);
    leaves[i]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i]->children();
      for (unsigned j1=0; j1<8; ++j1)
        cc[j1].split();
    }
    leaves[i+4]->set_data(0.9f);
    leaves[i+4]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i+4]->children();
      for (unsigned j1=0; j1<8; ++j1)
        cc[j1].split();
    }
    //split children
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  }
}

bool octree_kernel()
{
  //Create kernel
  bvpl_edge3d_kernel_factory edge_factory(2,1,1);
  bvpl_kernel_sptr kernel = new bvpl_kernel(edge_factory.create());

  kernel->print();

  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;

  //Create the octree
  boct_tree<short,float> *tree = new boct_tree<short,float>(3);
  fill_sample_octree(tree);

  //operate kernel on octree
  bvpl_octree_kernel_operator<float, bvpl_edge_geometric_mean_functor<float> > oper(functor);

  boct_tree<short,float> *tree_out = tree->clone();

  //tree_out -> print();
  oper.operate(tree, kernel, tree_out, 1, 0.5);
  // tree_out ->print();

  delete tree;
  delete tree_out;
  return true;
}

bool octree_vector_operator()
{
  //Create the vector of kernels
  bvpl_edge3d_kernel_factory kernels_3d(5,5,5);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);


  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;

  //Create tree. this tree is initialized with 4 leves as a regular grid
  boct_tree<short,float> *tree = new boct_tree<short,float>(4);

  fill_edge3d_tree(tree);

  bvpl_octree_kernel_operator<float,bvpl_edge_geometric_mean_functor<float> >  kernel_op(functor);
  bvpl_octree_vector_operator vector_op;

  boct_tree<short,float> *tree_out = tree->clone();
  tree_out->init_cells(0.0f);
  short level = 0;
  double cell_length = 1.0/(double)(1<<(3 -level));
  boct_tree<short,int> *id_tree = tree->clone_to_type<int>();

  id_tree->init_cells(-1);

  vector_op.max_response(tree, kernel_vec, &kernel_op, tree_out, id_tree, level, cell_length);

  //test response at the center is correct
  boct_loc_code<short> loc_code;

  bool result = true;
  for (short x=3; x<5; ++x)
    for (short y=3; y<5; ++y)
      for (short z=3; z<5; ++z)
      {
        loc_code.set_code(x,y,z);
        loc_code.set_level(0);
        result = result && id_tree->get_cell(loc_code)->data() == 0;
        vcl_cout << id_tree->get_cell(loc_code)->data();
      }

  return result;
}


MAIN(test_octree_kernel_operator)
{
  //TEST("Octree kernel", true, octree_kernel());
  TEST("Octree vector operator", true, octree_vector_operator());
  return 0;
}
