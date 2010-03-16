// This is gel/mrc/vpgl/vpgl_affine_camera.txx
#ifndef vpgl_affine_camera_txx_
#define vpgl_affine_camera_txx_
//:
// \file

#include "vpgl_affine_camera.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vcl_cassert.h>

//-------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera()
{
  vnl_matrix_fixed<T,3,4> C( (T)0 );
  C(0,0) = C(1,1) = C(2,3) = (T)1;
  set_matrix( C );
  view_distance_ = (T)0;
}


//-------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera( const vnl_vector_fixed<T,4>& row1,
                                           const vnl_vector_fixed<T,4>& row2 )
{
  set_rows( row1, row2 );
  view_distance_ = (T)0;
}


//------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera( const vnl_matrix_fixed<T,3,4>& camera_matrix )
{
  assert( camera_matrix(2,3) != 0 );
  vnl_matrix_fixed<T,3,4> C( camera_matrix );
  C = C/C(2,3);
  C(2,0) = (T)0; C(2,1) = (T)0; C(2,2) = (T)0;
  set_matrix( C );
  view_distance_ = (T)0;
}

template <class T>
vpgl_affine_camera<T>::
vpgl_affine_camera(vgl_vector_3d<T> ray, vgl_vector_3d<T> up,
                   vgl_point_3d<T> stare_pt,
                   T u0, T v0, T su, T sv){

  vgl_vector_3d<T> uvec = normalized(up), rvec = normalized(ray);
  vnl_matrix_fixed<T,3,3> R;
  if(vcl_fabs(dot_product<T>(uvec,rvec)-T(1))<1e-5)
  {
  
    T r[] = { 1, 0, 0,
              0, 1, 0,
              0, 0, 1 };

    R = vnl_matrix_fixed<T,3,3>(r);
  }
  else if(vcl_fabs(dot_product<T>(uvec,rvec)-T(-1))<1e-5)
  {
      T r[] = { 1, 0, 0,
              0, 1, 0,
              0, 0, -1 };

      R = vnl_matrix_fixed<T,3,3>(r);
  }
  else
  {
  vgl_vector_3d<T> x = cross_product(-uvec,rvec);
  vgl_vector_3d<T> y = cross_product(rvec,x);
  normalize(x);
  normalize(y);

  T r[] = { x.x(), x.y(), x.z(),
            y.x(), y.y(), y.z(),
            rvec.x(), rvec.y(), rvec.z() };

  R = vnl_matrix_fixed<T,3,3>(r);
  }

  //form affine camera
  vnl_vector_fixed<T, 4> r0, r1;
  for(unsigned i = 0; i<3; ++i){
    r0[i] = su*R[0][i];
    r1[i] = sv*R[1][i];
  }
  r0[3]= 0.0;   r1[3]= 0.0;
  this->set_rows(r0, r1);
  T u, v;
  this->project(stare_pt.x(), stare_pt.y(), stare_pt.z(), u, v);
  T tu = (u0-u);
  T tv = (v0-v);
  r0[3]=tu; r1[3]=tv;
  this->set_rows(r0, r1);
  view_distance_ = (T)0;
}


//------------------------------------------
template <class T>
void vpgl_affine_camera<T>::set_rows(
  const vnl_vector_fixed<T,4>& row1,
  const vnl_vector_fixed<T,4>& row2 )
 {
  vnl_matrix_fixed<T,3,4> C( (T)0 );
  for ( unsigned int i = 0; i < 4; i++ ) {
    C(0,i) = row1(i);
    C(1,i) = row2(i);
  }
  C(2,3) = (T)1;
  set_matrix( C );
 }

  //: Find the 3d ray that goes through the camera center and the provided image point.
template <class T>
vgl_homg_line_3d_2_points<T> vpgl_affine_camera<T>::
backproject( const vgl_homg_point_2d<T>& image_point ) const
{
  //get line from projective camera
  vgl_homg_line_3d_2_points<T> line = 
    vpgl_proj_camera::backproject(image_point);
  vgl_homg_point_3d<T> cph = vgl_closest_point_origin(line);
  vgl_point_3d<T> cp(cph);
  vgl_homg_point_3d<T> pi = line.point_infinite();
  vgl_vector_3d<T> dir(pi.x(), pi.y(), pi.z());
  dir = normalize(dir);
  vgl_point_3d<T> eye_pt = cp-(view_distance_*dir);
  vgl_homg_point_3d<T> pt_fin(eye_pt.x(), eye_pt.y(), eye_pt.z());
  vgl_homg_line_3d_2_points<T> ret(pt_fin, pi);
  return ret;
}

  //: Find the world plane parallel to the image plane intersecting the camera center.
template <class T>
vgl_homg_plane_3d<T> vpgl_affine_camera<T>::
principal_plane() const
{
  //get line from projective camera
  vgl_homg_line_3d_2_points<T> line = 
    vpgl_proj_camera::backproject(vgl_homg_point_2d<T>((T)0,(T)0));
  //get the ray direction which is the plane normal
  vgl_homg_point_3d<T> pt_inf = line.point_infinite();
  vgl_vector_3d<T> dir(pt_inf.x(), pt_inf.y(), pt_inf.z());
  dir = normalize(dir);
  //note that d = view_distance_ not -view_distance_,
  //since dir points towards the origin
  vgl_homg_plane_3d<T> ret(dir.x(), dir.y(), dir.z(), view_distance_);
  return ret;
}


// Code for easy instantiation.
#undef vpgl_AFFINE_CAMERA_INSTANTIATE
#define vpgl_AFFINE_CAMERA_INSTANTIATE(T) \
template class vpgl_affine_camera<T >


#endif // vpgl_affine_camera_txx_
