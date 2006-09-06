// This is gel/mrc/vpgl/algo/vpgl_construct_cameras.cxx
#include "vpgl_construct_cameras.h"
//:
// \file

#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_4.h>
#include <vnl/algo/vnl_svd.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include "vpgl_fm_compute_8_point.h"

//: constructor
vpgl_construct_cameras::vpgl_construct_cameras()
{
}

//: constructor with initialization of corresponding points
vpgl_construct_cameras::vpgl_construct_cameras(
    vcl_vector<vgl_point_2d<double> > p0,
    vcl_vector<vgl_point_2d<double> > p1,
    vpgl_calibration_matrix<double>* K )
{
    points0_=p0;
    points1_=p1;

    if( p0.size() < 8 ) 
      vcl_cerr << "ERROR: vpgl_construct_cameras: need at least 7 correspondences.\n";

    if( K == NULL ){
      K_[0][0]=2000;K_[0][1]=0;K_[0][2]=512;
      K_[1][0]=0;K_[1][1]=2000;K_[1][2]=384;
      K_[2][0]=0;K_[2][1]=0;K_[2][2]=1;
    }
    else
      K_ = K->get_matrix();

}

vpgl_construct_cameras::~vpgl_construct_cameras()
{
}

//: to construct the cameras according to the correspondence given
bool vpgl_construct_cameras::construct()
{
    vcl_vector<vgl_homg_point_2d<double> > p0,p1;

    for (unsigned int i=0;i<points0_.size();i++)
    {
      vgl_homg_point_2d<double> p(points0_[i]);
      p0.push_back(p);
    }
    for (unsigned int i=0;i<points1_.size();i++)
    {
      vgl_homg_point_2d<double> p(points1_[i]);
      p1.push_back(p);
    }

    vpgl_fundamental_matrix<double> fm;
    vpgl_fm_compute_8_point fmcomp(false); // Set preconditioning false, otherwise breaks!!
    fmcomp.compute( p0, p1, fm );

    double fm_error = 0;
    for( int i = 0; i < p0.size(); i++ ){
      vnl_vector<double> pt0(3), pt1(3);
      pt0(0) = p0[i].x(); pt0(1) = p0[i].y(); pt0(2) = 1;
      pt1(0) = p1[i].x(); pt1(1) = p1[i].y(); pt1(2) = 1;
      vnl_vector<double> m = fm.get_matrix()*pt0;
      fm_error += abs( pt1(0)*m(0)+pt1(1)*m(1)+pt1(2)*m(2) );
    }
    if( fm_error > .2 ){
      vcl_cerr << "\nWARNING: vpgl_construct_cameras: fundamental matrix error is " <<
        fm_error << '\n';
    }

    vnl_double_3x3  Kt,Kinv;
    Kt=K_.transpose();
    Kinv=vnl_inverse(K_);

    //: computing the essential matrix
    E_=Kt*fm.get_matrix()*K_;
    vnl_double_3x3 U, V, W;
    
    W[0][0]=0;W[0][1]=-1;W[0][2]=0;
    W[1][0]=1;W[1][1]=0;W[1][2]=0;
    W[2][0]=0;W[2][1]=0;W[2][2]=1;

    vnl_svd<double> SVD(E_);
    U=SVD.U();
    V=SVD.V();

    // Get some image points to test possible cameras on.
    vnl_vector<double> point2d1(3);    vnl_vector<double> point2d2(3);
    point2d1[0]=points0_[0].x();       point2d2[0]=points1_[0].x();
    point2d1[1]=points0_[0].y();       point2d2[1]=points1_[0].y();
    point2d1[2]=1;                     point2d2[2]=1;
    point2d1=Kinv*point2d1;
    point2d2=Kinv*point2d2;
    vgl_point_2d<double> pnorm1(point2d1[0]/point2d1[2],point2d1[1]/point2d1[2]);
    vgl_point_2d<double> pnorm2(point2d2[0]/point2d2[2],point2d2[1]/point2d2[2]);

    // checking for the correct combination of cameras
    for( int c = 0; c < 4; c++ ){
      vnl_double_3x3 R;
      vnl_vector<double> t;
      if( c == 0 ){ //case 1
        R=U*W.transpose()*V.transpose();
        t=U.get_column(2);
      }
      if( c == 1 ){ //case 2
        R=U*W*V.transpose();
        t=U.get_column(2);
      }
      if( c == 2 ){ //case 3
        R=U*W.transpose()*V.transpose();
        t=-U.get_column(2);
      }
      if( c == 3 ){ //case 4
        R=U*W*V.transpose();
        t=-U.get_column(2);
      } 
      if( vnl_det<double>( R ) < 0 ) R = -R;
      vnl_vector<double> cc = -R.transpose()*t;
      vnl_vector_fixed<double,3> zeros(0.0);
      vgl_h_matrix_3d<double> R_vgl(R,zeros);
      P2_.set_rotation_matrix( R_vgl );
      P2_.set_camera_center( vgl_point_3d<double>( cc(0), cc(1), cc(2) ) );
      vgl_point_3d<double> p3d = triangulate_3d_point(
        pnorm1, P1_.get_matrix(), pnorm2, P2_.get_matrix() );
      vgl_homg_point_3d<double> ph3d(p3d);
      if (!P1_.is_behind_camera(ph3d) && !P2_.is_behind_camera(ph3d))
        break;
      if( c == 3 ){
        vcl_cerr << "ERROR: vpgl_construct_cameras failed.\n";
        return false;
      }
    }

    P1_.set_calibration( K_ );
    P2_.set_calibration( K_ );
    return true;
};


vgl_point_3d<double>
vpgl_construct_cameras::triangulate_3d_point(
  const vgl_point_2d<double>& x1, 
  const vnl_double_3x4& P1,
  const vgl_point_2d<double>& x2, 
  const vnl_double_3x4& P2 )
{
  vnl_double_4x4 A;

  for (int i=0; i<4; i++){
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }

  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}
