#include <iostream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_fwd.h"
#include "vnl/vnl_random.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_box_3d.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/algo/vpgl_ray.h>
#include "vpgl/vpgl_rational_camera.h"
#include "vpgl/vpgl_perspective_camera.h"
#include "vpgl/vpgl_generic_camera.h"
#include "vul/vul_timer.h"
#include "vpgl/vpgl_RSM_camera.h" //replacement sensor model
// a rational camera from a commercial satellite image
vpgl_rational_camera<double>
construct_rational_camera()
{
  double n_u[20] = { 3.89528e-006,  -7.41303e-008, 1.25847e-005,  0.00205724,    5.38537e-008,
                     -6.45546e-007, -9.52482e-005, 1.50544e-007,  0.000665835,   1.00827,
                     -1.58873e-006, -3.85506e-007, 9.73561e-006,  -6.13083e-007, -0.000361219,
                     0.0238256,     9.65955e-008,  -1.94861e-005, 0.0155788,     -0.0040918 };

  double d_u[20] = { -1.68064e-006, -9.85769e-007, 3.49708e-005,  4.28561e-006,  2.25889e-007,
                     -1.49309e-007, -3.56398e-007, 5.07889e-007,  -4.95792e-006, 0.00204326,
                     3.18231e-008,  -8.82645e-008, -1.68424e-007, 2.79561e-007,  -1.87894e-007,
                     0.000199085,   -1.71801e-007, -4.7099e-007,  -0.000610182,  1 };

  double n_v[20] = { -1.21292e-006, 7.83222e-006,  -6.63094e-005, -0.000224627,  -4.85207e-006,
                     -2.07948e-006, -1.55515e-005, -3.03478e-007, -6.18288e-005, 0.0318755,
                     4.11169e-005,  -4.28023e-007, 0.000196132,   1.06738e-005,  0.000307117,
                     -0.960259,     -9.56042e-007, 2.42754e-005,  -0.0712854,    -0.000395718 };

  double d_v[20] = { 1.44065e-005,  -2.47213e-006, -8.52173e-006, -1.28085e-005, 4.81707e-006,
                     1.50614e-006,  -5.23036e-006, 1.4626e-006,   -3.12355e-007, -1.20281e-006,
                     -3.51175e-005, -6.15114e-006, 2.89264e-005,  1.55209e-006,  1.07945e-005,
                     -0.000160384,  -1.15965e-007, -7.89301e-006, 0.000275139,   1 };

  vnl_matrix_fixed<double, 4, 20> cmatrix;
  for (unsigned i = 0; i < 20; ++i)
  {
    cmatrix[0][i] = n_u[i];
    cmatrix[1][i] = d_u[i];
    cmatrix[2][i] = n_v[i];
    cmatrix[3][i] = d_v[i];
  }
  // The scales and offsets
  vpgl_scale_offset<double> sox(0.0347, -71.4049);
  vpgl_scale_offset<double> soy(0.0219, 41.8216);
  vpgl_scale_offset<double> soz(501, -30);
  vpgl_scale_offset<double> sou(4764, 4693);
  vpgl_scale_offset<double> sov(4221, 3921);
  std::vector<vpgl_scale_offset<double>> scale_offsets;
  scale_offsets.push_back(sox);
  scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);
  scale_offsets.push_back(sou);
  scale_offsets.push_back(sov);
  // Construct the rational camera
  vpgl_rational_camera<double> rat_cam(cmatrix, scale_offsets);
  return rat_cam;
}

