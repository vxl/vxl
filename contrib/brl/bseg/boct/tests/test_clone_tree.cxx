
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>

static void test_clone_tree()
{
  auto *tree = new boct_tree<short,vgl_point_3d<double> >(5, 3);
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves = tree->leaf_cells();
  TEST("No of Leaf Cells after 3 levels", 8*8, leaves.size());
  tree->init_cells(vgl_point_3d<double>(1,1,1));

  // clone tree
  boct_tree<short,vgl_point_3d<double> > *cloned = tree->clone();
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> cloned_leaves = cloned->leaf_cells();
  TEST("No of Leaf Cells of cloned", 8*8, cloned_leaves.size());

  // compare the codes
  bool eq=true;
  for (unsigned i=0; i<cloned_leaves.size(); ++i) {
    if (!cloned_leaves[i]->code_.isequal(&(leaves[i]->code_)))
      eq=false;
  }
  TEST("Leaf nodes' Code equality", eq, true);

  delete tree;
  delete cloned;
}

TESTMAIN(test_clone_tree);
