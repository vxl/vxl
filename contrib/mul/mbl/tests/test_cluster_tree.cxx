// This is mul/mbl/tests/test_cluster_trees.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_cluster_tree.h>
#include <testlib/testlib_test.h>

#include <mbl/mbl_cluster_tree.txx>

//: Distance class
class mbl_scalar_distance {
public:
  static double d(double a, double b) { return (a<b?b-a:a-b); }
};

// Compile template
MBL_CLUSTER_TREE_INSTANTIATE(double,mbl_scalar_distance);

void test_cluster_tree()
{
  vcl_cout << "*******************\n"
           << " Testing mbl_cluster_tree\n"
           << "*******************\n";

  mbl_cluster_tree<double,mbl_scalar_distance> clusters;

  unsigned n=15;
  unsigned nL = 3;
  vcl_vector<double> max_r(nL);
  double r0 = 2.001;
  for (unsigned L=0;L<nL;++L) 
  {
    max_r[L]=r0;  r0*=2.0;
    vcl_cout<<"L="<<L<<" max_r: "<<max_r[L]<<vcl_endl;
  }


  clusters.set_max_r(max_r);

  vcl_vector<double> data(n);
  for (unsigned i=0;i<n;++i) data[i]=i;

  clusters.set_data(data);

  clusters.print_summary(vcl_cout);
  vcl_cout<<"Tree:"<<vcl_endl;
  clusters.print_tree(vcl_cout);


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
