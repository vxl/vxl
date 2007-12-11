#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_rational_geo_adjust.h>

static void test_rational_geo_adjust()
{
  //Make the rational cameras
  //Rational polynomial coefficients
  //from digital globe image  02JUL05075233-P1BS-005630614010_01_P001
  double neu_u[20] =
    {-1.45002e-005, 8.88441e-005, -1.69845e-006,  0.000125672,
     -1.3334e-005,  1.66753e-006,  -0.00191472,  -5.49752e-006,
      0.000626966,  1.00493,        0.000149249,  5.56542e-007,
     -0.00243291,  -4.19472e-007,  -0.000322203,  0.00751094,
      0,            1.0456e-005,    0.00335227,  -0.00171778};
  double den_u[20] =
    { -1.11389e-007,  1.12986e-007,  2.60026e-008,  1.37888e-006,
       1.59082e-006, -2.23928e-007, -4.61017e-005,  0,
       2.26253e-006,  0.00161218,   -1.99999e-006,  3.05839e-007,
      -4.65254e-006, -3.57289e-008, -1.38706e-006,  0.00195803,
       1.95078e-008, -6.02895e-006, -0.000621955,   1};
  double neu_v[20] =
    { -1.34506e-007,  3.30571e-006,  1.12006e-007, -0.000720573,
       2.18505e-005, -1.04205e-006, -6.89949e-005,  6.89221e-008,
       7.62809e-006, -0.0233525,     1.04041e-005, -1.01309e-005,
       0.00294398,    2.71637e-006,  7.06142e-005, -1.00083,
      -6.37659e-008, -4.99594e-006,  0.0227909,    -0.00222785};
  double den_v[20] =
    { 5.17035e-008,  1.79981e-007,  -1.70495e-008, -2.0714e-006,
      3.631e-006,   -9.81742e-008,   1.38937e-005,  0,
     -5.91773e-007, -0.000137415,    0.000102628,  -3.96336e-006,
     -2.34085e-006,  6.79756e-008,  -3.48424e-006,  0.000686196,
      0,            -2.33034e-006,   0.000124036,   1 };
  //Scale and offsets

  double sx1 = 0.1038, ox1 = 44.3526 ;
  double sy1 = 0.1131, oy1 = 33.3405;
  double sz1 = 501,            oz1 = 35.0;
  double su = 13827, ou = 13793;
  double sv = 15657, ov = 15296;

  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v,
                                     sx1, ox1, sy1, oy1, sz1, oz1,
                                     su, ou, sv, ov);
  //
  //test 3-d registration
  // world points
  vgl_point_3d<double> c0(44.363757, 33.289254, 31.000366);
  vgl_point_3d<double> c1(44.369636, 33.293019, 35.749939);
  vgl_point_3d<double> c2(44.373331, 33.292956, 32.587067);
  vgl_point_3d<double> c3(44.395048, 33.292964, 41.883163);
  vgl_point_3d<double> c4(44.395221, 33.289763, 38.543938);
  vgl_point_3d<double> c5(44.384865, 33.290064, 39.703369);
  vgl_point_3d<double> c6(44.351024, 33.290036, 36.203430);
  vgl_point_3d<double> c7(44.341150, 33.293502, 38.745071);
  vgl_point_3d<double> c8(44.352379, 33.294193, 44.503021);
  vcl_vector<vgl_point_3d<double> > geo_points;
  geo_points.push_back(c0);   geo_points.push_back(c1);
  geo_points.push_back(c2);   geo_points.push_back(c3);
  geo_points.push_back(c4);   geo_points.push_back(c5);
  geo_points.push_back(c6);   geo_points.push_back(c7);
  geo_points.push_back(c8);
  // corresponding image points for rcam
  vgl_point_2d<double> p0(14960.3, 22724.9);
  vgl_point_2d<double> p1(15754.8, 22182.7);
  vgl_point_2d<double> p2(16247.6, 22177.6);
  vgl_point_2d<double> p3(19160.2, 22103.7);
  vgl_point_2d<double> p4(19179.8, 22544.4);
  vgl_point_2d<double> p5(17791.3, 22541.2);
  vgl_point_2d<double> p6(13253.2, 22665.5);
  vgl_point_2d<double> p7(11932.3, 22221.1);
  vgl_point_2d<double> p8(13441.2, 22089.2);

  vcl_vector<vgl_point_2d<double> > img_points;
  img_points.push_back(p0);   img_points.push_back(p1);
  img_points.push_back(p2);   img_points.push_back(p3);
  img_points.push_back(p4);   img_points.push_back(p5);
  img_points.push_back(p6);   img_points.push_back(p7);
  img_points.push_back(p8);

  //New 3-d alignment
  vpgl_rational_camera<double> adj_rcam;
  vpgl_rational_geo_adjust::adjust(rcam, img_points, geo_points,  adj_rcam);
  vcl_cout << "\nInitial Cam\n" << rcam << '\n'
           << "\nAdjusted Cam\n" << adj_rcam << '\n';
  double zoff = adj_rcam.offset(vpgl_rational_camera<double>::Z_INDX);
  TEST_NEAR("test adjust_geo",zoff, 35.1515 , 0.001);
}

TESTMAIN(test_rational_geo_adjust);
