// This is core/vpgl/algo/vpgl_camera_compute.cxx
#ifndef vpgl_camera_compute_cxx_
#define vpgl_camera_compute_cxx_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "vpgl_camera_compute.h"
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/algo/vpgl_backproject.h>

//#define CAMERA_DEBUG
//------------------------------------------
bool
vpgl_proj_camera_compute::compute(
                                  const std::vector< vgl_point_2d<double> >& image_pts,
                                  const std::vector< vgl_point_3d<double> >& world_pts,
                                  vpgl_proj_camera<double>& camera )
{
  std::vector< vgl_homg_point_2d<double> > image_pts2;
  std::vector< vgl_homg_point_3d<double> > world_pts2;
  image_pts2.reserve(image_pts.size());
for (auto image_pt : image_pts)
    image_pts2.emplace_back( image_pt );
  world_pts2.reserve(world_pts.size());
for (const auto & world_pt : world_pts)
    world_pts2.emplace_back( world_pt );
  return compute( image_pts2, world_pts2, camera );
}


//------------------------------------------
bool
vpgl_proj_camera_compute::compute(
                                  const std::vector< vgl_homg_point_2d<double> >& image_pts,
                                  const std::vector< vgl_homg_point_3d<double> >& world_pts,
                                  vpgl_proj_camera<double>& camera )
{
  auto num_correspondences = static_cast<unsigned int>(image_pts.size());
  if ( world_pts.size() < num_correspondences ) num_correspondences = static_cast<unsigned int>(world_pts.size());
  assert( num_correspondences >= 6 );

  // Form the solution matrix.
  vnl_matrix<double> S( 2*num_correspondences, 12, 0);
  for ( unsigned i = 0; i < num_correspondences; ++i ) {
    S(2*i,0) = -image_pts[i].w()*world_pts[i].x();
    S(2*i,1) = -image_pts[i].w()*world_pts[i].y();
    S(2*i,2) = -image_pts[i].w()*world_pts[i].z();
    S(2*i,3) = -image_pts[i].w()*world_pts[i].w();
    S(2*i,8) = image_pts[i].x()*world_pts[i].x();
    S(2*i,9) = image_pts[i].x()*world_pts[i].y();
    S(2*i,10) = image_pts[i].x()*world_pts[i].z();
    S(2*i,11) = image_pts[i].x()*world_pts[i].w();
    S(2*i+1,4) = -image_pts[i].w()*world_pts[i].x();
    S(2*i+1,5) = -image_pts[i].w()*world_pts[i].y();
    S(2*i+1,6) = -image_pts[i].w()*world_pts[i].z();

    S(2*i+1,7) = -image_pts[i].w()*world_pts[i].w();
    S(2*i+1,8) = image_pts[i].y()*world_pts[i].x();
    S(2*i+1,9) = image_pts[i].y()*world_pts[i].y();
    S(2*i+1,10) = image_pts[i].y()*world_pts[i].z();
    S(2*i+1,11) = image_pts[i].y()*world_pts[i].w();
  }
  vnl_svd<double> svd( S );
  vnl_vector<double> c = svd.nullvector();
  vnl_matrix_fixed<double,3,4> cm;
  cm(0,0)=c(0); cm(0,1)=c(1); cm(0,2)=c(2); cm(0,3)=c(3);
  cm(1,0)=c(4); cm(1,1)=c(5); cm(1,2)=c(6); cm(1,3)=c(7);
  cm(2,0)=c(8); cm(2,1)=c(9); cm(2,2)=c(10); cm(2,3)=c(11);
  camera = vpgl_proj_camera<double>( cm );
  return true;
}


