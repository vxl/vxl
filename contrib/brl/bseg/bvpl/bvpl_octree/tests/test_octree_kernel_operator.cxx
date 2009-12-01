//
//:
//\file
//\author Isabel Restrepo
//\date 11/16/09.

#include <testlib/testlib_test.h>

#include "../bvpl_octree_kernel_operator.h"
#include "../bvpl_octree_vector_operator.h"
#include "../bvpl_scene_kernel_operator.h"
#include "../bvpl_octree_vector_operator_impl.h"
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
    for (unsigned i=0; i<leaves.size(); i++) {
          vcl_cout<< leaves[i]->get_code().x_loc_ << ",";
          vcl_cout<< leaves[i]->get_code().y_loc_ << ",";
          vcl_cout<< leaves[i]->get_code().z_loc_ << ",";
          vcl_cout<< leaves[i]->data() << vcl_endl;
        }
#endif
    
    leaves[1]->split();
    //leaves[1]->set_data(0.6);

  }
  
  
#if 0
  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); i++) {
      vcl_cout<< leaves[i]->get_code().x_loc_ << ",";
      vcl_cout<< leaves[i]->get_code().y_loc_ << ",";
      vcl_cout<< leaves[i]->get_code().z_loc_ << ",";
      vcl_cout<< tree->local_origin(leaves[i]) << ",";
      vcl_cout<< leaves[i]->get_code().level << ",";
      vcl_cout<< tree->cell_size(leaves[i]) << ",";
      vcl_cout<< leaves[i]->data() << vcl_endl;
    }
  }
#endif
  return;
}

void fill_edge3d_tree(boct_tree<short,float>* tree)
{
  tree-> split();
  vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  for(unsigned i= 0; i<4; i++)
  {
    leaves[i]->set_data(0.2);
    leaves[i]->split();
    {
    boct_tree_cell<short,float>* cc =leaves[i]->children();
    for(unsigned j1=0; j1<8; j1++)
      cc[j1].split();
    }
    leaves[i+4]->set_data(0.9);
    leaves[i+4]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i+4]->children();
      for(unsigned j1=0; j1<8; j1++)
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
  
  //kernel->print();
  
  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;
  
  //Create the octree
  boct_tree<short,float> *tree = new boct_tree<short,float>(3);
  fill_sample_octree(tree);
  
  //operate kernel on octree
  bvpl_octree_kernel_operator<float> oper(tree);
  
  boct_tree<short,float> *tree_out = tree->clone();
  
  //tree_out -> print();
  oper.operate(functor, kernel, tree_out, 1, 0.5); 
  // tree_out ->print();

  delete tree;
  delete tree_out;
  return true;
    
}

bool scene_kernel_operator()
{
  //Create kernel
  bvpl_edge3d_kernel_factory edge_factory(2,1,1);
  bvpl_kernel_sptr kernel = new bvpl_kernel(edge_factory.create());
  
  //kernel->print();
  
  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;
  
  //Create Scene
  short nlevels=4;

  typedef boct_tree<short,float > tree_type;
  boct_tree<short,float> *tree = new boct_tree<short,float>(3);
  
  fill_sample_octree(tree);
  
  //tree->print();
  
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  
  //world coordinate dimensions of a block
  vgl_vector_3d<double> block_dim(1,1,1);
  
  //number of blocks in a scene
  vgl_vector_3d<unsigned> world_dim(2,2,2);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("/Projects/vxl/bin/temp", "in_block");
 
  boxm_block_iterator<tree_type>  iter =scene.iterator();
  iter.begin();
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type> *block = scene.get_active_block();
    block->init_tree(tree->clone());
    scene.write_active_block();
  }
  
  //operate on octree
  bvpl_octree_kernel_operator<float> oper(tree);
  
  //tree->print();
  tree_type *tree_out = tree->clone();
  
  boxm_scene<tree_type> scene_out(lvcs, origin, block_dim, world_dim);
  scene_out.set_paths("/Projects/vxl/bin/temp", "out_block");
 
  oper.operate(functor,kernel, tree_out, 1, 0.5);
  
  
  //operate on scene
  bvpl_scene_kernel_operator scene_oper;
  scene_oper.operate(scene,functor, kernel, scene_out, 1, 0.5); 
  
  //compare blocks of scene with single tree, result should be the same
  vcl_vector<boct_tree_cell<short,float>* > leaves_out = tree_out->leaf_cells();
  boxm_block_iterator<tree_type>  iter2 =scene_out.iterator();
  bool eq=true;
  iter2.begin();
  for (; !iter2.end(); iter2++) {
    scene_out.load_block(iter2.index());
    boxm_block<tree_type> *block = scene_out.get_active_block();
    tree_type *temp_tree = block->get_tree();
    vcl_vector<boct_tree_cell<short,float>* > temp_leaves = temp_tree->leaf_cells();
    // compare tree values
    for (unsigned i=0; i<temp_leaves.size(); i++) {
      eq =eq && (temp_leaves[i]->data() == leaves_out[i]->data());
    }
  }
  TEST("tree operator == scene operator", true, eq);

  
  return eq;
  
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
  


  
  boct_tree<short,float> *tree_out = tree->clone();
  tree_out->init_cells(0.0f);
  short level = 0;
  double cell_length = 1.0/(double)(1<<(3 -level));
  boct_tree<short,int> *id_tree = tree->clone_to_type<int>();
  
  id_tree->init_cells(-1);
  
  bvpl_max_vector_operator<float> vector_op;
  
  
  vector_op.operate(functor, tree, kernel_vec, tree_out, id_tree, level, cell_length);
     

  //test response at the center is correct
  boct_loc_code<short> loc_code;
  
  bool result = true;
  for(short x=3; x<5; x++)
    for(short y=3; y<5; y++)
      for(short z=3; z<5; z++)
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
  bool result = octree_kernel();
  TEST("Octree kernel", true, result );
  result = octree_vector_operator();
  TEST("Octree vector operator", true, result);
  scene_kernel_operator();
  return 0;
}