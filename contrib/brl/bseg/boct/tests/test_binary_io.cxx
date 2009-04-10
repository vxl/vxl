#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_test_util.h>
#include <vgl/vgl_point_3d.h>
#include <vpl/vpl.h>

MAIN( test_binary_io )
{
  START ("Binary Write-Read");

  boct_tree<short,vgl_point_3d<double>,void> *tree_in=new boct_tree<short,vgl_point_3d<double>,void >(6,3);
  tree_in->split();
  vcl_vector<boct_tree_cell<short,vgl_point_3d<double>,void >*> leaves = tree_in->leaf_cells();
  int num_leaves = leaves.size();
  for (unsigned i=0; i<leaves.size(); i++) {
    boct_tree_cell<short,vgl_point_3d<double>,void >* leaf = static_cast<boct_tree_cell<short,vgl_point_3d<double>,void >*>(leaves[i]);
    leaf->set_data(vgl_point_3d<double>(i,i,i));
  }

  //tree_in->print();
  vsl_b_ofstream os("tree.bin");
  tree_in->b_write(os);
  os.close();

  boct_tree<short,vgl_point_3d<double>,void > *tree_out=new boct_tree<short,vgl_point_3d<double>,void >();
  vsl_b_ifstream is("tree.bin", vcl_ios_binary);
  tree_out->b_read(is);
  TEST("Returns the correct level", tree_out->num_levels(), tree_in->num_levels());
  //tree_out->print();
  leaves.empty();
  leaves = tree_out->leaf_cells();
  TEST("No of Leaf Cells", num_leaves, leaves.size());

  bool same=true;
  for (unsigned i=0; i<leaves.size(); i++) {
    boct_tree_cell<short,vgl_point_3d<double>,void >* leaf = static_cast<boct_tree_cell<short,vgl_point_3d<double>,void >*>(leaves[i]);
    vgl_point_3d<double> data = leaf->data();
    if (data != vgl_point_3d<double>(i,i,i))
      same=false;
  }
  TEST("The data at the leaf nodes are the same", same, true);
  vpl_unlink("tree.bin");
  SUMMARY();
}