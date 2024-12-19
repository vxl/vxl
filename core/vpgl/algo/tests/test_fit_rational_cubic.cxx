#include <iostream>
#include <vector>
#include <cmath>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vpl/vpl.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_random.h"
#include <vpgl/algo/vpgl_fit_rational_cubic.h>

static vnl_vector_fixed<double, 20>
power_vector(double x, double y, double z)
{
  // Form the monomials in homogeneous form
  double w = 1;
  double xx = x * x;
  double xy = x * y;
  double xz = x * z;
  double yy = y * y;
  double yz = y * z;
  double zz = z * z;
  double xxx = x * xx;
  double xxy = x * xy;
  double xxz = x * xz;
  double xyy = x * yy;
  double xyz = x * yz;
  double xzz = x * zz;
  double yyy = y * yy;
  double yyz = y * yz;
  double yzz = y * zz;
  double zzz = z * zz;
  double xww = x * w * w;
  double yww = y * w * w;
  double zww = z * w * w;
  double www = w * w * w;
  double xxw = xx * w;
  double xyw = xy * w;
  double xzw = xz * w;
  double yyw = yy * w;
  double yzw = yz * w;
  double zzw = zz * w;

  // fill the vector
  vnl_vector_fixed<double, 20> pv;
  pv.put(0, double(xxx));
  pv.put(1, double(xxy));
  pv.put(2, double(xxz));
  pv.put(3, double(xxw));
  pv.put(4, double(xyy));
  pv.put(5, double(xyz));
  pv.put(6, double(xyw));
  pv.put(7, double(xzz));
  pv.put(8, double(xzw));
  pv.put(9, double(xww));
  pv.put(10, double(yyy));
  pv.put(11, double(yyz));
  pv.put(12, double(yyw));
  pv.put(13, double(yzz));
  pv.put(14, double(yzw));
  pv.put(15, double(yww));
  pv.put(16, double(zzz));
  pv.put(17, double(zzw));
  pv.put(18, double(zww));
  pv.put(19, double(www));
  return pv;
}

static void
project(const double x,
        const double y,
        const double z,
        double & u,
        double & v,
        const vnl_vector_fixed<double, 20> & neu_u,
        const vnl_vector_fixed<double, 20> & den_u,
        const vnl_vector_fixed<double, 20> & neu_v,
        const vnl_vector_fixed<double, 20> & den_v)
{
  vnl_matrix_fixed<double, 4, 20> rational_coeffs;
  for (size_t i = 0; i < 20; ++i)
  {
    rational_coeffs[0][i] = neu_u[i];
    rational_coeffs[1][i] = den_u[i];
    rational_coeffs[2][i] = neu_v[i];
    rational_coeffs[3][i] = den_v[i];
  }
  vnl_vector_fixed<double, 4> polys = rational_coeffs * power_vector(x, y, z);
  u = polys[0] / polys[1];
  v = polys[2] / polys[3];
}

