#include <iostream>
#include <cstdlib>
#include <fstream>
#include <testlib/testlib_test.h>
#include <brad/brad_illum_util.h>
#include <brad/brad_sun_pos.h>
#include <bsta/bsta_spherical_histogram.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef DEBUG
// illumination directions for longitude = 33.331465, latitude =44.376970 deg
// for images taken over a 7 year period at roughly 07:30Z
static std::vector<vnl_double_3> illum_dirs()
{
  vnl_double_3 ill_dirs[]={
    vnl_double_3(0.344759944,-0.307169525,0.887010408),
    vnl_double_3(0.358640131,-0.377927323,0.853550347),
    vnl_double_3(0.359314323,-0.71441535,0.60041979),
    vnl_double_3(0.294631455,-0.153375276,0.943222312),
    vnl_double_3(0.324632254,-0.176995073,0.929132199),
    vnl_double_3(0.271352601,-0.176892346,0.946085019),
    vnl_double_3(0.242155829,-0.727944712,0.64144918),
    vnl_double_3(0.309160992,-0.444823599,0.840566147),
    vnl_double_3(0.31978229,-0.451643953,0.832920781),
    vnl_double_3(0.403196666,-0.308266776,0.861628715),
    vnl_double_3(0.344537352,-0.784875912,0.515037683),
    vnl_double_3(0.334663709,-0.168318145,0.927183479),
    vnl_double_3(0.309220889,-0.328136784,0.892585398),
    vnl_double_3(0.410189725,-0.638934973,0.650773763),
    vnl_double_3(0.321029856,-0.16997639,0.93169086),
    vnl_double_3(0.288541577,-0.24643749,0.92520934),
    vnl_double_3(0.292178258,-0.43317131,0.852639714),
    vnl_double_3(0.272629314,-0.25691254,0.927183479),
    vnl_double_3(0.252426572,-0.283316967,0.92520934),
    vnl_double_3(0.226285422,-0.312600525,0.922537707),
    vnl_double_3(0.245479874,-0.16186398,0.955792699),
    vnl_double_3(0.332982112,-0.549817743,0.766043969),
    vnl_double_3(0.150315903,-0.803200724,0.576431892),
    vnl_double_3(0.252962587,-0.658988064,0.708339368),
    vnl_double_3(0.208629911,-0.185230074,0.960293382),
    vnl_double_3(0.203003209,-0.235181137,0.950515402),
    vnl_double_3(0.129604627,-0.766263041,0.629319944),
    vnl_double_3(0.293558722,-0.764744293,0.573576014),
    vnl_double_3(0.217739267,-0.769471044,0.60041979),
    vnl_double_3(0.200950347,-0.171021944,0.964557128),
    vnl_double_3(0.181560261,-0.207394314,0.961261395)
  };
  std::vector<vnl_double_3> illumination_dirs(ill_dirs, ill_dirs+31);
  return illumination_dirs;
}
#endif

static void test_sun_hist()
{
  START("illumination direction histogram test");
  int oyear = 2002, ohour = 7, omin = 43, orange = 20, inter_years = 5, inter_days = 0;
  double longitude = 44.56780378, latitude = 33.34870538;
  bsta_spherical_histogram<double> h;
  brad_sun_direction_hist(oyear, ohour, omin, orange, inter_years, inter_days,
                          longitude, latitude, h);
  double cnts = h.counts(355);
  TEST_NEAR("sun direction histogram", cnts, 115.0, 0.00001);
  double mean_az, mean_el;
  h.mean(mean_az, mean_el);
  //h.print_to_text(std::cout);
  vnl_matrix_fixed<double, 2, 2> covar = h.covariance_matrix();
  std::cout << covar << '\n';
  double std_dev_az, std_dev_el;
  h.std_dev(std_dev_az, std_dev_el);
  vnl_symmetric_eigensystem<double> es(covar);
  for (unsigned i = 0; i<2; ++i){
  std::cout <<es.get_eigenvector(i) << '\n'
           << std::sqrt(es.get_eigenvalue(i)) << '\n';
  }
  double er = std::fabs(std_dev_az - 16.546094) + std::fabs(std_dev_el - 14.13252);
  er += std::fabs(std::sqrt(es.get_eigenvalue(0))-5.79397) +
        std::fabs(std::sqrt(es.get_eigenvalue(1))-20.9745);
  TEST_NEAR("sun eigensystem " , er, 0.0, 0.01);

#ifdef DEBUG //for debugging
  std::cout << h << '\n'
           << "az  el counts\n";
  h.print_to_text(std::cout);
  for (unsigned j = 0; j<h.n_elevation(); ++j)
    for (unsigned i = 0; i<h.n_azimuth(); ++i){
      if (h.counts(int(i), int(j))==0) continue;
      double azc = h.azimuth_center(i), elc = h.elevation_center(j);
      double x, y, z;
      h.convert_to_cartesian(azc, elc, x, y, z);
      std::cout << x << ' ' << y << ' ' << z << '\n';
    }
  std::ofstream os("c:/images/BaghdadBoxm2/sun.wrl");
  h.print_to_vrml(os);
  os.close();
  bsta_spherical_histogram<double>::ang_units deg =
    bsta_spherical_histogram<double>::DEG;
  bsta_spherical_histogram<double>::angle_bounds azbr =
    bsta_spherical_histogram<double>::B_0_360;
  bsta_spherical_histogram<double>::angle_bounds elpole =
    bsta_spherical_histogram<double>::B_0_180;
  bsta_spherical_histogram<double> hsun(72, 18, 0, 360.0, 0.0, 90.0, deg,
                                        azbr, elpole);
  std::vector<vnl_double_3> ill_dirs = illum_dirs();
  for (std::vector<vnl_double_3>::iterator iit = ill_dirs.begin();
       iit != ill_dirs.end(); ++iit){
    double azimuth, elevation;
    hsun.convert_to_spherical((*iit)[0],(*iit)[1],(*iit)[2],azimuth,elevation);
    hsun.upcount(azimuth, elevation);
  }
  std::ofstream oss("c:/images/BaghdadBoxm2/sun_sat.wrl");
  hsun.print_to_vrml(oss);
  oss.close();
#endif
}

TESTMAIN( test_sun_hist );
