#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <vgl/vgl_box_3d.h>


MAIN( test_locate_region )
{
  START ("Locate Region");
  short nlevels=10;
  boct_tree * block=new boct_tree(nlevels);
  vgl_box_3d<double> box(vgl_point_3d<double>(0.1,0.1,0.1),vgl_point_3d<double>(0.9,0.9,0.9));
  boct_tree_cell* cell=block->locate_region(box);
  TEST("Returns the correct level",nlevels-1, cell->level());
  SUMMARY();

  
}