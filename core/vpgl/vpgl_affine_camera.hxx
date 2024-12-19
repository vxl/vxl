// This is core/vpgl/vpgl_affine_camera.hxx
#ifndef vpgl_affine_camera_hxx_
#define vpgl_affine_camera_hxx_
//:
// \file

#include "vpgl_affine_camera.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_ray_3d.h>
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
vpgl_affine_camera<T>::vpgl_affine_camera(const vnl_vector_fixed<T, 4> & row1, const vnl_vector_fixed<T, 4> & row2)
{
  set_rows(row1, row2);
  view_distance_ = (T)0;
}


//------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera(const vnl_matrix_fixed<T, 3, 4> & camera_matrix)
{
  set_matrix(camera_matrix);
  view_distance_ = (T)0;
}

template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera(vgl_vector_3d<T> ray,
                                          vgl_vector_3d<T> up,
                                          vgl_point_3d<T> stare_pt,
                                          T u0,
                                          T v0,
                                          T su,
                                          T sv)
{

  vgl_vector_3d<T> uvec = normalized(up), rvec = normalized(ray);
  vnl_matrix_fixed<T, 3, 3> R;
  if (std::fabs(dot_product<T>(uvec, rvec) - T(1)) < 1e-5)
  {
    T r[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

    R = vnl_matrix_fixed<T, 3, 3>(r);
  }
  else if (std::fabs(dot_product<T>(uvec, rvec) - T(-1)) < 1e-5)
  {
    T r[] = { 1, 0, 0, 0, 1, 0, 0, 0, -1 };

    R = vnl_matrix_fixed<T, 3, 3>(r);
  }
  else
  {
    vgl_vector_3d<T> x = cross_product(-uvec, rvec);
    vgl_vector_3d<T> y = cross_product(rvec, x);
    normalize(x);
    normalize(y);

    T r[] = { x.x(), x.y(), x.z(), y.x(), y.y(), y.z(), rvec.x(), rvec.y(), rvec.z() };

    R = vnl_matrix_fixed<T, 3, 3>(r);
  }

  // form affine camera
  vnl_vector_fixed<T, 4> r0, r1;
  for (unsigned i = 0; i < 3; ++i)
  {
    r0[i] = su * R[0][i];
    r1[i] = sv * R[1][i];
  }
  r0[3] = 0.0;
  r1[3] = 0.0;
  this->set_rows(r0, r1);
  T u, v;
  this->project(stare_pt.x(), stare_pt.y(), stare_pt.z(), u, v);
  T tu = (u0 - u);
  T tv = (v0 - v);
  r0[3] = tu;
  r1[3] = tv;
  this->set_rows(r0, r1);
  view_distance_ = (T)0;
  ray_dir_.set(rvec.x(), rvec.y(), rvec.z());
}


//------------------------------------------
template <class T>
void
vpgl_affine_camera<T>::set_rows(const vnl_vector_fixed<T, 4> & row1, const vnl_vector_fixed<T, 4> & row2)
{
  vnl_matrix_fixed<T, 3, 4> C((T)0);
  for (unsigned int i = 0; i < 4; ++i)
  {
    C(0, i) = row1(i);
    C(1, i) = row2(i);
  }
  C(2, 3) = (T)1;
  vpgl_proj_camera<T>::set_matrix(C);

  // camera center for affine ray direction r is given by
  //  r . (a00, a01, a02) = r. A0 = 0
  //  r . (a10, a11, a12) = r. A1 = 0
  // so r is orthogonal to both A0 and A1
  vnl_vector_fixed<T, 3> A0, A1, r;
  for (size_t i = 0; i < 3; ++i)
  {
    A0[i] = row1[i];
    A1[i] = row2[i];
  }
  r = vnl_cross_3d<T>(A0, A1);
  ray_dir_.set(r[0], r[1], r[2]);
  ray_dir_ /= ray_dir_.length();
}

template <class T>
bool
vpgl_affine_camera<T>::set_matrix(const vnl_matrix_fixed<T, 3, 4> & new_camera_matrix)
{
  T norm = new_camera_matrix(2, 3);
  if (norm == T(0))
  {
    std::cerr << "vpgl_affine_camera::set_matrix normalization failure" << std::endl;
    return false;
  }

  vnl_vector_fixed<T, 4> row0, row1;
  for (size_t i = 0; i < 4; ++i)
  {
    row0[i] = new_camera_matrix[0][i] / norm;
    row1[i] = new_camera_matrix[1][i] / norm;
  }
  this->set_rows(row0, row1);
  return true;
}

template <class T>
bool
vpgl_affine_camera<T>::set_matrix(const T * new_camera_matrix_p)
{
  vnl_matrix_fixed<T, 3, 4> new_camera_matrix(new_camera_matrix_p);
  return set_matrix(new_camera_matrix);
}

//: Find the 3d coordinates of the center of the camera. Will be an ideal point with the sense of the ray direction.
template <class T>
vgl_homg_point_3d<T>
vpgl_affine_camera<T>::camera_center() const
{
  vgl_homg_point_3d<T> temp(ray_dir_.x(), ray_dir_.y(), ray_dir_.z(), (T)0);
  return temp;
}

//: Find the 3d ray that goes through the camera center and the provided image point.
template <class T>
vgl_homg_line_3d_2_points<T>
vpgl_affine_camera<T>::backproject(const vgl_homg_point_2d<T> & image_point) const
{
  // use affine construction of ray to produce the line
  vgl_ray_3d<T> r = this->backproject_ray(image_point);
  vgl_point_3d<T> org = r.origin();
  vgl_vector_3d<T> dir = r.direction();

  // return line (degenerate if appropriate)
  if (dir.length() == T(0))
    return vgl_homg_line_3d_2_points<T>(vgl_homg_point_3d<T>(org), vgl_homg_point_3d<T>(org));
  else
    return vgl_homg_line_3d_2_points<T>(vgl_homg_point_3d<T>(org), vgl_homg_point_3d<T>(org + dir));
}

template <class T>
vgl_ray_3d<T>
vpgl_affine_camera<T>::backproject_ray(const vgl_homg_point_2d<T> & image_point) const
{
  // return degnerate ray on failure
  if (image_point.ideal(vgl_tolerance<T>::position))
  {
    std::cerr << "Backproject ray from ideal point - degenerate result" << std::endl;
    vgl_point_3d<T> org = vgl_point_3d<T>(T(0), T(0), T(0));
    return vgl_ray_3d<T>(org, org);
  }

  // ray direction is already known only have to get a point in the ray.
  // form the A0, A1 vectors
  vnl_matrix_fixed<T, 3, 4> P = this->get_matrix();
  vnl_vector_fixed<T, 3> A0, A1;
  for (size_t i = 0; i < 3; ++i)
  {
    A0[i] = P[0][i];
    A1[i] = P[1][i];
  }
  vnl_matrix_fixed<T, 2, 2> D(T(0)), Dinv;
  D[0][0] = dot_product(A0, A0);
  D[0][1] = dot_product(A0, A1);
  D[1][0] = D[0][1];
  D[1][1] = dot_product(A1, A1);
  double det = vnl_det(D);

  // check singular determinant
  if (fabs(det) < T(2) * vgl_tolerance<T>::position)
  {
    std::cerr << "Backproject ray singular determinant - degenerate result" << std::endl;
    vgl_point_3d<T> org = vgl_point_3d<T>(T(0), T(0), T(0));
    return vgl_ray_3d<T>(org, org);
  }

  Dinv = vnl_inverse(D);
  vnl_vector_fixed<T, 2> uv;
  uv[0] = image_point.x() / image_point.w();
  uv[1] = image_point.y() / image_point.w();
  vnl_vector_fixed<T, 2> UV, a;
  UV[0] = uv[0] - P[0][3];
  UV[1] = uv[1] - P[1][3];
  a = Dinv * UV;
  vnl_vector_fixed<T, 3> p;
  p = a[0] * A0 + a[1] * A1;
  vgl_point_3d<T> org(p[0], p[1], p[2]);

  // intersect ray with principal plane (specified by "view_distance")
  // to obtain ray origin on the plane
  if (view_distance_ == T(0))
  {
    return vgl_ray_3d<T>(org, ray_dir_);
  }
  else
  {
    T md = -view_distance_;
    vgl_point_3d<T> pl_org = org + md * ray_dir_;
    return vgl_ray_3d<T>(pl_org, ray_dir_);
  }
}

template <class T>
vpgl_affine_camera<T> *
vpgl_affine_camera<T>::clone() const
{
  return new vpgl_affine_camera<T>(*this);
}

//: Find the world plane parallel to the image plane intersecting the camera center.
template <class T>
vgl_homg_plane_3d<T>
vpgl_affine_camera<T>::principal_plane() const
{
  // note that d = view_distance_ not -view_distance_,
  // since dir points towards the origin
  vgl_homg_plane_3d<T> ret(ray_dir_.x(), ray_dir_.y(), ray_dir_.z(), view_distance_);
  return ret;
}

//: flip the ray direction so that dot product with look_dir is positive
template <class T>
void
vpgl_affine_camera<T>::orient_ray_direction(const vgl_vector_3d<T> & look_dir)
{
  if (dot_product(look_dir, ray_dir_) < 0)
  {
    ray_dir_ = -ray_dir_;
  }
}

//: Write vpgl_affine_camera to stream
template <class Type>
std::ostream &
operator<<(std::ostream & s, const vpgl_affine_camera<Type> & c)
{
  s << c.get_matrix() << '\n';
  return s;
}

//: Read camera from stream
template <class Type>
std::istream &
operator>>(std::istream & s, vpgl_affine_camera<Type> & c)
{
  vnl_matrix_fixed<Type, 3, 4> P;
  s >> P;
  c = vpgl_affine_camera<Type>(P);
  return s;
}

// the homography that transforms the camera to [I2x2|02x2]
//                                              [  01x3 |1]
//
// Note that A  = [I2x2|02x2] H, where H4x4 = [  A_3x4  ]
//                [  01x3 |1]                 [ 01x3  |1]
//
// thus, canonical_h = H^-1
//
template <class T>
vgl_h_matrix_3d<T>
get_canonical_h(const vpgl_affine_camera<T> & camera)
{

  vnl_matrix_fixed<T, 3, 4> A = camera.get_matrix();
  vnl_svd<T> temp(A.as_ref());
  vnl_matrix_fixed<T, 4, 3> Ainv = temp.pinverse();
  vnl_matrix_fixed<T, 4, 4> Hinv(0.0), Hp(0.0);
  for (size_t r = 0; r < 4; ++r)
  {
    Hp[r][r] = T(1);
    for (size_t c = 0; c < 3; ++c)
    {
      Hinv[r][c] = Ainv[r][c];
    }
  }
  Hinv[3][3] = T(1);
  Hp[0][3] = -A[0][3];
  Hp[1][3] = -A[1][3]; // remove 4th column (translation)
  vgl_h_matrix_3d<T> ret(Hinv * Hp);
  return ret;
}

template <class T>
vpgl_affine_camera<T>
premultiply_a(const vpgl_affine_camera<T> & in_camera, const vnl_matrix_fixed<T, 3, 3> & transform)
{
  return vpgl_affine_camera<T>(transform * in_camera.get_matrix());
}

template <class T>
vpgl_affine_camera<T>
postmultiply_a(const vpgl_affine_camera<T> & in_camera, const vnl_matrix_fixed<T, 4, 4> & transform)
{
  return vpgl_affine_camera<T>(in_camera.get_matrix() * transform);
}

template <class T>
vpgl_affine_camera<T>
postmultiply_a(const vpgl_affine_camera<T> & in_camera, const vnl_vector_fixed<T, 3> & translation)
{
  const vnl_matrix_fixed<T, 3, 4> m = in_camera.get_matrix();
  vnl_matrix_fixed<T, 3, 4> tm = m;
  vnl_vector_fixed<T, 4> r0 = m.get_row(0), r1 = m.get_row(1);
  vnl_vector_fixed<T, 4> t;
  for (size_t c = 0; c < 3; ++c)
    t[c] = translation[c];
  t[3] = T(1);
  T dp0 = dot_product(r0, t);
  tm[0][3] = dp0;
  T dp1 = dot_product(r1, t);
  tm[1][3] = dp1;
  return vpgl_affine_camera<T>(tm);
}

// Code for easy instantiation.
#undef vpgl_AFFINE_CAMERA_INSTANTIATE
#define vpgl_AFFINE_CAMERA_INSTANTIATE(T)                                                                          \
  template class vpgl_affine_camera<T>;                                                                            \
  template std::ostream & operator<<(std::ostream &, const vpgl_affine_camera<T> &);                               \
  template std::istream & operator>>(std::istream &, vpgl_affine_camera<T> &);                                     \
  template vgl_h_matrix_3d<T> get_canonical_h(const vpgl_affine_camera<T> &);                                      \
  template vpgl_affine_camera<T> premultiply_a(const vpgl_affine_camera<T> &, const vnl_matrix_fixed<T, 3, 3> &);  \
  template vpgl_affine_camera<T> premultiply_a(const vpgl_affine_camera<T> &, const vgl_h_matrix_2d<T> &);         \
  template vpgl_affine_camera<T> postmultiply_a(const vpgl_affine_camera<T> &, const vnl_matrix_fixed<T, 4, 4> &); \
  template vpgl_affine_camera<T> postmultiply_a(const vpgl_affine_camera<T> &, const vgl_h_matrix_3d<T> &);        \
  template vpgl_affine_camera<T> postmultiply_a(const vpgl_affine_camera<T> &, const vnl_vector_fixed<T, 3> &);    \
  template vpgl_affine_camera<T> postmultiply_a(const vpgl_affine_camera<T> &, const vgl_vector_3d<T> &)


#endif // vpgl_affine_camera_hxx_
