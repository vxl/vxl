// This is core/vpgl/vpgl_affine_camera.hxx
#ifndef vpgl_affine_camera_hxx_
#define vpgl_affine_camera_hxx_
//:
// \file

#include "vpgl_affine_camera.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_ray_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera()
{
  vnl_vector_fixed<T, 4> row1((T)1, (T)0, (T)0, (T)0);
  vnl_vector_fixed<T, 4> row2((T)0, (T)1, (T)0, (T)0);
  set_rows(row1, row2);
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
  vpgl_proj_camera<T>::set_matrix( C );
  view_distance_ = (T)0;
  vgl_homg_point_3d<T> cc = vpgl_proj_camera<T>::camera_center();
  ray_dir_.set(cc.x(), cc.y(), cc.z());
  ray_dir_ = normalize(ray_dir_);
}

template <class T>
vpgl_affine_camera<T>::
vpgl_affine_camera(vgl_vector_3d<T> ray, vgl_vector_3d<T> up,
                   vgl_point_3d<T> stare_pt,
                   T u0, T v0, T su, T sv) {

  vgl_vector_3d<T> uvec = normalized(up), rvec = normalized(ray);
  vnl_matrix_fixed<T,3,3> R;
  if (std::fabs(dot_product<T>(uvec,rvec)-T(1))<1e-5)
  {
    T r[] = { 1, 0, 0,
              0, 1, 0,
              0, 0, 1 };

    R = vnl_matrix_fixed<T,3,3>(r);
  }
  else if (std::fabs(dot_product<T>(uvec,rvec)-T(-1))<1e-5)
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
  for (unsigned i = 0; i<3; ++i) {
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
  ray_dir_.set(rvec.x(), rvec.y(), rvec.z());
}


//------------------------------------------
template <class T>
void vpgl_affine_camera<T>::set_rows(
  const vnl_vector_fixed<T,4>& row1,
  const vnl_vector_fixed<T,4>& row2 )
{
  vnl_matrix_fixed<T,3,4> C( (T)0 );
  for ( unsigned int i = 0; i < 4; ++i ) {
    C(0,i) = row1(i);
    C(1,i) = row2(i);
  }
  C(2,3) = (T)1;
  vpgl_proj_camera<T>::set_matrix( C );

  // set ray_dir to match new projection matrix
  vgl_homg_point_3d<T> cc = vpgl_proj_camera<T>::camera_center();
  ray_dir_.set(cc.x(), cc.y(), cc.z());
  ray_dir_ = normalize(ray_dir_);
}

template <class T>
bool vpgl_affine_camera<T>::set_matrix( const vnl_matrix_fixed<T,3,4>& new_camera_matrix )
{
  assert( new_camera_matrix(2,3) != 0 );
  vnl_matrix_fixed<T,3,4> C( new_camera_matrix );
  C = C/C(2,3);
  C(2,0) = (T)0; C(2,1) = (T)0; C(2,2) = (T)0;
  vpgl_proj_camera<T>::set_matrix( C );

  vgl_homg_point_3d<T> cc = vpgl_proj_camera<T>::camera_center();
  vgl_vector_3d<T> old_ray_dir = ray_dir_;
  ray_dir_.set(cc.x(), cc.y(), cc.z());
  ray_dir_ = normalize(ray_dir_);
  // assume that new and old ray directions should not differ by more than 90 deg.
  // if this assumption is false, caller should call orient_ray_direction() afterwards.
  orient_ray_direction(old_ray_dir);
  return true;
}

template <class T>
bool vpgl_affine_camera<T>::set_matrix( const T* new_camera_matrix_p )
{
  vnl_matrix_fixed<T,3,4> new_camera_matrix( new_camera_matrix_p );
  set_matrix( new_camera_matrix );
  return true;
}

//: Find the 3d coordinates of the center of the camera. Will be an ideal point with the sense of the ray direction.
template <class T>
vgl_homg_point_3d<T> vpgl_affine_camera<T>::camera_center() const
{
  vgl_homg_point_3d<T> temp(ray_dir_.x(), ray_dir_.y(), ray_dir_.z(), (T)0);
  return temp;
}

//: Find the 3d ray that goes through the camera center and the provided image point.
template <class T>
vgl_homg_line_3d_2_points<T> vpgl_affine_camera<T>::
backproject( const vgl_homg_point_2d<T>& image_point ) const
{
  vgl_homg_line_3d_2_points<T> ret;
  //get line from projective camera
  vgl_homg_line_3d_2_points<T> line =
    vpgl_proj_camera<T>::backproject(image_point);
  vgl_homg_point_3d<T> cph = vgl_closest_point_origin(line);
  if (!is_ideal(cph, vgl_tolerance<T>::position)) {
  vgl_point_3d<T> cp(cph);
  vgl_point_3d<T> eye_pt = cp-(view_distance_*ray_dir_);
  vgl_homg_point_3d<T> pt_fin(eye_pt.x(), eye_pt.y(), eye_pt.z());
  vgl_homg_point_3d<T> pinf(ray_dir_.x(), ray_dir_.y(), ray_dir_.z(), (T)0);
  ret = vgl_homg_line_3d_2_points<T>(pt_fin, pinf);
  }
  else
    std::cout << "Warning vpgl_affine_camera::backproject produced line at infinity\n";
  return ret;
}

template <class T>
vgl_ray_3d<T> vpgl_affine_camera<T>::
backproject_ray( const vgl_homg_point_2d<T>& image_point ) const
{
  vgl_homg_line_3d_2_points<T> line = backproject( image_point );
  return vgl_ray_3d<T>(vgl_point_3d<T>(line.point_finite()), ray_dir_);
}

template <class T>
vpgl_affine_camera<T>* vpgl_affine_camera<T>::clone(void) const
{
  return new vpgl_affine_camera<T>(*this);
}


//: Find the world plane parallel to the image plane intersecting the camera center.
template <class T>
vgl_homg_plane_3d<T> vpgl_affine_camera<T>::
principal_plane() const
{
  //note that d = view_distance_ not -view_distance_,
  //since dir points towards the origin
  vgl_homg_plane_3d<T> ret(ray_dir_.x(), ray_dir_.y(),
                           ray_dir_.z(), view_distance_);
  return ret;
}

//: flip the ray direction so that dot product with look_dir is positive
template <class T>
void vpgl_affine_camera<T>::orient_ray_direction(vgl_vector_3d<T> const& look_dir)
{
  if (dot_product(look_dir, ray_dir_) < 0 ) {
    ray_dir_ = -ray_dir_;
  }
}

//: Write vpgl_affine_camera to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s,
                         vpgl_affine_camera<Type> const& c)
{
  s << c.get_matrix() << '\n';
  return s;
}

//: Read camera from stream
template <class Type>
std::istream&  operator >>(std::istream& s,
                          vpgl_affine_camera<Type>& c)
{
  vnl_matrix_fixed<Type, 3, 4> P;
  s >> P;
  c = vpgl_affine_camera<Type>(P);
  return s ;
}


// Code for easy instantiation.
#undef vpgl_AFFINE_CAMERA_INSTANTIATE
#define vpgl_AFFINE_CAMERA_INSTANTIATE(T) \
template class vpgl_affine_camera<T >; \
template std::ostream& operator<<(std::ostream&, const vpgl_affine_camera<T >&); \
template std::istream& operator>>(std::istream&, vpgl_affine_camera<T >&)


#endif // vpgl_affine_camera_hxx_