//------------------------------------------
bool
vpgl_affine_camera_compute::compute(
                                    const std::vector< vgl_point_2d<double> >& image_pts,
                                    const std::vector< vgl_point_3d<double> >& world_pts,
                                    vpgl_affine_camera<double>& camera )
{
  assert( image_pts.size() == world_pts.size() );
  assert( image_pts.size() > 3 );

  // Form the solution matrix.
  vnl_matrix<double> A(static_cast<unsigned int>(world_pts.size()), 4, 1 );
  for (unsigned int i = 0; i < world_pts.size(); ++i) {
    A(i,0) = world_pts[i].x(); A(i,1) = world_pts[i].y(); A(i,2) = world_pts[i].z();
  }
  vnl_vector<double> b1( image_pts.size() );
  vnl_vector<double> b2( image_pts.size() );
  for (unsigned int i = 0; i < image_pts.size(); ++i) {
    b1(i) = image_pts[i].x(); b2(i) = image_pts[i].y();
  }
  vnl_matrix<double> AtA = A.transpose()*A;
  vnl_svd<double> svd(AtA);
  if ( svd.rank() < 4 ) {
    std::cerr << "vpgl_affine_camera_compute:compute() cannot compute,\n"
             << "    input data has insufficient rank.\n";
    return false;
  }
  vnl_matrix<double> S = svd.inverse()*A.transpose();
  vnl_vector_fixed<double,4> x1, x2;
  x1 = S*b1;
  x2 = S*b2;

  // Fill in the camera.
  camera.set_rows( x1, x2 );
  return true;
}


