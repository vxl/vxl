
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>

MAIN( test_create_tree )
{
  START ("CREATE TREE");
  short nlevels=5;
  boct_tree<short,vgl_point_3d<double> > * tree=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  TEST("No of Max levels of tree",nlevels, tree->number_levels());

  tree->split(); 

  vcl_vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves = tree->leaf_cells();
  TEST("No of Leaf Cells", 8, leaves.size());
  //for (unsigned i=0; i<leaves.size(); i++)
  //  leaves[i]->print();


  boct_tree<short,vgl_point_3d<double> > *init_tree = new boct_tree<short,vgl_point_3d<double> >(5, 3);
  vcl_vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves2 = init_tree->leaf_cells();
  TEST("No of Leaf Cells after 3 levels", 8*8, leaves2.size());
  //init_tree->print();
  
  // TEST THE TREE CREATION FROM THE LEAF NODES
  vcl_vector<boct_tree_cell<short,vgl_point_3d<double> > > leaf_nodes;
  for (unsigned i=0; i<leaves2.size(); i++) {
    leaf_nodes.push_back(*leaves2[i]);
    leaf_nodes[i].set_data(vgl_point_3d<double>(i,i,i));
  }
    
  boct_tree_cell<short,vgl_point_3d<double> > *root = init_tree->construct_tree(leaf_nodes, init_tree->root_level() + 1);
  boct_tree<short,vgl_point_3d<double> > new_tree(root, init_tree->number_levels());
  vcl_vector<boct_tree_cell<short,vgl_point_3d<double> >*> new_leaves=new_tree.leaf_cells();
  bool good=true;
  for (unsigned i=0; i<leaves2.size(); i++) {
    if (!new_leaves[i]->code_.isequal(&leaf_nodes[i].code_))
      good=false;
    if (new_leaves[i]->data() != leaf_nodes[i].data())
      good=false;
  }
  TEST("Constructiong a tree from leaf nodes", true, good);
  //new_tree.print();

  delete tree;
  delete init_tree;
  SUMMARY();
}
