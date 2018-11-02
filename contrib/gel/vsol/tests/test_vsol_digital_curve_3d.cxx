// This is gel/vsol/tests/test_vsol_digital_curve_3d.cxx
// \author Peter Vanroose
// \date 13 November 2004
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_digital_curve_3d.h>
#include <vsol/vsol_digital_curve_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

void test_vsol_digital_curve_3d()
{
  std::vector<vsol_point_3d_sptr> samples(5);
  samples[0]=new vsol_point_3d(0.0,0.0,0.0);
  samples[1]=new vsol_point_3d(1.0,2.0,3.0);
  samples[2]=new vsol_point_3d(2.5,3.5,4.5);
  samples[3]=new vsol_point_3d(4.5,3.0,-1.0);
  samples[4]=new vsol_point_3d(6.0,4.5,0.0);

  vsol_digital_curve_3d_sptr dc=new vsol_digital_curve_3d(samples);
  TEST("Constructor", !dc, false);

  TEST("vsol_digital_curve_3d::size()", dc->size(), 5);

  vsol_point_3d_sptr p=dc->point(0);
  TEST("vsol_digital_curve_3d::point(0)", p->x(), 0.0);
  TEST("vsol_digital_curve_3d::point(0)", p->y(), 0.0);
  TEST("vsol_digital_curve_3d::point(0)", p->z(), 0.0);

  p=dc->point(3);
  TEST("vsol_digital_curve_3d::point(3)", p->x(), 4.5);
  TEST("vsol_digital_curve_3d::point(3)", p->y(), 3.0);
  TEST("vsol_digital_curve_3d::point(3)", p->z(), -1.0);

  vsol_digital_curve_3d_sptr dc2=new vsol_digital_curve_3d(*dc);
  TEST("Copy constructor", !dc2, false);

  TEST("== operator", *dc2, *dc);

  TEST("vsol_digital_curve_3d::interp(2.5)",
       dc->interp(2.5), vgl_point_3d<double>(3.5,3.25,1.75));

  TEST("vsol_digital_curve_3d::interp(2.0)",
       dc->interp(2.0), vgl_point_3d<double>(2.5,3.5,4.5));

  std::cout << "digital curve: " << *dc << std::endl;
  vsol_digital_curve_3d_sptr curve1, curve2;
  // Split the curve at a segment
  double index = closest_index(vgl_point_3d<double>(1.5,3.0,4.0),dc);
  TEST_NEAR("closest_index at (1.5,3.0,4.0)", index, 14.0/9, 1e-12);
  bool split_test = split(dc, 1.5, curve1, curve2);
  TEST("split curve at 1.5", split_test && curve1 && curve2, true);
  std::cout << "curve 1: " << *curve1 << std::endl
           << curve1->point(2)->get_p() << std::endl;
  TEST("split result 1", curve1->point(2)->get_p() == vgl_point_3d<double>(1.75,2.75,3.75)
                         && curve1->size() == 3, true);
  std::cout << "curve 2: " << *curve2 << std::endl
           << curve2->point(0)->get_p() << std::endl;
  TEST("split result 2", curve2->point(0)->get_p() == vgl_point_3d<double>(1.75,2.75,3.75)
                         && curve2->size() == 4, true);

  // Split the curve at a point
  index = closest_index(vgl_point_3d<double>(5.0,2.0,-1.0),dc);
  std::cout << index << std::endl;
  TEST("closest_index at (5.0,2.0,-1)", index, 3.0);
  split_test = split(dc, index, curve1, curve2);
  TEST("split curve at this index", split_test && curve1 && curve2, true);
  std::cout << "curve 1: " << *curve1 << std::endl;
  TEST("split result 1", curve1->point(3)->get_p() == vgl_point_3d<double>(4.5,3.0,-1.0)
                         && curve1->size() == 4, true);
  std::cout << "curve 2: " << *curve2 << std::endl;
  TEST("split result 2", curve2->point(0)->get_p() == vgl_point_3d<double>(4.5,3.0,-1.0)
                         && curve2->size() == 2, true);

  // Split curve at its end points (this should fail)
  index = closest_index(vgl_point_3d<double>(7.0,5.0,1.0),dc);
  std::cout << index << std::endl;
  TEST("closest_index at (7.0,5.0,1.0)", index, 4.0);
  split_test = split(dc, index, curve1, curve2);
  TEST("split curve at 4.0 (end)", split_test, false);
  index = closest_index(vgl_point_3d<double>(0.0,-1.0,-2.0),dc);
  std::cout << index << std::endl;
  TEST("closest_index at (0.0,-1.0)", index, 0.0);
  split_test = split(dc, index, curve1, curve2);
  TEST("split curve at 0.0 (start)", split_test, false);
}

TESTMAIN(test_vsol_digital_curve_3d);
