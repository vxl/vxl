#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_bounding_box.h>
#include <testlib/testlib_test.h>

static
void vbl_test_bounding_box()
{
  std::cout << "\n\n\n"
           << "***********************\n"
           << " Test vbl_bounding_box\n"
           << "***********************\n";
  vbl_bounding_box<double,3> bb;
  TEST("bounding box dimension", bb.dimension(), 3);
  bb.update(-3.0,4.0,5.0);
  bb.update(3.0,-4.0,5.0);
  bb.update(3.0,4.0,-5.0);
  TEST("bounding box min_x", bb.min()[0], -3.0);
  TEST("bounding box min_y", bb.min()[1], -4.0);
  TEST("bounding box min_z", bb.min()[2], -5.0);
  TEST("bounding box max_x", bb.max()[0], 3.0);
  TEST("bounding box max_y", bb.max()[1] ,4.0);
  TEST("bounding box max_z", bb.max()[2] ,5.0);
  TEST("bounding box volume", bb.volume() ,480.0);
  bb.reset();
  TEST("bounding box volume", bb.volume() ,0.0);
  bb.update(3.0,-4.0,5.0);
  TEST("bounding box min_x", bb.min()[0] ,3.0);
  TEST("bounding box min_y", bb.min()[1] ,-4.0);
  TEST("bounding box min_z", bb.min()[2] ,5.0);
  TEST("bounding box max_x", bb.max()[0] ,3.0);
  TEST("bounding box max_y", bb.max()[1] ,-4.0);
  TEST("bounding box max_z", bb.max()[2] ,5.0);
  TEST("bounding box volume", bb.volume() ,0.0);
}

TESTMAIN(vbl_test_bounding_box);
