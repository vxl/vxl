
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>

static void test_create_tree()
{
  short nlevels=5;
  auto * tree=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  TEST("No of Max levels of tree",nlevels, tree->number_levels());

  tree->split();
  tree->init_cells(vgl_point_3d<double>(0,0,0));
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves = tree->leaf_cells();
  TEST("No of Leaf Cells", 8, leaves.size());

  auto *init_tree = new boct_tree<short,vgl_point_3d<double> >(5, 3);
  tree->init_cells(vgl_point_3d<double>(1,2,3));
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves2 = init_tree->leaf_cells();
  TEST("No of Leaf Cells after 3 levels", 8*8, leaves2.size());

  // Test the tree creation from the fleaf nodes
  std::vector<boct_tree_cell<short,vgl_point_3d<double> > > leaf_nodes;
  for (unsigned i=0; i<leaves2.size(); i++) {
    leaf_nodes.push_back(*leaves2[i]);
    leaf_nodes[i].set_data(vgl_point_3d<double>(i,i,i));
  }

  boct_tree_cell<short,vgl_point_3d<double> > *root = init_tree->construct_tree(leaf_nodes, init_tree->root_level() + 1);
  boct_tree<short,vgl_point_3d<double> > new_tree(root, init_tree->number_levels());
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> new_leaves=new_tree.leaf_cells();
  bool good=true;
  for (unsigned i=0; i<leaves2.size(); i++) {
    if (!new_leaves[i]->code_.isequal(&leaf_nodes[i].code_))
      good=false;
    if (new_leaves[i]->data() != leaf_nodes[i].data())
      good=false;
  }
  TEST("Constructiong a tree from leaf nodes", true, good);
  delete tree;
  delete init_tree;

  // Test the tree insertion
  // create the main tree
  tree=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  tree->split();
  tree->init_cells(vgl_point_3d<double>(0,0,0));

  //create the tree to be inserted
  auto * subtree=new boct_tree<short,vgl_point_3d<double> >(3, 3);
  subtree->init_cells(vgl_point_3d<double> (10,10,10) );
  boct_tree_cell<short,vgl_point_3d<double> > *node = tree->leaf_cells()[3];
  boct_tree_cell<short,vgl_point_3d<double> > *sub_root = subtree->root();
  int before = tree->leaf_cells().size();
  int sub_count=subtree->leaf_cells().size();
  node->insert_subtree(sub_root);

  leaves.clear();
  leaves = tree->leaf_cells();
  TEST_EQUAL("Inserting the subtree, leaf nodes count", before+sub_count-1, leaves.size());
  TEST("Inserting the subtree, leaf nodes data at index 3", leaves[3]->data(), vgl_point_3d<double> (10,10,10));
  TEST("Inserting the subtree, leaf nodes data at index 67", leaves[67]->data(), vgl_point_3d<double> (0,0,0));

  delete tree;
  delete subtree;
}

TESTMAIN(test_create_tree);
