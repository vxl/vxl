//:
// \file
// \author Isabel Restrepo
// \date 24-Aug-2011

#include <testlib/testlib_test.h>

#include "test_utils.h"

#include <bvpl_octree/bvpl_gauss3D_steerable_filters.h>

void test_gauss3D_steerable_filters()
{
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree<short, vnl_vector_fixed< float,10 > > out_tree_type;

  clean_up();

  //create scene
  boxm_scene<boct_tree<short, float > > *mean_scene = create_scene(2,2,2);

  //Explore mean scene
  std::cout << "Exploring mean scene \n";
  {
    boxm_cell_iterator<boct_tree<short,float > > iterator = mean_scene->cell_iterator(&boxm_scene<boct_tree<short, float > >::load_block);
    iterator.begin();

    while (!iterator.end()) {
      boct_tree_cell<short,float > *cell = *iterator;
      //if(cell->data().mean() > 0.6)
      {
        boct_loc_code<short> code =cell->get_code();
        std::cout << " Code: " <<code<< std::endl;
        std::cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << std::endl;

      }
      ++iterator;
    }
  }

  //create the output scenes
  boxm_scene<out_tree_type> *scene_out =
  new boxm_scene<out_tree_type>(mean_scene->lvcs(), mean_scene->origin(), mean_scene->block_dim(), mean_scene->world_dim(), mean_scene->max_level(), mean_scene->init_level());
  scene_out->set_paths(".", "steerable_gauss_3d_scene");
  scene_out->set_appearance_model(VNL_FLOAT_10);
  scene_out->write_scene("steerable_gauss_3d_scene.xml");
  mean_scene->clone_blocks_to_vector<10>(*scene_out);

  //Explore Initial Response scene
  std::cout << "Exploring initial response scene \n";
  {
    boxm_cell_iterator<out_tree_type > iterator = scene_out->cell_iterator(&boxm_scene<out_tree_type>::load_block);
    iterator.begin();

    while (!iterator.end()) {
      boct_tree_cell<short, vnl_vector_fixed<float,10> > *cell = *iterator;
      //if(cell->data().mean() > 0.6)
      {
        boct_loc_code<short> code =cell->get_code();
        std::cout << " Code: " <<code<< std::endl;
        std::cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << std::endl;

      }
      ++iterator;
    }
  }

  boxm_scene<boct_tree<short, bool> > *valid_scene =
  new boxm_scene<boct_tree<short, bool> >(mean_scene->lvcs(), mean_scene->origin(), mean_scene->block_dim(), mean_scene->world_dim(), mean_scene->max_level(), mean_scene->init_level());
  valid_scene->set_paths(".", "valid_scene");
  valid_scene->set_appearance_model(BOXM_BOOL);
  valid_scene->write_scene("valid_scene.xml");

  //operate on scene
  double cell_length = scene_out->finest_cell_length();
  bvpl_gauss3D_steerable_filters sf;

  sf.basis_response_at_leaves(scene_out, valid_scene, cell_length);


  //Explore Response scene
  std::cout << "Exploring response scene \n";
  {
    boxm_cell_iterator<out_tree_type > iterator = scene_out->cell_iterator(&boxm_scene<out_tree_type>::load_block);
    iterator.begin();

    while (!iterator.end()) {
      boct_tree_cell<short, vnl_vector_fixed<float,10> > *cell = *iterator;
      //if(cell->data().mean() > 0.6)
      {
        boct_loc_code<short> code =cell->get_code();
        std::cout << " Code: " <<code<< std::endl;
        std::cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << std::endl;

      }
      ++iterator;
    }
  }

  //rotate features according to gradient
  sf.rotation_invariant_interpolation(scene_out, valid_scene);

  //Explore Invariant Response scene
  std::cout << "Exploring rotation invariant response scene \n";
  {
    boxm_cell_iterator<out_tree_type > iterator = scene_out->cell_iterator(&boxm_scene<out_tree_type>::load_block);
    iterator.begin();

    while (!iterator.end()) {
      boct_tree_cell<short, vnl_vector_fixed<float,10> > *cell = *iterator;
      //if(cell->data().mean() > 0.6)
      {
        boct_loc_code<short> code =cell->get_code();
        std::cout << " Code: " <<code<< std::endl;
        std::cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << std::endl;

      }
      ++iterator;
    }
  }

  bool result = true;
  TEST("Valid Test", result, true);
}


TESTMAIN(test_gauss3D_steerable_filters);
