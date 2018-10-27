
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_tree_cell_reader.h>

#include <vgl/io/vgl_io_box_3d.h>

static void test_tree_cell_reader()
{
  short nlevels=5;
  auto * block=new boct_tree<short,float>(nlevels,3);
  TEST("No of Max levels of tree",nlevels, block->number_levels());

  block->split();

  std::vector<boct_tree_cell<short,float>*> leaves = block->leaf_cells();
  for (unsigned i=0; i<leaves.size(); i++) {
    leaves[i]->set_data((float)i);
  }

  vsl_b_ofstream os("tree.bin");
  // mimic writing the block info first, then the tree
  vsl_b_write(os, 1);  //version
  vgl_box_3d<double> bbox;
  vsl_b_write(os, bbox);  //global bbox of the block
  bool save_internal_nodes = false;
  vsl_b_write(os,save_internal_nodes, *block);
  os.close();

  vsl_b_ifstream is("tree.bin", std::ios::binary);
  boct_tree_cell_reader<short,float> reader("tree.bin");//&is);
  boct_tree_cell<short,float> cell;
  std::vector<boct_tree_cell<short,float> > cells;
  reader.begin();
  while (reader.next(cell)) {
    cells.push_back(cell);
  }

  TEST("No of Leaf Cells after reading", cells.size(), leaves.size());
  delete block;
}

TESTMAIN(test_tree_cell_reader);
