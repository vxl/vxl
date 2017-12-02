//:
// \file
// \brief Test linear combinations of blocks
// \author Isabel Restrepo
// \date 2-Feb-2011

#include <testlib/testlib_test.h>
#include <boxm/algo/boxm_linear_operations.h>
#include "test_utils.h"

void test_linear_operations()
{
  clean_up();
  //create scene
  boxm_scene<boct_tree<short, float> > *scene1 = create_scene(2,2,2,true, "test_scene1");
  boxm_scene<boct_tree<short, float> > *scene2 = create_scene(2,2,2,true, "test_scene2");

  float s1 = 2.0, s2=3.0;
  for (unsigned i = 0; i< 2; i++)
    for (unsigned j = 0; j < 2; j++)
      for (unsigned k = 0; k< 2; k++)
      {
        scene1->load_block(i,j,k);
        scene2->load_block(i,j,k);

        boxm_block<boct_tree<short, float> > *block1 = scene1->get_block(i,j,k);
        boxm_block<boct_tree<short, float> > *block2 = scene2->get_block(i,j,k);

        //The result is block1 = s1*block1+ s2*block2
        boxm_linear_combination(block1, block2, s1, s2);

        //make sure to write the block back to disk
        scene1->write_active_block();

        //clean memory
        scene1->unload_active_blocks();
        scene2->unload_active_blocks();
      }


  //Test the output scene. Scene1 should contain the result i.e (2*0.8 + 3*0.8) and scene2 should be unchanged
  boxm_cell_iterator<boct_tree<short, float> > it1 =
  scene1->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block, true);

  boxm_cell_iterator<boct_tree<short, float > > it2 =
  scene2->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block, true);

  it1.begin();
  it2.begin();
  bool result = true;

  while ( !(it1.end() || it2.end()) ) {

    boct_tree_cell<short,float > *cell1 = *it1;
    boct_tree_cell<short,float > *cell2 = *it2;

    if (std::abs(cell1->data()- 4.0f) > 1.0e-7){
      std::cerr << "Data1 is incorrect: "<< cell1->data()  <<"Error: " << std::abs(cell1->data()- 4.0f) << '\n';
      result = false;
      break;
    }

    if (std::abs(cell2->data()- 0.8f) > 1.0e-7){
      std::cerr << "Data2 is incorrect: " << cell2->data()<<'\n';
      result = false;
      break;
    }

    ++it1;
    ++it2;
  }

#ifdef DEBUG_LEAKS
  std::cerr << "Leaks at test_linear_operations " << boct_tree_cell<short, float >::nleaks() << std::endl;
#endif

  TEST("Output data is correct", result, true);
  clean_up();
}


TESTMAIN(test_linear_operations);
