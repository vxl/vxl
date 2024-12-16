#include "vpgl_equi_rectification.h"
#include <vnl/algo/vnl_lsqr.h>
#include "vnl/vnl_sparse_matrix_linear_system.h"
#include <vpgl/algo/vpgl_camera_compute.h>
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_tolerance.h"
#include <math.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>
#include <vpgl/vpgl_essential_matrix.h>
bool
vpgl_equi_rectification::column_transform(const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
                                          const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
                                          const vnl_matrix_fixed<double, 3, 3> & H0,
                                          const vnl_matrix_fixed<double, 3, 3> & H1,
                                          vnl_matrix_fixed<double, 3, 3> & Usqt,
                                          vnl_matrix_fixed<double, 3, 3> & Usqt_inv,
                                          double min_scale)
{
  double u0_avg = 0.0, u1_avg = 0.0, v1_avg = 0.0;
  size_t n = img_pts0.size();
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    p0rot /= p0rot[2];
    p1rot /= p1rot[2];
    u0_avg += p0rot[0];
    u1_avg += p1rot[0];
    v1_avg += p1rot[1];
  }
  u0_avg /= n;
  u1_avg /= n;
  v1_avg /= n;

  double Su0u1 = 0.0, Su1u1 = 0.0;
  double Su0v1 = 0.0, Su1v1 = 0.0, Sv1v1 = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    p0rot /= p0rot[2];
    p1rot /= p1rot[2];
    double u0 = p0rot[0] - u0_avg, u1 = p1rot[0] - u1_avg, v1 = p1rot[1] - v1_avg;
    Su0u1 += u0 * u1;
    Su1u1 += u1 * u1;
    Su0v1 += u0 * v1;
    Su1v1 += u1 * v1;
    Sv1v1 += v1 * v1;
  }
  Su0u1 /= n;
  Su1u1 /= n;
  Su0v1 /= n;
  Su1v1 /= n;
  Sv1v1 /= n;
  vnl_matrix_fixed<double, 2, 2> AA, AAinv;
  vnl_vector_fixed<double, 2> bb, x;
  AA[0][0] = Su1u1;
  AA[0][1] = AA[1][0] = Su1v1;
  AA[1][1] = Sv1v1;
  bb[0] = Su0u1;
  bb[1] = Su0v1;
  double d = fabs(vnl_det(AA));
  if (d < 100.0 * vgl_tolerance<double>::position)
  {
    std::cout << "Singular solution for u affine transform" << std::endl;
    return false;
  }
  AAinv = vnl_inverse(AA);
  x = AAinv * bb;
  double neg_scale = x[0] < 0.0; // determine if column scale factor is negative
  double su = fabs(x[0]), sigma_u = x[1], sqtsu = sqrt(su), ufact = 1.0 / (1.0 + sqtsu);
  // un-normalize to get the translation term
  double tu = u0_avg - x[0] * u1_avg - sigma_u * v1_avg;
  std::cout << "affine column trans: " << x[0] << ' ' << sigma_u << ' ' << tu << std::endl;
  if (su < min_scale)
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification(), row scale " << x[0] << " too small "
              << std::endl;
    return false;
  }
  // compute sqrt of transform
  Usqt.set_identity();
  Usqt[0][0] = sqtsu;
  if (neg_scale)
    Usqt[0][0] = -sqtsu;
  Usqt[0][1] = ufact * sigma_u;
  Usqt[0][2] = ufact * tu;

  Usqt_inv.set_identity();
  Usqt_inv[0][0] = 1.0 / sqtsu;
  Usqt_inv[0][1] = -ufact * sigma_u / sqtsu;
  Usqt_inv[0][2] = -ufact * tu / sqtsu;
  return true;
}
bool
vpgl_equi_rectification::rectify_pair(const vpgl_affine_fundamental_matrix<double> & aF,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
                                      vnl_matrix_fixed<double, 3, 3> & H0,
                                      vnl_matrix_fixed<double, 3, 3> & H1,
                                      double min_scale)
{
  double tol = 100.0 * vgl_tolerance<double>::position;
  vnl_matrix_fixed<double, 3, 3> Mf = aF.get_matrix();

  //  vnl_vector_fixed<double, 3> e0; e0[0] = -Mf[2][1]; e0[1] = Mf[2][0]; e0[2] = 0;
  //  vnl_vector_fixed<double, 3> e1; e1[0] = -Mf[1][2]; e1[1] = Mf[0][2]; e1[2] = 0;
  // retains original image orientation compared to other solution above
  vnl_vector_fixed<double, 3> e0;
  e0[0] = Mf[2][1];
  e0[1] = -Mf[2][0];
  e0[2] = 0;
  vnl_vector_fixed<double, 3> e1;
  e1[0] = Mf[1][2];
  e1[1] = -Mf[0][2];
  e1[2] = 0;
  double e0m = e0.magnitude();
  double e1m = e1.magnitude();
  if (e0m < tol || e1m < tol)
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification(affine), null epipoles" << std::endl;
    return false;
  }
  e0 /= e0m;
  e1 /= e1m;
  H0.set_identity();
  H1.set_identity();

  // rotation of the left image so that epipolar lines become parallel
  H0[0][0] = H0[1][1] = e0[0];
  H0[0][1] = e0[1];
  H0[1][0] = -e0[1];

  // rotation of the right image so that epipolar lines become parallel
  H1[0][0] = H1[1][1] = e1[0];
  H1[0][1] = e1[1];
  H1[1][0] = -e1[1];

  auto n = static_cast<unsigned int>(img_pts0.size());
  if (n != img_pts1.size())
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification()--img_pts0 and img_pts1 do not have equal size"
              << std::endl;
    return false;
  }

  // find a scaling and offset for the row coordinates
  double v0_avg = 0, rv1_avg = 0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    v0_avg += p0rot[1];
    rv1_avg += p1rot[1];
  }
  v0_avg /= n;
  rv1_avg /= n;
  double rSv0v1 = 0.0, rSv1v1 = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    double v0 = p0rot[1] - v0_avg, v1 = p1rot[1] - rv1_avg;
    rSv0v1 += v0 * v1;
    rSv1v1 += v1 * v1;
  }
  if (fabs(rSv1v1) < 100.0 * vgl_tolerance<double>::position)
  {
    std::cout << "row scaling problem is singular" << std::endl;
    return false;
  }
  double sr = rSv0v1 / rSv1v1;
  double tv = v0_avg - sr * rv1_avg;
  std::cout << "affine row trans: " << sr << ' ' << tv << std::endl;

  // Assign the transformation equally between the left and right images
  bool neg_scale = sr < 0.0; // determine if scale factor is negative
  double sv = fabs(sr), sqts = sqrt(sv), tfact = 1.0 / (1.0 + sqts);
  if (sv < min_scale)
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification(), row scale " << sv << " too small " << std::endl;
    return false;
  }
  vnl_matrix_fixed<double, 3, 3> Vsqt, Vsqt_inv;
  Vsqt.set_identity();
  Vsqt[1][1] = sqts;
  if (neg_scale)
    Vsqt[1][1] = -sqts;
  Vsqt[1][2] = tfact * tv;
  H1 = Vsqt * H1;

  Vsqt_inv.set_identity();
  Vsqt_inv[1][1] = 1.0 / sqts;
  Vsqt_inv[1][2] = -tfact * tv / sqts;
  H0 = Vsqt_inv * H0;

  double u0_avg = 0.0, u1_avg = 0.0, v1_avg = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    u0_avg += p0rot[0];
    u1_avg += p1rot[0];
    v1_avg += p1rot[1];
  }
  u0_avg /= n;
  u1_avg /= n;
  v1_avg /= n;

  double Su0u1 = 0.0, Su1u1 = 0.0;
  double Su0v1 = 0.0, Su1v1 = 0.0, Sv1v1 = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0rot = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1rot = H1 * img_pts1[i];
    double u0 = p0rot[0] - u0_avg, u1 = p1rot[0] - u1_avg, v1 = p1rot[1] - v1_avg;
    Su0u1 += u0 * u1;
    Su1u1 += u1 * u1;
    Su0v1 += u0 * v1;
    Su1v1 += u1 * v1;
    Sv1v1 += v1 * v1;
  }
  Su0u1 /= n;
  Su1u1 /= n;
  Su0v1 /= n;
  Su1v1 /= n;
  Sv1v1 /= n;
  vnl_matrix_fixed<double, 2, 2> AA, AAinv;
  vnl_vector_fixed<double, 2> bb, x;
  AA[0][0] = Su1u1;
  AA[0][1] = AA[1][0] = Su1v1;
  AA[1][1] = Sv1v1;
  bb[0] = Su0u1;
  bb[1] = Su0v1;
  double d = fabs(vnl_det(AA));
  if (d < 100.0 * vgl_tolerance<double>::position)
  {
    std::cout << "Singular solution for u affine transform" << std::endl;
    return false;
  }
  AAinv = vnl_inverse(AA);
  x = AAinv * bb;
  neg_scale = x[0] < 0.0; // determine if column scale factor is negative
  double su = fabs(x[0]), sigma_u = x[1], sqtsu = sqrt(su), ufact = 1.0 / (1.0 + sqtsu);
  // un-normalize to get the translation term
  double tu = u0_avg - x[0] * u1_avg - sigma_u * v1_avg;
  std::cout << "affine column trans: " << x[0] << ' ' << sigma_u << ' ' << tu << std::endl;
  if (su < min_scale)
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification(), row scale " << x[0] << " too small "
              << std::endl;
    return false;
  }
  // compute sqrt of transform
  vnl_matrix_fixed<double, 3, 3> Usqt, Usqt_inv;
  Usqt.set_identity();
  Usqt[0][0] = sqtsu;
  if (neg_scale)
    Usqt[0][0] = -sqtsu;
  Usqt[0][1] = ufact * sigma_u;
  Usqt[0][2] = ufact * tu;
  H1 = Usqt * H1;

  Usqt_inv.set_identity();
  Usqt_inv[0][0] = 1.0 / sqtsu;
  Usqt_inv[0][1] = -ufact * sigma_u / sqtsu;
  Usqt_inv[0][2] = -ufact * tu / sqtsu;
  H0 = Usqt_inv * H0;
  return true;
}

