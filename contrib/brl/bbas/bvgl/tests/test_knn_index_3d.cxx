//:
// \file
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_cross_section.h>
#include <bvgl/bvgl_knn_index_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#define TEST_KNN_INDEX 1
//: Test changes
static void test_knn_index_3d()
{
#if TEST_KNN_INDEX
  vgl_point_3d<double> p0(0.0, 0.0, 0.0), p1(1.0, 0.0, 0.0), p2(0.0, 1.0, 0.0), p3(0.0, 0.0, 1.0);
  vgl_point_3d<double> p4(1.0, 0.0, 1.0), p5(0.0, 1.0, 1.0), p6(1.0, 1.0, 1.0), p7(0.5, 0.75, 0.5);
  vgl_vector_3d<double> n(0.577, 0.577, 0.577);
  vgl_pointset_3d<double> ptset;
  ptset.add_point_with_normal(p0, n);  ptset.add_point_with_normal(p1, n);
  ptset.add_point_with_normal(p2, n);  ptset.add_point_with_normal(p3, n);
  ptset.add_point_with_normal(p4, n);  ptset.add_point_with_normal(p5, n);
  ptset.add_point_with_normal(p5, n);  ptset.add_point_with_normal(p6, n); ptset.add_point_with_normal(p7, n);
  bvgl_knn_index_3d<double> knn_index(ptset);
  vgl_point_3d<double> p(0.4, 0.7, 0.6), pc;
  knn_index.closest_point(p, pc);
  TEST_NEAR("grid closest point", (p-pc).length(), 0.0, 0.05);
#endif
}

TESTMAIN( test_knn_index_3d );
