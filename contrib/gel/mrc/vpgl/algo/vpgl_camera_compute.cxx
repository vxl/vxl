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
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vgl/vgl_box_3d.h>

#include <bgeo/bgeo_lvcs.h>

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
  //Normalize polys so that ratio of neumerator and denominator is unchanged
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
  for ( int i = 0; i < 3; i++ )
    for ( int j = 0; j < 3; j++ )
      Mf(i,j) = M(2-j,2-i);
  vnl_qr<double> QR( Mf );
  vnl_matrix_fixed<double,3,3> q,r,Qf,Rf, uq,ur;
  q = QR.Q();
  r = QR.R();
  for ( int i = 0; i < 3; i++ ){
    for ( int j = 0; j < 3; j++ ){
      Qf(i,j) = q(2-j,2-i);
      Rf(i,j) = r(2-j,2-i);
    }
  }
  vcl_cout << "Flipped Rotation\n" << Qf << '\n'
           << "Flipped Upper Triangular\n" << Rf << '\n';
  vnl_qr<double> uqr(M);
  uq = uqr.Q();
  ur = uqr.R();
  vcl_cout << "UnFlipped Rotation\n" << uq << '\n'
           << "UnFlipped Upper Triangular\n" << ur << '\n'
           << "Det uq " << vnl_det<double>(uq) << '\n';
  //Normalized denominators
  vnl_vector<double> c1(3), c3(3);
  for (unsigned i = 0; i<3; ++i){
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
  for ( int i = 0; i < 3; i++ )
    for ( int j = 0; j < 3; j++ )
      KRf(i,j) = KR(2-j,2-i);
  vnl_qr<double> krQR( KRf );
  vnl_matrix_fixed<double,3,3> krq,krr,krQf,krRf;
  krq = krQR.Q();
  krr = krQR.R();
  for ( int i = 0; i < 3; i++ ){
    for ( int j = 0; j < 3; j++ ){
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
  for ( int i = 0; i < 3; i++ ){
    for ( int j = 0; j < 3; j++ ){
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
      depth[i]=average_depth;

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
    .5*(approximation_volume.min_z()+approximation_volume.max_z()), bgeo_lvcs::wgs84, bgeo_lvcs::DEG );

  // Get a new local bounding box.
  double min_lx, min_ly, min_lz, max_lx, max_ly, max_lz;
  for ( int cx = 0; cx < 2; cx++ ) {
    for ( int cy = 0; cy < 2; cy++ ) {
      for ( int cz = 0; cz < 2; cz++ ) {
        vgl_point_3d<double> wc(
          approximation_volume.min_x()*cx + approximation_volume.max_x()*(1-cx),
          approximation_volume.min_y()*cy + approximation_volume.max_y()*(1-cy),
          approximation_volume.min_z()*cz + approximation_volume.max_z()*(1-cz) );
        double lcx, lcy, lcz;
        lvcs_converter.global_to_local(
          wc.x(), wc.y(), wc.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
        vgl_point_3d<double> wc_loc( lcx, lcy, lcz );
        if ( cx == 0 && cy == 0 && cz == 0 ){
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

    // Convert to local coords.
    double lcx, lcy, lcz;
    lvcs_converter.global_to_local(
      wp.x(), wp.y(), wp.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
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

#endif // vpgl_camera_compute_cxx_