static void
test_fit_rational_cubic()
{
  // deterministic random generator
  vnl_random rng(9667566);

  // Rational polynomial coefficients (RPCs)
  vnl_vector_fixed<double, 20> neu_u, den_u, neu_v, den_v;
  neu_u.fill(0.0);
  den_u.fill(0.0);
  neu_v.fill(0.0);
  den_v.fill(0.0);
  neu_u[0] = 0.1;
  neu_u[10] = 0.071;
  neu_u[7] = 0.01;
  neu_u[9] = 0.3;
  neu_u[15] = 1.0;
  neu_u[18] = 1.0, neu_u[19] = 1.0;

  den_u[0] = 0.1;
  den_u[10] = 0.05;
  den_u[17] = 0.01;
  den_u[9] = 1.0;
  den_u[15] = 1.0;
  den_u[18] = 1.0;
  den_u[19] = 1.0;

  neu_v[0] = 0.02;
  neu_v[10] = 0.014;
  neu_v[7] = 0.1;
  neu_v[9] = 0.4;
  neu_v[15] = 0.5;
  neu_v[18] = 0.01;
  neu_v[19] = 0.33;

  den_v[0] = 0.1;
  den_v[10] = 0.05;
  den_v[17] = 0.03;
  den_v[9] = 1.0;
  den_v[15] = 1.0;
  den_v[18] = 0.3;
  den_v[19] = 1.0;

  std::vector<vgl_point_2d<double>> image_pts;
  std::vector<vgl_point_3d<double>> ground_pts;
  size_t n_points = 1000;
  for (unsigned i = 0; i < n_points; i++)
  {
    double x = 2.0 * rng.drand64() - 1.0;
    double y = 2.0 * rng.drand64() - 1.0;
    double z = 2.0 * rng.drand64() - 1.0;
    vgl_point_3d<double> p3d(x, y, z);
    ground_pts.push_back(p3d);
    double u, v;
    project(x, y, z, u, v, neu_u, den_u, neu_v, den_v);
    image_pts.emplace_back(u, v);
  }
  vnl_vector<double> coeffs;
  coeffs.set_size(80);
  coeffs.fill(0.0);
  for (size_t i = 0; i < 20; ++i)
  {
    coeffs[i] = neu_u[i];
    coeffs[20 + i] = den_u[i];
    coeffs[40 + i] = neu_v[i];
    coeffs[60 + i] = den_v[i];
  }
  vpgl_fit_rational_cubic frc(image_pts, ground_pts);
  frc.compute_initial_guess();
  std::cout << "initial rms error " << frc.initial_rms_error() << std::endl;
  frc.fit();
  std::cout << "final rms error " << frc.final_rms_error() << std::endl;
  bool good = frc.final_rms_error() < 1.0e-6;
  std::vector<std::vector<double>> result = frc.rational_coeffs();
  TEST("fit test rpc", good, true);

  // realistic RPC
  neu_u.fill(0.0);
  den_u.fill(0.0);
  neu_v.fill(0.0);
  den_v.fill(0.0);
  neu_u[0] = 0.01666376;
  neu_u[1] = 1.025488;
  neu_u[2] = -0.0001935893;
  neu_u[3] = 0.02406288;
  neu_u[4] = 0.002217166;
  neu_u[5] = -7.561634e-05;
  neu_u[6] = 4.181984e-05;
  neu_u[7] = -0.01500574;
  neu_u[8] = -0.0005594865;
  neu_u[9] = 6.275384e-06;
  neu_u[10] = 4.613564e-06;
  neu_u[11] = 0.0002020006;
  neu_u[12] = -7.284628e-05;
  neu_u[13] = -9.125171e-06;
  neu_u[14] = -0.0001030499;
  neu_u[15] = -7.775356e-05;
  neu_u[16] = -2.434858e-07;
  neu_u[17] = -1.238075e-06;
  neu_u[18] = 7.033904e-06;
  neu_u[19] = -2.06226e-07;

  den_u[0] = 1.0;
  den_u[1] = -0.001694469;
  den_u[2] = -0.002273012;
  den_u[3] = -0.0003236357;
  den_u[4] = 7.259351e-05;
  den_u[5] = 3.494455e-06;
  den_u[6] = -7.274219e-06;
  den_u[7] = 3.526226e-05;
  den_u[8] = 7.985993e-05;
  den_u[9] = -8.700142e-06;
  den_u[10] = 2.69718e-07;
  den_u[11] = 2.318892e-06;
  den_u[12] = -4.58529e-06;
  den_u[13] = -1.084096e-07;
  den_u[14] = -3.374917e-06;
  den_u[15] = -3.194725e-06;
  den_u[16] = 3.890611e-08;
  den_u[17] = -1.892156e-07;
  den_u[18] = 1.864785e-07;
  den_u[19] = 0.0;

  neu_v[0] = 0.02715755;
  neu_v[1] = -1.736789;
  neu_v[2] = -2.811587;
  neu_v[3] = 0.07540262;
  neu_v[4] = -0.009770075;
  neu_v[5] = 3.424154e-05;
  neu_v[6] = 0.0005777122;
  neu_v[7] = -0.007835224;
  neu_v[8] = -0.02033735;
  neu_v[9] = -7.760178e-06;
  neu_v[10] = 2.818243e-06;
  neu_v[11] = 9.863188e-05;
  neu_v[12] = 4.299843e-05;
  neu_v[13] = 8.353142e-05;
  neu_v[14] = 8.882562e-05;
  neu_v[15] = 9.245976e-05;
  neu_v[16] = 0.0001351957;
  neu_v[17] = -3.178406e-06;
  neu_v[18] = -1.656909e-06;
  neu_v[19] = -3.632718e-06;

  den_v[0] = 1.0;
  den_v[1] = 0.006090274;
  den_v[2] = 0.002161391;
  den_v[3] = -0.0003634971;
  den_v[4] = 0.0008519521;
  den_v[5] = -2.178864e-05;
  den_v[6] = -3.838019e-05;
  den_v[7] = 0.0001847707;
  den_v[8] = 0.0006151288;
  den_v[9] = -4.760751e-05;
  den_v[10] = 8.566625e-06;
  den_v[11] = -1.928669e-05;
  den_v[12] = -0.0001572831;
  den_v[13] = -4.532898e-07;
  den_v[14] = -9.574717e-05;
  den_v[15] = -8.064254e-05;
  den_v[16] = 6.411648e-08;
  den_v[17] = 2.63149e-06;
  den_v[18] = 6.70456e-06;
  den_v[19] = 1.77085e-08;

  image_pts.clear();
  ground_pts.clear();
  for (unsigned i = 0; i < n_points; i++)
  {
    double x = 2.0 * rng.drand64() - 1.0;
    double y = 2.0 * rng.drand64() - 1.0;
    double z = 2.0 * rng.drand64() - 1.0;
    vgl_point_3d<double> p3d(x, y, z);
    ground_pts.push_back(p3d);
    double u, v;
    project(x, y, z, u, v, neu_u, den_u, neu_v, den_v);
    image_pts.emplace_back(u, v);
  }
  vpgl_fit_rational_cubic act_frc(image_pts, ground_pts);
  act_frc.compute_initial_guess();
  std::cout << "initial rms error " << act_frc.initial_rms_error() << std::endl;
  act_frc.fit();
  std::cout << "final rms error " << act_frc.final_rms_error() << std::endl;
  good = act_frc.final_rms_error() < 1.0e-6;
  std::vector<std::vector<double>> act_result = act_frc.rational_coeffs();
  TEST("fit test realistic rpc", good, true);
}

TESTMAIN(test_fit_rational_cubic);