//Compute the rotation matrix and translation vector for a
//perspective camera given world to image correspondences and
//the calibration matrix
bool vpgl_perspective_camera_compute::
compute( const std::vector< vgl_point_2d<double> >& image_pts,
         const std::vector< vgl_point_3d<double> >& world_pts,
         const vpgl_calibration_matrix<double>& K,
         vpgl_perspective_camera<double>& camera )
{
  auto N = static_cast<unsigned int>(world_pts.size());
  if (image_pts.size()!=N)
  {
    std::cout << "Unequal points sets in"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }
  if (N<6)
  {
    std::cout << "Need at least 6 points for"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }

  //get the inverse calibration map
  vnl_matrix_fixed<double, 3, 3> km = K.get_matrix();
  vnl_matrix_fixed<double, 3, 3> k_inv = vnl_inverse<double>(km);

  //Form the world point matrix

  //Solve for the unknown point depths (projective scale factors)
  vnl_matrix<double> wp(4, N);
  for (unsigned c = 0; c<N; ++c)
  {
    vgl_point_3d<double> p = world_pts[c];
    wp[0][c] = p.x(); wp[1][c] = p.y(); wp[2][c] = p.z();
    wp[3][c] = 1.0;
  }
#ifdef CAMERA_DEBUG
  std::cout << "World Points\n" << wp << '\n';
#endif
  vnl_svd<double> svd(wp);
  unsigned rank = svd.rank();
  if (rank != 4)
  {
    std::cout << "Insufficient rank for world point"
             << " matrix in vpgl_perspective_camera_compute::compute()\n";
    return false;
  }
  //extract the last N-4 columns of V as the null space of wp
  vnl_matrix<double> V = svd.V();
  unsigned nr = V.rows(), nc = V.columns();
  vnl_matrix<double> null_space(nr, nc-4);
  for (unsigned c = 4; c<nc; ++c)
    for (unsigned r = 0; r<nr; ++r)
      null_space[r][c-4] = V[r][c];
#ifdef CAMERA_DEBUG
  std::cout << "Null Space\n" << null_space << '\n';
#endif
  //form Kronecker product of the null space (transpose) with K inverse
  unsigned nrk = 3*(nc-4), nck = 3*nr;
  vnl_matrix<double> v2k(nrk, nck);
  for (unsigned r = 0; r<(nc-4); ++r)
    for (unsigned c = 0; c<nr; ++c)
      for (unsigned rk = 0; rk<3; ++rk)
        for (unsigned ck = 0; ck<3; ++ck)
          v2k[rk+3*r][ck+3*c] = k_inv[rk][ck]*null_space[c][r];
#ifdef CAMERA_DEBUG
  std::cout << "V2K\n" << v2k << '\n';
#endif
  //Stack the image points in homogeneous form in a diagonal matrix
  vnl_matrix<double> D(3*N, N);
  D.fill(0);
  for (unsigned c = 0; c<N; ++c)
  {
    vgl_point_2d<double> p = image_pts[c];
    D[3*c][c] = p.x();  D[3*c+1][c] = p.y(); D[3*c+2][c] = 1.0;
  }
#ifdef CAMERA_DEBUG
  std::cout << "D\n" << D << '\n';
#endif
  //form the singular matrix
  vnl_matrix<double> M = v2k*D;
  vnl_svd<double> svdm(M);

  //The point depth solution
  vnl_vector<double> depth = svdm.nullvector();

#ifdef CAMERA_DEBUG
  std::cout << "depths\n" << depth << '\n';
#endif

  //Check if depths are all approximately the same (near affine projection)
  double average_depth = 0;
  auto nd = static_cast<unsigned int>(depth.size());
  for (unsigned i = 0; i<nd; ++i)
    average_depth += depth[i];
  average_depth /= nd;
  double max_dev = 0;
  for (unsigned i = 0; i<nd; ++i)
  {
    double dev = std::fabs(depth[i]-average_depth);
    if (dev>max_dev)
      max_dev = dev;
  }
  double norm_max_dev = max_dev/average_depth;
  //if depths are nearly the same make them exactly equal
  //since variations are not meaningful
  if (norm_max_dev < 0.01)
    for (unsigned i = 0; i<nd; ++i)
      depth[i]=std::fabs(average_depth);

  //Set up point sets for ortho Procrustes
  vnl_matrix<double> X(3,N), Y(3,N);
  for (unsigned c = 0; c<N; ++c)
  {
    vgl_point_2d<double> pi = image_pts[c];
    vgl_point_3d<double> pw = world_pts[c];
    //image points are multiplied by projective scale factor (depth)
    X[0][c] = pi.x()*depth[c]; X[1][c] = pi.y()*depth[c]; X[2][c] = depth[c];
    // X[0][c] = pi.x();          X[1][c] = pi.y();          X[2][c] = 1.0;
    Y[0][c] = pw.x();          Y[1][c] = pw.y();          Y[2][c] = pw.z();
  }

  vpgl_ortho_procrustes op(X, Y);
  if (!op.compute_ok())
    return false;

  vgl_rotation_3d<double> R = op.R();
  vnl_matrix_fixed<double, 3, 3> rr = R.as_matrix();

  vnl_vector_fixed<double, 3> t = op.t();
#ifdef CAMERA_DEBUG
  std::cout << "translation\n" << t << '\n'
           << "scale = " << op.s() << '\n'
           << "residual = " << op.residual_mean_sq_error() << '\n';
#endif

  vnl_vector_fixed<double, 3> center = -(rr.transpose())*t;
  vgl_point_3d<double> vgl_center(center[0],center[1],center[2]);
  vpgl_perspective_camera<double> tcam;
  tcam.set_calibration(K);
  tcam.set_camera_center(vgl_center);
  tcam.set_rotation(R);

  //perform a final non-linear optimization
  std::vector<vgl_homg_point_3d<double> > h_world_pts;
  for (unsigned i = 0; i<N; ++i)
    h_world_pts.emplace_back(world_pts[i]);
  camera = vpgl_optimize_camera::opt_orient_pos_cal(tcam, h_world_pts, image_pts, 0.00005, 20000);
  return true;
}


