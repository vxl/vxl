#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_adjust_rational_trans_onept.h>

static void test_adjust_rational_trans_onept()
{

  double neu_u1[20] =
    { 8.96224e-005,  5.01302e-005,  -7.67889e-006,  -0.010342,
      -6.89891e-005,  7.97337e-006,  0.00149824,  -1.38598e-005,
      0.000188125,  0.980305,  -6.26076e-005,  2.9209e-006,
      -0.0021416,  3.32719e-008,  -0.000322074,  -7.1486e-005,
      -2.9938e-007,  1.36383e-005,  0.023163, 0.0108915};
  double den_u1[20] =
    { 7.47152e-007,  1.90129e-006,  -1.90195e-007,  -1.31851e-005,
      1.99123e-006,  -3.32589e-008,  -3.46297e-005,  -1.27463e-007,
      5.46123e-006,  -0.000542706,  1.32208e-006,  -6.82432e-008,
      7.26265e-005,  3.35976e-008,  -3.78315e-006,  -0.00145714,
      0,  -1.42542e-005,  -0.000437505, 1};
  double neu_v1[20] =
    { -8.19635e-006,  -4.83573e-005,  7.79866e-007,  -0.00106039,
      -5.42129e-006,  0,  -0.00193506,  -6.93011e-006,
      4.74326e-005, -0.132446,  -0.000200527,  -2.45944e-006,
      0.00300919,  -5.85355e-005,  0.000148533,  -1.12081,
      5.08764e-007,  -2.81721e-006,  0.0116155,  -0.00375141};
  double den_v1[20] =
    { -6.9545e-007,  -1.47008e-005,  2.14085e-007,  2.83534e-005,
      -0.000119086,  2.32147e-006,  -3.8376e-005,  -2.77014e-007,
      5.07729e-007,  -0.00280166,  -0.000309513,  9.6094e-006,
      -0.000178701,  -2.26873e-007,  1.61618e-006,  -0.00102174,
      -2.21943e-008,  5.21377e-005,  0.000211917, 1};
  //Scale and offsets

  double sx1 = 0.117798, ox1 =44.2834  ;
  double sy1 = 0.114598, oy1 = 33.2609;
  double sz1 = 526.733, oz1 = 36.9502;
  double su1 = 14106 , ou1 = 13785;
  double sv1 = 15402 , ov1 = 15216;

  vpgl_rational_camera<double> rcam1(neu_u1, den_u1, neu_v1, den_v1,
                                     sx1, ox1, sy1, oy1, sz1, oz1,
                                     su1, ou1, sv1, ov1);

  double neu_u2[20] =
    { -1.45002e-005,  8.88441e-005,  -1.69845e-006,  0.000125672,
      -1.3334e-005,  1.66753e-006,  -0.00191472,  -5.49752e-006,
      0.000626966,  1.00493,  0.000149249,  5.56542e-007,
      -0.00243291,  -4.19472e-007,  -0.000322203,  0.00751094,
      0,  1.0456e-005,  0.00335227, -0.00171778};
  double den_u2[20] =
    { -1.11389e-007,  1.12986e-007,  2.60026e-008,  1.37888e-006,
      1.59082e-006,  -2.23928e-007,  -4.61017e-005,  0,
      2.26253e-006,  0.00161218,  -1.99999e-006,  3.05839e-007,
      -4.65254e-006,  -3.57289e-008,  -1.38706e-006,  0.00195803,
      1.95078e-008,  -6.02895e-006,  -0.000621955, 1};
  double neu_v2[20] =
    { -1.34506e-007,  3.30571e-006,  1.12006e-007,  -0.000720573,
      2.18505e-005,  -1.04205e-006,  -6.89949e-005,  6.89221e-008,
      7.62809e-006,  -0.0233525,  1.04041e-005,  -1.01309e-005,
      0.00294398,  2.71637e-006,  7.06142e-005,  -1.00083,
      -6.37659e-008,  -4.99594e-006,  0.0227909, -0.00222785};
  double den_v2[20] =
    { 5.17035e-008,  1.79981e-007,  -1.70495e-008,  -2.0714e-006,
      3.631e-006,  -9.81742e-008,  1.38937e-005,  0,
      -5.91773e-007,  -0.000137415,  0.000102628,  -3.96336e-006,
      -2.34085e-006,  6.79756e-008,  -3.48424e-006,  0.000686196,
      0,  -2.33034e-006,  0.000124036, 1};
  //Scale and offsets

  double sx2 = 0.103798, ox2 = 44.3544 ;
  double sy2 = 0.113098, oy2 = 33.3433;
  double sz2 = 498.875, oz2 = 37.6053;
  double su2 = 13827 , ou2 = 13793;
  double sv2 = 15657 , ov2 = 15296;

  vpgl_rational_camera<double> rcam2(neu_u2, den_u2, neu_v2, den_v2,
                                     sx2, ox2, sy2, oy2, sz2, oz2,
                                     su2, ou2, sv2, ov2);

  //single image correspondence to correct cameras
  vgl_point_2d<double> p1(25479.9, 409.113), p2(17528.2, 14638);

  vcl_vector<vgl_point_2d<double> > corrs;
  corrs.push_back(p1);   corrs.push_back(p2);

  vcl_vector<vpgl_rational_camera<double> > cams(2);
  cams[0]= rcam1;
  cams[1]= rcam2;
  vcl_vector<vgl_vector_2d<double> > cam_trans;

  vgl_point_3d<double> intersection;
  bool good  = vpgl_adjust_rational_trans_onept::adjust(cams, corrs, cam_trans,
                                                        intersection);
  vcl_cout << "3-d intersection point " << intersection <<'\n';
  for (unsigned i = 0; i<2; ++i)
    vcl_cout << "T[" << i << "] " << cam_trans[i] << '\n';

  double elevation = 34.5121;
  vgl_point_2d<double> ap0(1.14572,1.67109);
  vgl_point_2d<double> ap1(-1.31294,-1.66164);
  vgl_point_2d<double> t0(cam_trans[0].x(), cam_trans[0].y());
  vgl_point_2d<double> t1(cam_trans[1].x(), cam_trans[1].y());
  double d = vgl_distance<double>(ap0, t0) + vgl_distance<double>(ap1, t1);

  TEST_NEAR("test 3-d elevation", intersection.z(), elevation, 0.1);
  TEST_NEAR("test adjust_rational_trans_one_point_translations",d, 0 , 0.1);
  //Additional test ideas:
  // 1) compute adjusted cameras and prove that the new projection is
  //    near the correspondence point.
  //    This test will prove that the sense of the camera translations
  //    are correct
  // 2) Find the camera translations by intentionally adjusting the
  //    camera image offsets and determining the adjustment. Should
  //    agree.
  double u01, v01, u02, v02;
  cams[0].image_offset(u01, v01);
  cams[1].image_offset(u02, v02);
  cams[0].set_image_offset(u01+10, v01+15);
  cams[1].set_image_offset(u02-10, v02-15);

  good  = vpgl_adjust_rational_trans_onept::adjust(cams, corrs, cam_trans,
                                                        intersection);
  vcl_cout << "3-d intersection point " << intersection <<'\n';
  for (unsigned i = 0; i<2; ++i)
    vcl_cout << "T[" << i << "] " << cam_trans[i] << '\n';

  cams[0].image_offset(u01, v01);
  cams[1].image_offset(u02, v02);
  cams[0].set_image_offset(u01+cam_trans[0].x(), v01+cam_trans[0].y());
  cams[1].set_image_offset(u02+cam_trans[1].x(), v02+cam_trans[1].y());
  vgl_point_2d<double> q0 = cams[0].project(intersection);
  vgl_point_2d<double> q1 = cams[1].project(intersection);
  d = vgl_distance<double>(corrs[0], q0) + vgl_distance<double>(corrs[1], q1);
  TEST_NEAR("test shifted cams, reprojection", d, 0, 0.1);
}

TESTMAIN(test_adjust_rational_trans_onept);
