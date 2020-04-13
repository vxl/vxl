#include <limits>
#include <iostream>
#include "testlib/testlib_test.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include <vnl/algo/vnl_svd.h>
#include "vgl/vgl_box_3d.h"
#include "vgl/vgl_distance.h"
#include <vgl/algo/vgl_h_matrix_2d.h>
#include "vpgl/vpgl_local_rational_camera.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vpgl/vpgl_affine_tri_focal_tensor.h"
#include <vpgl/algo/vpgl_affine_rectification.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#include <vpgl/algo/vpgl_camera_convert.h>

static void
rat_cameras(vpgl_local_rational_camera<double> & cam0,
            vpgl_local_rational_camera<double> & cam1,
            vpgl_local_rational_camera<double> & cam2)
{
  std::stringstream ss0, ss1, ss2;
  ss0 << " satId ="
         "????"
      << ";" << std::endl;
  ss0 << "bandId =" << ' ' << "RGB"
      << ";" << std::endl;
  ss0 << "SpecId =" << ' ' << "RPC00B"
      << ";" << std::endl;
  ss0 << "BEGIN_GROUP = IMAGE" << std::endl;
  ss0 << std::endl;
  ss0 << "lineOffset = 12171" << std::endl;
  ss0 << "sampOffset = -4028" << std::endl;
  ss0 << "latOffset = 39.7487" << std::endl;
  ss0 << "longOffset = -84.1037" << std::endl;
  ss0 << "heightOffset = 235" << std::endl;
  ss0 << "lineScale = 18024" << std::endl;
  ss0 << "sampScale = 21250" << std::endl;
  ss0 << "latScale = 0.0547" << std::endl;
  ss0 << "longScale = 0.079" << std::endl;
  ss0 << "heightScale = 501" << std::endl;
  ss0 << "lineNumCoef = (" << std::endl;
  ss0 << "  0.0007974927," << std::endl;
  ss0 << "  0.006422505," << std::endl;
  ss0 << "  -1.028402," << std::endl;
  ss0 << "  -0.02072569," << std::endl;
  ss0 << "  -0.0003661282," << std::endl;
  ss0 << "  -7.416224e-06," << std::endl;
  ss0 << "  8.616331e-06," << std::endl;
  ss0 << "  -0.0003544481," << std::endl;
  ss0 << "  -0.0001918033," << std::endl;
  ss0 << "  1.848756e-06," << std::endl;
  ss0 << "  5.986469e-08," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  3.055645e-06," << std::endl;
  ss0 << "  1.633479e-07," << std::endl;
  ss0 << "  -1.989319e-05," << std::endl;
  ss0 << "  -0.0001026535," << std::endl;
  ss0 << "  -2.612502e-05," << std::endl;
  ss0 << "  -4.010298e-07," << std::endl;
  ss0 << "  -3.647204e-06," << std::endl;
  ss0 << "  -5.246009e-07);" << std::endl;
  ss0 << "lineDenCoef = (" << std::endl;
  ss0 << "  1," << std::endl;
  ss0 << "  -0.0003774478," << std::endl;
  ss0 << "  0.0006511098," << std::endl;
  ss0 << "  2.550236e-05," << std::endl;
  ss0 << "  6.993369e-06," << std::endl;
  ss0 << "  2.088266e-07," << std::endl;
  ss0 << "  -5.19759e-06," << std::endl;
  ss0 << "  1.835162e-05," << std::endl;
  ss0 << "  -6.221043e-05," << std::endl;
  ss0 << "  2.529734e-05," << std::endl;
  ss0 << "  1.673582e-07," << std::endl;
  ss0 << "  1.256364e-08," << std::endl;
  ss0 << "  3.881639e-06," << std::endl;
  ss0 << "  -1.783357e-08," << std::endl;
  ss0 << "  -3.795141e-07," << std::endl;
  ss0 << "  -9.648494e-05," << std::endl;
  ss0 << "  -1.012712e-07," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  -5.681886e-06," << std::endl;
  ss0 << "  0);" << std::endl;
  ss0 << "sampNumCoef = (" << std::endl;
  ss0 << "  0.001066424," << std::endl;
  ss0 << "  1.002723," << std::endl;
  ss0 << "  -0.0001855127," << std::endl;
  ss0 << "  0.001750345," << std::endl;
  ss0 << "  -0.0003072863," << std::endl;
  ss0 << "  0.0004452334," << std::endl;
  ss0 << "  -0.000198721," << std::endl;
  ss0 << "  -0.0006711932," << std::endl;
  ss0 << "  -0.0001650729," << std::endl;
  ss0 << "  -3.311361e-06," << std::endl;
  ss0 << "  -1.837305e-07," << std::endl;
  ss0 << "  -4.568539e-06," << std::endl;
  ss0 << "  -5.987382e-06," << std::endl;
  ss0 << "  -1.853512e-06," << std::endl;
  ss0 << "  2.21958e-05," << std::endl;
  ss0 << "  6.981202e-05," << std::endl;
  ss0 << "  2.788669e-08," << std::endl;
  ss0 << "  -3.453303e-07," << std::endl;
  ss0 << "  4.344633e-06," << std::endl;
  ss0 << "  0);" << std::endl;
  ss0 << "sampDenCoef = (" << std::endl;
  ss0 << "  1," << std::endl;
  ss0 << "  -0.0003971277," << std::endl;
  ss0 << "  0.000308897," << std::endl;
  ss0 << "  -0.0004483379," << std::endl;
  ss0 << "  -8.382545e-06," << std::endl;
  ss0 << "  -1.705975e-07," << std::endl;
  ss0 << "  8.337555e-08," << std::endl;
  ss0 << "  1.514784e-06," << std::endl;
  ss0 << "  3.583066e-06," << std::endl;
  ss0 << "  -2.171271e-06," << std::endl;
  ss0 << "  -3.152416e-08," << std::endl;
  ss0 << "  -1.567683e-08," << std::endl;
  ss0 << "  1.562718e-08," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  -4.272562e-07," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  0," << std::endl;
  ss0 << "  -1.623804e-08," << std::endl;
  ss0 << "  0);" << std::endl;
  ss0 << "END_GROUP = IMAGE" << std::endl;
  ss0 << "END"
      << ";" << std::endl;
  ss0 << "lvcs" << std::endl;
  ss0 << "-84.0884722667" << std::endl;
  ss0 << "39.7765084138" << std::endl;
  ss0 << "232.111831665" << std::endl;

  std::istringstream istr(ss0.str());
  vpgl_local_rational_camera<double> * cam0_ptr = read_local_rational_camera<double>(istr);

  ss1 << " satId ="
         "????"
      << ";" << std::endl;
  ss1 << "bandId =" << ' ' << "RGB"
      << ";" << std::endl;
  ss1 << "SpecId =" << ' ' << "RPC00B"
      << ";" << std::endl;
  ss1 << "BEGIN_GROUP = IMAGE" << std::endl;
  ss1 << std::endl;
  ss1 << "lineOffset = 12398" << std::endl;
  ss1 << "sampOffset = -3588" << std::endl;
  ss1 << "latOffset = 39.7417" << std::endl;
  ss1 << "longOffset = -84.1031" << std::endl;
  ss1 << "heightOffset = 236" << std::endl;
  ss1 << "lineScale = 18038" << std::endl;
  ss1 << "sampScale = 21250" << std::endl;
  ss1 << "latScale = 0.0661" << std::endl;
  ss1 << "longScale = 0.0854" << std::endl;
  ss1 << "heightScale = 500" << std::endl;
  ss1 << "lineNumCoef = (" << std::endl;
  ss1 << "  0.00100691," << std::endl;
  ss1 << "  0.02209501," << std::endl;
  ss1 << "  -1.060084," << std::endl;
  ss1 << "  -0.0363005," << std::endl;
  ss1 << "  -0.0006619213," << std::endl;
  ss1 << "  -2.108747e-05," << std::endl;
  ss1 << "  -4.259502e-05," << std::endl;
  ss1 << "  -0.0001856683," << std::endl;
  ss1 << "  0.000267939," << std::endl;
  ss1 << "  1.050523e-06," << std::endl;
  ss1 << "  -4.042144e-07," << std::endl;
  ss1 << "  -1.021602e-06," << std::endl;
  ss1 << "  -4.319654e-06," << std::endl;
  ss1 << "  -1.212165e-06," << std::endl;
  ss1 << "  4.317543e-05," << std::endl;
  ss1 << "  0.0001923413," << std::endl;
  ss1 << "  5.784949e-05," << std::endl;
  ss1 << "  1.47375e-06," << std::endl;
  ss1 << "  1.05804e-05," << std::endl;
  ss1 << "  1.97994e-06);" << std::endl;
  ss1 << "lineDenCoef = (" << std::endl;
  ss1 << "  1," << std::endl;
  ss1 << "  -0.0007258401," << std::endl;
  ss1 << "  0.001433577," << std::endl;
  ss1 << "  -6.980484e-06," << std::endl;
  ss1 << "  -7.096595e-06," << std::endl;
  ss1 << "  -1.019274e-07," << std::endl;
  ss1 << "  1.656431e-05," << std::endl;
  ss1 << "  -4.098971e-05," << std::endl;
  ss1 << "  0.000128035," << std::endl;
  ss1 << "  -5.411963e-05," << std::endl;
  ss1 << "  2.209094e-06," << std::endl;
  ss1 << "  6.107361e-08," << std::endl;
  ss1 << "  3.23453e-05," << std::endl;
  ss1 << "  1.104355e-07," << std::endl;
  ss1 << "  -8.683568e-07," << std::endl;
  ss1 << "  -0.0002785862," << std::endl;
  ss1 << "  -1.069094e-06," << std::endl;
  ss1 << "  -2.733262e-08," << std::endl;
  ss1 << "  -2.87273e-05," << std::endl;
  ss1 << "  0);" << std::endl;
  ss1 << "sampNumCoef = (" << std::endl;
  ss1 << "  0.001336209," << std::endl;
  ss1 << "  1.004861," << std::endl;
  ss1 << "  0.0001018478," << std::endl;
  ss1 << "  0.002896619," << std::endl;
  ss1 << "  -0.0006634305," << std::endl;
  ss1 << "  0.0004390646," << std::endl;
  ss1 << "  -0.0002026649," << std::endl;
  ss1 << "  -0.0009310784," << std::endl;
  ss1 << "  -0.0002714375," << std::endl;
  ss1 << "  -5.621063e-06," << std::endl;
  ss1 << "  2.457906e-07," << std::endl;
  ss1 << "  -5.831878e-06," << std::endl;
  ss1 << "  4.290885e-07," << std::endl;
  ss1 << "  -2.082883e-06," << std::endl;
  ss1 << "  4.622606e-06," << std::endl;
  ss1 << "  -0.0003137592," << std::endl;
  ss1 << "  -1.168792e-06," << std::endl;
  ss1 << "  -2.421341e-07," << std::endl;
  ss1 << "  -3.197768e-05," << std::endl;
  ss1 << "  -2.119234e-08);" << std::endl;
  ss1 << "sampDenCoef = (" << std::endl;
  ss1 << "  1," << std::endl;
  ss1 << "  -0.0004084112," << std::endl;
  ss1 << "  0.0006661495," << std::endl;
  ss1 << "  -0.0004450848," << std::endl;
  ss1 << "  -2.992829e-05," << std::endl;
  ss1 << "  -2.997011e-07," << std::endl;
  ss1 << "  -2.365301e-07," << std::endl;
  ss1 << "  1.701695e-06," << std::endl;
  ss1 << "  -4.089198e-06," << std::endl;
  ss1 << "  -2.409449e-06," << std::endl;
  ss1 << "  -1.19718e-08," << std::endl;
  ss1 << "  -1.531511e-08," << std::endl;
  ss1 << "  1.499643e-07," << std::endl;
  ss1 << "  0," << std::endl;
  ss1 << "  -3.290989e-08," << std::endl;
  ss1 << "  2.484775e-06," << std::endl;
  ss1 << "  0," << std::endl;
  ss1 << "  0," << std::endl;
  ss1 << "  2.731918e-07," << std::endl;
  ss1 << "  0);" << std::endl;
  ss1 << "END_GROUP = IMAGE" << std::endl;
  ss1 << "END"
      << ";" << std::endl;
  ss1 << "lvcs" << std::endl;
  ss1 << "-84.0884722667" << std::endl;
  ss1 << "39.7765084138" << std::endl;
  ss1 << "232.111831665" << std::endl;

  std::istringstream istr1(ss1.str());
  vpgl_local_rational_camera<double> * cam1_ptr = read_local_rational_camera<double>(istr1);

  ss2 << " satId ="
         "????"
      << ";" << std::endl;
  ss2 << "bandId =" << ' ' << "RGB"
      << ";" << std::endl;
  ss2 << "SpecId =" << ' ' << "RPC00B"
      << ";" << std::endl;
  ss2 << "BEGIN_GROUP = IMAGE" << std::endl;
  ss2 << std::endl;
  ss2 << "  lineOffset = 8787" << std::endl;
  ss2 << "sampOffset = -3257" << std::endl;
  ss2 << "latOffset = 39.7515" << std::endl;
  ss2 << "longOffset = -84.1026" << std::endl;
  ss2 << "heightOffset = 234" << std::endl;
  ss2 << "lineScale = 16996" << std::endl;
  ss2 << "sampScale = 21250" << std::endl;
  ss2 << "latScale = 0.0741" << std::endl;
  ss2 << "longScale = 0.0934" << std::endl;
  ss2 << "heightScale = 500" << std::endl;
  ss2 << "lineNumCoef = (" << std::endl;
  ss2 << "  -5.041263e-05," << std::endl;
  ss2 << "  0.1641781," << std::endl;
  ss2 << "  -1.200941," << std::endl;
  ss2 << "  -0.03680886," << std::endl;
  ss2 << "  -0.0005573231," << std::endl;
  ss2 << "  -1.016026e-06," << std::endl;
  ss2 << "  -6.182713e-05," << std::endl;
  ss2 << "  -0.0001920816," << std::endl;
  ss2 << "  0.0006355286," << std::endl;
  ss2 << "  3.691588e-07," << std::endl;
  ss2 << "  5.29601e-06," << std::endl;
  ss2 << "  1.217832e-05," << std::endl;
  ss2 << "  9.789816e-05," << std::endl;
  ss2 << "  1.642516e-05," << std::endl;
  ss2 << "  -9.576933e-05," << std::endl;
  ss2 << "  -0.0004005543," << std::endl;
  ss2 << "  -0.0001197687," << std::endl;
  ss2 << "  -3.165e-06," << std::endl;
  ss2 << "  -2.048884e-05," << std::endl;
  ss2 << "  -3.660415e-06);" << std::endl;
  ss2 << "lineDenCoef = (" << std::endl;
  ss2 << "  1," << std::endl;
  ss2 << "  -0.0006338235," << std::endl;
  ss2 << "  0.0008238488," << std::endl;
  ss2 << "  -7.561508e-05," << std::endl;
  ss2 << "  0.0001493306," << std::endl;
  ss2 << "  3.267417e-06," << std::endl;
  ss2 << "  -3.597267e-05," << std::endl;
  ss2 << "  6.35075e-05," << std::endl;
  ss2 << "  -0.0003530819," << std::endl;
  ss2 << "  9.881746e-05," << std::endl;
  ss2 << "  4.296455e-06," << std::endl;
  ss2 << "  5.011442e-07," << std::endl;
  ss2 << "  7.187247e-05," << std::endl;
  ss2 << "  -3.75426e-08," << std::endl;
  ss2 << "  -1.114306e-05," << std::endl;
  ss2 << "  -0.0001569543," << std::endl;
  ss2 << "  -2.783627e-07," << std::endl;
  ss2 << "  -3.425017e-07," << std::endl;
  ss2 << "  -1.404321e-05," << std::endl;
  ss2 << "  -1.827016e-08);" << std::endl;
  ss2 << "sampNumCoef = (" << std::endl;
  ss2 << "  0.004249981," << std::endl;
  ss2 << "  1.018208," << std::endl;
  ss2 << "  0.0004369837," << std::endl;
  ss2 << "  0.01776027," << std::endl;
  ss2 << "  -0.0002453736," << std::endl;
  ss2 << "  0.0003724575," << std::endl;
  ss2 << "  -0.0002041859," << std::endl;
  ss2 << "  -0.003993695," << std::endl;
  ss2 << "  -0.0003492056," << std::endl;
  ss2 << "  -6.277124e-07," << std::endl;
  ss2 << "  1.125463e-06," << std::endl;
  ss2 << "  5.478942e-06," << std::endl;
  ss2 << "  -6.188967e-06," << std::endl;
  ss2 << "  -3.34794e-06," << std::endl;
  ss2 << "  2.495365e-05," << std::endl;
  ss2 << "  -1.494333e-05," << std::endl;
  ss2 << "  -1.209495e-07," << std::endl;
  ss2 << "  -2.34347e-06," << std::endl;
  ss2 << "  -1.354266e-06," << std::endl;
  ss2 << "  -6.296676e-08);" << std::endl;
  ss2 << "sampDenCoef = (" << std::endl;
  ss2 << "  1," << std::endl;
  ss2 << "  -0.000267517," << std::endl;
  ss2 << "  0.0002506499," << std::endl;
  ss2 << "  -0.0004546227," << std::endl;
  ss2 << "  -1.888736e-05," << std::endl;
  ss2 << "  1.211527e-06," << std::endl;
  ss2 << "  -3.375621e-07," << std::endl;
  ss2 << "  5.104388e-06," << std::endl;
  ss2 << "  6.113908e-06," << std::endl;
  ss2 << "  -3.577199e-06," << std::endl;
  ss2 << "  6.160238e-08," << std::endl;
  ss2 << "  -2.374394e-08," << std::endl;
  ss2 << "  1.608924e-06," << std::endl;
  ss2 << "  0," << std::endl;
  ss2 << "  -3.628372e-08," << std::endl;
  ss2 << "  -3.843161e-06," << std::endl;
  ss2 << "  -1.322236e-08," << std::endl;
  ss2 << "  -1.027834e-08," << std::endl;
  ss2 << "  -3.352738e-07," << std::endl;
  ss2 << "  0);" << std::endl;
  ss2 << "END_GROUP = IMAGE" << std::endl;
  ss2 << "END"
      << ";" << std::endl;
  ss2 << "lvcs" << std::endl;
  ss2 << "-84.0884722667" << std::endl;
  ss2 << "39.7765084138" << std::endl;
  ss2 << "232.111831665" << std::endl;

  std::istringstream istr2(ss2.str());
  vpgl_local_rational_camera<double> * cam2_ptr = read_local_rational_camera<double>(istr2);
  cam0 = *cam0_ptr;
  cam1 = *cam1_ptr;
  cam2 = *cam2_ptr;
}

