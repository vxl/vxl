// This is gel/mrc/vpgl/algo/vpgl_camera_compute.cxx
#ifndef vpgl_camera_compute_cxx_
#define vpgl_camera_compute_cxx_

#include "vpgl_camera_compute.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_cmath.h>
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
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/algo/vpgl_backproject.h>

//#define CAMERA_DEBUG
//------------------------------------------
bool
vpgl_proj_camera_compute::compute(
                                  const vcl_vector< vgl_point_2d<double> >& image_pts,
                                  const vcl_vector< vgl_point_3d<double> >& world_pts,
                                  vpgl_proj_camera<double>& camera )
{
  vcl_vector< vgl_homg_point_2d<double> > image_pts2;
  vcl_vector< vgl_homg_point_3d<double> > world_pts2;
  for (unsigned int i = 0; i < image_pts.size(); ++i)
    image_pts2.push_back( vgl_homg_point_2d<double>( image_pts[i] ) );
  for (unsigned int i = 0; i < world_pts.size(); ++i)
    world_pts2.push_back( vgl_homg_point_3d<double>( world_pts[i] ) );
  return compute( image_pts2, world_pts2, camera );
}


//------------------------------------------
bool
vpgl_proj_camera_compute::compute(
                                  const vcl_vector< vgl_homg_point_2d<double> >& image_pts,
                                  const vcl_vector< vgl_homg_point_3d<double> >& world_pts,
                                  vpgl_proj_camera<double>& camera )
{
  unsigned int num_correspondences = image_pts.size();
  if ( world_pts.size() < num_correspondences ) num_correspondences = world_pts.size();
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
                                    const vcl_vector< vgl_point_2d<double> >& image_pts,
                                    const vcl_vector< vgl_point_3d<double> >& world_pts,
                                    vpgl_affine_camera<double>& camera )
{
  assert( image_pts.size() == world_pts.size() );
  assert( image_pts.size() > 3 );

  // Form the solution matrix.
  vnl_matrix<double> A( world_pts.size(), 4, 1 );
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
    vcl_cerr << "vpgl_affine_camera_compute:compute() cannot compute,\n"
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

static vcl_vector<double>
pvector(const double x, const double y, const double z)
{  vcl_vector<double> pv(20);
 pv[0]= x*x*x;
 pv[1]= x*x*y;
 pv[2]= x*x*z;
 pv[3]= x*x;
 pv[4]= x*y*y;
 pv[5]= x*y*z;
 pv[6]= x*y;
 pv[7]= x*z*z;
 pv[8]= x*z;
 pv[9]= x;
 pv[10]= y*y*y;
 pv[11]= y*y*z;
 pv[12]= y*y;
 pv[13]= y*z*z;
 pv[14]= y*z;
 pv[15]= y;
 pv[16]= z*z*z;
 pv[17]= z*z;
 pv[18]= z;
 pv[19]= 1;
 return pv;
}

static vcl_vector<double>
power_vector_dx(const double x, const double y, const double z)
{
  //fill the vector
  vcl_vector<double> pv(20, 0.0);
  pv[0]= 3*x*x;
  pv[1]= 2*x*y;
  pv[2]= 2*x*z;
  pv[3]= 2*x;
  pv[4]= y*y;
  pv[5]= y*z;
  pv[6]= y;
  pv[7]= z*z;
  pv[8]= z;
  pv[9]= 1;
  return pv;
}

static vcl_vector<double>
power_vector_dy(const double x, const double y, const double z)
{
  //fill the vector
  vcl_vector<double> pv(20, 0.0);
  pv[1]= x*x;
  pv[4]= 2*x*y;
  pv[5]= x*z;
  pv[6]= x;
  pv[10]= 3*y*y;
  pv[11]= 2*y*z;
  pv[12]= 2*y;
  pv[13]= z*z;
  pv[14]= z;
  pv[15]= 1;
  return pv;
}

static vcl_vector<double>
power_vector_dz(const double x, const double y, const double z)
{
  //fill the vector
  vcl_vector<double> pv(20, 0.0);
  pv[2]= x*x;
  pv[5]= x*y;
  pv[7]= 2*x*z;
  pv[8]= x;
  pv[11]= y*y;
  pv[13]= 2*y*z;
  pv[14]= y;
  pv[16]= 3*z*z;
  pv[17]= 2*z;
  pv[18]= 1;
  return pv;
}

//compute the value of the polynomial
static double pval(vcl_vector<double> const& pv, vcl_vector<double>const& coef)
{
  double sum = 0;
  for (unsigned i = 0; i<20; ++i)
    sum += pv[i]*coef[i];
  return sum;
}

// Find an approximate projective camera that approximates a rational camera
// at the world center.
bool vpgl_proj_camera_compute::
compute( vpgl_rational_camera<double> const& rat_cam,
         vgl_point_3d<double> const& world_center,
         vpgl_proj_camera<double>& camera)
{
  double x0 = world_center.x(), y0 = world_center.y(), z0 = world_center.z();
  //normalized world center
  double nx0 =
    rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX).normalize(x0);
  double ny0 =
    rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX).normalize(y0);
  double nz0 =
    rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX).normalize(z0);

  // get the rational coefficients
  vcl_vector<vcl_vector<double> > coeffs = rat_cam.coefficients();
  vcl_vector<double> neu_u = coeffs[0];
  vcl_vector<double> den_u = coeffs[1];
  vcl_vector<double> neu_v = coeffs[2];
  vcl_vector<double> den_v = coeffs[3];

  // normalize for numerical precision
  double nmax_u = -vnl_numeric_traits<double>::maxval;
  double dmax_u  = nmax_u, nmax_v = nmax_u,dmax_v = nmax_u;
  for (unsigned i = 0; i<20; ++i)
  {
    if (vcl_fabs(neu_u[i])>nmax_u)
      nmax_u=vcl_fabs(neu_u[i]);
    if (vcl_fabs(den_u[i])>dmax_u)
      dmax_u=vcl_fabs(den_u[i]);
    if (vcl_fabs(neu_v[i])>nmax_v)
      nmax_v=vcl_fabs(neu_v[i]);
    if (vcl_fabs(den_v[i])>dmax_v)
      dmax_v=vcl_fabs(den_v[i]);
  }
  // Normalize polys so that ratio of numerator and denominator is unchanged
  double norm_u = nmax_u, norm_v = nmax_v;
  if (norm_u<dmax_u) norm_u = dmax_u;
  if (norm_v<dmax_v) norm_v = dmax_v;
  for (unsigned i = 0; i<20; ++i)
  {
    neu_u[i] /= norm_u; den_u[i] /= norm_u;
    neu_v[i] /= norm_v; den_v[i] /= norm_v;
  }

  // Compute linear approximations to each poly
  //lin_p(x, y, z)=p(0,0,0) + (dp/dx)(x-nx0) + (dp/dy)(y-ny0) + (dp/dz)(z-nz0)
  vcl_vector<double> pv0 = pvector(nx0,ny0,nz0);
  double neu_u_0 = pval(pv0,neu_u), den_u_0 = pval(pv0,den_u);
  double neu_v_0 = pval(pv0,neu_v), den_v_0 = pval(pv0,den_v);
  vcl_vector<double> pv_dx0 = power_vector_dx(nx0,ny0,nz0);
  double neu_u_dx0 = pval(pv_dx0,neu_u), den_u_dx0 = pval(pv_dx0,den_u);
  double neu_v_dx0 = pval(pv_dx0,neu_v), den_v_dx0 = pval(pv_dx0,den_v);
  vcl_vector<double> pv_dy0 = power_vector_dy(nx0,ny0,nz0);
  double neu_u_dy0 = pval(pv_dy0,neu_u), den_u_dy0 = pval(pv_dy0,den_u);
  double neu_v_dy0 = pval(pv_dy0,neu_v), den_v_dy0 = pval(pv_dy0,den_v);
  vcl_vector<double> pv_dz0 = power_vector_dz(nx0,ny0,nz0);
  double neu_u_dz0 = pval(pv_dz0,neu_u), den_u_dz0 = pval(pv_dz0,den_u);
  double neu_v_dz0 = pval(pv_dz0,neu_v), den_v_dz0 = pval(pv_dz0,den_v);

  //Construct the matrix to compute the center of projection
  vnl_matrix<double> C(4,4);
  C[0][0]=neu_u_dx0;   C[0][1]=neu_u_dy0; C[0][2]=neu_u_dz0; C[0][3]=neu_u_0;
  C[1][0]=den_u_dx0;   C[1][1]=den_u_dy0; C[1][2]=den_u_dz0; C[1][3]=den_u_0;
  C[2][0]=neu_v_dx0;   C[2][1]=neu_v_dy0; C[2][2]=neu_v_dz0; C[2][3]=neu_v_0;
  C[3][0]=den_v_dx0;   C[3][1]=den_v_dy0; C[3][2]=den_v_dz0; C[3][3]=den_v_0;

  vnl_svd<double> svd(C);
  vnl_vector<double> nv = svd.nullvector();
  //assume not at infinity
  nv/=nv[3];
