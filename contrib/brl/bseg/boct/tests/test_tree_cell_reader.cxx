
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_tree_cell_reader.h>

MAIN( test_tree_cell_reader )
{
  START ("TREE CELL READER");
  short nlevels=5;
  boct_tree<short,float> * block=new boct_tree<short,float>(nlevels,3);
  TEST("No of Max levels of tree",nlevels, block->num_levels());

  block->split();

  vcl_vector<boct_tree_cell<short,float>*> leaves = block->leaf_cells();
  for (unsigned i=0; i<leaves.size(); i++) {
    leaves[i]->set_data((float)i);
  }
 
  vsl_b_ofstream os("tree.bin");
  block->b_write(os);
  os.close();

  vsl_b_ifstream is("tree.bin", vcl_ios_binary);
  boct_tree_cell_reader<short,float> reader("tree.bin");//&is);
  boct_tree_cell<short,float> cell;
  vcl_vector<boct_tree_cell<short,float> > cells;
  reader.begin();
  while (reader.next(cell)) {
    cells.push_back(cell);
  }
  
  TEST("No of Leaf Cells after reading", cells.size(), leaves.size());
  delete block;
  SUMMARY();
}
