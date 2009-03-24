#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>


MAIN( test_find_neighbors )
{
  START ("Find Neighbors");
  short nlevels=10;
  boct_tree * block=new boct_tree(nlevels);
  
  //: two layer tree;
  block->split();
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell* cell=block->locate_point(p1);

  vcl_vector<boct_tree_cell*> n;
  cell->find_neighbors(boct_tree_cell::X_HIGH,n,10);

  //: ground truth for the code of the neighbor 
  boct_loc_code gt_code;
  gt_code.set_code((cell->get_code().x_loc_|1<<(cell->level())),cell->get_code().y_loc_, cell->get_code().z_loc_);
  
  TEST("Returns the correct  Neighbor for X_HIGH",gt_code.x_loc_,n[0]->get_code().x_loc_);
  //TEST("Returns the correct Neighbor",1,n.size());


  SUMMARY();

  
}