#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <vgl/vgl_box_3d.h>


static void test_locate_region()
{
  short nlevels=10;
  auto* block=new boct_tree<short,vgl_point_3d<double> >(nlevels,5);
  vgl_box_3d<double> box(vgl_point_3d<double>(0.1,0.1,0.1),vgl_point_3d<double>(0.9,0.9,0.9));
  boct_tree_cell<short,vgl_point_3d<double> >* cell=block->locate_region(box);
  TEST("Returns the correct level",9, cell->level());

  // two layer tree;
  block->split();
  vgl_box_3d<double> box1(vgl_point_3d<double>(0.05,0.05,0.05),vgl_point_3d<double>(0.1,0.1,0.1));
  cell=block->locate_region(box1);
  TEST("Returns the correct level",6, cell->level());
}

TESTMAIN(test_locate_region);
