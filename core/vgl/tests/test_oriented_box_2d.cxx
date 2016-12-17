// Some tests for vgl_oriented_box
// J.L. Mundy Dec. 2016
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_compiler.h>

static void test_oriented_box_2d()
{
  //test constructors and corner accessor
  vgl_point_2d<double> pmaj1(0.0, 100.0), pmaj2(200.0, 100.0);
  vgl_point_2d<double> pmin1(50.0, 150.0), pmin2(50.0, 0.0);
  vgl_oriented_box_2d<double> ob(pmaj1,pmaj2, pmin1, pmin2);
  std::vector<vgl_point_2d<double> > corns0 = ob.corners();
  vgl_point_2d<double> pmaj11(50.0, 50.0), pmaj21(150.0, 50.0);
  vgl_point_2d<double> pmin11(100.0, 75.0), pmin21(100.0, 25.0);
  vgl_oriented_box_2d<double> ob1(pmaj11,pmaj21, pmin11, pmin21);
  std::vector<vgl_point_2d<double> > corns1 = ob1.corners();
  
  double iou = obox_int_over_union(ob, ob1);
  TEST("intersection over union", iou, 1.0/6.0);

  vgl_oriented_box_2d<double> tbox1 = translate_obox(ob1, vgl_vector_2d<double>(200.0, 50.0));
  std::vector<vgl_point_2d<double> > trans_corns1 = tbox1.corners();
  TEST("translate obox", trans_corns1[2].x()+trans_corns1[2].y(), 475.0);

  vgl_point_2d<double> cent = tbox1.center();
  TEST("center", cent.x()+cent.y(), 400.0);

  vgl_oriented_box_2d<double> rtbox1 = tbox1.rotate(cent, 3.1415926535897/2.0);
  std::vector<vgl_point_2d<double> > rot_trans_corns1 = rtbox1.corners();
  TEST_NEAR("rotate box", rot_trans_corns1[2].x()+rot_trans_corns1[2].y(), 425.0, 0.000001);

  vgl_box_2d<double> bb = rtbox1.bounding_box();
  double area = bb.area();
  TEST_NEAR("bounding box", area, 5000.0, 0.000001);
  std::stringstream ss;
  ob.write(ss);
  vgl_oriented_box_2d<double> ibox;
  ibox.read(ss);
  bool eq = ibox == ob;
  TEST("read/write equal", eq, true);
}

TESTMAIN(test_oriented_box_2d);
