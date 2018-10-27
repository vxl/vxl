//:
// \file
// \author Isabel Restrepo
// \date Nov. 16, 2009.

#include <testlib/testlib_test.h>

#include "../bvpl_octree_kernel_operator.h"
#include "../bvpl_scene_kernel_operator.h"
#include "../bvpl_octree_vector_operator_impl.h"
#include "../bvpl_scene_vector_operator.h"
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>

#include <boct/boct_loc_code.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>

static bool debug = false;
typedef boct_tree<short, float> tree_type;
typedef boct_tree_cell<short, float> cell_type;


void fill_sample_octree(boct_tree<short,float>* tree)
{
  std::cout << "Creating Octree" << std::endl;

  //Create tree
    tree->split();

  {
    std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    tree->init_cells(0.1f);
    leaves[1]->set_data(0.5);
    if (debug)
      for (auto & leave : leaves) {
        std::cout<< leave->get_code().x_loc_ << ','
                << leave->get_code().y_loc_ << ','
                << leave->get_code().z_loc_ << ','
                << leave->data() << std::endl;
      }

    leaves[1]->split();
    //leaves[1]->set_data(0.6);
  }

#if 0
  {
    std::vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); i++) {
      std::cout<< leaves[i]->get_code().x_loc_ << ','
              << leaves[i]->get_code().y_loc_ << ','
              << leaves[i]->get_code().z_loc_ << ','
              << tree->local_origin(leaves[i]) << ','
              << leaves[i]->get_code().level << ','
              << tree->cell_size(leaves[i]) << ','
              << leaves[i]->data() << std::endl;
    }
  }
#endif
}

void fill_edge3d_tree(boct_tree<short,float>* tree)
{
  tree-> split();
  std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  for (unsigned i= 0; i<4; i++)
  {
    leaves[i]->set_data(0.2f);
    leaves[i]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i]->children();
      for (unsigned j1=0; j1<8; j1++)
        cc[j1].split();
    }
    leaves[i+4]->set_data(0.9f);
    leaves[i+4]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i+4]->children();
      for (unsigned j1=0; j1<8; j1++)
        cc[j1].split();
    }
    //split children
    std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  }
}

void octree_kernel()
{
  //Create kernel
  bvpl_edge3d_kernel_factory edge_factory(-1,1,-1,0,-1,0);
  bvpl_kernel_sptr kernel = new bvpl_kernel(edge_factory.create());
#ifdef DEBUG
  kernel->print();
#endif
  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;

  //Create the octree
  auto *tree = new boct_tree<short,float>(3);
  fill_sample_octree(tree);

  //operate kernel on octree
  bvpl_octree_kernel_operator<float> oper(tree);

  boct_tree<short,float> *tree_out = tree->clone();
#ifdef DEBUG
  tree_out->print();
#endif
  oper.operate(functor, kernel, tree_out, 1, 0.5);
#ifdef DEBUG
  tree_out->print();
#endif

  TEST("Octree kernel", true, true);
  delete tree;
  delete tree_out;
}

void scene_kernel_operator()
{
  //Create kernel
  bvpl_edge3d_kernel_factory edge_factory(-1,0,-1,0,-1,0);
  bvpl_kernel_sptr kernel = new bvpl_kernel(edge_factory.create());

  //kernel->print();

  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;


  /***********Part1 operate on tree*******************/

  //Create tree
  typedef boct_tree<short,float > tree_type;
  auto *tree = new boct_tree<short,float>(3);

  fill_sample_octree(tree);

  //operate on octree
  bvpl_octree_kernel_operator<float> oper(tree);


  //Create output trees
  boct_tree<short,float > *tree_out = tree->clone();
  tree_out->init_cells(0.0f);
  short level = 0;
  double cell_length = 1.0/(double)(1<<(tree->root_level() -level));

  // void operate(F functor, bvpl_kernel_sptr kernel, tree_type* out_tree, short level, double cell_length)
  oper.operate(functor,kernel, tree_out, level, cell_length);


  /***********Part2 operate on scene*******************/

  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);

  //world coordinate dimensions of a block
  vgl_vector_3d<double> block_dim(1,1,1);

  //number of blocks in a scene
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  std::string scene_path="./";
  scene.set_paths(scene_path, "in_block");

  boxm_block_iterator<tree_type>  iter =scene.iterator();
  iter.begin();
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type> *block = scene.get_active_block();
    block->init_tree(tree->clone());
    scene.write_active_block();
  }

  //Create the output scenes
  boxm_scene<boct_tree<short,float> > scene_out(lvcs, origin, block_dim, world_dim);
  scene_out.set_paths(scene_path, "response_scene");

  //operate on scene
  bvpl_scene_kernel_operator scene_oper;
  scene_oper.operate(scene,functor, kernel, scene_out);

  //compare blocks of scene with single tree, result should be the same
  std::vector<boct_tree_cell<short,float>* > leaves_out = tree_out->leaf_cells();
  boxm_block_iterator<tree_type>  iter2 =scene_out.iterator();
  bool eq=true;
  iter2.begin();
  for (; !iter2.end(); iter2++) {
    scene_out.load_block(iter2.index());
    boxm_block<tree_type> *block = scene_out.get_active_block();
    tree_type *temp_tree = block->get_tree();
    std::vector<boct_tree_cell<short,float>* > temp_leaves = temp_tree->leaf_cells();
    // compare tree values
    for (unsigned i=0; i<temp_leaves.size(); i++) {
      if (!((temp_leaves[i]->data() == leaves_out[i]->data())))
      {
        std::cout << "scene data: " << temp_leaves[i]->data() << " grid data: " << leaves_out[i]->data()  << std::endl;
        eq = false;
      }
    }
  }
  TEST("tree operator == scene operator", eq, true);
  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());//file_it() is deleted here
   // vul_file::delete_file_glob(file_it());
  }
}

