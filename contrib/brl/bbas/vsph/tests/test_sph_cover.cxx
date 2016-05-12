#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_segment_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_sph_cover_2d.h>
#include <vsph/vsph_utils.h>
#include <vnl/vnl_math.h>

static void test_sph_cover()
{
  // construct a cover with two regions
  vsph_sph_point_2d p10(80.0, 0.0, false);
  vsph_sph_point_2d p11(110.0,10.0, false);
  vsph_sph_point_2d p12(80.0, 60.0, false);
  vsph_sph_point_2d p13(110.0, 110.0, false);
  vsph_sph_point_2d p19(80.0, -10.0, false);
  vsph_sph_box_2d bb_a(p10, p12, p11);
  vsph_sph_box_2d bb_b(p19, p13, p11);
  double area_a = bb_a.area(), area_b = bb_b.area();
  std::vector<vsph_sph_box_2d> boxes_a, boxes_b, boxes_int;
  bb_a.sub_divide(boxes_a);
  bb_b.sub_divide(boxes_b);
  std::vector<cover_el> cels_a, cels_b;
  for (std::vector<vsph_sph_box_2d>::iterator bit = boxes_a.begin();
       bit != boxes_a.end(); ++bit)
    cels_a.push_back(cover_el(*bit, 1.0));
  for (std::vector<vsph_sph_box_2d>::iterator bit = boxes_b.begin();
       bit != boxes_b.end(); ++bit)
    cels_b.push_back(cover_el(*bit, 1.0));
  vsph_sph_cover_2d cov_a, cov_b, cov_int;
  cov_a.set(1.0, area_a, 1.0, cels_a);
  cov_b.set(1.0, area_b, 1.0, cels_b);
 intersection(cov_a, cov_b, cov_int);
 intersection(bb_a, bb_b, boxes_int);
 double orig_int_area = 0.0;
 for (std::vector<vsph_sph_box_2d>::iterator iit = boxes_int.begin();
      iit != boxes_int.end(); ++iit)
 orig_int_area += (*iit).area();
 double cover_int_area = cov_int.area();
 TEST_NEAR("intersection of disjoint covers", orig_int_area, cover_int_area, 0.001);
 double cover_inter_area = intersection_area(cov_a, cov_b);
 TEST_NEAR("intersection area of two covers", orig_int_area, cover_inter_area, 0.001);

}

TESTMAIN(test_sph_cover);