#if 1
  vcl_cout << "Center of projection\n" << nv << '\n'
           << "Residual\n" << C*nv << '\n';
#endif
  //Normalize with respect to the principal plane normal (principal ray)
  double ndu = vcl_sqrt(den_u_dx0*den_u_dx0 + den_u_dy0*den_u_dy0 +
                        den_u_dz0*den_u_dz0);
  double ndv = vcl_sqrt(den_v_dx0*den_v_dx0 + den_v_dy0*den_v_dy0 +
                        den_v_dz0*den_v_dz0);

  // determine if the projection is affine
  if (ndu/vcl_fabs(den_u_0)<1.0e-10||ndv/vcl_fabs(den_v_0)<1.0e-10)
  {
    vcl_cout << "Camera is nearly affine - approximation not implemented!\n";
    return false;
  }
  //Construct M by joined scale factor vector
  vnl_matrix_fixed<double, 3, 3> M;
  for (unsigned i = 0; i<3; ++i)
  {
    M[0][i]=C[0][i]/ndu;
    M[1][i]=C[2][i]/ndv;
    M[2][i]=(C[1][i]/ndu + C[3][i]/ndv)/2;
  }
#if 1
  vcl_cout << "M matrix\n" << M << '\n';

  vnl_matrix_fixed<double,3,3> Mf;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      Mf(i,j) = M(2-j,2-i);
  vnl_qr<double> QR( Mf.as_ref() );
  vnl_matrix_fixed<double,3,3> q,r,Qf,Rf, uq,ur;
  q = QR.Q();
  r = QR.R();
  for ( int i = 0; i < 3; ++i ) {
    for ( int j = 0; j < 3; ++j ) {
      Qf(i,j) = q(2-j,2-i);
      Rf(i,j) = r(2-j,2-i);
    }
  }
  vcl_cout << "Flipped Rotation\n" << Qf << '\n'
           << "Flipped Upper Triangular\n" << Rf << '\n';
  vnl_qr<double> uqr(M.as_ref());
  uq = uqr.Q();
  ur = uqr.R();
  vcl_cout << "UnFlipped Rotation\n" << uq << '\n'
           << "UnFlipped Upper Triangular\n" << ur << '\n'
           << "Det uq " << vnl_det<double>(uq) << '\n';
  //Normalized denominators
  vnl_vector<double> c1(3), c3(3);
  for (unsigned i = 0; i<3; ++i) {
    c1[i]=C[1][i]/ndu;
    c3[i]=C[3][i]/ndv;
  }

  vcl_cout << "Denominators\n"
           << "C1 " << c1
           << "C3 " << c3;
#endif
  //compute p3 the fourth column of the projection matrix
  vnl_vector_fixed<double, 3> c;
  for (unsigned i = 0; i<3; ++i)
    c[i]=nv[i];
  vnl_vector_fixed<double, 3> p3 = -M*c;
  //Form the full projection matrix
  vnl_matrix_fixed<double, 3, 4> pmatrix;
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<3; ++c)
      pmatrix[r][c] = M[r][c];
  for (unsigned r = 0; r<3; ++r)
    pmatrix[r][3] = p3[r];
  //account for the image scale and offsets
  double uscale = rat_cam.scale(vpgl_rational_camera<double>::U_INDX);
  double uoff = rat_cam.offset(vpgl_rational_camera<double>::U_INDX);
  double vscale = rat_cam.scale(vpgl_rational_camera<double>::V_INDX);
  double voff = rat_cam.offset(vpgl_rational_camera<double>::V_INDX);
  vnl_matrix_fixed<double, 3, 3> Kr;
  Kr.fill(0.0);
  Kr[0][0]=uscale;   Kr[0][2]=uoff;
  Kr[1][1]=vscale;   Kr[1][2]=voff;
  Kr[2][2]=1.0;
