
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <vnl/vnl_random.h>

MAIN( test_create_tree )
{
  START ("CREATE TREE");
  short nlevels=5;
  boct_tree<vgl_point_3d<double> > * block=new boct_tree<vgl_point_3d<double> >(nlevels);
  TEST("No of Max levels of tree",nlevels, block->num_levels());
  
  block->split();
  block->print();

  vsl_b_ofstream os("C:\\test_images\\octree\\tree.bin");
  block->b_write(os);

  boct_tree<vgl_point_3d<double> > *tree=new boct_tree<vgl_point_3d<double> >();
  vsl_b_ifstream is("C:\\test_images\\octree\\tree.bin");
  tree->b_read(is);
  tree->print();

  vcl_vector<boct_tree_cell_base_sptr> leaves = block->leaf_cells();
  TEST("No of Leaf Cells", 8, leaves.size());
  for (unsigned i=0; i<leaves.size(); i++)
    leaves[i]->print();


  boct_tree<vgl_point_3d<double> > *init_tree = new boct_tree<vgl_point_3d<double> >(5, 3);
  vcl_vector<boct_tree_cell_base_sptr> leaves2 = init_tree->leaf_cells();
  TEST("No of Leaf Cells after 3 levels", 8*8, leaves2.size());
  init_tree->print();

  SUMMARY();

  
}
