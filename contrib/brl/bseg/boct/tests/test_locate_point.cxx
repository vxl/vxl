#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>



MAIN( test_locate_point )
{
  START ("Locate Point");
  short nlevels=10;
  boct_tree * block=new boct_tree(nlevels);
  vgl_point_3d<double> p(0.1,0.1,0.1);
  boct_tree_cell* cell=block->locate_point(p);
  TEST("Returns the correct level",nlevels-1, cell->level());
  SUMMARY();

  
}