bool
vpgl_equi_rectification::rectify_pair(const vpgl_perspective_camera<double> & P0,
                                      const vpgl_perspective_camera<double> & P1,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
                                      vnl_matrix_fixed<double, 3, 3> & H0,
                                      vnl_matrix_fixed<double, 3, 3> & H1)
{

  // construct the essential matrix and extract epipoles
  vpgl_essential_matrix<double> E(P0, P1);
  vgl_homg_point_2d<double> hepi0, hepi1;
  E.get_epipoles(hepi0, hepi1);
  vnl_matrix_fixed<double, 3, 3> Em = E.get_matrix();
  vnl_vector_fixed<double, 3> epi0(hepi0.x(), hepi0.y(), hepi0.w());
  epi0 /= epi0.magnitude();
  vnl_vector_fixed<double, 3> epi1(hepi1.x(), hepi1.y(), hepi1.w());
  epi1 /= epi1.magnitude();

  // get camera information, rotation and calibration matrices
  vnl_matrix_fixed<double, 3, 3> R0 = P0.get_rotation().as_matrix();
  vnl_matrix_fixed<double, 3, 3> R1 = P1.get_rotation().as_matrix();
  vnl_matrix_fixed<double, 3, 3> K0 = P0.get_calibration().get_matrix(), K0_inv;
  vnl_matrix_fixed<double, 3, 3> K1 = P1.get_calibration().get_matrix(), K1_inv;
  K0_inv = vnl_inverse(K0);
  K1_inv = vnl_inverse(K1);

  // rotate about camera1 center so that both cameras have parallel focal planes
  // rotation that takes camera1 rotation, R1 to camera0 rotation, R0
  vnl_matrix_fixed<double, 3, 3> R10 = R0 * R1.transpose();

  // define rotation that takes right epipole of E to an ideal point
  vnl_matrix_fixed<double, 3, 3> R0r(0.0), R00, R11, ix;

  // the principal ray of camera 0 (camera z axis)
  vnl_vector_fixed<double, 3> pray0(R0[2][0], R0[2][1], R0[2][2]), v1, v2;
  pray0 /= pray0.magnitude();

  v1 = vnl_cross_3d(epi0, pray0); // a vector perpendicular to the epipole vector and the principal ray
  v2 = vnl_cross_3d(epi0, v1);    // a vector perpendicular to both

  // set up the rotation matrix. The vectors just defined are the rows of the matrix
  for (size_t c = 0; c < 3; ++c)
  {
    R0r[0][c] = epi0[c];
    R0r[1][c] = v1[c];
    R0r[2][c] = v2[c];
  }
  // sanity check to see if the rotations reduce the essential matrix to the ix form, where
  //
  //       [ 0  0  0]
  // ix =  [ 0  0  a]
  //       [ 0 -a  0]
  //
  // this form guarantes that the image rows are epipolar lines and the rows are in correspondence
  //
  // transform the essential matrix to ix
  R00 = R0r;
  R11 = R0r * R10;
  ix = R11 * Em * (R00.transpose());

  // check if ix is valid
  double sum_zeros = 0.0;
  double tol = 1.0e-10;
  for (size_t r = 0; r < 3; ++r)
    for (size_t c = 0; c < 3; ++c)
      if (!((r == 1 && c == 2) || (r == 2 && c == 1)))
        sum_zeros += ix[r][c];
  double row_eq = ix[1][2] + ix[2][1];
  if (fabs(sum_zeros) > tol || fabs(row_eq / ix[1][2]) > tol)
  {
    std::cerr << "epipolar lines not horizontal and/or rows not aligned" << std::endl;
    return false;
  }
  // update the rectificaion homographies
  // map image points to focal plane space using K_inverse
  // map to rectified focal plane image space
  // map back to rectified image0 space by applying K0
  H0 = K0 * R0r * K0_inv;
  H1 = K0 * R0r * R10 * K1_inv;
  // compute offset and scale transforms
  size_t n = img_pts0.size();

  //  offset for the row coordinates
  double v0_avg = 0, v1_avg = 0;
  for (unsigned i = 0; i < n; i++)
  {
    vnl_vector_fixed<double, 3> p0h = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1h = H1 * img_pts1[i];
    p0h /= p0h[2];
    p1h /= p1h[2];
    v0_avg += p0h[1];
    v1_avg += p1h[1];
  }
  v0_avg /= n;
  v1_avg /= n;
  double v_off = v0_avg;
  if (v1_avg < v_off)
    v_off = v1_avg;
  vnl_matrix_fixed<double, 3, 3> Tv0, Tv1;
  Tv0.set_identity();
  Tv1.set_identity();
  Tv0[1][2] = -v_off;
  Tv1[1][2] = -v_off;

  // update the rectification homographies
  H0 = Tv0 * H0;
  H1 = Tv1 * H1;

  // scale columns with an affine skew transform to minimize disparity on the pointsets img_pta0 and img_pts1
  double min_scale = 0.5;
  vnl_matrix_fixed<double, 3, 3> Usqt, Usqt_inv;
  if (!column_transform(img_pts0, img_pts1, H0, H1, Usqt, Usqt_inv, min_scale))
  {
    return false;
  }
  // equal skew transforms for H0 and H1
  H0 = Usqt_inv * H0;
  H1 = Usqt * H1;
#if 0
  for (size_t i = 0; i < n; ++i) {
    vnl_vector_fixed<double, 3> p0h = H0 * img_pts0[i];
    vnl_vector_fixed<double, 3> p1h = H1 * img_pts1[i];
    p0h /= p0h[2]; p1h /= p1h[2];
    std::cout << "p0 " << p0h[0] << ' ' << p0h[1] << " p1 " << p1h[0] << ' ' << p1h[1] << std::endl;
  }
#endif
  return true;
}
