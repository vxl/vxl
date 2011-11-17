//:
// \file
// \author Isabel Restrepo
// \date Nov. 16, 2009.

#include <testlib/testlib_test.h>
#include <boxm/algo/boxm_crop_scene.h>
#include <boxm/boxm_apm_traits.h>

const bool debug = true;

void fill_sample_octree(boct_tree<short,float>* tree)
{
  vcl_cout << "Creating Octree" << vcl_endl;

  //Create tree
  tree->init_cells(0.1f);
  tree->split();
  vcl_cout << "Level 1" << vcl_endl;

  {
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
    tree->init_cells(0.1f);
    leaves[1]->set_data(0.5f);
    if (debug)
      for (unsigned i=0; i<leaves.size(); i++) {
        vcl_cout<< leaves[i]->get_code().x_loc_ << ','
                << leaves[i]->get_code().y_loc_ << ','
                << leaves[i]->get_code().z_loc_ << ','
                << leaves[i]->data() << vcl_endl;
      }

    leaves[1]->split();
  }

  if (debug)
  {
    vcl_cout << "Level 0" << vcl_endl;
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->cells_at_level((short)0);
    for (unsigned i=0; i<leaves.size(); i++) {
      leaves[i]->set_data(0.9f);
      vcl_cout<< leaves[i]->get_code().x_loc_ << ','
              << leaves[i]->get_code().y_loc_ << ','
              << leaves[i]->get_code().z_loc_ << ','
              << leaves[i]->get_code().level << ','
              << leaves[i]->data() << vcl_endl;
    }
  }

  if (debug)
  {
    vcl_cout << "All Levels" << vcl_endl;
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->all_cells();
    for (unsigned i=0; i<leaves.size(); i++) {
      vcl_cout<< leaves[i]->get_code().x_loc_ << ','
              << leaves[i]->get_code().y_loc_ << ','
              << leaves[i]->get_code().z_loc_ << ','
              << leaves[i]->get_code().level << ','
              << leaves[i]->data() << vcl_endl;
    }
  }
}

static void test_boxm_scene_crop()
{
  //Create a dummy scene
  typedef boct_tree<short,float > tree_type;
  boct_tree<short,float> *tree = new boct_tree<short,float>(3);
  fill_sample_octree(tree);

  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0.0,0.0,0.0);

  //world coordinate dimensions of a block
  vgl_vector_3d<double> block_dim(10.0,10.0,10.0);

  //number of blocks in a scene
  vgl_vector_3d<unsigned> world_dim(1,1,1);

  boxm_scene<tree_type> *scene= new boxm_scene<tree_type>(lvcs, origin, block_dim, world_dim,true);
  vcl_string scene_path=".";
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

  boxm_scene<tree_type> *scene_out = boxm_crop_scene<float>(scene, bbox);

  //check that output scene is correct
  boxm_block_iterator<tree_type >  iter_out =scene_out->iterator();
  iter_out.begin();
  scene_out->load_block(iter_out.index());
  boxm_block<tree_type> *block_out = scene_out->get_active_block();
  boct_tree<short, float>  *tree_out= block_out->get_tree();

  vcl_vector<boct_tree_cell<short,float>*> leaves_out = tree_out->leaf_cells();
  bool result = true;
  for (unsigned i=0; i<leaves_out.size(); i++)
  {
    result = result && (vcl_abs(leaves_out[i]->data() - 0.9)< 1e-5);
  }

  TEST("Valid cropped tree", result, true);

  vcl_vector<boct_tree_cell<short,float>*> leaves = tree_out->all_cells();
  if (debug) {
    vcl_cout << "Printing Output Tree" << vcl_endl;
    for (unsigned i=0; i<leaves.size(); i++) {
      vcl_cout<< leaves[i]->get_code().x_loc_ << ','
              << leaves[i]->get_code().y_loc_ << ','
              << leaves[i]->get_code().z_loc_ << ','
              << leaves[i]->get_code().level << ','
              << leaves[i]->data() << vcl_endl;
    }
  }

  scene->clean_scene();
  scene_out->clean_scene();
}

TESTMAIN(test_boxm_scene_crop);
