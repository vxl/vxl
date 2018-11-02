//:
// \file
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <bsta/bsta_spherical_histogram.h>


//: Test bsta spherical_histograms
void test_spherical_histogram()
{
  //test default constructor
  bsta_spherical_histogram<double> dsh;
  //test default units and coordinates
  bsta_spherical_histogram<double> sh(8, 4, -180.0, 360.0, 0.0, 90.0);
  int az_ind = sh.azimuth_index(10.0);
  int el_ind = sh.elevation_index(10.0);
  double az_start, az_range;
  sh.azimuth_interval( az_ind, az_start, az_range);
  double el_start, el_range;
  sh.elevation_interval( el_ind, el_start, el_range);
  sh.upcount(10.0, 10.0);
  std::cout << sh << '\n';
  double er = std::fabs(az_start) + std::fabs(az_range-45.0);
   er += std::fabs(el_start) + std::fabs(el_range-22.5);
   double cnt = sh.counts(10.0, 10.0);
  er += std::fabs(cnt-1.0);
  TEST_NEAR("Earth coordinates - degrees", er, 0.0, 0.0001);
  double pi = vnl_math::pi;
  bsta_spherical_histogram<double> shr(8, 4, -pi, 2.0*pi, 0.0, pi/2,
                                       bsta_spherical_histogram<double>::RADIANS);
  az_ind = shr.azimuth_index(pi/16.0);
  el_ind = shr.elevation_index(pi/16.0);
  shr.azimuth_interval( az_ind, az_start, az_range);
  shr.elevation_interval( el_ind, el_start, el_range);
  shr.upcount(pi/16.0, pi/16.0);
  std::cout << shr << '\n';
  er = std::fabs(az_start) + std::fabs(az_range-pi/4.0);
  er += std::fabs(el_start) + std::fabs(el_range-pi/8.0);
  cnt = shr.counts(pi/16.0, pi/16.0);
  er += std::fabs(cnt-1.0);
  TEST_NEAR("Earth coordinates - radians", er, 0.0, 0.0001);
  bsta_spherical_histogram<double> sh0360(8, 4, 0.0, 360.0, 0.0, 90.0,
                                          bsta_spherical_histogram<double>::DEG, bsta_spherical_histogram<double>::B_0_360);
  az_ind = sh0360.azimuth_index(10.0);
  el_ind = sh0360.elevation_index(10.0);
  sh0360.azimuth_interval( az_ind, az_start, az_range);
  sh0360.elevation_interval( el_ind, el_start, el_range);
  sh0360.upcount(10.0, 10.0);
  std::cout << sh0360 << '\n';
  er = std::fabs(az_start) + std::fabs(az_range-45.0);
  er += std::fabs(el_start) + std::fabs(el_range-22.5);
  cnt = sh0360.counts(10.0, 10.0);
  er += std::fabs(cnt-1.0);
  TEST_NEAR("Math spherical coordinates - degrees", er, 0.0, 0.0001);
  bsta_spherical_histogram<double>
    sh0360cut(3, 4, 337.5, 45.0, 0.0, 90.0,
              bsta_spherical_histogram<double>::DEG,
              bsta_spherical_histogram<double>::B_0_360);
  int az_ind_10 = sh0360cut.azimuth_index(7.0);
  int az_ind_350 = sh0360cut.azimuth_index(353);
  sh0360cut.upcount(7.0, 10.0);
  sh0360cut.upcount(353.0, 10.0);
  cnt = sh0360cut.counts(0.0, 10.0);
  er = std::fabs(az_ind_10 + az_ind_350 - 2.0);
  er += std::fabs(cnt-2.0);
  double azc = sh0360cut.azimuth_center(az_ind_10);
  er += std::fabs(azc);
  TEST_NEAR("bin spans cut, math coords - degrees", er , 0.0, 0.0001);
  double x, y, z, x1, y1, z1;
  sh0360cut.convert_to_cartesian(318.3, 62.5, x, y, z);
  er = std::fabs(0.344759944-x)+std::fabs(-0.307169525-y);
  sh0360cut.convert_to_cartesian(291.0, 35.0, x1, y1, z1);
  er += std::fabs(0.293558722-x1)+ std::fabs(-0.764744293-y1);
  TEST_NEAR("convert to Cartesian", er, 0.0, 0.0001);
  double azim, elev;
  sh0360cut.convert_to_spherical(x, y, z, azim, elev);
  er = std::fabs(azim-318.3) + std::fabs(elev-62.5);
  double azim1, elev1;
  sh0360cut.convert_to_spherical(x1, y1, z1, azim1, elev1);
  er += std::fabs(azim1-291) + std::fabs(elev1-35.0);
  TEST_NEAR("convert to spherical", er, 0.0, 0.0001);
  sh0360cut.upcount(10.0, 26.0); sh0360cut.upcount(20.0, 35.0);
  sh0360cut.upcount(338.0, 8.0);  sh0360cut.upcount(350.0, 5.0);
  std::cout << sh0360cut << '\n';
  sh0360cut.print_to_text(std::cout);
  double mean_az, mean_el;
  sh0360cut.mean(mean_az, mean_el);
  std::cout << "mean(" << mean_az << ' ' << mean_el << ")\n";
  er = std::fabs(mean_az -359.189) + std::fabs(mean_el- 19.0974);
  vnl_matrix_fixed<double, 2, 2> cmat = sh0360cut.covariance_matrix();
  std::cout << cmat << '\n';
  er += std::fabs(cmat[0][0]-150.657) + std::fabs(cmat[0][1]-112.218);
  er += std::fabs(cmat[1][1]-112.621);
  TEST_NEAR("mean and covariance matrix", er, 0.0, 0.01);
  std::vector<int> inter_bins = sh0360cut.bins_intersecting_cone(mean_az, mean_el, 25.0);
  double el_center, az_center;
  double tcnts = 0;
  for (int & inter_bin : inter_bins) {
    sh0360cut.center(inter_bin, az_center, el_center);
    tcnts += sh0360cut.counts(inter_bin);
    std::cout << "c(" << az_center << ' ' << el_center << "): "
             << sh0360cut.counts(inter_bin) << '\n';
  }
  std::cout << "Cone total = " << tcnts << " Full total = " <<  sh0360cut.total_counts() << '\n';
  TEST_NEAR("bins intersecting cone", tcnts, sh0360cut.total_counts(), 0.01);
}

TESTMAIN(test_spherical_histogram);
