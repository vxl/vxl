// This is brl/bseg/sdet/tests/test_k_means.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <sdet/sdet_k_means.h>
void test_k_means()
{
  vcl_vector<vnl_vector<double> > pts(8, vnl_vector<double>(2,0.0));
  pts[0][0] = -1.0;  pts[0][1] = -1.0;
  pts[1][0] = -1.0;  pts[1][1] =  1.0;
  pts[2][0] =  1.0;  pts[2][1] =  1.0;
  pts[3][0] =  1.0;  pts[2][1] = -1.0;

  pts[4][0] = 4.0;  pts[4][1] = 4.0;
  pts[5][0] = 4.0;  pts[5][1] = 6.0;
  pts[6][0] = 6.0;  pts[6][1] = 6.0;
  pts[7][0] = 6.0;  pts[7][1] = 4.0;
  unsigned k = 2;
  vcl_vector<vnl_vector<double> > centers;
  vcl_vector<unsigned> partition;
  unsigned n_iter = sdet_k_means(pts, k, &centers, &partition);
  vcl_cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<centers.size(); ++i)
    vcl_cout << "c[" << i << "]:(" << centers[i][0] << ' ' 
             << centers[i][1] << ")\n";
  vcl_cout << "The partition is \n";
  for(unsigned i = 0; i<partition.size(); ++i)
    vcl_cout << "k[" << i << "]: " << partition[i] << '\n';
    
  double er = vcl_fabs(double(n_iter-2));
  vnl_vector<double> c0(2), c1(2);
  c0[0]=0; c0[1]=-0.25;
  c1[0]=5.0; c1[1]=5.0;
  er += vnl_vector_ssd(c0, centers[0]);
  er += vnl_vector_ssd(c1, centers[1]);
  TEST_NEAR("k_means ", er, 0.0, 0.001);

  centers.clear();
  partition.clear();
  vcl_vector<double> weights(8, 1.0);
  n_iter = sdet_k_means_weighted(pts, k, weights, &centers, &partition);
  vcl_cout << "\nweighted k_means\n";
  vcl_cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<centers.size(); ++i)
    vcl_cout << "c[" << i << "]:(" << centers[i][0] << ' ' 
             << centers[i][1] << ")\n";
  vcl_cout << "The partition is \n";
  for(unsigned i = 0; i<partition.size(); ++i)
    vcl_cout << "k[" << i << "]: " << partition[i] << '\n';
  
  er = vcl_fabs(double(n_iter-2));
  er += vnl_vector_ssd(c0, centers[0]);
  er += vnl_vector_ssd(c1, centers[1]);
  TEST_NEAR("k_means weighted", er, 0.0, 0.001);
}
TESTMAIN(test_k_means);
