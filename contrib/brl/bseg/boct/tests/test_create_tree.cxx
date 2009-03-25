
#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <vnl/vnl_random.h>
#include <boct/boct_test_util.h>
//void create_random_configuration_tree(boct_tree* tree)
//{
//    for(unsigned int i=0;i<10;i++)
//    {
//        vnl_random rand;
//        vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
//        boct_tree_cell * curr_cell=tree->locate_point(p);
//        if(curr_cell && curr_cell->level()>0)
//            curr_cell->split();
//    }
//}

MAIN( test_create_tree )
{
  START ("CREATE TREE");
  short nlevels=5;
  boct_tree * block=new boct_tree(nlevels);
  TEST("No of Max levels of tree",nlevels, block->num_levels());
  
  block->split();
  block->print();

  vcl_vector<boct_tree_cell*> leaves = block->leaf_cells();
  TEST("No of Leaf Cells", 8, leaves.size());
  for (unsigned i=0; i<leaves.size(); i++)
    leaves[i]->print();

  boct_tree * randomtree=new boct_tree(3);
  create_random_configuration_tree(randomtree);

  randomtree->print();

  SUMMARY();

  
}
