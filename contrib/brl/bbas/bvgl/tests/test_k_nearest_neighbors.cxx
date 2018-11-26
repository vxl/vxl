//:
// \file
#include <iostream>
#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <bnabo/bnabo.h>
#define TEST_K_NEAREST_NEIGHBORS 1
//: Test changes
static void test_k_nearest_neighbors_3d()
{
#if TEST_K_NEAREST_NEIGHBORS
#if 0 // shows nabo usage
  vnl_matrix<double> M(3, 100);
  for(int i = 0; i<100; ++i){
    M[0][i] = static_cast<double>(i);
    M[1][i] = static_cast<double>(i);
    M[2][i] = static_cast<double>(i);
  }
  double* ptr = &M[0][22];
  vnl_vector<double> q(3, 22.01);
  int k = 1;
  vnl_vector<int> indices(k);
  vnl_vector<double> dists2(k);
  unsigned cflags = 0,flags = 0;
  cflags = cflags |  Nabo::NearestNeighbourSearch<double, vnl_matrix<double> >::TOUCH_STATISTICS;
  flags = flags |  Nabo::NearestNeighbourSearch<double, vnl_matrix<double> >::ALLOW_SELF_MATCH;
  flags = flags |  Nabo::NearestNeighbourSearch<double, vnl_matrix<double> >::SORT_RESULTS;

  double eps = 0.0;
  Nabo::NearestNeighbourSearch<double, vnl_matrix<double> >* search = Nabo::NearestNeighbourSearch<double, vnl_matrix<double> >::createKDTreeLinearHeap(M, 3, cflags);
  search->knn(q, indices, dists2, k, eps, flags);
  for(int i = 0; i<k; ++i)
    std::cout << indices[i] << ' ' << M.get_column(indices[i]) << ' ' << dists2[i] << '\n';
#endif
  vgl_pointset_3d<double> ptset;
  vgl_point_3d<double> p0(0.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 0.0, 1.0);
  vgl_point_3d<double> p2(0.0, 1.0, 0.0);
  vgl_point_3d<double> p3(0.0, 1.0, 1.0);
  vgl_point_3d<double> p4(1.0, 0.0, 0.0);
  vgl_point_3d<double> p5(1.0, 0.0, 1.0);
  vgl_point_3d<double> p6(1.0, 1.0, 0.0);
  vgl_point_3d<double> p7(1.0, 1.0, 1.0);
  vgl_point_3d<double> p8(0.5, 0.5, 0.5);
  vgl_point_3d<double> p9(0.5, 0.5, 1.5);
  vgl_point_3d<double> p10(0.5, 1.5, 0.5);
  vgl_point_3d<double> p11(0.5, 1.5, 1.5);
  vgl_point_3d<double> p12(1.5, 0.5, 0.5);
  vgl_point_3d<double> p13(1.5, 0.5, 1.5);
  vgl_point_3d<double> p14(1.5, 1.5, 0.5);
  vgl_point_3d<double> p15(1.5, 1.5, 1.5);
  ptset.add_point(p0);   ptset.add_point(p1);   ptset.add_point(p2);   ptset.add_point(p3);
  ptset.add_point(p4);   ptset.add_point(p5);   ptset.add_point(p6);   ptset.add_point(p7);
  ptset.add_point(p8);   ptset.add_point(p9);   ptset.add_point(p10);   ptset.add_point(p11);
  ptset.add_point(p12);   ptset.add_point(p13);   ptset.add_point(p14);   ptset.add_point(p15);
  bvgl_k_nearest_neighbors_3d<double> knn3d(ptset);
  unsigned index=0;
  vgl_point_3d<double> q(0.5, 0.0, 0.5);
  bool good = knn3d.closest_index(q, index);
  if(good) {
    std::cout << "closest index " << ptset.p(index) << '\n';
  }
  vgl_point_3d<double> cp;
  good = knn3d.closest_point(q, cp);
  if(good) {
         std::cout << "closest point " << cp << '\n';
  }
  double d = (p8-cp).length();
  TEST_NEAR("closest point" , d ,0.0 , 0.001);
  vgl_pointset_3d<double> k_neighbors;
  unsigned k = 5;
  good = knn3d.knn(q, k, k_neighbors);
  if(good){
    std::cout << "K neighbors\n" << k_neighbors << '\n';
    vgl_point_3d<double> nb(1.0, 0.0, 0.0);
    d = (nb-k_neighbors.p(4)).length();
    TEST_NEAR("k neighbors", d, 0.0, 0.001);
  } else {
    TEST("k neighbors", true, false);
  }

  // test index retrieval
  vnl_vector<int> k_indices;
  good = knn3d.knn_indices(q, k, k_indices);
  if(good) {
    std::cout << "K neighbor indices\n" << k_indices << '\n';
    TEST_EQUAL("k indices", k_indices[4], 4);
  } else {
    TEST("k indices", true, false);
  }
#endif
}

static void test_k_nearest_neighbors_2d()
{
  vgl_point_2d<double> p0(1.5, 3.0);
  vgl_point_2d<double> p1(4.0, 0.0);
  vgl_point_2d<double> p2(-3.0, -2.0);
  vgl_point_2d<double> p3(0.0, 0.0);
  vgl_point_2d<double> p4(0.0, 4.0);
  std::vector<vgl_point_2d<double>> ptset {p0, p1, p2, p3, p4};

  bvgl_k_nearest_neighbors_2d<double> knn2d(ptset);

  unsigned index=0;
  vgl_point_2d<double> q1(0.5, 0.5);
  bool good = knn2d.closest_index(q1, index);
  TEST("closest_index success", good, true);
  TEST("closest_index correct index", index, 3);

  vgl_point_2d<double> cp;
  vgl_point_2d<double> q2(-1000, -1000);
  good = knn2d.closest_point(q2, cp);
  TEST("closest_point success", good, true);
  double d = (cp - vgl_point_2d<double>(-3.0, -2.0)).length();
  TEST_NEAR("closest_point correct point", d, 0, 1e-6);

  vgl_point_2d<double> q3(2.0, 1.0);
  std::vector<vgl_point_2d<double> > neighbors;
  good = knn2d.knn(q3, 3, neighbors);
  TEST("knn success", good, true);
  bool correct = true;
  correct &= neighbors.size() == 3;
  for (auto pt : std::vector<vgl_point_2d<double>>{p0, p1, p3}) {
    double mindist = 1000.0;
    for (auto pn : neighbors) {
      double dist = (pt - pn).length();
      if (dist < mindist) {
        mindist = dist;
      }
    }
    correct &= mindist < 1e-6;
  }
  TEST("knn correct neighbors", correct, true);
}

void test_k_nearest_neighbors()
{
  test_k_nearest_neighbors_2d();
  test_k_nearest_neighbors_3d();
}

TESTMAIN( test_k_nearest_neighbors );
