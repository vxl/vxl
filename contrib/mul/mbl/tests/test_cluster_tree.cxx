// This is mul/mbl/tests/test_cluster_tree.cxx
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_cluster_tree.h>

#include <mbl/mbl_clusters.hxx>
#include <mbl/mbl_cluster_tree.hxx>

//: Distance class
class mbl_scalar_distance
{
 public:
  static double d(double a, double b) { return a<b?b-a:a-b; }
};

// Compile templates
MBL_CLUSTERS_INSTANTIATE(double,mbl_scalar_distance);
MBL_CLUSTER_TREE_INSTANTIATE(double,mbl_scalar_distance);

void test_cluster_tree()
{
  std::cout << "*******************\n"
           << " Testing mbl_cluster_tree\n"
           << "*******************\n";

  mbl_cluster_tree<double,mbl_scalar_distance> clusters;

  unsigned n=15;
  unsigned nL = 3;
  std::vector<double> max_r(nL);
  double r0 = 2.001;
  for (unsigned L=0;L<nL;++L)
  {
    max_r[L]=r0;  r0*=2.0;
    std::cout<<"L="<<L<<" max_r: "<<max_r[L]<<std::endl;
  }

  clusters.set_max_r(max_r);

  std::vector<double> data(n);
  for (unsigned i=0;i<n;++i) data[i]=i;

  clusters.set_data(data);

  clusters.print_summary(std::cout);
  std::cout<<"Tree:"<<std::endl;
  clusters.print_tree(std::cout);


  double d;
  TEST("Nearest point to 0 is 0",clusters.nearest(0.0,d),0);
  TEST("Nearest point to 1.1 is 1",clusters.nearest(1.1,d),1);
  TEST_NEAR("  - distance is 0.1",d,0.1,1e-6);
  TEST("Nearest point to 6.7 is 7",clusters.nearest(6.7,d),7);
  TEST_NEAR("  - distance is 0.3",d,0.3,1e-6);
  TEST("Nearest point to -3 is 0",clusters.nearest(-3,d),0);
  TEST("Nearest point to 13.2 is 13",clusters.nearest(13.2,d),13);
  TEST_NEAR("  - distance is 0.2",d,0.2,1e-6);
  TEST("Nearest point to 11.2 is 11",clusters.nearest(11.2,d),11);
  TEST_NEAR("  - distance is 0.2",d,0.2,1e-6);
}

TESTMAIN(test_cluster_tree);