#if 1
  vcl_cout << "Kr\n" << Kr << '\n';
  vnl_matrix_fixed<double,3,3> KRf, KR=Kr*uq;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      KRf(i,j) = KR(2-j,2-i);
  vnl_qr<double> krQR( KRf.as_ref() );
  vnl_matrix_fixed<double,3,3> krq,krr,krQf,krRf;
  krq = krQR.Q();
  krr = krQR.R();
  for ( int i = 0; i < 3; ++i ) {
    for ( int j = 0; j < 3; ++j ) {
      krQf(i,j) = krq(2-j,2-i);
      krRf(i,j) = krr(2-j,2-i);
    }
  }
  vcl_cout << "Flipped Rotation (KR)\n" << krQf << '\n'
           << "Flipped Upper Triangular (KR)\n" << krRf << '\n';

  int r0pos = krRf(0,0) > 0 ? 1 : -1;
  int r1pos = krRf(1,1) > 0 ? 1 : -1;
  int r2pos = krRf(2,2) > 0 ? 1 : -1;
  int diag[3] = { r0pos, r1pos, r2pos };
  vnl_matrix_fixed<double,3,3> K1,R1;
  for ( int i = 0; i < 3; ++i ) {
    for ( int j = 0; j < 3; ++j ) {
      K1(i,j) = diag[j]*krRf(i,j);
      R1(i,j) = diag[i]*krQf(i,j);
    }
  }
  K1 = K1/K1(2,2);
  vcl_cout << "K1\n" << K1 <<'\n'
           << "R1\n" << R1 << '\n'
           << "Det R1 " << vnl_det<double>(R1) << '\n';
#endif

  //Need to offset x0, y0 and z0 as well.
  vnl_matrix_fixed<double, 4, 4> T;
  T.fill(0.0);
  T[0][0]=1.0; T[1][1]=1.0; T[2][2]=1.0; T[3][3]=1.0;
  T[0][3] = -nx0; T[1][3]= -ny0; T[2][3]=-nz0;
  pmatrix = Kr*pmatrix*T;
#if 0
  vcl_cout << "P Matrix\n" << pmatrix << '\n';
#endif
  camera.set_matrix(pmatrix);
  return true;
}

//:obtain a scaling transformation to normalize world geographic coordinates
//The resulting values will be on the range [-1, 1]
//The transform is valid anywhere the rational camera is valid
vgl_h_matrix_3d<double>
vpgl_proj_camera_compute::norm_trans(vpgl_rational_camera<double> const& rat_cam)
{
  double xscale = rat_cam.scale(vpgl_rational_camera<double>::X_INDX);
  double xoff = rat_cam.offset(vpgl_rational_camera<double>::X_INDX);
  double yscale = rat_cam.scale(vpgl_rational_camera<double>::Y_INDX);
  double yoff = rat_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  double zscale = rat_cam.scale(vpgl_rational_camera<double>::Z_INDX);
  double zoff = rat_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  vgl_h_matrix_3d<double> T;
  T.set_identity();
  T.set(0,0,1/xscale); T.set(1,1,1/yscale); T.set(2,2,1/zscale);
  T.set(0,3, -xoff/xscale);   T.set(1,3, -yoff/yscale);
  T.set(2,3, -zoff/zscale);
  return T;
}