//: Uses the direct linear transform algorithm described in "Multiple
// View Geometry in Computer Vision" to find the projection matrix,
// and extracts the parameters of the camera from this projection matrix.
// Requires: image_pts and world_pts are correspondences. image_pts is
//  the projected form, and world_pts is the unprojected form. There
//  need to be at least 6 points.
// Returns: true if successful. err is filled with the two-norm of the
//  projection error vector. camera is filled with the perspective
//  decomposition of the projection matrix
bool vpgl_perspective_camera_compute::
compute_dlt (const std::vector< vgl_point_2d<double> >& image_pts,
             const std::vector< vgl_point_3d<double> >& world_pts,
             vpgl_perspective_camera<double> &camera,
             double &err)
{
  if (image_pts.size() < 6) {
    std::cout<<"vpgl_perspective_camera_compute::compute needs at"
            << " least 6 points!" << std::endl;
    return false;
  }
  else if (image_pts.size() != world_pts.size()) {
    std::cout<<"vpgl_perspective_camera_compute::compute needs to"
            << " have input vectors of the same size!" << std::endl
            << "Currently, image_pts is size " << image_pts.size()
            << " and world_pts is size " << world_pts.size() << std::endl;
    return false;
  }
  else //Everything is good!
  {
    // Two equations for each point, one for the x's, the other for
    // the ys
    int num_eqns = static_cast<int>(2 * image_pts.size());

    // A 3x4 projection matrix has 11 free vars
    int num_vars = 11;

    //---------------Set up and solve a system of linear eqns----
    vnl_matrix<double> A(num_eqns, num_vars);
    vnl_vector<double> b(num_eqns);

    // If the world pt is (x,y,z), and the image pt is (u,v),
    // A is of the form
    // [...]
    // [x, y, z, 1, 0, 0, 0, 0, -u*x, -u*y, -u*z]
    // [0, 0, 0, 0, x, y, z, 1, -v*x, -v*y, -v*z]
    // [...]
    //
    // and b is of the form [...; v; u; ...]
    for (unsigned int i = 0; i < image_pts.size(); ++i)
    {
      //Set the first row of A
      A.put(2*i, 0, world_pts[i].x());
      A.put(2*i, 1, world_pts[i].y());
      A.put(2*i, 2, world_pts[i].z());
      A.put(2*i, 3, 1.0);

      A.put(2*i, 4, 0.0);
      A.put(2*i, 5, 0.0);
      A.put(2*i, 6, 0.0);
      A.put(2*i, 7, 0.0);

      A.put(2*i, 8, -image_pts[i].x() * world_pts[i].x());
      A.put(2*i, 9, -image_pts[i].x() * world_pts[i].y());
      A.put(2*i, 10, -image_pts[i].x() * world_pts[i].z());

      //Set the second row of A
      A.put(2*i+1, 0, 0.0);
      A.put(2*i+1, 1, 0.0);
      A.put(2*i+1, 2, 0.0);
      A.put(2*i+1, 3, 0.0);

      A.put(2*i+1, 4, world_pts[i].x());
      A.put(2*i+1, 5, world_pts[i].y());
      A.put(2*i+1, 6, world_pts[i].z());
      A.put(2*i+1, 7, 1.0);

      A.put(2*i+1, 8, -image_pts[i].y() * world_pts[i].x());
      A.put(2*i+1, 9, -image_pts[i].y() * world_pts[i].y());
      A.put(2*i+1, 10, -image_pts[i].y() * world_pts[i].z());

      //Set the current rows of the RHS vector
      b[2 * i] = image_pts[i].x();
      b[2 * i + 1] = image_pts[i].y();
    }

    //Solve the system
    vnl_svd<double> svd(A);
    vnl_vector<double> x = svd.solve(b);

    //Transform the linearized version into the matrix form
    vnl_matrix_fixed<double, 3, 4> proj;

    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 4; col++) {
        if (row*4 + col < 11) {
          proj.put(row, col, x[row*4 + col]);
        }
      }
    }

    proj.set(2, 3, 1.0);

    //-------------Find the error rate--------------------
    err = 0;
    for (unsigned int i = 0; i < image_pts.size(); ++i) {
      vnl_vector_fixed<double, 4> world_pt;
      world_pt[0] = world_pts[i].x();
      world_pt[1] = world_pts[i].y();
      world_pt[2] = world_pts[i].z();
      world_pt[3] = 1.0;

      vnl_vector_fixed<double, 3> projed_pt = proj * world_pt;

      projed_pt[0] /= projed_pt[2];
      projed_pt[1] /= projed_pt[2];

      double dx = projed_pt[0] - image_pts[i].x();
      double dy = projed_pt[1] - image_pts[i].y();

      err += dx*dy;
    }

    //-----Get the camera------------------------------
    return vpgl_perspective_decomposition(proj, camera);
  }
}

