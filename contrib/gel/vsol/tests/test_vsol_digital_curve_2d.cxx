//*****************************************************************************
// File name: test_vsol_digital_curve_2d.cxx
// Description: Test the vsol_digital_curve_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2004/07/13| Matt Leotta          |Creation
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vcl_vector.h>

#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

void test_vsol_digital_curve_2d()
{
  vcl_vector<vsol_point_2d_sptr> samples(5);
  samples[0]=new vsol_point_2d(0.0,0.0);
  samples[1]=new vsol_point_2d(1.0,2.0);
  samples[2]=new vsol_point_2d(2.5,3.5);
  samples[3]=new vsol_point_2d(4.5,3.0);
  samples[4]=new vsol_point_2d(6.0,4.5);

  vsol_digital_curve_2d_sptr dc=new vsol_digital_curve_2d(samples);
  TEST("Constructor", !dc, false);

  TEST("vsol_digital_curve_2d::size()", dc->size(), 5);

  vsol_point_2d_sptr p=dc->point(0);
  TEST("vsol_digital_curve_2d::sample(0)", p->x(), 0.0);
  TEST("vsol_digital_curve_2d::sample(0)", p->y(), 0.0);

  p=dc->point(1);
  TEST("vsol_digital_curve_2d::vertex(1)", p->x(), 1.0);
  TEST("vsol_digital_curve_2d::vertex(1)", p->y(), 2.0);

  p=dc->point(2);
  TEST("vsol_digital_curve_2d::vertex(2)", p->x(), 2.5);
  TEST("vsol_digital_curve_2d::vertex(2)", p->y(), 3.5);

  p=dc->point(3);
  TEST("vsol_digital_curve_2d::vertex(3)", p->x(), 4.5);
  TEST("vsol_digital_curve_2d::vertex(3)", p->y(), 3.0);

  p=dc->point(4);
  TEST("vsol_digital_curve_2d::vertex(4)", p->x(), 6.0);
  TEST("vsol_digital_curve_2d::vertex(4)", p->y(), 4.5);

  vsol_digital_curve_2d_sptr dc2=new vsol_digital_curve_2d(*dc);
  TEST("Copy constructor", !dc2, false);

  TEST("== operator", *dc2, *dc);

  TEST("vsol_digital_curve_2d::interp(2.5)",
       dc->interp(2.5), vgl_point_2d<double>(3.5,3.25));

  TEST("vsol_digital_curve_2d::interp(2.0)",
       dc->interp(2.0), vgl_point_2d<double>(2.5,3.5));

  vcl_cout << "digital curve: " << *dc << vcl_endl;
  vsol_digital_curve_2d_sptr curve1, curve2;
  // Split the curve at a segment
  double index = closest_index(vgl_point_2d<double>(1.5,3.0),dc);
  TEST("closest_index at (1.5,3.0)", index, 1.5);
  bool split_test = split(dc, 1.5, curve1, curve2);
  TEST("split curve at 1.5", split_test && curve1 && curve2, true);
  TEST("split result 1", curve1->point(2)->get_p() == vgl_point_2d<double>(1.75,2.75)
                         && curve1->size() == 3, true);
  TEST("split result 2", curve2->point(0)->get_p() == vgl_point_2d<double>(1.75,2.75)
                         && curve2->size() == 4, true);   
  vcl_cout << "curve 1: " << *curve1 << vcl_endl;
  vcl_cout << "curve 2: " << *curve2 << vcl_endl;

  // Split the curve at a point
  index = closest_index(vgl_point_2d<double>(5.0,2.0),dc);
  TEST("closest_index at (5.0,2.0)", index, 3.0);
  split_test = split(dc, 3.0, curve1, curve2);
  TEST("split curve at 3.0", split_test && curve1 && curve2, true);
  TEST("split result 1", curve1->point(3)->get_p() == vgl_point_2d<double>(4.5,3.0)
                         && curve1->size() == 4, true);
  TEST("split result 2", curve2->point(0)->get_p() == vgl_point_2d<double>(4.5,3.0)
                         && curve2->size() == 2, true); 
  vcl_cout << "curve 1: " << *curve1 << vcl_endl;
  vcl_cout << "curve 2: " << *curve2 << vcl_endl;

  // Split curve at its end points (this should fail)
  index = closest_index(vgl_point_2d<double>(7.0,5.0),dc);
  TEST("closest_index at (7.0,5.0)", index, 4.0);
  split_test = split(dc, 4.0, curve1, curve2);
  TEST("split curve at 4.0 (end)", split_test, false);
  index = closest_index(vgl_point_2d<double>(0.0,-1.0),dc);
  TEST("closest_index at (0.0,-1.0)", index, 0.0);
  split_test = split(dc, 0.0, curve1, curve2);
  TEST("split curve at 0.0 (start)", split_test, false);

}

TESTMAIN(test_vsol_digital_curve_2d);
