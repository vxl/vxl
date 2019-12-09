#include <iostream>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_compute_cremona_2d.h>
#include <vgl/algo/vgl_cremona_trans_2d.h>
#include <vnl/vnl_vector.h>
#include <fstream>
#include <vpl/vpl.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
static void
test_compute_cremona_2d()
{
  vnl_random rand;
  vgl_compute_cremona_2d<double, 3> cc;
  double x = 2, y = 3;
  vnl_vector<double> pv = vgl_cremona_trans_2d<double, 3>::power_vector(x, y);
  std::vector<double> tv = { 1, 3, 9, 27, 2, 6, 18, 4, 12, 8 };
  double er = 0.0;
  size_t npv = pv.size();
  for (size_t i = 0; i < npv; ++i)
    er += fabs(pv[i] - tv[i]);
  TEST("power vector", er, 0.0);
  vnl_vector<double> x_neu(npv, 0.0), x_den(npv, 0.0), y_neu(npv, 0.0), y_den(npv, 0.0);
  x_neu[1] = 10;
  x_neu[4] = 20;
  x_neu[9] = 1;
  x_den[0] = 5;
  x_den[3] = 1;
  x_den[5] = 1;
  y_neu[1] = 10.1;
  y_neu[4] = 20.1;
  y_neu[9] = 1.1;
  y_den[0] = 5.1;
  y_den[3] = 1.1;
  y_den[5] = 1.1;
  size_t npts = 1000;
  std::vector<vgl_homg_point_2d<double>> from_pts, to_pts;
  double r = 1.0;
  for (size_t i = 0; i < npts; ++i)
  {
    double X = rand.drand32(-r, r);
    double Y = rand.drand32(-r, r);
    vnl_vector<double> pv = vgl_cremona_trans_2d<double, 3>::power_vector(X, Y);
    double x = dot_product(x_neu, pv) / dot_product(x_den, pv);
    double y = dot_product(y_neu, pv) / dot_product(y_den, pv);
    from_pts.emplace_back(X, Y);
    to_pts.emplace_back(x, y);
  }
  bool good = cc.compute_linear(from_pts, to_pts);
  vnl_vector<double> lin_coeff = cc.linear_coeff();

  TEST("linear computation deg = 3", good, true);
  TEST_NEAR("linear solution error deg = 3", cc.linear_error(), 0.0, 1.0e-6);

  vgl_cremona_trans_2d<double, 3> ct = cc.linear_trans();
  x = 0.0;
  y = 0.0;
  ct.project(from_pts[0].x(), from_pts[0].y(), x, y);
  double ter = fabs(to_pts[0].x() - x) + fabs(to_pts[0].y() - y);
  TEST_NEAR("cremona_trans class constructor deg = 3 ", ter, 0.0, 1.0e-6);

  // TEST the second order transform //
  from_pts.clear();
  to_pts.clear();
  size_t nc2 = vgl_cremona_trans_2d<double, 2>::n_coeff();
  vnl_vector<double> x_neu2(nc2, 0.0), x_den2(nc2, 0.0), y_neu2(nc2, 0.0), y_den2(nc2, 0.0);
  x_neu2[0] = 100.0;
  x_neu2[1] = 10.0;
  x_neu2[2] = 1.0;
  x_den2[0] = 10.0;
  x_den2[1] = 5.0;
  x_neu2[4] = 1.5;
  y_neu2[0] = 10.0;
  y_neu2[5] = 20.0;
  y_neu2[3] = 3.0;
  y_den2[0] = 5.0;
  y_den2[1] = 3.0;
  y_neu2[2] = 2.5;
  for (size_t i = 0; i < npts; ++i)
  {
    double X = rand.drand32(-r, r);
    double Y = rand.drand32(-r, r);
    vnl_vector<double> pv2 = vgl_cremona_trans_2d<double, 2>::power_vector(X, Y);
    double x = dot_product(x_neu2, pv2) / dot_product(x_den2, pv2);
    double y = dot_product(y_neu2, pv2) / dot_product(y_den2, pv2);
    from_pts.emplace_back(X, Y);
    to_pts.emplace_back(x, y);
  }
  std::cout << "\ntest compute bi-rational cremona with deg = 2" << std::endl;
  vgl_compute_cremona_2d<double, 2> cc2;
  good = cc2.compute_linear(from_pts, to_pts);
  TEST("linear computation deg = 2 ", good, true);
  TEST_NEAR("linear solution error deg = 2", cc2.linear_error(), 0.0, 1.0e-6);

  // Test stream operators for previous cremona trans
  vgl_cremona_trans_2d<double, 2> ctrans_2 = cc2.linear_trans(), in_ctrans_2;
  std::ofstream ostr("./test_stream.txt");
  bool ostr_good = false, istr_good = false, strop_good = false;
  if (ostr)
  {
    ostr_good = true;
    ostr << ctrans_2;
    ostr.close();
    std::ifstream istr("./test_stream.txt");
    if (istr)
    {
      istr_good = true;
      istr >> in_ctrans_2;
      vnl_vector<double> in_coeffs = in_ctrans_2.coeff();
      vnl_vector<double> coeffs = ctrans_2.coeff();
      strop_good = coeffs.size() == in_coeffs.size();
      if (strop_good)
      {
        double er = 0.0;
        size_t n = in_coeffs.size();
        for (size_t i = 0; i < coeffs.size(); ++i)
          er += fabs(coeffs[i] - in_coeffs[i]);
        er /= n;
        strop_good = er < 1.0e-6;
      }
    }
  }
  TEST("stream operators", ostr_good && istr_good && strop_good, true);
  vpl_unlink("./test_stream.txt");
  std::cout << "\ntest compute common_denominator cremona with deg = 2" << std::endl;
  from_pts.clear();
  to_pts.clear();
  x_neu2.fill(0.0);
  x_den2.fill(0.0);
  y_neu2.fill(0.0);
  x_neu2[0] = 5.0;
  x_neu2[1] = 2.0;
  x_neu2[2] = 1.0;
  x_den2[0] = 2.0;
  x_den2[1] = 5.0;
  x_den2[3] = 1.5;
  y_neu2[0] = 1.1;
  y_neu2[5] = 3.0;
  y_neu2[3] = 3.0;
  for (size_t i = 0; i < npts; ++i)
  {
    double X = rand.drand32(-r, r);
    double Y = rand.drand32(-r, r);
    vnl_vector<double> pv3 = vgl_cremona_trans_2d<double, 2>::power_vector(X, Y);
    double x = dot_product(x_neu2, pv3) / dot_product(x_den2, pv3);
    double y = dot_product(y_neu2, pv3) / dot_product(x_den2, pv3);
    from_pts.emplace_back(X, Y);
    to_pts.emplace_back(x, y);
  }
  vgl_compute_cremona_2d<double, 2> cc3;
  good = cc3.compute_linear(from_pts, to_pts, vgl_compute_cremona_2d<double, 2>::COMMON_DENOMINATOR);
  TEST("linear computation common_denominator deg = 2 ", good, true);
  TEST_NEAR("linear solution common_denominator error deg = 2", cc3.linear_error(), 0.0, 1.0e-6);

  std::cout << "\ntest compute unity_denominator cremona with deg = 2" << std::endl;
  from_pts.clear();
  to_pts.clear();
  x_neu2.fill(0.0);
  x_den2.fill(0.0);
  y_neu2.fill(0.0);
  x_neu2[0] = 5.0;
  x_neu2[1] = 2.0;
  x_neu2[2] = 1.0;
  x_den2[0] = 1.0;
  y_neu2[0] = 1.1;
  y_neu2[5] = 3.0;
  y_neu2[3] = 3.0;
  for (size_t i = 0; i < npts; ++i)
  {
    double X = rand.drand32(-r, r);
    double Y = rand.drand32(-r, r);
    vnl_vector<double> pv3 = vgl_cremona_trans_2d<double, 2>::power_vector(X, Y);
    double x = dot_product(x_neu2, pv3) / dot_product(x_den2, pv3);
    double y = dot_product(y_neu2, pv3) / dot_product(x_den2, pv3);
    from_pts.emplace_back(X, Y);
    to_pts.emplace_back(x, y);
  }
  vgl_compute_cremona_2d<double, 2> cc4;
  good = cc4.compute_linear(from_pts, to_pts, vgl_compute_cremona_2d<double, 2>::UNITY_DENOMINATOR);
  TEST("linear computation unity_denominator deg = 2 ", good, true);
  TEST_NEAR("linear solution unity_denominator error deg = 2", cc4.linear_error(), 0.0, 1.0e-6);
}

TESTMAIN(test_compute_cremona_2d);
