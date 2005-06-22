// This is core/vgl/algo/vgl_norm_trans_2d.txx
#ifndef vgl_norm_trans_2d_txx_
#define vgl_norm_trans_2d_txx_
//:
// \file

#include "vgl_norm_trans_2d.h"
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//

//: Default constructor
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d() : vgl_h_matrix_2d<T>()
{
}

//: Copy constructor
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d(const vgl_norm_trans_2d<T>& M)
: vgl_h_matrix_2d<T>(M)
{
}


//: Constructor from vcl_istream
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d(vcl_istream& s)
: vgl_h_matrix_2d<T>(s)
{
}

//: Constructor from file
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d(char const* filename)
: vgl_h_matrix_2d<T>(filename)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d(vnl_matrix_fixed<T,3,3> const& M)
: vgl_h_matrix_2d<T>(M)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_norm_trans_2d<T>::vgl_norm_trans_2d(const T* H)
: vgl_h_matrix_2d<T>(H)
{
}

//: Destructor
template <class T>
vgl_norm_trans_2d<T>::~vgl_norm_trans_2d()
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
bool vgl_norm_trans_2d<T>::
compute_from_points(vcl_vector<vgl_homg_point_2d<T> > const& points,
                    bool isotropic)
{
  T cx, cy;
  this->center_of_mass(points, cx, cy);
  this->t12_matrix_.set_identity();
  this->t12_matrix_.put(0,2, -cx);    this->t12_matrix_.put(1,2, -cy);
  vcl_vector<vgl_homg_point_2d<T> > temp;
  for (typename vcl_vector<vgl_homg_point_2d<T> >::const_iterator
       pit = points.begin(); pit != points.end(); pit++)
  {
    vgl_homg_point_2d<T> p((*this)(*pit));
    temp.push_back(p);
  }
  
  if (isotropic){
    T radius = 1;
    //Points might be coincident
    if(!this->scale_xyroot2(temp, radius))
      return false;
    T scale = 1/radius;
    this->t12_matrix_.put(0,0, scale);
    this->t12_matrix_.put(1,1, scale);
    this->t12_matrix_.put(0,2, -cx*scale);
    this->t12_matrix_.put(1,2, -cy*scale);
    return true;
  }
  T sdx = 1, sdy = 1, c = 1, s = 0;
  if(!this->scale_aniostropic(temp, sdx, sdy, c, s))
    return false;
  T scx = 1/sdx, scy = 1/sdy;
  this->t12_matrix_.put(0, 0, c*scx);
  this->t12_matrix_.put(0, 1, -s*scx);
  this->t12_matrix_.put(0, 2, (-c*scx*cx+s*scx*cy));
  this->t12_matrix_.put(1, 0, s*scy);
  this->t12_matrix_.put(1, 1, c*scy);
  this->t12_matrix_.put(1, 2, (-s*scy*cx-c*scy*cy));
  return true;
}

//-----------------------------------------------------------------
//:
//  The normalizing transform for lines is computed from the
//  set of points defined by the intersection of the perpendicular from
//  the origin with the line.  Each such point is given by:
//    $p = [-a*c, -b*c, \sqrt(a^2+b^2)]^T$
//  If we assume the line is normalized then:
//    $p = [-a*c, -b*c, 1]^T$
//
template <class T>
bool vgl_norm_trans_2d<T>::
compute_from_lines(vcl_vector<vgl_homg_line_2d<T> > const& lines,
                   bool isotropic)
{
  vcl_vector<vgl_homg_point_2d<T> > points;
  for (typename vcl_vector<vgl_homg_line_2d<T> >::const_iterator lit=lines.begin();
       lit != lines.end(); lit++)
  {
    vgl_homg_line_2d<T> l = (*lit);
    vgl_homg_point_2d<T> p(-l.a()*l.c(), -l.b()*l.c(), l.a()*l.a()+l.b()*l.b());
    points.push_back(p);
  }
  return this->compute_from_points(points, isotropic);
}

//-----------------------------------------------------------------
//:
// The normalizing transform for points and lines is computed from the set of
// points used by compute_from_points() & the one used by compute_from_lines().
//
template <class T>
bool vgl_norm_trans_2d<T>::
compute_from_points_and_lines(vcl_vector<vgl_homg_point_2d<T> > const& pts,
                              vcl_vector<vgl_homg_line_2d< T> > const& lines,
                              bool isotropic)
{
  vcl_vector<vgl_homg_point_2d<T> > points = pts;
  for (typename vcl_vector<vgl_homg_line_2d<T> >::const_iterator lit=lines.begin();
       lit != lines.end(); lit++)
  {
    vgl_homg_line_2d<T> l = (*lit);
    vgl_homg_point_2d<T> p(-l.a()*l.c(), -l.b()*l.c(), l.a()*l.a()+l.b()*l.b());
    points.push_back(p);
  }
  return this->compute_from_points(points, isotropic);
}

//-------------------------------------------------------------------
// Find the center of a point cloud
//
template <class T>
void vgl_norm_trans_2d<T>::
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
  if (cog_count > 0)
  {
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
bool vgl_norm_trans_2d<T>::
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
//------------------------------------------------------------
// Anisotropic scaling of the point set around the center of gravity.
// Determines the principal axes and standard deviations along the principal
// axes.  Assumes that the pointset has zero mean, so ::center_of_mass should
// be removed before calling this function.
template <class T>
bool vgl_norm_trans_2d<T>::
scale_aniostropic(vcl_vector<vgl_homg_point_2d<T> > const& in,
                       T& sdx, T& sdy, T& c, T& s)
{
  T tol = T(1e-06);
  unsigned count = 0;
  unsigned n = in.size();
  //The point scatter matrix coefficients
  double Sx2=0, Sxy=0, Sy2=0;
  for (unsigned i = 0; i < n; ++i)
  {
    if (in[i].ideal(tol))
      continue;
    ++count;
    vgl_point_2d<T> p(in[i]);
    T x = p.x();
    T y = p.y();
    Sx2 += (double)x*x;
    Sxy += (double)x*y;
    Sy2 += (double)y*y;
  }
  if(!count)
    return false;

  double t =0.0;
  // Compute the rotation that makes Sxy zero
  if( Sx2 != Sy2 )
    t = 0.5*vcl_atan( -2.0*Sxy/(Sx2-Sy2) );

  double dc = vcl_cos(t),  ds = vcl_sin(t);

  /* determine the standard deviations in the rotated frame */
  double sddx = vcl_sqrt( (dc*dc*Sx2-2.0*dc*ds*Sxy+ds*ds*Sy2)/count );
  double sddy = vcl_sqrt( (ds*ds*Sx2+2.0*dc*ds*Sxy+dc*dc*Sy2)/count );
  
  //cast back to T
  sdx = static_cast<T>(sddx);
  sdy = static_cast<T>(sddy);
  c = static_cast<T>(dc);
  s = static_cast<T>(ds);
  return (sdx>tol && sdy >tol);
}

//----------------------------------------------------------------------------
#undef VGL_NORM_TRANS_2D_INSTANTIATE
#define VGL_NORM_TRANS_2D_INSTANTIATE(T) \
template class vgl_norm_trans_2d<T >

#endif // vgl_norm_trans_2d_txx_