//Compute the rotation matrix and translation vector for a
//perspective camera given world to image correspondences and
//the calibration matrix
bool vpgl_perspective_camera_compute::
compute( const vcl_vector< vgl_point_2d<double> >& image_pts,
         const vcl_vector< vgl_point_3d<double> >& world_pts,
         const vpgl_calibration_matrix<double>& K,
         vpgl_perspective_camera<double>& camera )
{
  unsigned N = world_pts.size();
  if (image_pts.size()!=N)
  {
    vcl_cout << "Unequal points sets in"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }
  if (N<6)
  {
    vcl_cout << "Need at least 6 points for"
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
  vcl_cout << "World Points\n" << wp << '\n';
#endif
  vnl_svd<double> svd(wp);
  unsigned rank = svd.rank();
  if (rank != 4)
  {
    vcl_cout << "Insufficient rank for world point"
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
  vcl_cout << "Null Space\n" << null_space << '\n';
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
  vcl_cout << "V2K\n" << v2k << '\n';
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
  vcl_cout << "D\n" << D << '\n';
#endif
  //form the singular matrix
  vnl_matrix<double> M = v2k*D;
  vnl_svd<double> svdm(M);

  //The point depth solution
  vnl_vector<double> depth = svdm.nullvector();

#ifdef CAMERA_DEBUG
  vcl_cout << "depths\n" << depth << '\n';
#endif

  //Check if depths are all approximately the same (near affine projection)
  double average_depth = 0;
  unsigned nd = depth.size();
  for (unsigned i = 0; i<nd; ++i)
    average_depth += depth[i];
  average_depth /= nd;
  double max_dev = 0;
  for (unsigned i = 0; i<nd; ++i)
  {
    double dev = vcl_fabs(depth[i]-average_depth);
    if (dev>max_dev)
      max_dev = dev;
  }
  double norm_max_dev = max_dev/average_depth;
  //if depths are nearly the same make them exactly equal
  //since variations are not meaningful
  if (norm_max_dev < 0.01)
    for (unsigned i = 0; i<nd; ++i)
      depth[i]=vcl_fabs(average_depth);

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
  vcl_cout << "translation\n" << t << '\n'
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
  vcl_vector<vgl_homg_point_3d<double> > h_world_pts;
  for (unsigned i = 0; i<N; ++i)
    h_world_pts.push_back(vgl_homg_point_3d<double>(world_pts[i]));
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
compute_dlt (const vcl_vector< vgl_point_2d<double> >& image_pts,
             const vcl_vector< vgl_point_3d<double> >& world_pts,
             vpgl_perspective_camera<double> &camera,
             double &err)
{
  if (image_pts.size() < 6) {
    vcl_cout<<"vpgl_perspective_camera_compute::compute needs at"
            << " least 6 points!" << vcl_endl;
    return false;
  }
  else if (image_pts.size() != world_pts.size()) {
    vcl_cout<<"vpgl_perspective_camera_compute::compute needs to"
            << " have input vectors of the same size!" << vcl_endl
            << "Currently, image_pts is size " << image_pts.size()
            << " and world_pts is size " << world_pts.size() << vcl_endl;
    return false;
  }
  else //Everything is good!
  {
    // Two equations for each point, one for the x's, the other for
    // the ys
    int num_eqns = 2 * image_pts.size();

    // A 3x4 projection matrix has 11 free vars
    int num_vars = 11;

    //---------------Set up and solve a system of linear eqns----
    vnl_matrix<double> A(num_eqns, num_vars);
    vnl_vector<double> b(num_eqns);

    // If the world pt is (x,y,z), and the image pt is (u,v),
    // A is of the form
    // [...]
    // [x, y, z, 1, 0, 0, 0, 0, u*x, u*y, u*z]
    // [0, 0, 0, 0, x, y, z, 1, v*x, v*y, v*z]
    // [...]
    //
    // and b is of the form [...; -v; -u; ...]
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

      A.put(2*i, 8, image_pts[i].x() * world_pts[i].x());
      A.put(2*i, 9, image_pts[i].x() * world_pts[i].y());
      A.put(2*i, 10, image_pts[i].x() * world_pts[i].z());

      //Set the second row of A
      A.put(2*i+1, 0, 0.0);
      A.put(2*i+1, 1, 0.0);
      A.put(2*i+1, 2, 0.0);
      A.put(2*i+1, 3, 0.0);

      A.put(2*i+1, 4, world_pts[i].x());
      A.put(2*i+1, 5, world_pts[i].y());
      A.put(2*i+1, 6, world_pts[i].z());
      A.put(2*i+1, 7, 1.0);

      A.put(2*i+1, 8, image_pts[i].y() * world_pts[i].x());
      A.put(2*i+1, 9, image_pts[i].y() * world_pts[i].y());
      A.put(2*i+1, 10, image_pts[i].y() * world_pts[i].z());

      //Set the current rows of the RHS vector
      b[2 * i] = -image_pts[i].x();
      b[2 * i + 1] = -image_pts[i].y();
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

      projed_pt[0] /= -projed_pt[2];
      projed_pt[1] /= -projed_pt[2];

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
compute( const vcl_vector< vgl_point_2d<double> >& image_pts,
         const vcl_vector< vgl_point_2d<double> >& ground_pts,
         vpgl_perspective_camera<double>& camera )
{
  unsigned num_pts = ground_pts.size();
  if (image_pts.size()!=num_pts)
  {
    vcl_cout << "Unequal points sets in"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }
  if (num_pts<4)
  {
    vcl_cout << "Need at least 4 points for"
             << " vpgl_perspective_camera_compute::compute()\n";
    return false;
  }

  vcl_vector<vgl_homg_point_2d<double> > pi, pg;
  for (unsigned i=0; i<num_pts; ++i) {
#ifdef CAMERA_DEBUG
    vcl_cout << '('<<image_pts[i].x()<<", "<<image_pts[i].y()<<") -> "
             << '('<<ground_pts[i].x()<<", "<<ground_pts[i].y()<<')'<<vcl_endl;
#endif
    pi.push_back(vgl_homg_point_2d<double>(image_pts[i].x(),image_pts[i].y()));
    pg.push_back(vgl_homg_point_2d<double>(ground_pts[i].x(),ground_pts[i].y()));
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
  vcl_vector<vgl_homg_point_3d<double> > h_world_pts;
  for (unsigned i = 0; i<num_pts; ++i) {
    h_world_pts.push_back(vgl_homg_point_3d<double>(ground_pts[i].x(),ground_pts[i].y(),0,1));
    if (camera.is_behind_camera(h_world_pts.back())) {
      vcl_cout << "behind camera" << vcl_endl;
      return false;
    }
  }
  camera = vpgl_optimize_camera::opt_orient_pos(camera, h_world_pts, image_pts);

  return true;
}


bool vpgl_perspective_camera_compute::
compute( vpgl_rational_camera<double> const& rat_cam,
         vgl_box_3d<double> const& approximation_volume,
         vpgl_perspective_camera<double>& camera,
         vgl_h_matrix_3d<double>& norm_trans)
{
  vpgl_scale_offset<double> sou =
    rat_cam.scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov =
    rat_cam.scl_off(vpgl_rational_camera<double>::V_INDX);
  vpgl_scale_offset<double> sox =
    rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX);
  unsigned ni = static_cast<unsigned>(2*sou.scale());//# image columns
  unsigned nj = static_cast<unsigned>(2*sov.scale());//# image rows
  norm_trans.set_identity();
  norm_trans.set(0,0,1/sox.scale()); norm_trans.set(1,1,1/soy.scale());
  norm_trans.set(2,2,1/soz.scale());
  norm_trans.set(0,3, -sox.offset()/sox.scale());
  norm_trans.set(1,3, -soy.offset()/soy.scale());
  norm_trans.set(2,3, -soz.offset()/soz.scale());

  vgl_point_3d<double> minp = approximation_volume.min_point();
  vgl_point_3d<double> maxp = approximation_volume.max_point();
  double xmin = minp.x(), ymin = minp.y(), zmin = minp.z();
  double xrange = maxp.x()-xmin, yrange = maxp.y()-ymin,
    zrange = maxp.z()-zmin;
  if (xrange<0||yrange<0||zrange<0)
    return false;
  //Randomly generate points
  unsigned n = 100;
  vcl_vector<vgl_point_3d<double> > world_pts;
  unsigned count = 0, ntrials = 0;
  while (count<n)
  {
    ntrials++;
    double rx = xrange*(vcl_rand()/(RAND_MAX+1.0));
    double ry = yrange*(vcl_rand()/(RAND_MAX+1.0));
    double rz = zrange*(vcl_rand()/(RAND_MAX+1.0));
    vgl_point_3d<double> wp(xmin+rx, ymin+ry, zmin+rz);
    vgl_point_2d<double> ip = rat_cam.project(wp);
    if (ip.x()<0||ip.x()>ni||ip.y()<0||ip.y()>nj)
      continue;
    world_pts.push_back(wp);
    count++;
  }
  vcl_cout << "Ntrials " << ntrials << '\n';

  //Normalize world and image points to the range [-1,1]
  vcl_vector<vgl_point_3d<double> > norm_world_pts;
  vcl_vector<vgl_point_2d<double> > image_pts, norm_image_pts;
  unsigned N = world_pts.size();
  for (unsigned i = 0; i<N; ++i)
  {
    vgl_point_3d<double> wp = world_pts[i];
    vgl_point_2d<double> ip = rat_cam.project(wp);
    image_pts.push_back(ip);
    vgl_point_2d<double> nip(sou.normalize(ip.x()), sov.normalize(ip.y()));
    norm_image_pts.push_back(nip);
    vgl_point_3d<double> nwp(sox.normalize(wp.x()),
                             soy.normalize(wp.y()),
                             soz.normalize(wp.z()));
    norm_world_pts.push_back(nwp);
  }
  //Assume identity calibration matrix initially, since image point
  //normalization remove any scale and offset from image coordinates
  vnl_matrix_fixed<double, 3, 3> kk;
  kk.fill(0);
  kk[0][0]= 1.0;
  kk[1][1]= 1.0;
  kk[2][2]=1.0;
  //Compute solution for rotation and translation and calibration matrix of
  //the perspective camera
  vpgl_calibration_matrix<double> K(kk);
  bool good = vpgl_perspective_camera_compute::compute(norm_image_pts,
                                                       norm_world_pts,
                                                       K, camera);
  if (!good)
    return false;
  vcl_cout << camera << '\n';
  //form the full camera by premultiplying by the image normalization
  vpgl_calibration_matrix<double> Kmin = camera.get_calibration();
  vnl_matrix_fixed<double, 3, 3> kk_min;
  kk_min = Kmin.get_matrix();
  kk[0][0]= sou.scale(); kk[0][2]= sou.offset();
  kk[1][1]= sov.scale(); kk[1][2]= sov.offset();
  kk *= kk_min;
  camera.set_calibration(kk);

  //project the points approximated
  double err_max = 0, err_min = 1e10;
  vgl_point_3d<double> min_pt, max_pt;
  for (unsigned i = 0; i<N; ++i)
  {
    vgl_point_3d<double> nwp = norm_world_pts[i];
    double U ,V;
    camera.project(nwp.x(), nwp.y(), nwp.z(), U, V);
    vgl_point_2d<double> ip = image_pts[i];
    double error = vcl_sqrt((ip.x()-U)*(ip.x()-U) + (ip.y()-V)*(ip.y()-V));
    if ( error > err_max )
    {
      err_max = error;
      max_pt = world_pts[i];
    }
    if (error < err_min)
    {
      err_min = error;
      min_pt = world_pts[i];
    }
  }
  vcl_cout << "Max Error = " << err_max << " at " << max_pt << '\n'
           << "Min Error = " << err_min << " at " << min_pt << '\n'

           << "final cam\n" << camera << '\n';
  return true;
}


bool vpgl_perspective_camera_compute::
compute_local( vpgl_rational_camera<double> const& rat_cam,
               vgl_box_3d<double> const& approximation_volume,
               vpgl_perspective_camera<double>& camera,
               vgl_h_matrix_3d<double>& norm_trans)
{
  // Set up the geo converter.
  double lon_low = approximation_volume.min_x();
  double lon_high = approximation_volume.max_x();
  double lat_low = approximation_volume.min_y();
  double lat_high = approximation_volume.max_y();
  assert( lat_low < lat_high && lon_low < lon_high );
  bgeo_lvcs lvcs_converter( lat_low, lon_low,
                            .5*(approximation_volume.min_z()+approximation_volume.max_z()),
                            bgeo_lvcs::wgs84, bgeo_lvcs::DEG );

  // Get a new local bounding box.
  double min_lx=1e99, min_ly=1e99, min_lz=1e99, max_lx=0, max_ly=0, max_lz=0;
  for ( int cx = 0; cx < 2; ++cx ) {
    for ( int cy = 0; cy < 2; ++cy ) {
      for ( int cz = 0; cz < 2; ++cz ) {
        vgl_point_3d<double> wc(approximation_volume.min_x()*cx + approximation_volume.max_x()*(1-cx),
                                approximation_volume.min_y()*cy + approximation_volume.max_y()*(1-cy),
                                approximation_volume.min_z()*cz + approximation_volume.max_z()*(1-cz) );
        double lcx, lcy, lcz;
        lvcs_converter.global_to_local(wc.x(), wc.y(), wc.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
        vgl_point_3d<double> wc_loc( lcx, lcy, lcz );
        if ( cx == 0 && cy == 0 && cz == 0 ) {
          min_lx = wc_loc.x(); max_lx = wc_loc.x();
          min_ly = wc_loc.y(); max_ly = wc_loc.y();
          min_lz = wc_loc.z(); max_lz = wc_loc.z();
          continue;
        }
        if ( wc_loc.x() < min_lx ) min_lx = wc_loc.x();
        if ( wc_loc.y() < min_ly ) min_ly = wc_loc.y();
        if ( wc_loc.z() < min_lz ) min_lz = wc_loc.z();
        if ( wc_loc.x() > max_lx ) max_lx = wc_loc.x();
        if ( wc_loc.y() > max_ly ) max_ly = wc_loc.y();
        if ( wc_loc.z() > max_lz ) max_lz = wc_loc.z();
      }
    }
  }
  double dlx = max_lx-min_lx, dly = max_ly-min_ly, dlz = max_lz-min_lz;

  norm_trans.set_identity();
  norm_trans.set(0,0,2/dlx); norm_trans.set(1,1,2/dly); norm_trans.set(2,2,2/dlz);
  norm_trans.set(0,3, -1-2*min_lx/dlx );
  norm_trans.set(1,3, -1-2*min_ly/dly);
  norm_trans.set(2,3, -1-2*min_lz/dlz);

  vpgl_scale_offset<double> sou =
    rat_cam.scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov =
    rat_cam.scl_off(vpgl_rational_camera<double>::V_INDX);
  vpgl_scale_offset<double> sox =
    rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX);
  unsigned ni = static_cast<unsigned>(2*sou.scale());//# image columns
  unsigned nj = static_cast<unsigned>(2*sov.scale());//# image rows

  vgl_point_3d<double> minp = approximation_volume.min_point();
  vgl_point_3d<double> maxp = approximation_volume.max_point();
  double xmin = minp.x(), ymin = minp.y(), zmin = minp.z();
  double xrange = maxp.x()-xmin, yrange = maxp.y()-ymin,
    zrange = maxp.z()-zmin;
  if (xrange<0||yrange<0||zrange<0)
    return false;
  //Randomly generate points
  unsigned n = 100;
  vcl_vector<vgl_point_3d<double> > world_pts;
  unsigned count = 0, ntrials = 0;
  while (count<n)
  {
    ++ntrials;
    double rx = xrange*(vcl_rand()/(RAND_MAX+1.0));
    double ry = yrange*(vcl_rand()/(RAND_MAX+1.0));
    double rz = zrange*(vcl_rand()/(RAND_MAX+1.0));
    vgl_point_3d<double> wp(xmin+rx, ymin+ry, zmin+rz);

    vgl_point_2d<double> ip = rat_cam.project(wp);
    if (ip.x()<0||ip.x()>ni||ip.y()<0||ip.y()>nj)
      continue;
    world_pts.push_back(wp);
    count++;
  }
  vcl_cout << "Ntrials " << ntrials << '\n';

  //Normalize world and image points to the range [-1,1]
  vcl_vector<vgl_point_3d<double> > norm_world_pts;
  vcl_vector<vgl_point_2d<double> > image_pts, norm_image_pts;
  unsigned N = world_pts.size();
  for (unsigned i = 0; i<N; ++i)
  {
    vgl_point_3d<double> wp = world_pts[i];
    vgl_point_2d<double> ip = rat_cam.project(wp);
    image_pts.push_back(ip);
    vgl_point_2d<double> nip(sou.normalize(ip.x()), sov.normalize(ip.y()));
    norm_image_pts.push_back(nip);

    // Convert to local coords.
    double lcx, lcy, lcz;
    lvcs_converter.global_to_local(wp.x(), wp.y(), wp.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
    vgl_homg_point_3d<double> wp_loc( lcx, lcy, lcz );

    vgl_homg_point_3d<double> nwp = norm_trans*wp_loc;
    assert(   vcl_fabs(nwp.x()) <= 1
              && vcl_fabs(nwp.y()) <= 1
              && vcl_fabs(nwp.z()) <= 1 );
    norm_world_pts.push_back(vgl_point_3d<double>(nwp) );
  }
  //Assume identity calibration matrix initially, since image point
  //normalization remove any scale and offset from image coordinates
  vnl_matrix_fixed<double, 3, 3> kk;
  kk.fill(0);
  kk[0][0]= 1.0;
  kk[1][1]= 1.0;
  kk[2][2]=1.0;
  //Compute solution for rotation and translation and calibration matrix of
  //the perspective camera
  vpgl_calibration_matrix<double> K(kk);
  bool good = vpgl_perspective_camera_compute::compute(norm_image_pts,
                                                       norm_world_pts,
                                                       K, camera);
  if (!good)
    return false;
  vcl_cout << camera << '\n';
  //form the full camera by premultiplying by the image normalization
  vpgl_calibration_matrix<double> Kmin = camera.get_calibration();
  vnl_matrix_fixed<double, 3, 3> kk_min;
  kk_min = Kmin.get_matrix();
  kk[0][0]= sou.scale(); kk[0][2]= sou.offset();
  kk[1][1]= sov.scale(); kk[1][2]= sov.offset();
  kk *= kk_min;
  camera.set_calibration(kk);

  //project the points approximated
  double err_max = 0, err_min = 1e10;
  vgl_point_3d<double> min_pt, max_pt;
  for (unsigned i = 0; i<N; ++i)
  {
    vgl_point_3d<double> nwp = norm_world_pts[i];
    double U ,V;
    camera.project(nwp.x(), nwp.y(), nwp.z(), U, V);
    vgl_point_2d<double> ip = image_pts[i];
    double error = vcl_sqrt((ip.x()-U)*(ip.x()-U) + (ip.y()-V)*(ip.y()-V));
    if ( error > err_max )
    {
      err_max = error;
      max_pt = world_pts[i];
    }
    if (error < err_min)
    {
      err_min = error;
      min_pt = world_pts[i];
    }
  }
  vcl_cout << "Max Error = " << err_max << " at " << max_pt << '\n'
           << "Min Error = " << err_min << " at " << min_pt << '\n'
           << "final cam\n" << camera << '\n';
  return true;
}

//
//linear interpolation based on a set of 4 neighboring rays
//          X
//        X r X
//          X
// ray r at the center pixel is interpolated from the neighboring X's
// as shown. This method is used to test if ray interpolation
// is sufficiently accurate
//
static bool interp_ray(vcl_vector<vgl_ray_3d<double> > const& ray_nbrs,
                       vgl_ray_3d<double> & intrp_ray)
{
  unsigned nrays = ray_nbrs.size();
  if (nrays!=4) return false;
  vgl_ray_3d<double> r0 = ray_nbrs[0], r1 = ray_nbrs[1];
  vgl_ray_3d<double> r2 = ray_nbrs[2], r3 = ray_nbrs[3];
  vgl_point_3d<double> org0 = r0.origin(), org1 = r1.origin();
  vgl_point_3d<double> org2 = r2.origin(), org3 = r3.origin();
  vgl_vector_3d<double> dir0 = r0.direction(), dir1 = r1.direction();
  vgl_vector_3d<double> dir2 = r2.direction(), dir3 = r3.direction();
  // first order partial derivatives
  vgl_vector_3d<double> dodu = 0.5*(org2-org1);
  vgl_vector_3d<double> dodv = 0.5*(org3-org0);
  vgl_vector_3d<double> dddu = 0.5*(dir2-dir1);
  vgl_vector_3d<double> dddv = 0.5*(dir3-dir0);
  vgl_point_3d<double> temp = org1 + dodu + dodv;
  double ox = 0.5*(org0.x()+ temp.x());
  double oy = 0.5*(org0.y()+ temp.y());
  double oz = 0.5*(org0.z()+ temp.z());
  vgl_point_3d<double> iorg(ox, oy, oz);
  vgl_vector_3d<double> idir = 0.5*(dir1 + dddu + dir0 + dddv);
  intrp_ray.set(iorg, idir);
  return true;
}

// compute tolerances on ray origin and ray direction to test interpolation
static bool ray_tol(vpgl_local_rational_camera<double> const& rat_cam,
                    double mid_u, double mid_v,
                    vgl_plane_3d<double> const& high,
                    vgl_point_3d<double> const& high_guess,
                    vgl_plane_3d<double> const& low,
                    vgl_point_3d<double> const& low_guess,
                    double& org_tol, double& dir_tol)
{
  vgl_point_2d<double> ip(mid_u,mid_v);
  vgl_point_3d<double> high_pt, low_pt, low_pt_pix;
  if (!vpgl_backproject::bproj_plane(&rat_cam, ip, high, high_guess, high_pt))
    return false;
  if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, low_guess, low_pt))
    return false;
  //move 1 pixel
  ip.set(mid_u+1.0, mid_v+1.0);
  if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, low_pt, low_pt_pix))
    return false;
  // Ground Sampling Distance
  double gsd = (low_pt-low_pt_pix).length();
  // tolerance
  double tfact = 0.001;
  org_tol = tfact*gsd;
  vgl_vector_3d<double> dir0 = low_pt-high_pt;
  vgl_vector_3d<double> dir1 = low_pt_pix-high_pt;
  double ang = angle(dir0, dir1);
  dir_tol = tfact*ang;
  return true;
}

// produce rays at sub pixel locations by interpolating four neighbors
//
//            X  <- r0
//         _______
//        | o   o |
// r1-> X |       | X <-r2
//        | o   o |
//         -------
//            X <- r3
// the interporlated rays are indicated by the o's
// the neighbor rays are shown as X's
// This method is used to populate higher resolution layers of the
// ray pyramid

bool vpgl_generic_camera_compute::
upsample_rays(vcl_vector<vgl_ray_3d<double> > const& ray_nbrs,
              vgl_ray_3d<double> const& ray,
              vcl_vector<vgl_ray_3d<double> >& interp_rays)
{
  unsigned nrays = ray_nbrs.size();
  if (nrays!=4) return false;
  vgl_ray_3d<double> r00 = ray_nbrs[0],
                     r01 = ray_nbrs[1];
  vgl_ray_3d<double> r10 = ray_nbrs[2],
                     r11 = ray_nbrs[3];
  vgl_point_3d<double> org = ray.origin();
  vgl_vector_3d<double> dir = ray.direction();
  vgl_point_3d<double> org00 = r00.origin(),
                       org01 = r01.origin();
  vgl_point_3d<double> org10 = r10.origin(),
                       org11 = r11.origin();
  vgl_vector_3d<double> dir00 = r00.direction(), dir01 = r01.direction();
  vgl_vector_3d<double> dir10 = r10.direction(), dir11 = r11.direction();

  //first sub ray
  interp_rays[0] = ray;

  //second sub ray
  vgl_point_3d<double>  iorg = org00+ (org01-org00)*0.5;
  vgl_vector_3d<double> idir = dir00*0.5 + dir01*0.5;
  interp_rays[1].set(iorg, idir);

  //third sub ray
  iorg = org00+ (org10-org00)*0.5;
  idir = 0.5*dir00 + 0.5*dir10;
  interp_rays[2].set(iorg, idir);

  //fourth sub ray
  iorg = org00+0.25*(org01-org00) + 0.25*(org10-org00)+ 0.25*(org11-org00);
  idir = 0.25*dir00 + 0.25*dir01+ 0.25*dir10+0.25*dir11;
  interp_rays[3].set(iorg, idir);

  return true;
}

// r0 and r1 are rays spaced a unit grid distane apart (either row or col)
// r is the interpolated ray at n_grid unit distances past r1
vgl_ray_3d<double>
vpgl_generic_camera_compute::interp_pair(vgl_ray_3d<double> const& r0,
                                         vgl_ray_3d<double> const& r1,
                                         double n_grid)
{
  vgl_vector_3d<double> v01 = r1.origin()-r0.origin();
  vgl_point_3d<double> intp_org = r1.origin()+ n_grid*v01;
  vgl_vector_3d<double> d01 = r1.direction()-r0.direction();
  vgl_vector_3d<double> intp_dir = r1.direction()+ n_grid*d01;
  return vgl_ray_3d<double>(intp_org, intp_dir);
}

//
// compute a generic camera from a local rational camera
// the approach is to form a pyramid and compute rays by
// back-projecting to top and bottom planes of the valid
// elevations of the rational camera. Successive higher resolution
// layers of the pyramid are populated until the interpolation of
// a ray by the four adjacent neighbors is sufficiently accurate
// The remaining layers of the pyramid are filled in by interpolation
//
bool vpgl_generic_camera_compute::
compute( vpgl_local_rational_camera<double> const& rat_cam,
         int ni, int nj, vpgl_generic_camera<double> & gen_cam)
{
  // get z bounds
  double zoff = rat_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  double zscl = rat_cam.scale(vpgl_rational_camera<double>::Z_INDX);
  // construct high and low planes
  vgl_plane_3d<double> high(0.0, 0.0, 1.0, -(zoff+zscl));
  vgl_plane_3d<double> low(0.0, 0.0, 1.0, -(zoff-zscl));
  // initial guess for backprojection to planes
  vgl_point_3d<double> org(0.0, 0.0, zoff+zscl), endpt(0.0, 0.0, zoff-zscl);

  // initialize the ray pyramid
  // compute the required number of levels
  double dim = ni;
  if (nj<ni)
    dim = nj;
  double lv = vcl_log(dim)/vcl_log(2.0);
  int n_levels = static_cast<int>(lv+1.0);// round up
  if (dim/vcl_pow(2.0, static_cast<double>(n_levels-1)) < 3.0) n_levels--;
  // construct pyramid of ray indices
  // the row and column dimensions at each level
  vcl_vector<int> nr(n_levels,0), nc(n_levels,0);
  // the scale factor at each level (to transform back to level 0)
  vcl_vector<double> scl(n_levels,1.0);
  vcl_vector<vbl_array_2d<vgl_ray_3d<double> > > ray_pyr(n_levels);
  ray_pyr[0].resize(nj, ni);
  ray_pyr[0].fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),vgl_vector_3d<double>(0,0,1)));
  nr[0]=nj;   nc[0]=ni; scl[0]=1.0;
  int di = vcl_ceil((float)ni/2)+1,
      dj = vcl_ceil((float)nj/2)+1;

  for (int i=1; i<n_levels; ++i)
  {
    ray_pyr[i].resize(dj,di);
    ray_pyr[i].fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),
                                       vgl_vector_3d<double>(0,0,1)));
    nr[i]=dj;
    nc[i]=di;
    scl[i]=2.0*scl[i-1];
    di = vcl_ceil((float)di/2)+1;
    dj = vcl_ceil((float)dj/2)+1;
  }
  // compute the ray interpolation tolerances
  double org_tol = 0.0;
  double ang_tol = 0.0;
  double max_org_err = 0.0, max_ang_err = 0.0;
  if (!ray_tol(rat_cam, ni/2.0, nj/2.0, high,
               org, low, endpt, org_tol, ang_tol))
    return false;
  bool need_interp = true;
  int lev = n_levels-1;
  for (;lev>=0&&need_interp; --lev) {
    // set rays at current pyramid level
    for (int j =0; j<nr[lev]; ++j) {
      int sj = static_cast<int>(scl[lev]*j);
      //if (sj>=nj) sj = nj;
      for (int i =0;i<nc[lev]; ++i)
      {
        int si = static_cast<int>(scl[lev]*i);
        //if (si>=ni) si = ni;
        vgl_point_2d<double> ip(si,sj);
        if (!vpgl_backproject::bproj_plane(&rat_cam, ip, high, vgl_point_3d<double>(0.0,0.0,0.0), org))
          return false;
        if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, vgl_point_3d<double>(0.0,0.0,0.0), endpt))
          return false;
        vgl_vector_3d<double> dir = endpt-org;
        ray_pyr[lev][j][i].set(org, dir);
      }
    }

    // check for interpolation accuracy at the current level
    // scan through the array and find largest discrepancy in
    // ray origin and ray direction
    need_interp = false;
    max_org_err = 0.0; max_ang_err = 0.0;
    vcl_vector<vgl_ray_3d<double> > ray_nbrs(4);
    for (int j =1; j<(nr[lev]-1)&&!need_interp; ++j) {
      for (int i =1;(i<nc[lev]-1)&&!need_interp; ++i) {
        vgl_ray_3d<double> ray = ray_pyr[lev][j][i];
        //
        //collect 4-neighbors of ray
        //
        //        0
        //      1 x 2
        //        3
        //
        ray_nbrs[0]=ray_pyr[lev][j-1][i];
        ray_nbrs[1]=ray_pyr[lev][j][i-1];
        ray_nbrs[2]=ray_pyr[lev][j][i+1];
        ray_nbrs[3]=ray_pyr[lev][j+1][i];
        //interpolate using neighbors
        vgl_ray_3d<double> intp_ray;
        if (!interp_ray(ray_nbrs, intp_ray))
          return false;
        double dorg = (ray.origin()-intp_ray.origin()).length();
        double dang = angle(ray.direction(), intp_ray.direction());
        if (dorg>max_org_err) max_org_err = dorg;
        if (dang>max_ang_err) max_ang_err = dang;
        need_interp = max_org_err>org_tol || max_ang_err>ang_tol;
      }
    }
  }
  // found level where interpolation is within tolerance
  // fill in values at lower levels
  for (++lev; lev>0; --lev) {
    unsigned int ncr = nc[lev];
    unsigned int nrb = nr[lev];
    vbl_array_2d<vgl_ray_3d<double> >& clev = ray_pyr[lev];
    vbl_array_2d<vgl_ray_3d<double> >& nlev = ray_pyr[lev-1];
    vcl_vector<vgl_ray_3d<double> > ray_nbrs(4);
    vcl_vector<vgl_ray_3d<double> > interp_rays(4);
    for (unsigned int j = 0; j<nrb; ++j)
      for (unsigned int i = 0; i<ncr; ++i) {
        ray_nbrs[0] = clev[j][i];
        ray_nbrs[1] = clev[j][i];
        ray_nbrs[2] = clev[j][i];
        ray_nbrs[3] = clev[j][i];
        if (i+1<ncr) ray_nbrs[1] = clev[j][i+1];
        if (j+1<nrb) ray_nbrs[2] = clev[j+1][i];
        if (i+1<ncr && j+1<nrb) ray_nbrs[3] = clev[j+1][i+1];
        if (!upsample_rays(ray_nbrs, clev[j][i], interp_rays))
          return false;
        if (2*i<nlev.cols() && 2*j<nlev.rows())
        {
          nlev[2*j][2*i]    =interp_rays[0];
          if (2*i+1<nlev.cols())                      nlev[2*j][2*i+1]  =interp_rays[1];
          if (2*j+1<nlev.rows())                      nlev[2*j+1][2*i]  =interp_rays[2];
          if (2*i+1<nlev.cols() && 2*j+1<nlev.rows()) nlev[2*j+1][2*i+1]=interp_rays[3];
        }
      }
  }
  gen_cam = vpgl_generic_camera<double>(ray_pyr[0]);
  return true;
}


