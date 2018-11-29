// This is core/vpgl/algo/vpgl_camera_convert.cxx
#ifndef vpgl_camera_convert_cxx_
#define vpgl_camera_convert_cxx_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "vpgl_camera_convert.h"
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/algo/vpgl_backproject.h>


static std::vector<double>
pvector(const double x, const double y, const double z)
{
  //fill the vector
  std::vector<double> pv(20);
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

static std::vector<double>
power_vector_dx(const double x, const double y, const double z)
{
  //fill the vector
  std::vector<double> pv(20, 0.0);
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

static std::vector<double>
power_vector_dy(const double x, const double y, const double z)
{
  //fill the vector
  std::vector<double> pv(20, 0.0);
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

static std::vector<double>
power_vector_dz(const double x, const double y, const double z)
{
  //fill the vector
  std::vector<double> pv(20, 0.0);
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

//convert the value of the polynomial
static double pval(std::vector<double> const& pv, std::vector<double>const& coef)
{
  double sum = 0.0;
  for (unsigned i = 0; i<20; ++i)
    sum += pv[i]*coef[i];
  return sum;
}

// Find an approximate projective camera that approximates a rational camera
// at the world center.
bool vpgl_proj_camera_convert::
convert( vpgl_rational_camera<double> const& rat_cam,
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
  std::vector<std::vector<double> > coeffs = rat_cam.coefficients();
  std::vector<double> neu_u = coeffs[0];
  std::vector<double> den_u = coeffs[1];
  std::vector<double> neu_v = coeffs[2];
  std::vector<double> den_v = coeffs[3];

  // normalize for numerical precision
  double nmax_u = -vnl_numeric_traits<double>::maxval;
  double dmax_u  = nmax_u, nmax_v = nmax_u,dmax_v = nmax_u;
  for (unsigned i = 0; i<20; ++i)
  {
    if (std::fabs(neu_u[i])>nmax_u)
      nmax_u=std::fabs(neu_u[i]);
    if (std::fabs(den_u[i])>dmax_u)
      dmax_u=std::fabs(den_u[i]);
    if (std::fabs(neu_v[i])>nmax_v)
      nmax_v=std::fabs(neu_v[i]);
    if (std::fabs(den_v[i])>dmax_v)
      dmax_v=std::fabs(den_v[i]);
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

  // Convert linear approximations to each poly
  //lin_p(x, y, z)=p(0,0,0) + (dp/dx)(x-nx0) + (dp/dy)(y-ny0) + (dp/dz)(z-nz0)
  std::vector<double> pv0 = pvector(nx0,ny0,nz0);
  double neu_u_0 = pval(pv0,neu_u), den_u_0 = pval(pv0,den_u);
  double neu_v_0 = pval(pv0,neu_v), den_v_0 = pval(pv0,den_v);
  std::vector<double> pv_dx0 = power_vector_dx(nx0,ny0,nz0);
  double neu_u_dx0 = pval(pv_dx0,neu_u), den_u_dx0 = pval(pv_dx0,den_u);
  double neu_v_dx0 = pval(pv_dx0,neu_v), den_v_dx0 = pval(pv_dx0,den_v);
  std::vector<double> pv_dy0 = power_vector_dy(nx0,ny0,nz0);
  double neu_u_dy0 = pval(pv_dy0,neu_u), den_u_dy0 = pval(pv_dy0,den_u);
  double neu_v_dy0 = pval(pv_dy0,neu_v), den_v_dy0 = pval(pv_dy0,den_v);
  std::vector<double> pv_dz0 = power_vector_dz(nx0,ny0,nz0);
  double neu_u_dz0 = pval(pv_dz0,neu_u), den_u_dz0 = pval(pv_dz0,den_u);
  double neu_v_dz0 = pval(pv_dz0,neu_v), den_v_dz0 = pval(pv_dz0,den_v);

  //Construct the matrix to convert the center of projection
  vnl_matrix<double> C(4,4);
  C[0][0]=neu_u_dx0; C[0][1]=neu_u_dy0; C[0][2]=neu_u_dz0; C[0][3]=neu_u_0;
  C[1][0]=den_u_dx0; C[1][1]=den_u_dy0; C[1][2]=den_u_dz0; C[1][3]=den_u_0;
  C[2][0]=neu_v_dx0; C[2][1]=neu_v_dy0; C[2][2]=neu_v_dz0; C[2][3]=neu_v_0;
  C[3][0]=den_v_dx0; C[3][1]=den_v_dy0; C[3][2]=den_v_dz0; C[3][3]=den_v_0;

  vnl_svd<double> svd(C);
  vnl_vector<double> nv = svd.nullvector();
  //assume not at infinity
  nv/=nv[3];
#if 1
  std::cout << "Center of projection\n" << nv << '\n'
           << "Residual\n" << C*nv << '\n';
#endif
  //Normalize with respect to the principal plane normal (principal ray)
  double ndu = std::sqrt(den_u_dx0*den_u_dx0 + den_u_dy0*den_u_dy0 +
                        den_u_dz0*den_u_dz0);
  double ndv = std::sqrt(den_v_dx0*den_v_dx0 + den_v_dy0*den_v_dy0 +
                        den_v_dz0*den_v_dz0);

  // determine if the projection is affine
  if (ndu/std::fabs(den_u_0)<1.0e-10||ndv/std::fabs(den_v_0)<1.0e-10)
  {
    std::cout << "Camera is nearly affine - approximation not implemented!\n";
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
  std::cout << "M matrix\n" << M << '\n';

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
  std::cout << "Flipped Rotation\n" << Qf << '\n'
           << "Flipped Upper Triangular\n" << Rf << '\n';
  vnl_qr<double> uqr(M.as_ref());
  uq = uqr.Q();
  ur = uqr.R();
  std::cout << "UnFlipped Rotation\n" << uq << '\n'
           << "UnFlipped Upper Triangular\n" << ur << '\n'
           << "Det uq " << vnl_det<double>(uq) << '\n';
  //Normalized denominators
  vnl_vector<double> c1(3), c3(3);
  for (unsigned i = 0; i<3; ++i) {
    c1[i]=C[1][i]/ndu;
    c3[i]=C[3][i]/ndv;
  }

  std::cout << "Denominators\n"
           << "C1 " << c1
           << "C3 " << c3;
#endif
  //convert p3 the fourth column of the projection matrix
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
  std::cout << "Kr\n" << Kr << '\n';
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
  std::cout << "Flipped Rotation (KR)\n" << krQf << '\n'
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
  std::cout << "K1\n" << K1 <<'\n'
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
  std::cout << "P Matrix\n" << pmatrix << '\n';
#endif
  camera.set_matrix(pmatrix);
  return true;
}

//:obtain a scaling transformation to normalize world geographic coordinates
//The resulting values will be on the range [-1, 1]
//The transform is valid anywhere the rational camera is valid
vgl_h_matrix_3d<double>
vpgl_proj_camera_convert::norm_trans(vpgl_rational_camera<double> const& rat_cam)
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


bool vpgl_perspective_camera_convert::
convert( vpgl_rational_camera<double> const& rat_cam,
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
  auto ni = static_cast<unsigned>(2*sou.scale());//# image columns
  auto nj = static_cast<unsigned>(2*sov.scale());//# image rows
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
  std::vector<vgl_point_3d<double> > world_pts;
  unsigned count = 0, ntrials = 0;
  while (count<n)
  {
    ntrials++;
    double rx = xrange*(std::rand()/(RAND_MAX+1.0));
    double ry = yrange*(std::rand()/(RAND_MAX+1.0));
    double rz = zrange*(std::rand()/(RAND_MAX+1.0));
    vgl_point_3d<double> wp(xmin+rx, ymin+ry, zmin+rz);
    vgl_point_2d<double> ip = rat_cam.project(wp);
    if (ip.x()<0||ip.x()>ni||ip.y()<0||ip.y()>nj)
      continue;
    world_pts.push_back(wp);
    count++;
  }
  std::cout << "Ntrials " << ntrials << '\n';

  //Normalize world and image points to the range [-1,1]
  std::vector<vgl_point_3d<double> > norm_world_pts;
  std::vector<vgl_point_2d<double> > image_pts, norm_image_pts;
  auto N = static_cast<unsigned int>(world_pts.size());
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
  //Convert solution for rotation and translation and calibration matrix of
  //the perspective camera
  vpgl_calibration_matrix<double> K(kk);
  if (! vpgl_perspective_camera_compute::compute(norm_image_pts,
                                                 norm_world_pts,
                                                 K, camera))
    return false;
  std::cout << camera << '\n';
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
    double error = std::sqrt((ip.x()-U)*(ip.x()-U) + (ip.y()-V)*(ip.y()-V));
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
  std::cout << "Max Error = " << err_max << " at " << max_pt << '\n'
           << "Min Error = " << err_min << " at " << min_pt << '\n'
           << "final cam\n" << camera << '\n';
  return true;
}


bool vpgl_perspective_camera_convert::
convert_local( vpgl_rational_camera<double> const& rat_cam,
               vgl_box_3d<double> const& approximation_volume,
               vpgl_perspective_camera<double>& camera,
               vgl_h_matrix_3d<double>& norm_trans)
{
  // Set up the geo converter.
  double lon_low = approximation_volume.min_x();
  double lat_low = approximation_volume.min_y();
#ifdef DEBUG
  double lon_high = approximation_volume.max_x();
  double lat_high = approximation_volume.max_y();
  assert( lat_low < lat_high && lon_low < lon_high );
#endif
  vpgl_lvcs lvcs_converter( lat_low, lon_low,
                            .5*(approximation_volume.min_z()+approximation_volume.max_z()),
                            vpgl_lvcs::wgs84, vpgl_lvcs::DEG );

  // Get a new local bounding box.
  double min_lx=1e99, min_ly=1e99, min_lz=1e99, max_lx=0, max_ly=0, max_lz=0;
  for ( int cx = 0; cx < 2; ++cx ) {
    for ( int cy = 0; cy < 2; ++cy ) {
      for ( int cz = 0; cz < 2; ++cz ) {
        vgl_point_3d<double> wc(approximation_volume.min_x()*cx + approximation_volume.max_x()*(1-cx),
                                approximation_volume.min_y()*cy + approximation_volume.max_y()*(1-cy),
                                approximation_volume.min_z()*cz + approximation_volume.max_z()*(1-cz) );
        double lcx, lcy, lcz;
        lvcs_converter.global_to_local(wc.x(), wc.y(), wc.z(), vpgl_lvcs::wgs84, lcx, lcy, lcz );
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
#if 0 // unused
  vpgl_scale_offset<double> sox =
    rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX);
#endif
  auto ni = static_cast<unsigned>(2*sou.scale());//# image columns
  auto nj = static_cast<unsigned>(2*sov.scale());//# image rows

  vgl_point_3d<double> minp = approximation_volume.min_point();
  vgl_point_3d<double> maxp = approximation_volume.max_point();
  double xmin = minp.x(), ymin = minp.y(), zmin = minp.z();
  double xrange = maxp.x()-xmin,
         yrange = maxp.y()-ymin,
         zrange = maxp.z()-zmin;
  if (xrange<0||yrange<0||zrange<0)
    return false;
  //Randomly generate points
  unsigned n = 100;
  std::vector<vgl_point_3d<double> > world_pts;
  unsigned count = 0, ntrials = 0;
  while (count<n)
  {
    ++ntrials;
    double rx = xrange*(std::rand()/(RAND_MAX+1.0));
    double ry = yrange*(std::rand()/(RAND_MAX+1.0));
    double rz = zrange*(std::rand()/(RAND_MAX+1.0));
    vgl_point_3d<double> wp(xmin+rx, ymin+ry, zmin+rz);
    vgl_point_2d<double> ip = rat_cam.project(wp);
    if (ip.x()<0||ip.x()>ni||ip.y()<0||ip.y()>nj)
      continue;
    world_pts.push_back(wp);
    count++;
  }
  std::cout << "Ntrials " << ntrials << '\n';

  //Normalize world and image points to the range [-1,1]
  std::vector<vgl_point_3d<double> > norm_world_pts;
  std::vector<vgl_point_2d<double> > image_pts, norm_image_pts;
  auto N = static_cast<unsigned int>(world_pts.size());
  for (unsigned i = 0; i<N; ++i)
  {
    vgl_point_3d<double> wp = world_pts[i];
    vgl_point_2d<double> ip = rat_cam.project(wp);
    image_pts.push_back(ip);
    vgl_point_2d<double> nip(sou.normalize(ip.x()), sov.normalize(ip.y()));
    norm_image_pts.push_back(nip);

    // Convert to local coords.
    double lcx, lcy, lcz;
    lvcs_converter.global_to_local(wp.x(), wp.y(), wp.z(), vpgl_lvcs::wgs84, lcx, lcy, lcz );
    vgl_homg_point_3d<double> wp_loc( lcx, lcy, lcz );

    vgl_homg_point_3d<double> nwp = norm_trans*wp_loc;
    assert(   std::fabs(nwp.x()) <= 1
           && std::fabs(nwp.y()) <= 1
           && std::fabs(nwp.z()) <= 1 );
    norm_world_pts.emplace_back(nwp );
  }
  //Assume identity calibration matrix initially, since image point
  //normalization remove any scale and offset from image coordinates
  vnl_matrix_fixed<double, 3, 3> kk;
  kk.fill(0);
  kk[0][0]= 1.0;
  kk[1][1]= 1.0;
  kk[2][2]=1.0;
  //Convert solution for rotation and translation and calibration matrix of
  //the perspective camera
  vpgl_calibration_matrix<double> K(kk);
  if (! vpgl_perspective_camera_compute::compute(norm_image_pts,
                                                 norm_world_pts,
                                                 K, camera))
    return false;
  std::cout << camera << '\n';
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
    double error = std::sqrt((ip.x()-U)*(ip.x()-U) + (ip.y()-V)*(ip.y()-V));
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
  std::cout << "Max Error = " << err_max << " at " << max_pt << '\n'
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
static bool interp_ray(std::vector<vgl_ray_3d<double> > const& ray_nbrs,
                       vgl_ray_3d<double> & intrp_ray)
{
  auto nrays = static_cast<unsigned int>(ray_nbrs.size());
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

// convert tolerances on ray origin and ray direction to test interpolation
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

bool vpgl_generic_camera_convert::
upsample_rays(std::vector<vgl_ray_3d<double> > const& ray_nbrs,
              vgl_ray_3d<double> const& ray,
              std::vector<vgl_ray_3d<double> >& interp_rays)
{
  auto nrays = static_cast<unsigned int>(ray_nbrs.size());
  if (nrays!=4) return false;
  vgl_ray_3d<double> r00 = ray_nbrs[0],
                     r01 = ray_nbrs[1];
  vgl_ray_3d<double> r10 = ray_nbrs[2],
                     r11 = ray_nbrs[3];
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
vpgl_generic_camera_convert::interp_pair(vgl_ray_3d<double> const& r0,
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
// convert a generic camera from a local rational camera
// the approach is to form a pyramid and convert rays by
// back-projecting to top and bottom planes of the valid
// elevations of the rational camera. Successive higher resolution
// layers of the pyramid are populated until the interpolation of
// a ray by the four adjacent neighbors is sufficiently accurate
// The remaining layers of the pyramid are filled in by interpolation
//
bool vpgl_generic_camera_convert::
convert( vpgl_local_rational_camera<double> const& rat_cam,
         int ni, int nj, vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  // get z bounds
  double zoff = rat_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  double zscl = rat_cam.scale(vpgl_rational_camera<double>::Z_INDX);
  // construct high and low planes
  // NOTE: z_scale seems to usually be much larger than actual dimensions of scene, which
  //       sometimes causes trouble for vpgl_backproj_plane, which causes the conversion to fail.
  //       Using half scale value for now, but maybe we should consider taking "top" and "bottom"
  //       z values as user-specified inputs.  -dec 15 Nov 2011
  double el_low = zoff-zscl/2;
  double el_high = zoff+zscl/2;
  double lon = rat_cam.offset(vpgl_rational_camera<double>::X_INDX);
  double lat = rat_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  double x,y, z_low, z_high;
  // convert high and low elevations to local z values
  rat_cam.lvcs().global_to_local(lon,lat,el_low,vpgl_lvcs::wgs84,x,y,z_low,vpgl_lvcs::DEG);
  rat_cam.lvcs().global_to_local(lon,lat,el_high,vpgl_lvcs::wgs84,x,y,z_high,vpgl_lvcs::DEG);
  return convert(rat_cam, ni, nj, gen_cam, z_low, z_high, level);
}

//
// convert a generic camera from a local rational camera
// the approach is to form a pyramid and convert rays by
// back-projecting to top and bottom planes of the valid
// elevations of the rational camera. Successive higher resolution
// layers of the pyramid are populated until the interpolation of
// a ray by the four adjacent neighbors is sufficiently accurate
// The remaining layers of the pyramid are filled in by interpolation
//
#if 0
bool vpgl_generic_camera_convert::
convert( vpgl_local_rational_camera<double> const& rat_cam,
         int ni, int nj, vpgl_generic_camera<double> & gen_cam,
         double local_z_min, double local_z_max, unsigned level)
{
  vgl_plane_3d<double> high(0.0, 0.0, 1.0, -local_z_max);
  vgl_plane_3d<double> low(0.0, 0.0, 1.0, -local_z_min);

  // initial guess for backprojection to planes
  vgl_point_3d<double> org(0.0, 0.0, local_z_max), endpt(0.0, 0.0, local_z_min);
  // initialize the ray pyramid
  // convert the required number of levels
  double dim = ni;
  if (nj<ni)
    dim = nj;
  double lv = std::log(dim)/std::log(2.0);
  int n_levels = static_cast<int>(lv+1.0);// round up
  if (dim*std::pow(0.5, static_cast<double>(n_levels-1)) < 3.0) n_levels--;
  // construct pyramid of ray indices
  // the row and column dimensions at each level
  std::vector<int> nr(n_levels,0), nc(n_levels,0);
  // the scale factor at each level (to transform back to level 0)
  std::vector<double> scl(n_levels,1.0);
  std::vector<vbl_array_2d<vgl_ray_3d<double> > > ray_pyr(n_levels);
  ray_pyr[0].resize(nj, ni);
  ray_pyr[0].fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),vgl_vector_3d<double>(0,0,1)));
  nr[0]=nj;   nc[0]=ni; scl[0]=1.0;
  std::cout<<"(ni,nj)"<<ni<<","<<nj<<std::endl;
  int di = (ni+1)/2 +1,
      dj = (nj+1)/2 +1;

  for (int i=1; i<n_levels; ++i)
  {
    std::cout<<"(di,dj)"<<di<<","<<dj<<std::endl;
    ray_pyr[i].resize(dj,di);
    ray_pyr[i].fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),
                                       vgl_vector_3d<double>(0,0,1)));
    nr[i]=dj;
    nc[i]=di;
    scl[i]=2.0*scl[i-1];
    di = (di+1)/2 +1 ;
    dj = (dj+1)/2 +1;

  }
  // convert the ray interpolation tolerances
  double org_tol = 0.0;
  double ang_tol = 0.0;
  double max_org_err = 0.0, max_ang_err = 0.0;
  if (!ray_tol(rat_cam, ni/2.0, nj/2.0, high,
               org, low, endpt, org_tol, ang_tol))
    return false;
  bool need_interp = true;
  int lev = n_levels-1;
  for (; lev>=0&&need_interp; --lev) {
    // set rays at current pyramid level
    for (int j =0; j<nr[lev]; ++j) {
      int sj = static_cast<int>(scl[lev]*j);
      if (sj>=nj)
          sj = nj-1;
      for (int i =0;i<nc[lev]; ++i)
      {
        int si = static_cast<int>(scl[lev]*i);
        if (si>=ni)
            si = ni-1;
        vgl_point_2d<double> ip(si,sj);
        vgl_point_3d<double> prev_org(0.0,0.0,local_z_max);
        vgl_point_3d<double> prev_endpt(0.0, 0.0, local_z_min);
        // initialize guess with
        if (lev < n_levels-1) {
          double rel_scale = scl[lev]/scl[lev+1];
          int i_above = static_cast<int>(rel_scale * i);
          int j_above = static_cast<int>(rel_scale * j);

          prev_org = ray_pyr[lev+1][j_above][i_above].origin();
          vgl_vector_3d<double> prev_dir = ray_pyr[lev+1][j_above][i_above].direction();
          // find endpoint
          double ray_len = (local_z_min - prev_org.z()) / prev_dir.z();
          prev_endpt = prev_org + (prev_dir * ray_len);
        }
        constexpr double error_tol = 0.25; // allow projection error of 0.25 pixel
        if (!vpgl_backproject::bproj_plane(&rat_cam, ip, high, prev_org, org, error_tol))
          return false;
        if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, prev_endpt, endpt, error_tol))
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
    std::vector<vgl_ray_3d<double> > ray_nbrs(4);

    for (int j =1; (j<nr[lev]-1)&&!need_interp; ++j) {
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
        if(need_interp)
            std::cout<<lev<<", "<<i<<","<<j<<std::endl;
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
    std::vector<vgl_ray_3d<double> > ray_nbrs(4);
    std::vector<vgl_ray_3d<double> > interp_rays(4);
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
  if ((int)level < n_levels) {
    gen_cam = vpgl_generic_camera<double>(ray_pyr,nr,nc);
    return true;
  }
  else
    return false;
}
#endif


bool vpgl_generic_camera_convert::pyramid_est(vpgl_local_rational_camera<double> const& rat_cam,
                                              int ni, int nj,int offseti, int offsetj,
                                               double local_z_min, double local_z_max,
                                              int n_levels,std::vector<int> nr, std::vector<int> nc,
                                              std::vector<unsigned int> scl,std::vector<vbl_array_2d<vgl_ray_3d<double> > > & ray_pyr )

{
  vgl_plane_3d<double> high(0.0, 0.0, 1.0, -local_z_max);
  vgl_plane_3d<double> low(0.0, 0.0, 1.0, -local_z_min);

  // initial guess for backprojection to planes
  vgl_point_3d<double> org(0.0, 0.0, local_z_max), endpt(0.0, 0.0, local_z_min);
  // convert the ray interpolation tolerances
  double org_tol = 0.0;
  double ang_tol = 0.0;
  double max_org_err = 0.0, max_ang_err = 0.0;
  if (!ray_tol(rat_cam,offseti+ ni/2.0, offsetj +nj/2.0, high,
      org, low, endpt, org_tol, ang_tol))
      return false;

  bool need_interp = true;
  int lev = n_levels-1;
  //std::cout<<" lev "<<lev<<" ";
  for (; lev>=0&&need_interp; --lev) {
      // set rays at current pyramid level
      for (int j =0; j<nr[lev]; ++j) {
          int sj = offsetj+static_cast<int>(scl[lev]*j);
          //sj = (j == 0)? sj : sj -1;
          //if (sj>=gnj)
          //    sj =gnj;
          for (int i =0;i<nc[lev]; ++i)
          {
              int si =  offseti+ static_cast<int>(scl[lev]*i);
              //si = (i == 0)? si : si -1;
              //if (si>=gni)
              //    si = gni;
              vgl_point_2d<double> ip(si,sj);
              vgl_point_3d<double> prev_org(0.0,0.0,local_z_max);
              vgl_point_3d<double> prev_endpt(0.0, 0.0, local_z_min);
              // initialize guess with
              if (lev < n_levels-1) {
                  double rel_scale =  scl[lev]/scl[lev+1];
                  int i_above = static_cast<int>(rel_scale * i);
                  int j_above = static_cast<int>(rel_scale * j);
                  prev_org = ray_pyr[lev+1][j_above][i_above].origin();
                  vgl_vector_3d<double> prev_dir = ray_pyr[lev+1][j_above][i_above].direction();
                  // find endpoint
                  double ray_len = (local_z_min - prev_org.z()) / prev_dir.z();
                  prev_endpt = prev_org + (prev_dir * ray_len);
              }
              constexpr double error_tol = 0.5; // allow projection error of 0.25 pixel
              if (!vpgl_backproject::bproj_plane(&rat_cam, ip, high, prev_org, org, error_tol))
                  return false;
              if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, prev_endpt, endpt, error_tol))
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
      std::vector<vgl_ray_3d<double> > ray_nbrs(4);

      for (int j =1; (j<nr[lev]-1)&&!need_interp; ++j) {
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
#if 0
              if(need_interp)
                  std::cout<<lev<<", "<<i<<","<<j<<std::endl;
#endif
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
      std::vector<vgl_ray_3d<double> > ray_nbrs(4);
      std::vector<vgl_ray_3d<double> > interp_rays(4);
      for (unsigned int j = 0; j<nrb; ++j) {
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
  }
  return true;
}
#if 1
//: Implementation of breaking up images in 256x256 blocks
bool vpgl_generic_camera_convert::
convert( vpgl_local_rational_camera<double> const& rat_cam,
         int gni, int gnj, vpgl_generic_camera<double> & gen_cam,
         double local_z_min, double local_z_max, unsigned level)
{
  vgl_plane_3d<double> high(0.0, 0.0, 1.0, -local_z_max);
  vgl_plane_3d<double> low(0.0, 0.0, 1.0, -local_z_min);

  // initial guess for backprojection to planes
  vgl_point_3d<double> org(0.0, 0.0, local_z_max), endpt(0.0, 0.0, local_z_min);
  // initialize the ray pyramid
  // convert the required number of levels

  unsigned int mindim = gni < gnj ? gni : gnj;
  unsigned int factor = 256;
  unsigned int n_levels  = 6;

  while( mindim < factor )
  {
      factor /= 2;
      n_levels--;

      std::cout<<"."<<std::endl;
  }
  unsigned int numi = gni/factor;
  unsigned int numj = gnj/factor;

  // construct pyramid of ray indices
  // the row and column dimensions at each level
  std::vector<int> nr(n_levels,0), nc(n_levels,0);
  vbl_array_2d<vgl_ray_3d<double> > finalrays(gnj,gni);

  std::vector<unsigned int> scl(n_levels);
  unsigned int running_scale = 1;
  std::vector<vbl_array_2d<vgl_ray_3d<double> > > ray_pyr(n_levels);
  for(unsigned int i = 0; i < n_levels; i++)
  {
    nr[i] = factor/running_scale + 1;
    nc[i] = factor/running_scale + 1;
    ray_pyr[i].resize(nr[i], nc[i]);
    ray_pyr[i].fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),vgl_vector_3d<double>(0,0,1)));
    scl[i]=running_scale;
    running_scale*=2;
    finalrays.fill(vgl_ray_3d<double>(vgl_point_3d<double>(0,0,0),vgl_vector_3d<double>(0,0,1)));
  }
  for (unsigned int bigi=0 ; bigi<=numi;bigi++)
  {
      unsigned int ni = factor;
      unsigned int offseti = bigi*factor;
      if(bigi == numi )
          offseti = gni-factor;
      for (unsigned int bigj=0 ; bigj<=numj;bigj++)
      {
          unsigned int nj = factor;
          unsigned int offsetj = bigj*factor;
          if(bigj == numj )
              offsetj = gnj-factor;
          // vpgl_generic_camera expects pixels centered at integer values (pyramid_est is agnostic;
          // although, as uints, offseti and offsetj will center pixels on integers)
          if(!vpgl_generic_camera_convert::pyramid_est(rat_cam,ni,nj,offseti,offsetj,local_z_min, local_z_max,n_levels,nr, nc,scl,ray_pyr ))
              return false;
          for(unsigned int i = 0; i < factor; i++)
          {
              for(unsigned int j = 0; j < factor; j++)
              {
                  if(j+offsetj < gnj && i+offseti <gni )
                      finalrays[j+offsetj][i+offseti] = ray_pyr[0][j][i];
              }
          }
      }
  }
  if (level < n_levels) {
      gen_cam = vpgl_generic_camera<double>(finalrays);
      return true;
  }
  else
      return false;
}
#endif
#if 1
bool vpgl_generic_camera_convert::
convert_bruteforce( vpgl_local_rational_camera<double> const& rat_cam,
         int gni, int gnj, vpgl_generic_camera<double> & gen_cam,
         double local_z_min, double local_z_max, unsigned  /*level*/)
{
  vgl_plane_3d<double> high(0.0, 0.0, 1.0, -local_z_max);
  vgl_plane_3d<double> low(0.0, 0.0, 1.0, -local_z_min);

  // initial guess for backprojection to planes
  vgl_point_3d<double> org(0.0, 0.0, local_z_max), endpt(0.0, 0.0, local_z_min);
  // initialize the ray pyramid
  // convert the required number of levels
    vbl_array_2d<vgl_ray_3d<double> > finalrays(gnj,gni);
  for(unsigned i = 0 ; i < gni; i++)
  {
      for(unsigned j = 0 ; j < gnj; j++)
      {
          vgl_point_2d<double> ip(i,j);
          constexpr double error_tol = 0.5; // allow projection error of 0.25 pixel
          if (!vpgl_backproject::bproj_plane(&rat_cam, ip, high, org, org, error_tol))
              return false;
          if (!vpgl_backproject::bproj_plane(&rat_cam, ip, low, endpt, endpt, error_tol))
              return false;
          vgl_vector_3d<double> dir = endpt-org;
          finalrays[j][i].set(org, dir);
      }
  }

      gen_cam = vpgl_generic_camera<double>(finalrays);
      return true;
}
#endif
bool vpgl_generic_camera_convert::
convert( vpgl_proj_camera<double> const& prj_cam, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_ray_3d<double> ray;
  vgl_homg_point_2d<double> ipt;
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      ipt.set(i*scale, j*scale, 1.0);
      ray = prj_cam.backproject_ray(ipt);
      rays[j][i]=ray;
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

bool vpgl_generic_camera_convert::
convert( vpgl_perspective_camera<double> const& per_cam, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_ray_3d<double> ray;
  vgl_homg_point_2d<double> ipt;
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      ipt.set(i*scale, j*scale, 1.0);
      ray = per_cam.backproject_ray(ipt);
      rays[j][i]=ray;
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

bool vpgl_generic_camera_convert::
convert_with_margin( vpgl_perspective_camera<double> const& per_cam, int ni, int nj,
                     vpgl_generic_camera<double> & gen_cam, int margin, unsigned level)
{
  vbl_array_2d<vgl_ray_3d<double> > rays(nj+2*margin, ni+2*margin);
  vgl_ray_3d<double> ray;
  vgl_homg_point_2d<double> ipt;
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));
  for (int j = -margin; j<nj+margin; ++j)
    for (int i = -margin; i<ni+margin; ++i) {
      ipt.set(i*scale, j*scale, 1.0);
      ray = per_cam.backproject_ray(ipt);
      rays[j+margin][i+margin]=ray;
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}


// the affine camera defines a principal plane which is
// far enough from the scene origin so that all the scene
// geometry is in front of the plane. The backproject function
// constructs finite ray origins on the principal plane.
bool vpgl_generic_camera_convert::
convert( vpgl_affine_camera<double> const& aff_cam, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));
  // The ray direction is the camera center (which is at inifnity)
  vgl_homg_point_3d<double> cent = aff_cam.camera_center();
  vgl_vector_3d<double> dir(cent.x(), cent.y(), cent.z());

  // get the principal plane, on which all ray origins will lie
  vgl_homg_plane_3d<double> plane = aff_cam.principal_plane();

  // compute the transformation from image coordinates to X,Y,Z on the principal plane
  vnl_matrix_fixed<double,3,4> P = aff_cam.get_matrix();
  double u0 = P(0,3);
  double v0 = P(1,3);

  vnl_matrix_fixed<double,3,3> A;
  for (unsigned j=0; j<3; ++j) {
    A(0,j) = P(0,j);
    A(1,j) = P(1,j);
  }
  A(2,0) = plane.a();
  A(2,1) = plane.b();
  A(2,2) = plane.c();

  // invA maps (u-u0, v-v0, -d) to X,Y,Z on the principal plane
  vnl_matrix_fixed<double,3,3> invA = vnl_svd<double>(A).inverse();

  // construct the array of camera rays
  vgl_point_3d<double> org;;
  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  for (int j = 0; j<nj; ++j) {
    for (int i = 0; i<ni; ++i) {
      vnl_vector_fixed<double,3> ipt(i*scale-u0, j*scale-v0, -plane.d());
      vnl_vector_fixed<double,3> org_vnl = invA * ipt;
      // convert from vnl_vector to vgl_vector
      org.set(org_vnl[0], org_vnl[1], org_vnl[2]);
      rays[j][i].set(org, dir);
    }
  }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}


bool vpgl_generic_camera_convert::
convert( vpgl_camera_double_sptr const& camera, int ni, int nj,
         vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  if (auto* cam =
      dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr()))
    return vpgl_generic_camera_convert::convert(*cam, ni, nj, gen_cam, level);

  if (auto* cam =
      dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr())) {
    return vpgl_generic_camera_convert::convert(*cam, ni, nj, gen_cam, level);
  }

  if (auto* cam =
      dynamic_cast<vpgl_affine_camera<double>*>(camera.ptr()))
    return vpgl_generic_camera_convert::convert(*cam, ni, nj, gen_cam, level);

  if (auto* cam =
      dynamic_cast<vpgl_proj_camera<double>*>(camera.ptr())) {
    return vpgl_generic_camera_convert::convert(*cam, ni, nj, gen_cam, level);
  }

  return false;
}


#if HAS_GEOTIFF
//: Convert a geocam (transformtaion matrix read from a geotiff header + an lvcs) to a generic camera
bool vpgl_generic_camera_convert::convert( vpgl_geo_camera& geocam, int ni, int nj, double height,
                                           vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));

  //: all rays have the same direction
  vgl_vector_3d<double> dir(0.0, 0.0, -1.0);

  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_point_3d<double> org;

  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      double x,y,z;
      geocam.backproject(i*scale, j*scale,x,y,z);
      org.set(x, y, height);
      rays[j][i].set(org, dir);
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

//: Convert a geocam (transformtaion matrix read from a geotiff header + an lvcs) to a generic camera using the specified ray direction (not necessarily nadir rays)
//  basically creates a camera with parallel rays but the rays can be in any direction
bool vpgl_generic_camera_convert::convert( vpgl_geo_camera& geocam, int ni, int nj, double height, vgl_vector_3d<double>& dir,
                                           vpgl_generic_camera<double> & gen_cam, unsigned level)
{
  double scale = (level < 32) ? double(1L<<level) : std::pow(2.0,static_cast<double>(level));

  vbl_array_2d<vgl_ray_3d<double> > rays(nj, ni);
  vgl_point_3d<double> org;

  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      double x,y,z;
      geocam.backproject(i*scale, j*scale,x,y,z);
      org.set(x, y, height);
      rays[j][i].set(org, dir);
    }
  gen_cam = vpgl_generic_camera<double>(rays);
  return true;
}

bool vpgl_affine_camera_convert::
convert( vpgl_local_rational_camera<double> const& camera_in,
         vgl_box_3d<double> const& region_of_interest,
         vpgl_affine_camera<double>& camera_out,
         unsigned int num_points)
{
  vnl_random rng;
  const double width = region_of_interest.width();
  const double height = region_of_interest.height();
  const double depth = region_of_interest.depth();
  double min_x = region_of_interest.min_x();
  double min_y = region_of_interest.min_y();
  double min_z = region_of_interest.min_z();

  std::vector< vgl_point_2d<double> > image_pts;
  std::vector< vgl_point_3d<double> > world_pts;
  for (unsigned i=0; i<num_points; ++i) {
    double x = rng.drand64()*width + min_x;  // sample in local coords
    double y = rng.drand64()*depth + min_y;
    double z = rng.drand64()*height + min_z;
    world_pts.emplace_back(x,y,z);
    double u, v;
    camera_in.project(x,y,z,u,v);  // local rational camera has an lvcs, so it handles, local coord to global to image point projection internally
    image_pts.emplace_back(u,v);
  }

  bool success = vpgl_affine_camera_compute::compute(image_pts, world_pts, camera_out);
  // it is assumed that the camera is above the region of interest
  camera_out.set_viewing_distance(height*10);
  camera_out.orient_ray_direction(vgl_vector_3d<double>(0,0,-1));
  return success;
}
#else
bool vpgl_affine_camera_convert::
convert(vpgl_local_rational_camera<double> const& camera_in,
  vgl_box_3d<double> const& region_of_interest,
  vpgl_affine_camera<double>& camera_out,
  unsigned int num_points)
{
  return false; //Always report failure if GEOTIFF not available.
}
#endif // HAS_GEOTIFF

#endif // vpgl_camera_convert_cxx_
