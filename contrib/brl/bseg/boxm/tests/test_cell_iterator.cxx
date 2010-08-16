//:
// \file
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <testlib/testlib_test.h>
#include "test_utils.h"

void test_cell_iterator()
{
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene();

  //get the iterator
  boxm_cell_iterator<boct_tree<short, float > > iterator = scene->cell_iterator();
  
  iterator.begin();
  
  boct_tree_cell<short,float> *cell = *iterator;  
  unsigned num_cells = 0;
  bool result = true;

  while (cell) {
     
    if( (vcl_abs(cell->data() - 0.8) > 1e-7) && (vcl_abs(cell->data() - 0.5) > 1e-7) ){
      result = false;
      vcl_cerr << " Wrong data: " << cell->data() << vcl_endl;
    }
    ++iterator;
    cell = *iterator;
    num_cells ++;
  }
  
  if(num_cells!=120){
    result = false;
    vcl_cerr << "Wrong number of cells: " << num_cells << vcl_endl;
  }
  
  TEST("Valid Test", result, true);
  clean_up();

}


TESTMAIN(test_cell_iterator);
