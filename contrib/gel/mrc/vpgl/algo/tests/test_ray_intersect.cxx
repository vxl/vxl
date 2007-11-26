#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_ray_intersect.h>

static void test_ray_intersect()
{
  //Make the rational cameras
  //Rational polynomial coefficients
  //from digital globe image  02JUL05075233-P1BS-005630613010_01_P001
  double neu_u1[20] =
    { -1.68813e-005 , 7.72759e-005 , -1.05831e-006 , -0.000212391,
      -2.5837e-005 , 2.25817e-006 , -0.00186827 , -7.91212e-006,
      0.000624679 , 1.01066 , -0.000167775 , 1.30219e-005,
      -0.00228311 , -5.8781e-007 , -0.000342256 , 0.0143708,
      7.62963e-008 , 1.10481e-005 , 0.00381736 , -0.00121939};
  double den_u1[20] =
    { -1.30931e-007 , 1.69418e-007 , 2.0272e-008 , -1.45873e-006,
      2.11783e-006 , -2.15762e-007 , -6.35984e-005 , -1.07058e-008,
      3.49203e-006 , 0.00147117 , -2.86589e-006 , 2.68989e-007,
      1.19205e-005 , -4.65223e-008 , -2.04604e-006 , 0.00193974,
      1.5139e-008 , -8.36577e-006 , -0.000621847 , 1};
  double neu_v1[20] =
    { 1.60189e-007 , 1.75875e-005 , -2.03537e-007 , -0.000717374,
      2.78957e-005 , -1.05314e-006 , -0.000112593 , 5.07345e-007,
      8.74784e-006 , -0.0228769 , 7.73345e-005 , -8.14762e-006,
      0.00318176 , 2.20927e-005 , 7.82713e-005 , -1.00092,
      -5.30236e-007 , -5.17096e-006 , 0.022347 , -0.00259056};
  double den_v1[20] =
    { 4.79097e-008 , -1.48662e-007 , -1.33299e-008 , -1.66728e-005,
      -2.12508e-005 , 8.2811e-008 , 1.98373e-005 , 2.63071e-008,
      -5.66165e-007 , -0.000188585 , -0.000302309 , 2.46881e-006,
      5.09931e-005 , 2.74573e-007 , -4.63648e-006 , 0.000667711,
      1.2639e-007 , -2.19341e-005 , 0.000132914 , 1};
  //Scale and offsets

  double sx1 = 0.1039, ox1 = 44.3542 ;
  double sy1 = 0.1129, oy1 = 33.1855;
  double sz1 = 501, oz1 = 32;
  double su1 = 13834, ou1 = 13792;
  double sv1 = 15787, ov1 = 15430;

  vpgl_rational_camera<double> rcam1(neu_u1, den_u1, neu_v1, den_v1,
                                     sx1, ox1, sy1, oy1, sz1, oz1,
                                     su1, ou1, sv1, ov1);

  //Rational polynomial coefficients
  // from digital globe image 04MAY04074308-P1BS-005630650010_01_P001
  double neu_u2[20] =
    { -3.70364e-005 , 8.53023e-005 , -3.65648e-007 , -0.000181464,
      -0.000131914 , -1.23801e-006 , -0.000170381 , -2.36028e-005,
      0.000596236 , 1.00214 , -0.000453892 , -4.36304e-006,
      0.000253576 , 1.09375e-006 , -0.000548561 , -0.0033657,
      2.61423e-007 , -2.78874e-006 , -0.00121924 , 0.000759294};
  double den_u2[20] =
    {-2.23781e-008 , 1.20735e-007 , 3.44245e-008 , -2.50049e-005,
     -1.41759e-007 , -8.8059e-008 , -8.87743e-005 , 1.22624e-008,
     -8.40265e-007 , -0.000578809 , 2.60812e-006 , -2.34832e-007,
     0.000102493 , 1.36238e-008 , 1.24993e-006 , 0.000173207,
     2.77082e-008 , -2.41848e-005 , -0.000598502 , 1};
  double neu_v2[20] =
    {3.02423e-007 , 6.48321e-005 , 3.30254e-007 , -0.000286958,
     4.753e-005 , 2.08211e-007 , 0.00018707 , 3.55252e-007,
     2.06051e-006 , -0.00355224 , 0.000337977 , 8.62332e-006,
     -0.00193573 , 9.37901e-005 , 0.000394162 , -1.00127,
     9.87611e-007 , 2.2541e-006 , -0.0032827 , 0.00225114};
  double den_v2[20] =
    {1.844e-008 , 4.64359e-007 , -2.55265e-008 , -7.50079e-005,
     -4.70263e-007 , -3.84721e-007 , 2.9448e-005 , -1.25872e-007,
     1.38401e-008 , 0.000194293 , -0.000127965 , -6.45576e-006,
     0.00020888 , -4.60266e-006 , 3.66616e-006 , 0.000356873,
     -1.88117e-006 , -9.17403e-005 , 0.000404611 , 1};

  //Scale and offsets
  double sx2 = 0.0902, ox2 =44.2278 ;
  double sy2 = 0.1011, oy2 =33.25 ;
  double sz2 = 501, oz2 = 34;
  double su2 = 13800, ou2 = 13759;
  //  double su2 = 13800, ou2 = 13759+251;
  double sv2 = 18330, ov2 = 18284;
  //double sv2 = 18330, ov2 = 18284-535;
  vpgl_rational_camera<double> rcam2(neu_u2, den_u2, neu_v2, den_v2,
                                     sx2, ox2, sy2, oy2, sz2, oz2,
                                     su2, ou2, sv2, ov2);
  //project a point from the center of the valid region of camera 1
  vgl_point_2d<double> p1, p2;
  vgl_point_3d<double> p3d(44.255196,33.284970 , 32);
  p1 = rcam1.project(p3d);
  p2 = rcam2.project(p3d);
  vcl_cout << "Projected point from rcam 1 " << p1 << '\n'
           << "Projected point from rcam 2 " << p2 << '\n';
  vcl_vector<vpgl_camera<double>* > cams(2);
  cams[0]= (vpgl_camera<double>*)(&rcam1);
  cams[1]= (vpgl_camera<double>*)(&rcam2);
  vcl_vector<vgl_point_2d<double> > image_pts;
  image_pts.push_back(p1);   image_pts.push_back(p2);
  vpgl_ray_intersect ri(2);
  vgl_point_3d<double> intersection;
  vgl_point_3d<double> initial_point(44.3542,33.1855 ,32);
  /*bool success =*/ ri.intersect(cams, image_pts, initial_point, intersection);
  vcl_cout << "Result " << intersection << '\n';
  TEST_NEAR("test ray_intersection", vcl_fabs(44.2552-intersection.x())+
            vcl_fabs(33.285-intersection.y())+
            vcl_fabs(32.001-intersection.z()),0 , 0.001);
}

TESTMAIN(test_ray_intersect);
