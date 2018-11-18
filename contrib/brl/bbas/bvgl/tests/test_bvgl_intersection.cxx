//:
// \file
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test changes
static void test_bvgl_intersection()
{
  //1 x 1 x 1 box at the origin
  vgl_box_3d<double> box(0,0,0, 1,1,1);

  //INSIDE test
  vgl_point_3d<double> ptA(.2, .2, .2),
                       ptB(.3, .3, .3),
                       ptC(.7, .7, .7);
  bvgl_triangle_3d<double> inside(ptA, ptB, ptC);
  TEST("Test triangle inside box: ", bvgl_intersection(box, inside), true);

  //Outside test, no intersection
  ptA.set(2, 2, 2);
  ptB.set(4, 5, 2);
  ptC.set(-1, 4, 2);
  bvgl_triangle_3d<double> outside(ptA, ptB, ptC);
  TEST("Test triangle outside box: ", bvgl_intersection(box, outside), false);

  //test points outside, slices box
  ptA.set(2, 0, .5);
  ptB.set(-2, 0, .5);
  ptC.set(0, -1, .5);
  bvgl_triangle_3d<double> slice(ptA, ptB, ptC);
  TEST("Test triangle slicing box: ", bvgl_intersection(box, slice), true);

  //test points outside box range, still slices
  ptA.set(-1, -1, -1);
  ptB.set( 2,  2,  2);
  ptC.set( 0,  2,  2);
  bvgl_triangle_3d<double> slice2(ptA, ptB, ptC);
  TEST("Test triangle slice2 box: ", bvgl_intersection(box, slice2), true);

  //test near intersection
  ptA.set(-.5, -.5, -.5);
  ptB.set(-.5,   0,   0);
  ptC.set(-.01, .5,   1);
  bvgl_triangle_3d<double> outside2(ptA, ptB, ptC);
  TEST("Test triangle near miss: ", bvgl_intersection(box, outside2), false);

  //test same plane
  ptA.set(.4, -.5, .5);
  ptB.set(-.5, .4, .5);
  ptC.set(-.5, -.5, .5);
  bvgl_triangle_3d<double> samePlane(ptA, ptB, ptC);
  TEST("Test triangle same plane miss X: ", bvgl_intersection(box, samePlane), false);

  //test same plane again
  ptA.set( .4, .5, -.5);
  ptB.set(-.5, .5,  .4);
  ptC.set(-.5, .5, -.5);
  bvgl_triangle_3d<double> spY(ptB, ptA, ptC);
  TEST("Test triangle same plane miss Y: ", bvgl_intersection(box, spY), false);

  //test same plane again
  ptA.set(.5,  .4, -.5);
  ptB.set(.5, -.5,  .4);
  ptC.set(.5, -.5, -.5);
  bvgl_triangle_3d<double> spZ(ptC, ptB, ptA);
  TEST("Test triangle same plane miss Z: ", bvgl_intersection(box, spZ), false);




}

TESTMAIN( test_bvgl_intersection );
