// This is core/vgl/algo/vgl_norm_trans_3d.txx
#ifndef vgl_norm_trans_3d_txx_
#define vgl_norm_trans_3d_txx_
//:
// \file

#include "vgl_norm_trans_3d.h"
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//

//: Default constructor
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d() : vgl_h_matrix_3d<T>()
{
}

//: Copy constructor
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d(const vgl_norm_trans_3d<T>& M)
: vgl_h_matrix_3d<T>(M)
{
}


//: Constructor from vcl_istream
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d(vcl_istream& s)
: vgl_h_matrix_3d<T>(s)
{
}

//: Constructor from file
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d(char const* filename)
: vgl_h_matrix_3d<T>(filename)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d(vnl_matrix_fixed<T,4,4> const& M)
: vgl_h_matrix_3d<T>(M)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_norm_trans_3d<T>::vgl_norm_trans_3d(const T* H)
: vgl_h_matrix_3d<T>(H)
{
}

//: Destructor
template <class T>
vgl_norm_trans_3d<T>::~vgl_norm_trans_3d()
{
}

// == OPERATIONS ==
//----------------------------------------------------------------
//  Get the normalizing transform for a set of points
// 1) Compute the center of gravity and form the normalizing
//    transformation matrix
// 2) Transform the point set to a temporary collection
// 3) Compute the average point radius
// 4) Complete the normalizing transform
template <class T>
bool vgl_norm_trans_3d<T>::
compute_from_points(vcl_vector<vgl_homg_point_3d<T> > const& points)
{
  T cx, cy, cz, radius;
  this->center_of_mass(points, cx, cy, cz);
  this->t12_matrix_.set_identity();
  this->t12_matrix_.put(0,3, -cx);
  this->t12_matrix_.put(1,3, -cy);
  this->t12_matrix_.put(2,3, -cz);
  vcl_vector<vgl_homg_point_3d<T> > temp;
  for (typename vcl_vector<vgl_homg_point_3d<T> >::const_iterator
       pit = points.begin(); pit != points.end(); pit++)
  {
    vgl_homg_point_3d<T> p((*this)(*pit));
    temp.push_back(p);
  }
  //Points might be coincident
  if (!this->scale_xyzroot2(temp, radius))
    return false;
  T scale = 1/radius;
  this->t12_matrix_.put(0,0, scale);
  this->t12_matrix_.put(1,1, scale);
  this->t12_matrix_.put(2,2, scale);
  this->t12_matrix_.put(0,3, -cx*scale);
  this->t12_matrix_.put(1,3, -cy*scale);
  this->t12_matrix_.put(2,3, -cz*scale);
  return true;
}

//-------------------------------------------------------------------
// Find the center of a point cloud
//
template <class T>
void vgl_norm_trans_3d<T>::
center_of_mass(vcl_vector<vgl_homg_point_3d<T> > const& in, T& cx, T& cy,
T& cz)
{
  T cog_x = 0;
  T cog_y = 0;
  T cog_z = 0;
  T cog_count = 0.0;
  T tol = 1e-06;
  unsigned n = in.size();
  for (unsigned i = 0; i < n; ++i)
  {
    if (in[i].ideal(tol))
      continue;
    vgl_point_3d<T> p(in[i]);
    T x = p.x();
    T y = p.y();
    T z = p.z();
    cog_x += x;
    cog_y += y;
    cog_z += z;
    ++cog_count;
  }
  if (cog_count > 0)
  {
    cog_x /= cog_count;
    cog_y /= cog_count;
    cog_z /= cog_count;
  }
  //output result
  cx = cog_x;
  cy = cog_y;
  cz = cog_z;
}

//-------------------------------------------------------------------
// Find the mean distance of the input pointset. Assumed to have zero mean
//
template <class T>
bool vgl_norm_trans_3d<T>::
scale_xyzroot2(vcl_vector<vgl_homg_point_3d<T> > const& in, T& radius)
{
  T magnitude = T(0);
  int numfinite = 0;
  T tol = T(1e-06);
  radius = T(0);
  for (unsigned i = 0; i < in.size(); ++i)
  {
    if (in[i].ideal(tol))
      continue;
    vgl_point_3d<T> p(in[i]);
    vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.z());
    magnitude += v.magnitude();
    ++numfinite;
  }

  if (numfinite > 0)
  {
    radius = magnitude / numfinite;
    return radius>=tol;
  }
  return false;
}

//----------------------------------------------------------------------------
#undef VGL_NORM_TRANS_3D_INSTANTIATE
#define VGL_NORM_TRANS_3D_INSTANTIATE(T) \
template class vgl_norm_trans_3d<T >

#endif // vgl_norm_trans_3d_txx_
