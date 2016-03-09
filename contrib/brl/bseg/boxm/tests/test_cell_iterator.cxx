//:
// \file
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <testlib/testlib_test.h>
#include "test_utils.h"

void test_cell_iterator()
{

  clean_up();
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene();

  //get the iterator
  boxm_cell_iterator<boct_tree<short, float > > iterator = scene->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block_and_neighbors);
  iterator.begin();

  boct_tree_cell<short,float> *cell = *iterator;
  unsigned num_cells = 0;
  bool result = true;

  while (!iterator.end()) {

    if( (std::abs(cell->data() - 0.8) > 1e-7) && (std::abs(cell->data() - 0.5) > 1e-7) ){
      result = false;
      std::cerr << " Wrong data: " << cell->data() << std::endl;
    }
    ++iterator;
    num_cells ++;
        if(!iterator.end()){
    cell = *iterator;
        }
  }

  if(num_cells!=120){
    result = false;
    std::cerr << "Wrong number of cells: " << num_cells << std::endl;
  }

  TEST("Valid Test", result, true);
#ifdef DEBUG_LEAKS
  std::cerr << "Leaks at test_cell_iterator " << boct_tree_cell<short, float >::nleaks() << std::endl;
#endif
  clean_up();

}


TESTMAIN(test_cell_iterator);
