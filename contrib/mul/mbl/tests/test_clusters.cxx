// This is mul/mbl/tests/test_clusters.cxx
#include <testlib/testlib_test.h>
//:
// \file
#include <mbl/mbl_clusters.h>
#include <mbl/mbl_clusters.txx>
#include <vcl_iostream.h>

//: Distance class
class mbl_scalar_distance
{
 public:
  static double d(double a, double b) { return a<b?b-a:a-b; }
};

// Compile template
MBL_CLUSTERS_INSTANTIATE(double,mbl_scalar_distance);

void test_clusters()
{
  vcl_cout << "**********************\n"
           << " Testing mbl_clusters\n"
           << "**********************\n";

  mbl_clusters<double,mbl_scalar_distance> clusters;

  unsigned n=15;
  double max_r = 5.001;
  vcl_vector<double> data(n);
  for (unsigned i=0;i<n;++i) data[i]=i;

  clusters.set_max_r(max_r);

  clusters.set_data(data);

  TEST("Number of clusters",clusters.n_clusters(),1+int((n-1)/max_r));

  double d;
  TEST("Nearest point to 0 is 0",clusters.nearest(0.0,d),0);
  TEST("Nearest point to 1.1 is 1",clusters.nearest(1.1,d),1);
  TEST_NEAR("  - distance is 0.1",d,0.1,1e-6);
  TEST("Nearest point to 6.7 is 7",clusters.nearest(6.7,d),7);
  TEST_NEAR("  - distance is 0.3",d,0.3,1e-6);
  TEST("Nearest point to -3 is 0",clusters.nearest(-3,d),0);

  data.push_back(n+17.3);
  TEST("Cluster for added point correct",
       clusters.add_object(data.size()-1),1+int((n-1)/max_r));
}

TESTMAIN(test_clusters);
