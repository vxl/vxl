// This is gel/mrc/vpgl/vpgl_rational_camera.txx
#ifndef vpgl_rational_camera_txx_
#define vpgl_rational_camera_txx_
//:
// \file

#include <vpgl/vpgl_rational_camera.h>
#include <vcl_cassert.h>
#include <vcl_vector.txx>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
//--------------------------------------
// Constructors
//

// Create an identity projection, i.e. (x,y) identically maps to (u,v)
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera()
{
  rational_coeffs_.fill(0);
  rational_coeffs_[DEN_U][19]=1;
  rational_coeffs_[DEN_V][19]=1;
  rational_coeffs_[NEU_U][9]=1; // x coefficient
  rational_coeffs_[NEU_V][15]=1;// y coefficient
  vpgl_scale_offset<T> soff;
  scale_offsets_.resize(5, soff);
}

//: Constructor with an array encoding of the coefficients
template <class T>
vpgl_rational_camera<T>::
vpgl_rational_camera(vcl_vector<vcl_vector<T> > const& rational_coeffs,
                     vcl_vector<vpgl_scale_offset<T> > const& scale_offsets)
{
  this->set_coefficients(rational_coeffs);
  this->set_scale_offsets(scale_offsets);
}

template <class T>
vpgl_rational_camera<T>::
  vpgl_rational_camera(vcl_vector<T> const& neu_u,
                       vcl_vector<T> const& den_u,
                       vcl_vector<T> const& neu_v,
                       vcl_vector<T> const& den_v,
                       const T x_scale, const T x_off,
                       const T y_scale, const T y_off,
                       const T z_scale, const T z_off,
                       const T u_scale, const T u_off,
                       const T v_scale, const T v_off
                      )
{
  for (unsigned i = 0; i<20; ++i)
  {
    rational_coeffs_[NEU_U][i] = neu_u[i];
    rational_coeffs_[DEN_U][i] = den_u[i];
    rational_coeffs_[NEU_V][i] = neu_v[i];
    rational_coeffs_[DEN_V][i] = den_v[i];
  }
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

template <class T>
vpgl_rational_camera<T>* vpgl_rational_camera<T>::clone(void) const
{
  return new vpgl_rational_camera<T>(*this);
}

template <class T>
void vpgl_rational_camera<T>::
set_coefficients(vcl_vector<vcl_vector<T> > const& rational_coeffs)
{
  for (unsigned j = 0; j<4; ++j)
    for (unsigned i = 0; i<20; ++i)
      rational_coeffs_[j][i] = rational_coeffs[j][i];
}

template <class T>
vcl_vector<vcl_vector<T> > vpgl_rational_camera<T>::coefficients() const
{
  vcl_vector<vcl_vector<T> > result(4);
  for (unsigned j = 0; j<4; ++j)
  {
    result[j].resize(20);
    for (unsigned i = 0; i<20; ++i)
      result[j][i]=rational_coeffs_[j][i];
  }
  return result;
}

//: Create a vector with the standard order of monomial terms
template <class T>
vnl_vector_fixed<T, 20>
vpgl_rational_camera<T>::power_vector(const T x, const T y, const T z) const
{
  // Form the monomials in homogeneous form
  double w  = 1;
  double xx = x*x;
  double xy = x*y;
  double xz = x*z;
  double yy = y*y;
  double yz = y*z;
  double zz = z*z;
  double xxx = x*xx;
  double xxy = x*xy;
  double xxz = x*xz;
  double xyy = x*yy;
  double xyz = x*yz;
  double xzz = x*zz;
  double yyy = y*yy;
  double yyz = y*yz;
  double yzz = y*zz;
  double zzz = z*zz;
  double xww = x*w*w;
  double yww = y*w*w;
  double zww = z*w*w;
  double www = w*w*w;
  double xxw = xx*w;
  double xyw = xy*w;
  double xzw = xz*w;
  double yyw = yy*w;
  double yzw = yz*w;
  double zzw = zz*w;

  //fill the vector
  vnl_vector_fixed<T, 20> pv;
  pv.put( 0, xxx);
  pv.put( 1, xxy);
  pv.put( 2, xxz);
  pv.put( 3, xxw);
  pv.put( 4, xyy);
  pv.put( 5, xyz);
  pv.put( 6, xyw);
  pv.put( 7, xzz);
  pv.put( 8, xzw);
  pv.put( 9, xww);
  pv.put(10, yyy);
  pv.put(11, yyz);
  pv.put(12, yyw);
  pv.put(13, yzz);
  pv.put(14, yzw);
  pv.put(15, yww);
  pv.put(16, zzz);
  pv.put(17, zzw);
  pv.put(18, zww);
  pv.put(19, www);
  return pv;
}


// Base projection method
template <class T>
void vpgl_rational_camera<T>::project(const T x, const T y, const T z,
                                      T& u, T& v) const
{
  // scale, offset the world point before projection
  T sx = scale_offsets_[X_INDX].normalize(x);
  T sy = scale_offsets_[Y_INDX].normalize(y);
  T sz = scale_offsets_[Z_INDX].normalize(z);
  vnl_vector_fixed<T, 4> polys = rational_coeffs_*power_vector(sx, sy, sz);
  T su = polys[NEU_U]/polys[DEN_U];
  T sv = polys[NEU_V]/polys[DEN_V];
  // unscale the resulting image coordinates
  u = scale_offsets_[U_INDX].un_normalize(su);
  v = scale_offsets_[V_INDX].un_normalize(sv);
}

//vnl interface methods
template <class T>
vnl_vector_fixed<T, 2>
vpgl_rational_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point)const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2],
                image_point[0], image_point[1]);
 return image_point;
}


//vgl interface methods
template <class T>
vgl_point_2d<T> vpgl_rational_camera<T>::project(vgl_point_3d<T> world_point)const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}


// Code for easy instantiation.
#undef vpgl_RATIONAL_CAMERA_INSTANTIATE
#define vpgl_RATIONAL_CAMERA_INSTANTIATE(T) \
template class vpgl_scale_offset<T >; \
template class vpgl_rational_camera<T >; \
typedef vpgl_scale_offset<T > soff; \
VCL_VECTOR_INSTANTIATE(soff)


#endif // vpgl_rational_camera_txx_