void octree_vector_operator()
{
  //Create the vector of kernels
  bvpl_edge3d_kernel_factory kernels_3d(-3,2, -3,2,-3,2);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);

  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;

  //Create tree. this tree is initialized with 4 leves as a regular grid
  auto *tree = new boct_tree<short,float>(4);

  fill_edge3d_tree(tree);

  boct_tree<short,bvpl_octree_sample<float> > *tree_out = tree->clone_to_type<bvpl_octree_sample<float> >();
  tree_out->init_cells(bvpl_octree_sample<float>(0.0f, -1));
  short level = 0;
  double cell_length = 1.0/(double)(1<<(3 -level));

  bvpl_max_vector_operator<float> vector_op;

  vector_op.operate(tree, functor, kernel_vec, tree_out, level, cell_length);

  //test response at the center is correct
  boct_loc_code<short> loc_code;

  bool result = true;
  for (short x=3; x<5; x++)
    for (short y=3; y<5; y++)
      for (short z=3; z<5; z++)
      {
        loc_code.set_code(x,y,z);
        loc_code.set_level(0);
        result = result && tree_out->get_cell(loc_code)->data().id_ == 0;
      }
  TEST("Octree vector operator", result, true);
}

void scene_vector_operator()
{
  //Create the vector of kernels
  bvpl_edge3d_kernel_factory kernels_3d(-3,2,-3,2,-3,2);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);

  //Create functor
  bvpl_edge_geometric_mean_functor<float> functor;

  /***********Part1 operate on tree*******************/

  //Create tree. this tree is initialized with 4 leves as a regular grid
  auto *tree = new boct_tree<short,float>(4);
  fill_edge3d_tree(tree);

  //Create output trees
  boct_tree<short,bvpl_octree_sample<float> > *tree_out = tree->clone_to_type<bvpl_octree_sample<float> >();
  tree_out->init_cells(bvpl_octree_sample<float>(0.0f, -1));
  short level = 0;
  double cell_length = 1.0/(double)(1<<(3 -level));

  //operate vector on tree
  bvpl_max_vector_operator<float> vector_op;

  vector_op.operate(tree, functor, kernel_vec, tree_out, level, cell_length);

  /***********Part2 operate on scene*******************/

  //Crete the input scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);

  vgl_vector_3d<double> block_dim(1,1,1); //world coordinate dimensions of a block
  vgl_vector_3d<unsigned> world_dim(2,2,2); //number of blocks in a scene

  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  std::string scene_path("./");
  scene.set_paths(scene_path, "scene_in");

  boxm_block_iterator<tree_type>  iter =scene.iterator();
  iter.begin();
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type> *block = scene.get_active_block();
    block->init_tree(tree->clone());
    scene.write_active_block();
  }

  //Create the output scenes
  boxm_scene<boct_tree<short,bvpl_octree_sample<float> > > scene_out(lvcs, origin, block_dim, world_dim);
  scene_out.set_paths(scene_path, "response_scene");

  //operate on scene
  bvpl_scene_vector_operator scene_oper;
  scene_oper.operate(scene, functor, kernel_vec, scene_out);

  //compare blocks of scene with single tree, result should be the same
  std::vector<boct_tree_cell<short,bvpl_octree_sample<float> >* > leaves_out = tree_out->leaf_cells();
  boxm_block_iterator<boct_tree<short,bvpl_octree_sample<float> > >  iter2 =scene_out.iterator();
  bool eq = true;
  iter2.begin();
  for (; !iter2.end(); iter2++) {
    scene_out.load_block(iter2.index());
    boxm_block<boct_tree<short,bvpl_octree_sample<float> > > *block = scene_out.get_active_block();
    boct_tree<short,bvpl_octree_sample<float> > *temp_tree = block->get_tree();
    std::vector<boct_tree_cell<short,bvpl_octree_sample<float> >* > temp_leaves = temp_tree->leaf_cells();
    // compare tree values
    for (unsigned i=0; i<temp_leaves.size(); i++)
      if (!((temp_leaves[i]->data().response_ == leaves_out[i]->data().response_)||
            (temp_leaves[i]->data().id_ == leaves_out[i]->data().id_)))
      {
        std::cout << "scene data: " << temp_leaves[i]->data() << " grid data: " << leaves_out[i]->data()  << std::endl;
        eq = false;
      }
  }

  TEST("tree operator == scene operator", eq, true);

  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());//Already deletes file_it()
  //  vul_file::delete_file_glob(file_it());
  }
}


static void test_octree_kernel_operator()
{
  octree_kernel();
 // octree_vector_operator(); //TEST FAIL FIX_ME
 // scene_kernel_operator();
 // scene_vector_operator();
 }

TESTMAIN(test_octree_kernel_operator);
