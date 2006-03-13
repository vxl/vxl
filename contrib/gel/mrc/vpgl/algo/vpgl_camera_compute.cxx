// This is gel/mrc/vpgl/algo/vpgl_camera_compute.cxx
#ifndef vpgl_camera_compute_cxx_
#define vpgl_camera_compute_cxx_

#include "vpgl_camera_compute.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>


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
    vcl_cerr << "vpgl_affine_camera_compute:compute() cannot compute, input data "
             << "has insufficient rank.\n";
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

#endif // vpgl_camera_compute_cxx_
