// This is brl/bbas/bvgl/bvgl_norm_trans_2d.txx
#ifndef bvgl_norm_trans_2d_txx_
#define bvgl_norm_trans_2d_txx_
//:
// \file

#include "bvgl_norm_trans_2d.h"
#include <vgl/vgl_point_2d.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//

//: Default constructor
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d() : bvgl_h_matrix_2d<T>()
{
}

//: Copy constructor
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d(const bvgl_norm_trans_2d<T>& M)
: bvgl_h_matrix_2d<T>(M)
{
}


//: Constructor from vcl_istream
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d(vcl_istream& s)
: bvgl_h_matrix_2d<T>(s)
{
}

//: Constructor from file
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d(char const* filename)
: bvgl_h_matrix_2d<T>(filename)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d(vnl_matrix_fixed<T,3,3> const& M)
: bvgl_h_matrix_2d<T>(M)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
bvgl_norm_trans_2d<T>::bvgl_norm_trans_2d(const T* H)
: bvgl_h_matrix_2d<T>(H)
{
}

//: Destructor
template <class T>
bvgl_norm_trans_2d<T>::~bvgl_norm_trans_2d()
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
bool bvgl_norm_trans_2d<T>::
compute_from_points(vcl_vector<vgl_homg_point_2d<T> > const& points)
 {
   T cx, cy, radius;
   this->center_of_mass(points, cx, cy);
   t12_matrix_.set_identity();
   t12_matrix_.put(0,2, -cx);    t12_matrix_.put(1,2, -cy);
   vcl_vector<vgl_homg_point_2d<T> > temp;
   typedef typename vcl_vector<vgl_homg_point_2d<T> >::const_iterator iter;
   for (iter pit = points.begin(); pit != points.end(); pit++)
     {
       vgl_homg_point_2d<T> p((*this)(*pit));
       temp.push_back(p);
     }
   //Points might be coincident
   if (!this->scale_xyroot2(temp, radius))
     return false;
   T scale = 1/radius;
   t12_matrix_.put(0,0, scale);
   t12_matrix_.put(1,1, scale);
   t12_matrix_.put(0,2, -cx*scale);
   t12_matrix_.put(1,2, -cy*scale);
   return true;
 }

template <class T>
bool bvgl_norm_trans_2d<T>::
compute_from_lines(vcl_vector<vgl_homg_line_2d<T>  > const& /*lines*/)
{
  vcl_cerr << "bvgl_norm_trans_2d<T>::compute_from_lines() not yet implemented\n";
  return false;
}

//-------------------------------------------------------------------
// Find the center of a point cloud
//
template <class T>
void bvgl_norm_trans_2d<T>::
center_of_mass(vcl_vector<vgl_homg_point_2d<T> > const& in, T& cx, T& cy)
{
  T cog_x = 0;
  T cog_y = 0;
  T cog_count = 0.0;
  T tol = 1e-06;
  unsigned n = in.size();
  for (unsigned i = 0; i < n; ++i)
    {
    if (in[i].ideal(tol))
      continue;
    vgl_point_2d<T> p(in[i]);
    T x = p.x();
    T y = p.y();
    cog_x += x;
    cog_y += y;
    ++cog_count;
    }
  if (cog_count > 0) {
    cog_x /= cog_count;
    cog_y /= cog_count;
  }
  //output result
  cx = cog_x;
  cy = cog_y;
}
//-------------------------------------------------------------------
// Find the mean distance of the input pointset. Assumed to have zero mean
//
template <class T>
bool bvgl_norm_trans_2d<T>::
scale_xyroot2(vcl_vector<vgl_homg_point_2d<T> > const& in, T& radius)
{
  T magnitude = T(0);
  int numfinite = 0;
  T tol = T(1e-06);
  radius = T(0);
  for (unsigned i = 0; i < in.size(); ++i)
    {
      if (in[i].ideal(tol))
        continue;
      vgl_point_2d<T> p(in[i]);
      vnl_vector_fixed<T, 2> v(p.x(), p.y());
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
#undef BVGL_NORM_TRANS_2D_INSTANTIATE
#define BVGL_NORM_TRANS_2D_INSTANTIATE(T) \
template class bvgl_norm_trans_2d<T >

#endif // bvgl_norm_trans_2d_txx_
