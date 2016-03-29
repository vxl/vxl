//:
// \file
// \author Isabel Restrepo
// \date Jan 19, 2010.

#include <testlib/testlib_test.h>
#include <bvxm/algo/bvxm_boxm_convert.h>
#include <boxm/boxm_apm_traits.h>

const bool debug = true;

void fill_octree(boct_tree<short,float>* tree)
{
  std::cout << "Creating Octree" << std::endl;

  //Create tree
  tree->init_cells(0.1f);
  tree->split();
  std::cout << "Level 1" << std::endl;

  {
    std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    tree->init_cells(0.1f);
    leaves[1]->set_data(0.5f);
    if (debug)
      for (unsigned i=0; i<leaves.size(); i++) {
        std::cout << leaves[i]->get_code().x_loc_ << ','
                 << leaves[i]->get_code().y_loc_ << ','
                 << leaves[i]->get_code().z_loc_ << ','
                 << leaves[i]->data() << std::endl;
      }

    leaves[1]->split();
  }

  if (debug)
  {
    std::cout << "Level 0" << std::endl;
    std::vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); i++) {
      leaves[i]->set_data(0.9f);
      std::cout << leaves[i]->get_code().x_loc_ << ','
               << leaves[i]->get_code().y_loc_ << ','
               << leaves[i]->get_code().z_loc_ << ','
               << leaves[i]->get_code().level << ','
               << leaves[i]->data() << std::endl;
    }
  }

  if (debug)
  {
    std::cout << "All Levels" << std::endl;
    std::vector<boct_tree_cell<short,float>*> leaves = tree->all_cells();
    for (unsigned i=0; i<leaves.size(); i++) {
      std::cout << leaves[i]->get_code().x_loc_ << ','
               << leaves[i]->get_code().y_loc_ << ','
               << leaves[i]->get_code().z_loc_ << ','
               << leaves[i]->get_code().level << ','
               << leaves[i]->data() << std::endl;
    }
  }
}

static void test_boxm_scene_to_bvxm_grid()
{
  //Create a dummy scene
  typedef boct_tree<short,float > tree_type;
  boct_tree<short,float> *tree = new boct_tree<short,float>(3);
  fill_octree(tree);

  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0.0,0.0,0.0);

  //world coordinate dimensions of a block
  vgl_vector_3d<double> block_dim(10.0,10.0,10.0);

  //number of blocks in a scene
  vgl_vector_3d<unsigned> world_dim(1,1,1);

  boxm_scene<tree_type> *scene= new boxm_scene<tree_type>(lvcs, origin, block_dim, world_dim,true);
  std::string scene_path=".";
  scene->set_paths(scene_path, "in_block");
  scene->set_appearance_model(BOXM_FLOAT);
  scene->clean_scene();

  boxm_block_iterator<tree_type >  iter =scene->iterator();
  iter.begin();
  scene->load_block(iter.index());
  boxm_block<tree_type> *block = scene->get_active_block();
  block->init_tree(tree);
  scene->write_active_block();
  std::string input_path("scene_to_grid.vox");

  bvxm_voxel_grid<float> *grid = boxm_scene_to_bvxm_grid(*scene,input_path, 0);

  //Check the grid
  bvxm_voxel_grid<float>::iterator grid_it = grid->slab_iterator(0, grid->grid_size().z());

  for (unsigned z = 0; z<grid->grid_size().z(); ++z)
    for (unsigned y = 0; y<grid->grid_size().y(); ++y)
      for (unsigned x= 0; x<grid->grid_size().x(); ++x)
      {
        std::cout << "Value at " << '(' << x << ',' << y <<',' <<z << ")= " << (*grid_it)(x,y,z) << std::endl;
      }

  // TEST("Valid cropped tree", result, true);

  scene->clean_scene();
}

TESTMAIN(test_boxm_scene_to_bvxm_grid);