// local rational camera for testing the generic camera conversion
vpgl_local_rational_camera<double>
construct_local_rational_camera()
{
  double n_u[20] = { -7.81964e-006, 6.39341e-005,  4.67275e-006,  0.00423585,   -9.69009e-005,
                     -5.10745e-006, -0.00114887,   -1.64125e-005, 0.000489928,  0.993274,
                     -0.000131889,  -4.58868e-006, 0.000902171,   2.37398e-007, -0.000430274,
                     -0.00825765,   2.42847e-007,  -1.07917e-005, -0.0143585,   -0.00327465 };
  double d_u[20] = { 7.0187e-008,
                     -5.40373e-007,
                     0,
                     -2.08288e-005,
                     -1.0929e-006,
                     -8.71068e-008,
                     -4.65334e-005,
                     8.89371e-008,
                     -3.75089e-006,
                     -0.000956589,
                     5.16632e-007,
                     -1.30439e-007,
                     7.87054e-005,
                     -4.07253e-008,
                     2.88935e-006,
                     0.00111511,
                     2.12671e-008,
                     -1.69864e-005,
                     -0.000537438,
                     1 };
  double n_v[20] = { 9.93585e-007, 4.86672e-006, 2.26215e-007, -0.000188252, 5.27564e-005, -1.5669e-007, 0.00153582,
                     6.71638e-007, 2.8673e-005,  -0.0486115,   3.90973e-005, 5.30928e-006, -0.00419213,  1.40611e-005,
                     0.000269255,  -1.03953,     1.22934e-007, 3.19812e-006, -0.00884028,  0.00575278 };
  double d_v[20] = { 8.51615e-007,  6.74855e-007,  1.26254e-007,  -1.59186e-005, 1.97482e-005,
                     -4.07044e-007, 3.29326e-005,  -1.30921e-008, -6.1929e-007,  0.0018664,
                     0.000136792,   -2.30798e-006, 4.13577e-005,  -1.38488e-007, 2.19449e-006,
                     0.00166379,    8.0006e-008,   -1.38346e-005, 0.000332436,   1 };
  vnl_matrix_fixed<double, 4, 20> cmatrix;
  for (unsigned i = 0; i < 20; ++i)
  {
    cmatrix[0][i] = n_u[i];
    cmatrix[1][i] = d_u[i];
    cmatrix[2][i] = n_v[i];
    cmatrix[3][i] = d_v[i];
  }

  // The scales and offsets
  vpgl_scale_offset<double> sox(0.0986, 44.4692);
  vpgl_scale_offset<double> soy(0.1058, 33.252);
  vpgl_scale_offset<double> soz(500, 34);
  vpgl_scale_offset<double> sou(13982, 12782.0673959);
  vpgl_scale_offset<double> sov(17213, 13798.1995849);
  std::vector<vpgl_scale_offset<double>> scale_offsets;
  scale_offsets.push_back(sox);
  scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);
  scale_offsets.push_back(sou);
  scale_offsets.push_back(sov);
  vpgl_lvcs lvcs(33.331465, 44.37697, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // Construct the rational camera
  vpgl_rational_camera<double> rat_cam(cmatrix, scale_offsets);
  // Construct the local rational camera
  vpgl_local_rational_camera<double> loc_rat_cam(lvcs, rat_cam);
  return loc_rat_cam;
}
vpgl_RSM_camera<double>
construct_replacement_sensor_model()
{
  std::vector<double> RNC = {
    -1.80499632096923e-04, -6.01457292323314e-02, -2.46123524472040e-04, +1.99192284960173e-06, -9.73587822908267e-01,
    +2.46668232138735e-04, -9.16103111931478e-07, +0.00000000000000e+00, +1.10919209893124e-05, +4.71933779624485e-06,
    +0.00000000000000e+00, +0.00000000000000e+00, -1.59867374037021e-08, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +5.11972425640570e-05, +1.37341658928740e-07, -2.03583433842386e-08, +0.00000000000000e+00,
    +1.68246013730763e-06, +9.54013489976635e-08, +0.00000000000000e+00, +0.00000000000000e+00, -1.28023508550997e-08,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, -2.13802517105284e-09, +1.02558278625126e-06, +0.00000000000000e+00,
    +0.00000000000000e+00, +1.55737956748011e-05, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, -1.09300375513605e-09, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00
  };

  std::vector<double> RDC = {
    +1.00000000000000e+00, +2.11672171239780e-04, -4.90299049386640e-06, -5.60493238759316e-08, -1.54273561761783e-04,
    +1.84198786124959e-06, +1.38931509256789e-07, +0.00000000000000e+00, -2.35319968881882e-06, -3.47384885781166e-07,
    +0.00000000000000e+00, +0.00000000000000e+00, -1.25291589427200e-06, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +1.11434414352537e-06, -6.12311842092910e-08, +2.43759674475174e-10, +0.00000000000000e+00,
    +4.08378352638849e-09, -2.09715939394377e-10, +0.00000000000000e+00, +0.00000000000000e+00, +2.95227400081906e-10,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, -1.60496332290470e-05, -2.47457995907950e-09, +0.00000000000000e+00,
    +0.00000000000000e+00, +7.11447737557060e-10, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, -4.32956793831398e-11, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00
  };

  std::vector<double> CNC = {
    +1.22165721663487e-02, +3.46007967690924e+00, -1.04127165466321e-02, -1.73398510100797e-03, -2.53203564155699e+00,
    +1.93550552015899e-03, -8.48212519563269e-04, +0.00000000000000e+00, -5.28342345496189e-03, -5.41764166496688e-05,
    +0.00000000000000e+00, +0.00000000000000e+00, +1.12625227491183e-03, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, -1.55617131343446e-02, +2.02457190162080e-04, +3.63401389960726e-05, +0.00000000000000e+00,
    -1.75745406764039e-04, +2.03212007754942e-05, +0.00000000000000e+00, +0.00000000000000e+00, -1.71102017542165e-05,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +7.98545075064823e-07, -3.94028164558276e-05, +0.00000000000000e+00,
    +0.00000000000000e+00, +2.85798540257351e-05, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +1.76227608236396e-07, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00
  };

  std::vector<double> CDC = {
    +1.00000000000000e+00, -5.20930000911596e-03, -4.71261930078356e-04, +5.04051923393812e-07, +5.46414343476839e-03,
    -6.60695582997035e-04, -1.07630603111371e-06, +0.00000000000000e+00, -4.02962242584699e-04, -5.67439054720973e-06,
    +0.00000000000000e+00, +0.00000000000000e+00, -4.72637243548261e-06, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, -9.79599192944519e-04, +1.12282270406520e-05, +4.92486517524047e-07, +0.00000000000000e+00,
    +5.59276654575213e-06, +6.70098164352108e-07, +0.00000000000000e+00, +0.00000000000000e+00, +4.88244737520739e-07,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, -1.14332181032924e-05, +1.26748243297999e-09, +0.00000000000000e+00,
    +0.00000000000000e+00, -7.50385938154649e-08, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +1.19426855179968e-08, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00,
    +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00, +0.00000000000000e+00
  };
  std::vector<double> num_u_coeff = CNC;
  std::vector<double> den_u_coeff = CDC;
  std::vector<double> num_v_coeff = RNC;
  std::vector<double> den_v_coeff = RDC;
  std::vector<std::vector<double>> coeffs;
  coeffs.push_back(num_u_coeff);
  coeffs.push_back(den_u_coeff);
  coeffs.push_back(num_v_coeff);
  coeffs.push_back(den_v_coeff);
  std::vector<std::vector<int>> powers(4, std::vector<int>(3, 3));
  vpgl_scale_offset<double> sox(0.0029411689968438198, 0.77378520899138403);
  vpgl_scale_offset<double> soy(0.0080796904012988002, 0.58005546402463803);
  vpgl_scale_offset<double> soz(657.20053824177000, 484.65013863099699);
  vpgl_scale_offset<double> sou(148.00000000000000, 128.00000000000000);
  vpgl_scale_offset<double> sov(1696.0000000000000, 1676.0000000000000);
  std::vector<vpgl_scale_offset<double>> scale_offsets;
  scale_offsets.push_back(sox);
  scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);
  scale_offsets.push_back(sou);
  scale_offsets.push_back(sov);
  vpgl_polycam<double> pcam(1, 1, powers, coeffs, scale_offsets);
  vpgl_region_selector<double> rsel;     // rnis ==1 , cnis == 1
  vpgl_RSM_camera<double> RSM_cam(rsel); // intializes polycam storage
  RSM_cam.set_polycam(1, 1, pcam);
  return RSM_cam;
}
void
test_rational_camera_approx_perspective()
{
  vpgl_rational_camera<double> rat_cam = construct_rational_camera();
  std::cout << rat_cam;
  vpgl_scale_offset<double> sox = rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy = rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz = rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX);
