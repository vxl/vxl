// This is gel/mrc/vpgl/algo/vpgl_camera_compute.cxx
#ifndef vpgl_camera_compute_cxx_
#define vpgl_camera_compute_cxx_

#include "vpgl_camera_compute.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>


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

  // Form the solution matrix. Only use the min configuration to compute the camera,
  // can extend this later.
  vnl_matrix<double> S( 11, 12, 0);
  for ( int i = 0; i < 6; ++i ) {
    S(2*i,0) = -image_pts[i].w()*world_pts[i].x();
    S(2*i,1) = -image_pts[i].w()*world_pts[i].y();
    S(2*i,2) = -image_pts[i].w()*world_pts[i].z();
    S(2*i,3) = -image_pts[i].w()*world_pts[i].w();
    S(2*i,8) = image_pts[i].x()*world_pts[i].x();
    S(2*i,9) = image_pts[i].x()*world_pts[i].y();
    S(2*i,10) = image_pts[i].x()*world_pts[i].z();
    S(2*i,11) = image_pts[i].x()*world_pts[i].w();
    if ( i == 5 ) break;
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
{
  vcl_vector<double> pv(20);
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
  for(unsigned i = 0; i<20; ++i)
    sum += pv[i]*coef[i];
  return sum;
}

// Find an approximate projective camera that approximates a rational camera
// at the world center.
bool vpgl_proj_camera_compute::
compute( vgl_point_3d<double> const& world_center,
         vpgl_proj_camera<double>& camera)
{
  double x0 = world_center.x(), y0 = world_center.y(), z0 = world_center.z();
  //normalized world center
  double nx0 = 
    rat_cam_.scl_off(vpgl_rational_camera<double>::X_INDX).normalize(x0);
  double ny0 = 
    rat_cam_.scl_off(vpgl_rational_camera<double>::Y_INDX).normalize(y0);
  double nz0 = 
    rat_cam_.scl_off(vpgl_rational_camera<double>::Z_INDX).normalize(z0);

  // get the rational coefficients
  vcl_vector<vcl_vector<double> > coeffs = rat_cam_.coefficients();
  vcl_vector<double> neu_u = coeffs[0];
  vcl_vector<double> den_u = coeffs[1];
  vcl_vector<double> neu_v = coeffs[2];
  vcl_vector<double> den_v = coeffs[3];

  // normalize for numerical precision
  double nmax_u = -vnl_numeric_traits<double>::maxval;
  double dmax_u  = nmax_u, nmax_v = nmax_u,dmax_v = nmax_u;
  for(unsigned i = 0; i<20; ++i)
    {
      if(vcl_fabs(neu_u[i])>nmax_u)
        nmax_u=vcl_fabs(neu_u[i]);
      if(vcl_fabs(den_u[i])>dmax_u)
        dmax_u=vcl_fabs(den_u[i]);
      if(vcl_fabs(neu_v[i])>nmax_v)
        nmax_v=vcl_fabs(neu_v[i]);
      if(vcl_fabs(den_v[i])>dmax_v)
        dmax_v=vcl_fabs(den_v[i]);
    }
  //Normalize polys so that ratio of neumerator and denominator is unchanged
  double norm_u = nmax_u, norm_v = nmax_v;
  if(norm_u<dmax_u) norm_u = dmax_u;
  if(norm_v<dmax_v) norm_v = dmax_v;
  for(unsigned i = 0; i<20; ++i)
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
#if 0
  vcl_cout << "Center of projection\n";
  vcl_cout << nv << '\n';


  vcl_cout << "Residual\n";
  vcl_cout << C*nv << '\n';
#endif
  //Normalize with respect to the principal plane normal (principal ray)
  double ndu = vcl_sqrt(den_u_dx0*den_u_dx0 + den_u_dy0*den_u_dy0 +
                        den_u_dz0*den_u_dz0);
  double ndv = vcl_sqrt(den_v_dx0*den_v_dx0 + den_v_dy0*den_v_dy0 +
                        den_v_dz0*den_v_dz0);

  // determine if the projection is affine
  if(ndu/vcl_fabs(den_u_0)<1.0e-10||ndv/vcl_fabs(den_v_0)<1.0e-10)
    {
      vcl_cout << "Camera is nearly affine - approximation not implemented!\n";
      return false;
    }
  //Construct M by joined scale factor vector
  vnl_matrix_fixed<double, 3, 3> M;
  for(unsigned i = 0; i<3; ++i)
    {
      M[0][i]=C[0][i]/ndu;
      M[1][i]=C[2][i]/ndv;
      M[2][i]=(C[1][i]/ndu + C[3][i]/ndv)/2;
    }
#if 0
  vcl_cout << "M matrix\n";
  vcl_cout << M << '\n';
#endif
  //compute p3 the fourth column of the projection matrix
  vnl_vector_fixed<double, 3> c;
  for(unsigned i = 0; i<3; ++i)
	  c[i]=nv[i];
  vnl_vector_fixed<double, 3> p3 = -M*c;
  //Form the full projection matrix
  vnl_matrix_fixed<double, 3, 4> pmatrix;
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<3; ++c)
      pmatrix[r][c] = M[r][c];
  for(unsigned r = 0; r<3; ++r)
    pmatrix[r][3] = p3[r];
  //account for the image scale and offsets
  double uscale = rat_cam_.scale(vpgl_rational_camera<double>::U_INDX);
  double uoff = rat_cam_.offset(vpgl_rational_camera<double>::U_INDX);
  double vscale = rat_cam_.scale(vpgl_rational_camera<double>::V_INDX);
  double voff = rat_cam_.offset(vpgl_rational_camera<double>::V_INDX);
  vnl_matrix_fixed<double, 3, 3> Kr;
  Kr.fill(0.0);
  Kr[0][0]=uscale;   Kr[0][2]=uoff;
  Kr[1][1]=vscale;   Kr[1][2]=voff;
  Kr[2][2]=1.0;
  //Need to offset x0, y0 and z0 as well.
    vnl_matrix_fixed<double, 4, 4> T;
    T.fill(0.0);
    T[0][0]=1.0; T[1][1]=1.0; T[2][2]=1.0; T[3][3]=1.0;
    T[0][3] = -nx0; T[1][3]= -ny0; T[2][3]=-nz0;
  pmatrix = Kr*pmatrix*T;
#if 0
   vcl_cout << "P Matrix \n" << pmatrix << '\n';
#endif
  camera.set_matrix(pmatrix);
  return true;  
}
//:obtain a scaling transformation to normalize world geographic coordinates
//The resulting values will be on the range [-1, 1]
//The transform is valid anywhere the rational camera is valid
vgl_h_matrix_3d<double> vpgl_proj_camera_compute::norm_trans()
{
  double xscale = rat_cam_.scale(vpgl_rational_camera<double>::X_INDX);
  double xoff = rat_cam_.offset(vpgl_rational_camera<double>::X_INDX);
  double yscale = rat_cam_.scale(vpgl_rational_camera<double>::Y_INDX);
  double yoff = rat_cam_.offset(vpgl_rational_camera<double>::Y_INDX);
  double zscale = rat_cam_.scale(vpgl_rational_camera<double>::Z_INDX);
  double zoff = rat_cam_.offset(vpgl_rational_camera<double>::Z_INDX);
  vgl_h_matrix_3d<double> T;
  T.set_identity();
  T.set(0,0,1/xscale); T.set(1,1,1/yscale); T.set(2,2,1/zscale);
  T.set(0,3, -xoff/xscale);   T.set(1,3, -yoff/yscale);
  T.set(2,3, -zoff/zscale);
  return T;
}




#endif // vpgl_camera_compute_cxx_
