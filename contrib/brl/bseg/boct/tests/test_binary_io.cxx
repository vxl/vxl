#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_test_util.h>
#include <vgl/vgl_point_3d.h>
#include <vpl/vpl.h>

static void test_binary_io()
{
  auto *tree_in=new boct_tree<short,vgl_point_3d<double> >(6,3);
  tree_in->split();
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves = tree_in->leaf_cells();
  unsigned int num_leaves = 0;
  for (unsigned i=0; i<leaves.size(); ++i) {
    auto* leaf = static_cast<boct_tree_cell<short,vgl_point_3d<double> >*>(leaves[i]);
    if (leaf) { ++num_leaves; leaf->set_data(vgl_point_3d<double>(i,i,i)); }
  }

  //tree_in->print();
  vsl_b_ofstream os("tree.bin");
  bool save_internal_nodes = false;
  tree_in->b_write(os, save_internal_nodes);
  os.close();

  auto *tree_out=new boct_tree<short,vgl_point_3d<double> >();
  vsl_b_ifstream is("tree.bin", std::ios::binary);
  tree_out->b_read(is);
  TEST("Returns the correct level", tree_out->number_levels(), tree_in->number_levels());
  //tree_out->print();
  leaves.empty();
  leaves = tree_out->leaf_cells();
  TEST("No of Leaf Cells", num_leaves, leaves.size());

  bool same=true;
  for (unsigned i=0; i<leaves.size(); i++) {
    auto* leaf = static_cast<boct_tree_cell<short,vgl_point_3d<double> >*>(leaves[i]);
    vgl_point_3d<double> data = leaf->data();
    if (data != vgl_point_3d<double>(i,i,i))
      same=false;
  }
  TEST("The data at the leaf nodes are the same", same, true);
  vpl_unlink("tree.bin");
}

TESTMAIN(test_binary_io);
