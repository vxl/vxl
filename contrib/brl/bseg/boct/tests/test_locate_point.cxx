#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>



MAIN( test_locate_point )
{
  START ("Locate Point");
  short nlevels=10;
  boct_tree * simpleblock=new boct_tree(nlevels);
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell* cell=simpleblock->locate_point(p1);
  TEST("Returns the correct level",nlevels-1, cell->level());
  
  boct_tree * twolevelblock=new boct_tree(nlevels);
  twolevelblock->split();
  
  vgl_point_3d<double> p2(0.1,0.1,0.1);
  cell=twolevelblock->locate_point(p2);
  TEST("Returns the correct level",nlevels-2, cell->level());
  
  
  SUMMARY();

  
}