//: Compute from two sets of corresponding 2D points (image and ground plane).
// \param ground_pts are 2D points representing world points with Z=0
// The calibration matrix of \a camera is enforced
// This computation is simpler than the general case above and only requires 4 points
// Put the resulting camera into \p camera, return true if successful.
bool vpgl_perspective_camera_compute::
compute( const std::vector< vgl_point_2d<double> >& image_pts,
         const std::vector< vgl_point_2d<double> >& ground_pts,
         vpgl_perspective_camera<double>& camera )
{
  auto num_pts = static_cast<unsigned int>(ground_pts.size());
  if (image_pts.size()!=num_pts)
  {
    std::cout << "Unequal points sets in"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }
  if (num_pts<4)
  {
    std::cout << "Need at least 4 points for"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }

  std::vector<vgl_homg_point_2d<double> > pi, pg;
  for (unsigned i=0; i<num_pts; ++i) {
#ifdef CAMERA_DEBUG
    std::cout << '('<<image_pts[i].x()<<", "<<image_pts[i].y()<<") -> "
             << '('<<ground_pts[i].x()<<", "<<ground_pts[i].y()<<')'<<std::endl;
#endif
    pi.emplace_back(image_pts[i].x(),image_pts[i].y());
    pg.emplace_back(ground_pts[i].x(),ground_pts[i].y());
  }

  // compute a homography from the ground plane to image plane
  vgl_h_matrix_2d_compute_linear est_H;
  vnl_double_3x3 H = est_H.compute(pg,pi).get_matrix();
  if (vnl_det(H) > 0)
    H *= -1.0;

  // invert the effects of intrinsic parameters
  vnl_double_3x3 Kinv = vnl_inverse(camera.get_calibration().get_matrix());
  vnl_double_3x3 A(Kinv*H);
  // get the translation vector (up to a scale)
  vnl_vector_fixed<double,3> t = A.get_column(2);
  t.normalize();

  // compute the closest rotation matrix
  A.set_column(2, vnl_cross_3d(A.get_column(0), A.get_column(1)));
  vnl_svd<double> svdA(A.as_ref());
  vnl_double_3x3 R = svdA.U()*svdA.V().conjugate_transpose();

  // find the point farthest from the origin
  int max_idx = 0;
  double max_dist = 0.0;
  for (unsigned int i=0; i < ground_pts.size(); ++i) {
    double d = (ground_pts[i]-vgl_point_2d<double>(0,0)).length();
    if (d >= max_dist) {
      max_dist = d;
      max_idx = i;
    }
  }

  // compute the unknown scale
  vnl_vector_fixed<double,3> i1 = Kinv*vnl_double_3(image_pts[max_idx].x(),image_pts[max_idx].y(),1.0);
  vnl_vector_fixed<double,3> t1 = vnl_cross_3d(i1, t);
  vnl_vector_fixed<double,3> p1 = vnl_cross_3d(i1, R*vnl_double_3(ground_pts[max_idx].x(),ground_pts[max_idx].y(),1.0));
  double s = p1.magnitude()/t1.magnitude();

  // compute the camera center
  t *= s;
  t = -R.transpose()*t;

  camera.set_rotation(vgl_rotation_3d<double>(R));
  camera.set_camera_center(vgl_point_3d<double>(t[0],t[1],t[2]));

  //perform a final non-linear optimization
  std::vector<vgl_homg_point_3d<double> > h_world_pts;
  for (unsigned i = 0; i<num_pts; ++i) {
    h_world_pts.emplace_back(ground_pts[i].x(),ground_pts[i].y(),0,1);
    if (camera.is_behind_camera(h_world_pts.back())) {
      std::cout << "behind camera" << std::endl;
      return false;
    }
  }
  camera = vpgl_optimize_camera::opt_orient_pos(camera, h_world_pts, image_pts);

  return true;
}

#endif // vpgl_camera_compute_cxx_