bool vpgl_generic_camera_compute::
compute( vpgl_proj_camera<double> const& prj_cam, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam)
{
  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_ray_3d<double> ray;
  vgl_homg_point_2d<double> ipt;
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      ipt.set(i, j, 1.0);
      ray = prj_cam.backproject_ray(ipt);
      rays[j][i]=ray;
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

// the affine camera defines a principal plane which is
// far enough from the scene origin so that all the scene
// geometry is in front of the plane. The backproject function
// finds constructs finite ray origins on the principal plane.
bool vpgl_generic_camera_compute::
compute( vpgl_affine_camera<double> const& aff_cam, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam)
{
  // is an ideal point defining the ray direction
  vgl_homg_point_3d<double> cent = aff_cam.camera_center();
  vgl_vector_3d<double> dir(cent.x(), cent.y(), cent.z());
  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_homg_point_3d<double> horg;
  vgl_point_3d<double> org;
  vgl_homg_point_2d<double> ipt;
  vgl_homg_line_3d_2_points<double> hline;
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      ipt.set(i, j, 1.0);
      hline = aff_cam.backproject(ipt);
      horg = hline.point_finite();
      org.set(horg.x()/horg.w(), horg.y()/horg.w(), horg.z()/horg.w());
      rays[j][i].set(org, dir);
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

bool vpgl_generic_camera_compute::
compute( vpgl_camera_double_sptr const& camera, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam)
{
  if (vpgl_local_rational_camera<double>* cam =
      dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr()))
    return vpgl_generic_camera_compute::compute(*cam, ni, nj, gen_cam);

  if (vpgl_proj_camera<double>* cam =
      dynamic_cast<vpgl_proj_camera<double>*>(camera.ptr()))
    return vpgl_generic_camera_compute::compute(*cam, ni, nj, gen_cam);

  if (vpgl_affine_camera<double>* cam =
      dynamic_cast<vpgl_affine_camera<double>*>(camera.ptr()))
    return vpgl_generic_camera_compute::compute(*cam, ni, nj, gen_cam);

  return false;
}

#endif // vpgl_camera_compute_cxx_