#if 0 // unused
  vpgl_scale_offset<double> sou = rat_cam.scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov = rat_cam.scl_off(vpgl_rational_camera<double>::V_INDX);
#endif
  vgl_point_3d<double> pmin(sox.offset() - sox.scale(), soy.offset() - soy.scale(), 0);
  vgl_point_3d<double> pmax(sox.offset() + sox.scale(), soy.offset() + soy.scale(), soz.scale());
  vgl_box_3d<double> approx_vol(pmin, pmax);

  vpgl_perspective_camera<double> pc;

  vgl_h_matrix_3d<double> norm_trans;
  vpgl_perspective_camera_convert::convert(rat_cam, approx_vol, pc, norm_trans);
  std::cout << "Test Result\n" << pc;
  TEST("vpgl_perspective_camera_convert", pc.get_calibration().get_matrix()(0, 1), 0);
  std::cout << '\n';
}

void
test_rational_camera_approx_affine()
{
#if HAS_GEOTIFF // Requires GEOTIFF to succeed
  vpgl_local_rational_camera<double> loc_rat_cam = construct_local_rational_camera();

  // a 500x500x100 roi centered at the lvcs origin
  vgl_point_3d<double> roi_center(0, 0, 0);
  vgl_point_3d<double> roi_min = roi_center - vgl_vector_3d<double>(250, 250, 0);
  vgl_point_3d<double> roi_max = roi_center + vgl_vector_3d<double>(250, 250, 100);
  vgl_box_3d<double> roi(roi_min, roi_max);

  vpgl_affine_camera<double> ac;
  bool success = vpgl_affine_camera_convert::convert(loc_rat_cam, roi, ac);

  TEST("vpgl_affine_camera_convert success", success, true);

  // project center and corners of volume into image

  std::vector<vgl_point_3d<double>> test_points{ roi_center, roi_min, roi_max };
  for (const vgl_point_3d<double> & test_pt : test_points)
  {
    vgl_point_2d<double> img_pt1 = loc_rat_cam.project(test_pt);
    vgl_point_2d<double> img_pt2 = ac.project(test_pt);
    double dist = (img_pt2 - img_pt1).length();
    // test to within one pixel
    TEST_NEAR("vpgl_affine_camera_convert projection", dist, 0.0, 0.5);
  }
#endif
}
void
test_replacement_sensor_model_camera_approx_affine()
{
#if HAS_GEOTIFF // Requires GEOTIFF to succeed
  vpgl_RSM_camera<double> rsm_cam = construct_replacement_sensor_model();
  if (rsm_cam.reg_sel().maxr_ == 0)
    return;
  // define lvcs
  vpgl_lvcs lvcs(33.315252, 44.366044, 35.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // a 500x500x100 roi centered at the lvcs origin
  vgl_point_3d<double> roi_center(0, 0, 0);
  vgl_point_3d<double> roi_min = roi_center - vgl_vector_3d<double>(250, 250, 0);
  vgl_point_3d<double> roi_max = roi_center + vgl_vector_3d<double>(250, 250, 100);
  vgl_box_3d<double> roi(roi_min, roi_max);

  vpgl_affine_camera<double> ac;
  bool success = vpgl_affine_camera_convert::convert(rsm_cam, lvcs, roi, ac);

  TEST("vpgl_affine_camera_convert success", success, true);

  // project center and corners of volume into image

  std::vector<vgl_point_3d<double>> test_points{ roi_center, roi_min, roi_max };
  for (const vgl_point_3d<double> & test_pt : test_points)
  {
    double X, Y, Z;
    lvcs.local_to_global(test_pt.x(), test_pt.y(), test_pt.z(), vpgl_lvcs::wgs84, X, Y, Z);
    X /= vnl_math::deg_per_rad;
    Y /= vnl_math::deg_per_rad;
    vgl_point_3d<double> tpg(X, Y, Z);
    vgl_point_2d<double> img_pt1 = rsm_cam.project(tpg);
    vgl_point_2d<double> img_pt2 = ac.project(test_pt);
    double dist = (img_pt2 - img_pt1).length();
    // test to within one pixel
    TEST_NEAR("vpgl_affine_camera_convert from RSM projection", dist, 0.0, 0.5);
  }
#endif
}

void
test_generic_camera_convert()
{
  //==================================================//
  // test construction from a local rational camera   //
  //==================================================//
  vpgl_local_rational_camera<double> lcam = construct_local_rational_camera();
  unsigned ni = 833, nj = 877;
  vul_timer t;
  vpgl_generic_camera<double> gcam;
  bool success = vpgl_generic_camera_convert::convert(lcam, ni, nj, gcam);
  std::cout << " converted generic cam in " << t.real() / 1000.0 << " secs.\n";
  TEST("convert generic cam", success, true);
  if (success)
  {
    double tu = 433, tv = 325;
    double x = 457.0765, y = 526.2103, z = 34.68;
    double u0 = 0, v0 = 0;
    lcam.project(x, y, z, u0, v0);
    std::cout << "(u0 v0)=(" << u0 << ' ' << v0 << ")\n";
    vgl_ray_3d<double> lray, gray;
    success = vpgl_ray::ray(lcam, tu, tv, lray);
    gray = gcam.ray(tu, tv);
    vgl_point_3d<double> lorg = lray.origin(), gorg = gray.origin();
    vgl_vector_3d<double> ldir = lray.direction(), gdir = gray.direction();
    double dorg = (lorg - gorg).length();
    dorg -= 265.285;
    double dang = std::fabs(angle(ldir, gdir));
    TEST("lcam rays", success && std::fabs(dorg) < 0.1 && dang < 0.001, true);
    double u, v;
    gcam.project(x, y, z, u, v);
    std::cout << "(u v)=(" << u << ' ' << v << ")\n";
    double del = std::fabs(u - u0) + std::fabs(v - v0);
    TEST_NEAR("test ray projection at center", del, 0.0, 0.5);
    // four corners
    int ua[4] = { 0, 832, 0, 832 };
    int va[4] = { 0, 0, 876, 876 };
    dorg = 0.0;
    dang = 0.0;
    // DEC reduced z range by 1/2 so need the following delta
    vgl_vector_3d<double> delta(72.792, -51, 250);
    for (unsigned k = 0; k < 4; ++k)
    {
      gray = gcam.ray(ua[k], va[k]);
      success = success && vpgl_ray::ray(lcam, ua[k], va[k], lray);
      lorg = lray.origin() - delta, gorg = gray.origin();
      ldir = lray.direction(), gdir = gray.direction();
      dorg += (lorg - gorg).length();
      dang += std::fabs(angle(ldir, gdir));
    }
    TEST("corner rays", success && dorg < 1.1 && dang < 0.001, true);
  }
  else
  {
    TEST("local rational to generic", false, true);
  }
  //==================================================//
  // test construction from a projective camera //
  //==================================================//
  vnl_matrix_fixed<double, 3, 4> m, C;
  vnl_matrix_fixed<double, 3, 3> K;
  // identity camera, [I|0]
  m[0][0] = 1.0;
  m[0][1] = 0.0;
  m[0][2] = 0.0;
  m[0][3] = 0.0;
  m[1][0] = 0.0;
  m[1][1] = 1.0;
  m[1][2] = 0.0;
  m[1][3] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  m[2][2] = 1.0;
  m[2][3] = 0.0;
  // 10 x 10 image
  K[0][0] = 1.0;
  K[0][1] = 0.0;
  K[0][2] = 5.0;
  K[1][0] = 0.0;
  K[1][1] = 1.0;
  K[1][2] = 5.0;
  K[2][0] = 0.0;
  K[2][1] = 0.0;
  K[2][2] = 1.0;
  C = K * m;
  vpgl_proj_camera<double> icam(C);
  ni = 10;
  nj = 10;
  success = vpgl_generic_camera_convert::convert(icam, ni, nj, gcam);
  if (success)
  {
    vgl_ray_3d<double> rayc = gcam.ray(5, 5), ray0 = gcam.ray(0, 0);
    vgl_point_3d<double> org(0, 0, 0);
    double eorg = (rayc.origin() - org).length();
    eorg += (ray0.origin() - org).length();
    vgl_vector_3d<double> z(0.0, 0.0, 1.0), cdir(-5.0, -5.0, 1.0);
    double edir = std::fabs(angle(z, rayc.direction()));
    edir += std::fabs(angle(cdir, ray0.direction()));
    TEST_NEAR("proj to generic camera", eorg + edir, 0.0, 1.0e-6);
  }
  else
  {
    TEST("proj cam to generic", false, true);
  }
  //==================================================//
  // test construction from an affine camera //
  //==================================================//
  vgl_vector_3d<double> view_dir(0.0, 0.0, -1.0);
  vgl_vector_3d<double> up(0.0, 0.0, 1.0);
  vgl_point_3d<double> stare(0.0, 0.0, 0.0);
  vpgl_affine_camera<double> acam(view_dir, up, stare, 5, 5, 1.0, 1.0);
  acam.set_viewing_distance(1.0);
  vgl_homg_point_2d<double> ipt(0, 0, 1);
  acam.backproject(ipt);
  ipt.set(5, 5, 1);
  acam.backproject(ipt);
  success = vpgl_generic_camera_convert::convert(acam, ni, nj, gcam);
  if (success)
  {
    vgl_ray_3d<double> rayc = gcam.ray(5, 5), ray0 = gcam.ray(0, 0);
    vgl_point_3d<double> orgc(0.0, 0.0, 1.0), org0(-5.0, -5.0, 1.0);
    double eorg = (rayc.origin() - orgc).length();
    eorg += (ray0.origin() - org0).length();
    double edir = (view_dir - rayc.direction()).length();
    TEST_NEAR("Affine to generic camera", eorg + edir, 0.0, 1.0e-6);
  }
  else
  {
    TEST("affine cam to generic", false, true);
  }
}
void
test_rational_camera_convert()
{
  //==================================================//
  // test construction from a ReplacementSensorModel camera   //
  //==================================================//
  vpgl_RSM_camera<double> rsm_cam = construct_replacement_sensor_model();
  vgl_point_3d<double> min_pt(44.254166, 32.77861, -100);
  vgl_point_3d<double> max_pt(44.41305, 33.6911, +100);
  vgl_box_3d<double> box;
  box.add(min_pt);
  box.add(max_pt);
  vpgl_rational_camera<double> rcam;
  vpgl_rational_camera_convert::convert(rsm_cam, box, rcam);
  vnl_random rng;
  double min_x = box.min_x(); // longitude degrees
  double min_y = box.min_y(); // latitude degrees
  double min_z = box.min_z(); // elevation meters
  double max_x = box.max_x();
  double max_y = box.max_y();
  double max_z = box.max_z();
  // test n points
  unsigned n = 100;
  std::vector<vgl_point_2d<double>> image_pts;
  std::vector<vgl_point_3d<double>> world_pts;
  double max_error = 0.0;
  for (unsigned i = 0; i < n; ++i)
  {
    double x = rng.drand64(min_x, max_x); // sample in local coords
    double y = rng.drand64(min_y, max_y);
    double z = rng.drand64(min_z, max_z);
    // convert to radians
    double X = x / vnl_math::deg_per_rad, Y = y / vnl_math::deg_per_rad;

    double uRSM, vRSM, uRPC, vRPC;
    rsm_cam.project(X, Y, z, uRSM, vRSM);
    rcam.project(x, y, z, uRPC, vRPC);
    double eru = fabs(uRPC - uRSM), erv = fabs(vRPC - vRSM);
    double er = eru + erv;
    if (er > max_error)
      max_error = er;
  }

  TEST_NEAR("RSM to rational camera convert", max_error, 0.0, 1.0e-3);
}

static void
test_camera_convert()
{
  test_rational_camera_approx_perspective();
  test_rational_camera_approx_affine();
  test_replacement_sensor_model_camera_approx_affine();
  std::cout << "=== End test_rational_camera_approx" << std::endl;
  test_generic_camera_convert();
  test_rational_camera_convert();
}

TESTMAIN(test_camera_convert);
