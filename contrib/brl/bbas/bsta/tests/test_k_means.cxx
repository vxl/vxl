// This is brl/bbas/bsta/tests/test_k_means.cxx
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_k_means.h>
void test_k_means()
{
  std::vector<vnl_vector<double> > pts(8, vnl_vector<double>(2,0.0));
  pts[0][0] = -1.0;  pts[0][1] = -1.0;
  pts[1][0] = -1.0;  pts[1][1] =  1.0;
  pts[2][0] =  1.0;  pts[2][1] =  1.0;
  pts[3][0] =  1.0;  pts[2][1] = -1.0;

  pts[4][0] = 4.0;  pts[4][1] = 4.0;
  pts[5][0] = 4.0;  pts[5][1] = 6.0;
  pts[6][0] = 6.0;  pts[6][1] = 6.0;
  pts[7][0] = 6.0;  pts[7][1] = 4.0;
  unsigned k = 2;
  std::vector<vnl_vector<double> > centers;
  std::vector<unsigned> partition;
  unsigned n_iter = bsta_k_means<double>(pts, k, &centers, &partition);
  std::cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<centers.size(); ++i)
    std::cout << "c[" << i << "]:(" << centers[i][0] << ' '
             << centers[i][1] << ")\n";
  std::cout << "The partition is \n";
  for(unsigned i = 0; i<partition.size(); ++i)
    std::cout << "k[" << i << "]: " << partition[i] << '\n';

  double er = std::fabs(double(n_iter-2));
  vnl_vector<double> c0(2), c1(2);
  c0[0]=0; c0[1]=-0.25;
  c1[0]=5.0; c1[1]=5.0;
  er += vnl_vector_ssd(c0, centers[0]);
  er += vnl_vector_ssd(c1, centers[1]);
  TEST_NEAR("k_means ", er, 0.0, 0.001);

  std::vector<vnl_vector_fixed<double, 3> > fpts(8, vnl_vector_fixed<double, 3>(0.0));
  fpts[0][0] = -1.0;  fpts[0][1] = -1.0;  fpts[0][2] = 0.0;
  fpts[1][0] = -1.0;  fpts[1][1] =  1.0;  fpts[1][2] = 0.0;
  fpts[2][0] =  1.0;  fpts[2][1] =  1.0;  fpts[2][2] = 0.0;
  fpts[3][0] =  1.0;  fpts[2][1] = -1.0;  fpts[3][2] = 0.0;

  fpts[4][0] = 4.0;  fpts[4][1] = 4.0;  fpts[4][2] = 0.0;
  fpts[5][0] = 4.0;  fpts[5][1] = 6.0;  fpts[5][2] = 0.0;
  fpts[6][0] = 6.0;  fpts[6][1] = 6.0;  fpts[6][2] = 0.0;
  fpts[7][0] = 6.0;  fpts[7][1] = 4.0;  fpts[7][2] = 0.0;
  k = 2;
  std::vector<vnl_vector_fixed<double, 3> > fcenters;
  std::vector<unsigned> fpartition;
  n_iter = bsta_k_means<double>(fpts, k, &fcenters, &fpartition);
  std::cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<fcenters.size(); ++i)
    std::cout << "c[" << i << "]:(" << fcenters[i][0] << ' '
             << fcenters[i][1] << ' ' << fcenters[i][2] << ")\n";
  std::cout << "The partition is \n";
  for(unsigned i = 0; i<partition.size(); ++i)
    std::cout << "k[" << i << "]: " << fpartition[i] << '\n';

  er = std::fabs(double(n_iter-2));
  vnl_vector_fixed<double, 3> cf0, cf1;
  cf0[0]=0; cf0[1]=-0.25; cf0[2] = 0.0;
  cf1[0]=5.0; cf1[1]=5.0; cf1[2] = 0.0;
  er += vnl_vector_ssd(cf0, fcenters[0]);
  er += vnl_vector_ssd(cf1, fcenters[1]);
  TEST_NEAR("k_means vnl_vector_fixed ", er, 0.0, 0.001);

  centers.clear();
  partition.clear();
  std::vector<double> weights(8, 1.0);
  n_iter = bsta_k_means_weighted<double>(pts, k, weights, &centers, &partition);
  std::cout << "\nweighted k_means\n";
  std::cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<centers.size(); ++i)
    std::cout << "c[" << i << "]:(" << centers[i][0] << ' '
             << centers[i][1] << ")\n";
  std::cout << "The partition is \n";
  for(unsigned i = 0; i<partition.size(); ++i)
    std::cout << "k[" << i << "]: " << partition[i] << '\n';

  er = std::fabs(double(n_iter-2));
  er += vnl_vector_ssd(c0, centers[0]);
  er += vnl_vector_ssd(c1, centers[1]);
  TEST_NEAR("k_means weighted", er, 0.0, 0.001);
  fcenters.clear();
  fpartition.clear();
  n_iter = bsta_k_means_weighted<double>(fpts, k, weights, &fcenters, &fpartition);
  std::cout << "\nweighted k_means vnl_vector_fixed\n";
  std::cout << "After " << n_iter << " iterations the cluster centers are:\n";
  for(unsigned i = 0; i<fcenters.size(); ++i)
    std::cout << "c[" << i << "]:(" << fcenters[i][0] << ' '
             << fcenters[i][1] << ' ' << fcenters[i][2] << ")\n";
  std::cout << "The partition is \n";
  for(unsigned i = 0; i<fpartition.size(); ++i)
    std::cout << "k[" << i << "]: " << fpartition[i] << '\n';

  er = std::fabs(double(n_iter-2));
  er += vnl_vector_ssd(cf0, fcenters[0]);
  er += vnl_vector_ssd(cf1, fcenters[1]);
  TEST_NEAR("k_means weighted vnl_vector_fixed", er, 0.0, 0.001);
}
TESTMAIN(test_k_means);
