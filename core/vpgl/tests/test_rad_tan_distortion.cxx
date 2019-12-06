#include <iostream>
#include <exception>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vpgl/vpgl_radial_tangential_distortion.h"
#include "vpgl/vpgl_calibration_matrix.h"
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_homg_point_2d.h"

static void
test_rad_tan_distortion()
{
  std::vector<double> k;
  k.push_back(-0.60150605440139770508);
  k.push_back(4.70203733444213867188);
  double                                    p1 = -0.00047452122089453042;
  double                                    p2 = -0.00782289821654558182;
  vpgl_radial_tangential_distortion<double> rdt(k, p1, p2);
  vgl_homg_point_2d<double>                 hp(0.7, 0.5);
  vgl_homg_point_2d<double>                 dhp = rdt.distort(hp);
  vgl_homg_point_2d<double>                 uhp = rdt.undistort(dhp);
  double                                    d = fabs(uhp.x() - hp.x()) + fabs(uhp.y() - hp.y());
  TEST_NEAR("undistort", d, 0, 0.0001);
  // check distort computation with simple coefficients
  k.clear();
  k.resize(6, 0.1);
  p1 = -0.01;
  p2 = -0.02;
  vpgl_radial_tangential_distortion<double> edt(k, p1, p2);
  vgl_homg_point_2d<double>                 he(1.0, 2.0);
  vgl_homg_point_2d<double>                 dhe = edt.distort(he);
  double                                    er = fabs(dhe.x() - 0.82) + fabs(dhe.y() - 1.79);
  TEST_NEAR("distort with all coefs", er, 0, 0.0001);
  vgl_point_2d<double>            pp(959.5, 539.5);
  vpgl_calibration_matrix<double> K(2696.35888671875, pp);
  // point on focal plane
  vgl_point_2d<double> pf(0.35566, 0.2), pu_img, pd_img;
  pu_img = K.map_to_image(pf);
  vgl_homg_point_2d<double> dpf = rdt.distort(vgl_homg_point_2d<double>(pf));
  pd_img = K.map_to_image(vgl_point_2d<double>(dpf));
  vgl_homg_point_2d<double> dis_pix(pd_img), undis_pix;
  undis_pix = rdt.undistort_pixel(dis_pix, K);
  double pix_er = (vgl_point_2d<double>(undis_pix) - pu_img).length();
  TEST_NEAR("undistort pixel", pix_er, 0, 0.0001);
  vgl_point_2d<double> dist_pix(rdt.distort_pixel(vgl_homg_point_2d<double>(pu_img), K));
  double               dist_pix_er = (dist_pix - pd_img).length();
  TEST_NEAR("distort pixel", dist_pix_er, 0, 0.0001);
}


TESTMAIN(test_rad_tan_distortion);