static void
test_affine_tensor_transfer()
{
#if HAS_GEOTIFF
  // affine cameras computed from actual rational cameras
  vpgl_local_rational_camera<double> rcam0, rcam1, rcam2;
  rat_cameras(rcam0, rcam1, rcam2);
  size_t ni_0 = 2592, nj_0 = 2913;
  size_t ni_1 = 2408, nj_1 = 2585;
  size_t ni_2 = 2295, nj_2 = 2710;
  std::vector<std::pair<size_t, size_t>> img_dims;
  img_dims.emplace_back(ni_0, nj_0);
  img_dims.emplace_back(ni_1, nj_1);
  img_dims.emplace_back(ni_2, nj_2);
  vpgl_affine_camera<double> acam0, acam1, acam2;
  vgl_point_3d<double> pmin(-100.0, -100.0, 0.0), pmax(1000.0, 1000.0, 100.0);
  vgl_box_3d<double> bb;
  bb.add(pmin);
  bb.add(pmax);
  size_t npoints = 1000;
  bool good0 = vpgl_affine_camera_convert::convert(rcam0, bb, acam0, npoints);
  bool good1 = vpgl_affine_camera_convert::convert(rcam1, bb, acam1, npoints);
  bool good2 = vpgl_affine_camera_convert::convert(rcam2, bb, acam2, npoints);

  vgl_point_3d<double> p3d(0.0, 0.0, 7.0);
  vgl_point_2d<double> p2d0 = acam0.project(p3d);
  vgl_point_2d<double> p2d1 = acam1.project(p3d);
  vgl_point_2d<double> p2d2 = acam2.project(p3d);
  std::cout << "ap0 " << p2d0 << std::endl;
  std::cout << "ap1 " << p2d1 << std::endl;
  std::cout << "ap2 " << p2d2 << std::endl;
  vpgl_affine_tri_focal_tensor<double> aT(acam0, acam1, acam2, img_dims);
  std::cout << "Tensor\n" << aT;
  aT.compute();
  vpgl_affine_fundamental_matrix<double> aF12, aF13, vF12, vF13;
  bool good12 = aT.fmatrix_12(aF12);
  bool good13 = aT.fmatrix_13(aF13);

  TEST("fmatrix_12 success", good12, true);
  TEST("fmatrix_13 success", good13, true);

  vpgl_affine_rectification::compute_affine_f(&acam0, &acam1, vF12);
  vpgl_affine_rectification::compute_affine_f(&acam0, &acam2, vF13);
  vnl_matrix_fixed<double, 3, 3> vFm12 = vF12.get_matrix();
  vnl_matrix_fixed<double, 3, 3> vFm13 = vF13.get_matrix();
  double vfrob_12 = vFm12.frobenius_norm(), vfrob_13 = vFm13.frobenius_norm();
  vFm12 /= vfrob_12;
  vFm13 /= vfrob_13;
  vFm12 /= vFm12[2][2]; // consistent sign
  vFm13 /= vFm13[2][2];
  std::cout << "aF12\n" << aF12 << std::endl;
  std::cout << "vF12\n" << vFm12 << std::endl;
  std::cout << "aF13\n" << aF13 << std::endl;
  std::cout << "vF13\n" << vFm13 << std::endl;
  vnl_matrix_fixed<double, 3, 3> aFm12 = aF12.get_matrix(), aFm13 = aF13.get_matrix();
  double er12 = (vFm12 - aFm12 / aFm12[2][2]).frobenius_norm();
  double er13 = (vFm13 - aFm13 / aFm13[2][2]).frobenius_norm();
  TEST_NEAR("Fundamental matrix from tensor", (er12 + er13) / 9.0, 0.0, 0.01);
  vgl_homg_point_2d<double> e12, e13;
  aT.get_epipoles(e12, e13);

  vgl_homg_point_2d<double> ve12r, ve12l, ve13r, ve13l;
  vF12.get_epipoles(ve12r, ve12l);
  vF13.get_epipoles(ve13r, ve13l);

  vnl_matrix_fixed<double, 3, 3> F12m = aF12.get_matrix(), F13m = aF13.get_matrix();
  vnl_svd<double> svd2{ F12m.as_ref() };
  vnl_vector_fixed<double, 3> e12m = svd2.nullvector();
  double r12 = e12m[0] / e12m[1];
  vnl_svd<double> svd3{ F13m.as_ref() };
  vnl_vector_fixed<double, 3> e13m = svd3.nullvector();
  double r13 = e13m[0] / e13m[1];

  std::cout << "r12 " << r12 << " ve12 " << ve12r.x() / ve12r.y() << std::endl;
  std::cout << "r13 " << r13 << " ve13 " << ve13r.x() / ve13r.y() << std::endl;
  double epi12_er = fabs(r12 - ve12r.x() / ve12r.y());
  double epi13_er = fabs(r13 - ve13r.x() / ve13r.y());
  TEST_NEAR("epipoles 12 and 13 ", (epi12_er + epi13_er) / 2.0, 0.0, 0.02);
  vgl_homg_point_2d<double> hp0(p2d0), hp1(p2d1), hp2(p2d2);
  vgl_homg_point_2d<double> thp0 = aT.image1_transfer(hp1, hp2);
  std::cout << "tr_p0 " << thp0.x() / thp0.w() << ' ' << thp0.y() / thp0.w() << " compared to " << p2d0.x() << ' '
            << p2d0.y() << std::endl;

  vgl_homg_point_2d<double> thp1 = aT.image2_transfer(hp0, hp2);
  std::cout << "tr_p1 " << thp1.x() / thp1.w() << ' ' << thp1.y() / thp1.w() << " compared to " << p2d1.x() << ' '
            << p2d1.y() << std::endl;

  vgl_homg_point_2d<double> thp2 = aT.image3_transfer(hp0, hp1);
  std::cout << "tr_p2 " << thp2.x() / thp2.w() << ' ' << thp2.y() / thp2.w() << " compared to " << p2d2.x() << ' '
            << p2d2.y() << std::endl;
  double er_p0 = fabs(thp0.x() / thp0.w() - p2d0.x()) + fabs(thp0.y() / thp0.w() - p2d0.y());
  double er_p1 = fabs(thp1.x() / thp1.w() - p2d1.x()) + fabs(thp1.y() / thp1.w() - p2d1.y());
  double er_p2 = fabs(thp2.x() / thp2.w() - p2d2.x()) + fabs(thp2.y() / thp2.w() - p2d2.y());
  TEST_NEAR("Tri focal transfer", (er_p0 + er_p1 + er_p2) / 3.0, 0.0, 0.25);
  // shift hp0
  vnl_matrix_fixed<double, 3, 3> zero = aT.point_constraint_3x3(hp0, hp1, hp2);
  double zero_scalar = aT.point_constraint(hp0, hp1, hp2);
  std::cout << "zero\n" << zero << std::endl;
  double ezero = (zero.frobenius_norm() / 9.0 + zero_scalar) / 2.0;
  TEST_NEAR("Tri focal 3x3 point constraint", ezero, 0.0, 1e-05);
  std::cout << "====3d point ==== " << p3d << std::endl;
  for (double del = -0.5; del < 0.5; del += 0.05)
  {
    vgl_homg_point_2d<double> hp0p(hp0.x() + del, hp0.y() + del);
    vgl_homg_point_2d<double> hp1p(hp1.x() + del, hp1.y() + del);
    vgl_homg_point_2d<double> hp2p(hp2.x() + del, hp2.y() + del);
    vnl_matrix_fixed<double, 3, 3> zerop = aT.point_constraint_3x3(hp0, hp1p, hp2);
    double norm = 0.0;
    for (size_t r = 0; r < 3; ++r)
      for (size_t c = 0; c < 3; ++c)
        norm += fabs(zerop[r][c]);
    double scalar_zero = aT.point_constraint(hp0, hp1p, hp2);
    std::cout << del << ' ' << fabs(scalar_zero) << ' ' << norm / 9.0 << std::endl;
  }
  vnl_matrix_fixed<double, 3, 1> mp0, mp1, mp2;

  mp0[0][0] = hp0.x();
  mp0[1][0] = hp0.y();
  mp0[2][0] = hp0.w();
  mp1[0][0] = hp1.x();
  mp1[1][0] = hp1.y();
  mp1[2][0] = hp1.w();
  mp2[0][0] = hp2.x();
  mp2[1][0] = hp2.y();
  mp2[2][0] = hp2.w();
  vnl_matrix_fixed<double, 1, 3> mp1_t = mp1.transpose(), mp2_t = mp2.transpose();
  // epipolar constraints
  vnl_matrix_fixed<double, 1, 1> er_epicon_12 = mp1_t * F12m * mp0;
  vnl_matrix_fixed<double, 1, 1> er_epicon_13 = mp2_t * F13m * mp0;
  double epi_er = er_epicon_12[0][0] + er_epicon_13[0][0];
  TEST_NEAR("epipolar constraint", epi_er / 2.0, 0.0, 0.005);
  vgl_homg_point_3d<double> hp3d_2(540.243, 78.4755, 53.832);
  vgl_homg_point_2d<double> hp0_2 = acam0.project(hp3d_2);
  vgl_homg_point_2d<double> hp1_2 = acam1.project(hp3d_2);
  vgl_homg_point_2d<double> hp2_2 = acam2.project(hp3d_2);
  std::cout << "======3d point====== " << hp3d_2 << std::endl;
  for (double del = -0.5; del < 0.5; del += 0.05)
  {
    vgl_homg_point_2d<double> hp0_2p(hp0_2.x() + del, hp0_2.y() + del);
    vgl_homg_point_2d<double> hp1_2p(hp1_2.x() + del, hp1_2.y() + del);
    vgl_homg_point_2d<double> hp2_2p(hp2_2.x() + del, hp2_2.y() + del);
    vnl_matrix_fixed<double, 3, 3> zerop = aT.point_constraint_3x3(hp0_2p, hp1_2, hp2_2);
    double norm = 0.0;
    for (size_t r = 0; r < 2; ++r)
      for (size_t c = 0; c < 2; ++c)
        norm += fabs(zerop[r][c]);
    double scalar_zero = aT.point_constraint(hp0_2p, hp1_2, hp2_2);
    std::cout << del << ' ' << fabs(scalar_zero) << ' ' << norm / 9.0 << std::endl;
  }
  vnl_matrix_fixed<double, 3, 3> zero_2 = aT.point_constraint_3x3(hp0_2, hp1_2, hp2_2);
  std::cout << "zero_2\n" << zero_2 << std::endl;

  vgl_homg_point_2d<double> thp0_2 = aT.image1_transfer(hp1_2, hp2_2);
  std::cout << "tr_p0 " << thp0_2.x() / thp0_2.w() << ' ' << thp0_2.y() / thp0_2.w() << " compared to "
            << hp0_2.x() / hp0_2.w() << ' ' << hp0_2.y() / hp0_2.w() << std::endl;

  vgl_homg_point_2d<double> thp1_2 = aT.image2_transfer(hp0_2, hp2_2);
  std::cout << "tr_p1 " << thp1_2.x() / thp1_2.w() << ' ' << thp1_2.y() / thp1_2.w() << " compared to "
            << hp1_2.x() / hp0_2.w() << ' ' << hp1_2.y() / hp0_2.w() << std::endl;

  vgl_homg_point_2d<double> thp2_2 = aT.image3_transfer(hp0_2, hp1_2);
  std::cout << "tr_p2 " << thp2_2.x() / thp2_2.w() << ' ' << thp2_2.y() / thp2_2.w() << " compared to "
            << hp2_2.x() / hp2_2.w() << ' ' << hp2_2.y() / hp2_2.w() << std::endl;

  vgl_homg_point_3d<double> hp3d_3(57.804, -60.4263, 24.1643);
  vgl_homg_point_2d<double> hp0_3 = acam0.project(hp3d_3);
  vgl_homg_point_2d<double> hp1_3 = acam1.project(hp3d_3);
  vgl_homg_point_2d<double> hp2_3 = acam2.project(hp3d_3);
  vnl_matrix_fixed<double, 3, 3> zero_3 = aT.point_constraint_3x3(hp0_3, hp1_3, hp2_3);
  std::cout << "zero_3\n" << zero_3 << std::endl;
  vgl_homg_point_2d<double> thp0_3 = aT.image1_transfer(hp1_3, hp2_3);
  std::cout << "tr_p0 " << thp0_3.x() / thp0_3.w() << ' ' << thp0_3.y() / thp0_3.w() << " compared to "
            << hp0_3.x() / hp0_3.w() << ' ' << hp0_3.y() / hp0_3.w() << std::endl;
  vgl_homg_point_2d<double> thp1_3 = aT.image2_transfer(hp0_3, hp2_3);
  std::cout << "tr_p1 " << thp1_3.x() / thp1_3.w() << ' ' << thp1_3.y() / thp1_3.w() << " compared to "
            << hp1_3.x() / hp0_3.w() << ' ' << hp1_3.y() / hp0_3.w() << std::endl;
  vgl_homg_point_2d<double> thp2_3 = aT.image3_transfer(hp0_3, hp1_3);
  std::cout << "tr_p2 " << thp2_3.x() / thp2_3.w() << ' ' << thp2_3.y() / thp2_3.w() << " compared to "
            << hp2_3.x() / hp2_3.w() << ' ' << hp2_3.y() / hp2_3.w() << std::endl;
  // construct a line in image 3
  vgl_homg_line_2d<double> line3(hp2_2, hp2_3);
  line3.normalize();
  // test the resulting homography between images 1 and 2
  vgl_h_matrix_2d<double> H12 = aT.hmatrix_12(line3);
  vgl_homg_point_2d<double> hp1_gt(1801.64, 2515.54);
  vgl_point_2d<double> p2_gt(1674.99, 2180.48);
  vgl_homg_point_2d<double> Hhp2 = H12 * hp1_gt;
  vgl_point_2d<double> Hep2(Hhp2);
  std::cout << "Hep2 " << Hep2 << " compared to " << p2_gt << std::endl;
  double er_H12 = (Hep2 - p2_gt).length();
  TEST_NEAR("Homography from 1 to 2", er_H12, 0.0, 0.5);

  // construct a line in image 3
  vgl_homg_line_2d<double> line2(hp1, hp1_2);
  line2.normalize();
  // test the resulting homography between images 1 and 2
  vgl_h_matrix_2d<double> H13 = aT.hmatrix_13(line2);
  vgl_point_2d<double> p3_gt(1603.97, 2249.03);
  vgl_homg_point_2d<double> Hhp3 = H13 * hp1_gt;
  vgl_point_2d<double> Hep3(Hhp3);
  std::cout << "Hep3 " << Hep3 << " compared to " << p3_gt << std::endl;
  double er_H13 = (Hep3 - p3_gt).length();
  TEST_NEAR("Homography from 1 to 3", er_H13, 0.0, 0.5);
#endif
}
TESTMAIN(test_affine_tensor_transfer);
