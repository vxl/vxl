#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_rtree.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <vnl/vnl_random.h>
#include <testlib/testlib_test.h>


static void test_point_box()
{
  //
  // construct rtree
  // An rtree takes three template arguments:
  // V the element to be stored at the leaves of the tree, e.g. points
  // B the bounds of a region, e.g. a box
  // C a helper class that determines bounding predicates so that
  //   V and B can remain unassociated.
  //
  typedef vgl_rtree_point_box_2d<float> C_; // the helper class
  typedef C_::v_type V_; // the contained object type
  typedef C_::b_type B_; // the bounding object type
  std::cout << "\n<<<<<<<   test point_box tree >>>>>>>>>>>>>>\n";
  vgl_rtree<V_, B_, C_> tr; // the rtree

  // insert  points
  vgl_point_2d<float> p0(0.0f, 0.0f), p1(1.0f,0.0f),
    p2(1.0f, 1.0f), p3(0.5f, 0.5f), p4(0.65f, 0.65f);
  tr.add(p0);   tr.add(p1);   tr.add(p2);   tr.add(p3); tr.add(p4);

  // test contains method
  bool p1_in = tr.contains(p1);
  TEST("test contains", p1_in, true);

  // test region search
  vgl_box_2d<float> bb;
  vgl_point_2d<float> pb0(0.25f, 0.25f), pb1(0.75f, 0.75f);
  bb.add(pb0); bb.add(pb1);
  std::vector<vgl_point_2d<float> > found;
  tr.get(bb, found);
  unsigned n = found.size();
  for (unsigned i = 0; i<n; ++i)
    std::cout << "point(s) in region " << found[i] << '\n';;
  if (n==1)
    TEST("test region search",found[0] , p3);
  //test iterator
  std::cout << "Traversing point_box rtree, num nodes = "<< tr.nodes()<< '\n';
  unsigned j = 0;
  vgl_rtree<V_, B_, C_>::iterator ti = tr.begin();
  for (; ti!= tr.end(); ++ti, ++j)
    std::cout << "tr[" << j << "] = " << *ti << '\n';
  TEST("test iterator", *tr.begin(),p0);

  //test polygon probe
  //a rectangle at 45 degrees (oriented box)
  vgl_point_2d<float> pr0(0.3f, 0.7f), pr1(0.7f, 0.3f), pr2(0.5f, 0.9f),
    pr3(0.9f, 0.5f);
  vgl_polygon<float> poly(1);
  poly.push_back(pr0); poly.push_back(pr1);
  poly.push_back(pr2); poly.push_back(pr3);
  vgl_rtree_polygon_probe<V_, B_, C_> probe(poly);
  found.clear();
  tr.get(probe, found);
  n = found.size();
  std::cout << "Polygon point probe found " << n << " points\n";
  for (unsigned i = 0; i<n; ++i)
    std::cout << "point(s) meeting point probe " << found[i] << '\n';;
  TEST("number found by poly point probe", n, 2);

  //test large numbers of point insertions
  vgl_rtree<V_, B_, C_> tr_big;
  vnl_random r;
  unsigned ni = 1000;
  for (unsigned i = 0; i<ni; ++i)
  {
    auto x = static_cast<float>(r.drand32(0.0,1.0));
    auto y = static_cast<float>(r.drand32(0.0,1.0));
    vgl_point_2d<float> p(x,y);
    tr_big.add(p);
  }
  std::cout << "Big rtree num nodes = "<< tr_big.nodes()<< '\n';
  vgl_point_2d<float> plow(0.25f, 0.25f);
  vgl_point_2d<float> phigh(0.5f, 0.5f);
  vgl_box_2d<float> bbig;
  bbig.add(plow);   bbig.add(phigh);
  std::vector<vgl_point_2d<float> > big_find;
  tr_big.get(bbig, big_find);
  n = big_find.size();
  std::cout << "Found " << n << " points in " << bbig << '\n';
  TEST("Large number of points", n>0, true);
}

static void test_box_box()
{
  //
  // construct rtree
  // An rtree takes three template arguments:
  // V the element to be stored at the leaves of the tree, e.g. points
  // B the bounds of a region, e.g. a box
  // C a helper class that determines bounding predicates so that
  //   V and B can remain unassociated.
  //
  typedef vgl_rtree_box_box_2d<float> C_; // the helper class
  typedef C_::v_type V_; // the contained object type
  typedef C_::b_type B_; // the bounding object type
  std::cout << "\n<<<<<<<   test box_box tree >>>>>>>>>>>>>>\n";
  vgl_rtree<V_, B_, C_> tr; // the rtree

  // points
  vgl_point_2d<float> p0(0.0f, 0.0f), p1(1.0f, 0.0f),
                      p2(1.0f, 1.0f), p3(0.45f,0.45f), p4(0.91f, 0.91f),
                      p5(0.25f,0.25f),p6(0.50f,0.50f);

  vgl_box_2d<float> bx1, bx2, bx3, bx4, bx5;
  bx1.add(p0); bx2.add(p2); bx2.add(p3); bx3.add(p1);
  bx4.add(p4); bx4.add(p5); bx5.add(p6);
  tr.add(bx1); tr.add(bx2); tr.add(bx3); tr.add(bx4); tr.add(bx5);

  vgl_bbox_2d<float> bb;
  bb.add(p0);   bb.add(p3);
  std::vector<vgl_box_2d<float> > box_find;
  tr.get(bb, box_find);
  unsigned n = box_find.size();
  std::cout << " boxes meeting " << bb << '\n';
  for (unsigned i = 0; i<n; ++i)
    std::cout << "find[" << i << "]= " << box_find[i] << '\n';
  TEST("box region query", n>0, true);
  if (n > 0)
    TEST("test box_box query find", box_find[0], bx1);

  //test iterator
  std::cout << "Traversing box_box rtree, num nodes = "<< tr.nodes()<< '\n';
  unsigned j = 0;
  vgl_rtree<V_, B_, C_>::iterator ti = tr.begin();
  for (; ti!= tr.end(); ++ti, ++j)
    std::cout << "tr[" << j << "] = " << *ti << '\n';
  TEST("test box_box iterator", *tr.begin(),bx1);

  //test polygon probe
  //a rectangle at 45 degrees (oriented box)
  vgl_point_2d<float> pr0(0.3f, 0.7f), pr1(0.7f, 0.3f), pr2(0.5f, 0.9f),
    pr3(0.9f, 0.5f);
  vgl_polygon<float> poly(1);
  poly.push_back(pr0); poly.push_back(pr1);
  poly.push_back(pr2); poly.push_back(pr3);

  vgl_rtree_polygon_probe<V_, B_, C_> probe(poly);

  box_find.clear();
  tr.get(probe, box_find);
  n = box_find.size();
  std::cout << "Polygon box probe found " << n << " points\n";
  for (unsigned i = 0; i<n; ++i)
    std::cout << "point(s) meeting box probe " << box_find[i] << '\n';;

  TEST("number found by poly box probe", n, 3);
}

static void test_rtree()
{
  test_point_box();
  test_box_box();
}

TESTMAIN(test_rtree);
