//:
// \file
// \author Isabel Restrepo
// \date February 4, 2010

#include <testlib/testlib_test.h>
#include <boxm/algo/boxm_fill_internal_cells.h>
#include <boxm/boxm_apm_traits.h>

const bool debug = false;

void build_tree(boct_tree<short,float>* tree)
{
  std::cout << "Creating Octree" << std::endl;

  // Create tree
  tree->split();
  {
    std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    leaves[1]->set_data(0.1f);
    leaves[1]->split();
    for (auto & leave : leaves)
      leave->set_data(0.1f);
  }

  if (debug)
  {
    std::cout << "All Levels" << std::endl;
    std::vector<boct_tree_cell<short,float>*> leaves = tree->all_cells();
    for (auto & leave : leaves) {
      std::cout<< leave->get_code().x_loc_ << ','
              << leave->get_code().y_loc_ << ','
              << leave->get_code().z_loc_ << ','
              << leave->get_code().level << ','
              << leave->data() << std::endl;
    }
  }
}

static void test_boxm_fill_internal_cells()
{
  //Create a dummy scene
  typedef boct_tree<short,float > tree_type;
  auto *tree = new boct_tree<short,float>(3);
  build_tree(tree);

  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0.0,0.0,0.0);

  //world coordinate dimensions of a block
  vgl_vector_3d<double> block_dim(10.0,10.0,10.0);

  //number of blocks in a scene
  vgl_vector_3d<unsigned> world_dim(1,1,1);

  auto *scene= new boxm_scene<tree_type>(lvcs, origin, block_dim, world_dim,true);
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

  vgl_box_3d<double> bbox(vgl_point_3d<double>(5, 0, 0), vgl_point_3d<double>(9, 4, 4));

  boxm_fill_internal_cells<float> filler;
  boxm_scene<tree_type> *scene_out = filler.traverse_and_fill(scene);

  //check that output scene is correct
  boxm_block_iterator<tree_type >  iter_out =scene_out->iterator();
  iter_out.begin();
  scene_out->load_block(iter_out.index());
  boxm_block<tree_type> *block_out = scene_out->get_active_block();
  boct_tree<short, float>  *tree_out= block_out->get_tree();
  std::vector<boct_tree_cell<short,float>*> cells = tree_out->all_cells();
  bool result = true;
  for (auto & cell : cells)
  {
    if (!cell->is_leaf())
    {
      boct_tree_cell<short,float>* children = cell->children();
      float avg =0;
      for (unsigned c=0; c<8; c++)
      {
        avg+=children[c].data();
      }
      result = result && (cell->data()==(avg/8.f));
    }
  }
  TEST("Valid average cells", result, true);

  if (debug) {
    std::cout << "Printing Output Tree" << std::endl;
    for (auto & cell : cells) {
      std::cout<< cell->get_code().x_loc_ << ','
              << cell->get_code().y_loc_ << ','
              << cell->get_code().z_loc_ << ','
              << cell->get_code().level << ','
              << cell->data() << std::endl;
    }
  }

  scene->clean_scene();
  scene_out->clean_scene();
}

TESTMAIN(test_boxm_fill_internal_cells);
