#include "vpgl_equi_rectification.h"
#include <vnl/algo/vnl_lsqr.h>
#include "vnl/vnl_sparse_matrix_linear_system.h"
#include <vpgl/algo/vpgl_camera_compute.h>
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_tolerance.h"
#include <math.h>
#include <vnl_det.h>
#include <vnl_inverse.h>

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
  AA[2][2] = 1.0;
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
vpgl_equi_rectification::rectify_pair(const vpgl_fundamental_matrix<double> & F,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
                                      const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
                                      vnl_matrix_fixed<double, 3, 3> & H0,
                                      vnl_matrix_fixed<double, 3, 3> & H1,
                                      double min_scale)
{
  size_t n = img_pts0.size();
  if (n == 0 || img_pts1.size() != n)
  {
    std::cout << "null image pointset or pointsets of unequal size" << std::endl;
    return false;
  }
  double tol = 100.0 * vgl_tolerance<double>::position;
  vgl_homg_point_2d<double> he0, he1;
  F.get_epipoles(he0, he1);
  vnl_vector_fixed<double, 3> e0(he0.x(), he0.y(), he0.w());
  vnl_vector_fixed<double, 3> e1(he1.x(), he1.y(), he1.w());
  double e0m = e0.magnitude(), e1m = e1.magnitude();
  if (e0m < tol || e1m < tol)
  {
    std::cout << "in vpgl_equi_rectification::compute_rectification(projective), null epipoles" << std::endl;
    return false;
  }
  e0 /= e0m;
  e1 /= e1m;
  // transform the right epipole (right side of F)
  vnl_matrix_fixed<double, 3, 3> T0, R0, G0;
  T0.set_identity();
  R0.set_identity();
  G0.set_identity();
  if (fabs(e0[2]) < tol)
  { // epipole already an ideal point
    R0[0][0] = R0[1][1] = e0[0];
    R0[0][1] = e0[1];
    R0[1][0] = -e0[1];
  }
  else
  {
    // follow the prescription in H&Z
    // translate the first image point to the origin
    e0 /= e0[2];
    vnl_vector_fixed<double, 3> p00 = img_pts0[0], e0tr;
    T0[0][2] = -p00[0] / p00[2];
    T0[1][2] = -p00[1] / p00[2];
    double theta = atan2(e0[1], e0[0]);
    double c = cos(-theta), s = sin(-theta);
    R0[0][0] = c;
    R0[0][1] = -s;
    R0[1][0] = s;
    R0[1][1] = c;
    e0tr = R0 * e0;
    double f = e0tr[0] / e0tr[2];
    G0[2][0] = -1.0 / f;
  }
  H0 = G0 * R0 * T0;
  // transform the left epipole (left side of F)
  vnl_matrix_fixed<double, 3, 3> T1, R1, G1;
  T1.set_identity();
  R1.set_identity();
  G1.set_identity();
  if (fabs(e1[2]) < tol)
  { // epipole already an ideal point
    R1[0][0] = R1[1][1] = e1[0];
    R1[0][1] = e1[1];
    R1[1][0] = -e1[1];
  }
  else
  {
    // follow the prescription in H&Z
    // translate the first image point to the origin
    vnl_vector_fixed<double, 3> p10 = img_pts1[0], e1tr;
    T0[0][2] = -p10[0] / p10[2];
    T0[1][2] = -p10[1] / p10[2];
    double theta = atan2(e1[1], e1[0]);
    double c = cos(-theta), s = sin(-theta);
    R1[0][0] = c;
    R1[0][1] = -s;
    R1[1][0] = s;
    R1[1][1] = c;
    e1tr = R1 * e1;
    double f = e1tr[0] / e1tr[2];
    G1[2][0] = -1.0 / f;
  }
  H1 = G1 * R1 * T1;
  // the following is the same as for the affine case
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
  AA[2][2] = 1.0;
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
