// This is core/vgl/vgl_box_2d.hxx
#ifndef vgl_box_2d_hxx_
#define vgl_box_2d_hxx_
//:
// \file

#include <iostream>
#include <algorithm>
#include <cmath>
#include "vgl_box_2d.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>

// Constructors/Destructor---------------------------------------------------

template <class Type>
vgl_box_2d<Type>::vgl_box_2d()
{
  min_pos_[0]=min_pos_[1]=(Type)1;
  max_pos_[0]=max_pos_[1]=(Type)0; // empty box
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type const corner1[2],
                             Type const corner2[2])
{
  min_pos_[0]=max_pos_[0]=corner1[0];
  min_pos_[1]=max_pos_[1]=corner1[1];
  this->add(corner2);
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(vgl_point_2d<Type> const& corner1,
                             vgl_point_2d<Type> const& corner2)
{
  min_pos_[0]=max_pos_[0]=corner1.x();
  min_pos_[1]=max_pos_[1]=corner1.y();
  this->add(corner2);
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax)
{
  min_pos_[0]=max_pos_[0]=xmin;
  min_pos_[1]=max_pos_[1]=ymin;
  this->add(vgl_point_2d<Type>(xmax,ymax));
  if (xmin > xmax || ymin > ymax) this->empty();
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type const ref_point[2],
                             Type w, Type h,
                             typename vgl_box_2d<Type>::point_type t)
{
  if (t == vgl_box_2d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point[0]-0.5*w);
    min_pos_[1]=Type(ref_point[1]-0.5*h);
    max_pos_[0]=Type(ref_point[0]+0.5*w);
    max_pos_[1]=Type(ref_point[1]+0.5*h);
  }
  else if (t == vgl_box_2d<Type>::min_pos)
  {
    min_pos_[0]=ref_point[0];
    min_pos_[1]=ref_point[1];
    max_pos_[0]=ref_point[0]+w;
    max_pos_[1]=ref_point[1]+h;
  }
  else if (t == vgl_box_2d<Type>::max_pos)
  {
    min_pos_[0]=ref_point[0]-w;
    min_pos_[1]=ref_point[1]-h;
    max_pos_[0]=ref_point[0];
    max_pos_[1]=ref_point[1];
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(vgl_point_2d<Type> const& ref_point,
                             Type w, Type h,
                             typename vgl_box_2d<Type>::point_type t)
{
  if (t == vgl_box_2d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point.x()-0.5*w);
    min_pos_[1]=Type(ref_point.y()-0.5*h);
    max_pos_[0]=Type(ref_point.x()+0.5*w);
    max_pos_[1]=Type(ref_point.y()+0.5*h);
  }
  else if (t == vgl_box_2d<Type>::min_pos)
  {
    min_pos_[0]=ref_point.x();
    min_pos_[1]=ref_point.y();
    max_pos_[0]=ref_point.x()+w;
    max_pos_[1]=ref_point.y()+h;
  }
  else if (t == vgl_box_2d<Type>::max_pos)
  {
    min_pos_[0]=ref_point.x()-w;
    min_pos_[1]=ref_point.y()-h;
    max_pos_[0]=ref_point.x();
    max_pos_[1]=ref_point.y();
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
Type vgl_box_2d<Type>::centroid_x() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[0] + max_pos_[0]));
}

template <class Type>
Type vgl_box_2d<Type>::centroid_y() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[1] + max_pos_[1]));
}

template <class Type>
Type vgl_box_2d<Type>::width() const
{
  return (max_pos_[0] > min_pos_[0]) ? max_pos_[0] - min_pos_[0] : 0;
}

template <class Type>
Type vgl_box_2d<Type>::height() const
{
  return (max_pos_[1] > min_pos_[1]) ? max_pos_[1] - min_pos_[1] : 0;
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::min_point() const
{
  assert(!is_empty());
  return vgl_point_2d<Type>(min_pos_[0],min_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::max_point() const
{
  assert(!is_empty());
  return vgl_point_2d<Type>(max_pos_[0],max_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::centroid() const
{
  assert(!is_empty());
  return vgl_point_2d<Type>(centroid_x(),centroid_y());
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_x(Type cent_x)
{
  assert(!is_empty());
  Type delta = cent_x - centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_y(Type cent_y)
{
  assert(!is_empty());
  Type delta = cent_y - centroid_y();
  min_pos_[1]= min_pos_[1] + delta;
  max_pos_[1]= max_pos_[1] + delta;
}

template <class T>
inline void set_dim_2d(T & minv, T& maxv, T spread);

// All this code is to avoid drift in the centroid.
template <>
inline void set_dim_2d(int & minv, int& maxv, int spread)
{
  int sum = minv + maxv;
  sum = sum | (spread&1); // if width is odd, then make sum odd
  minv = int(std::floor((sum-spread)/2.0));
  maxv = minv+spread;
}

template <class T>
inline void set_dim_2d(T & minv, T& maxv, T spread)
{
  T x = minv + maxv;
  minv = T( (x-spread)*0.5 );
  maxv = minv + spread;
}

//: Modify width, retaining centroid at current position
// For integer types, centroid might change slightly, but
// repeat calls to set_height will not cause centroid drift.
template <class Type>
void vgl_box_2d<Type>::set_width(Type w)
{
  assert(!is_empty());
  set_dim_2d(min_pos_[0], max_pos_[0], w);
}

//: Modify height, retaining centroid at current position
// For integer types, centroid might change slightly, but
// repeat calls to set_height will not cause centroid drift.
template <class Type>
void vgl_box_2d<Type>::set_height(Type h)
{
  assert(!is_empty());
  set_dim_2d(min_pos_[1], max_pos_[1], h);
}


//: Add to width and height, centroid unchanged.
// Will move each side by \p expand / 2.
template <class Type>
void vgl_box_2d<Type>::expand_about_centroid(Type expand)
{
  assert(!is_empty());
  set_dim_2d(min_pos_[0], max_pos_[0], width() + expand );
  set_dim_2d(min_pos_[1], max_pos_[1], height() + expand );
}

//: Scale width and height, centroid unchanged.
template <class Type>
void vgl_box_2d<Type>::scale_about_centroid(double s)
{
  assert(!is_empty());
  set_dim_2d(min_pos_[0], max_pos_[0], static_cast<Type>(width()*s));
  set_dim_2d(min_pos_[1], max_pos_[1], static_cast<Type>(height()*s));
}


//: Scale width and height, keeping scaled position of origin unchanged.
template <class Type>
void vgl_box_2d<Type>::scale_about_origin(double s)
{
  min_pos_[0] = static_cast<Type>(min_pos_[0] * s);
  min_pos_[1] = static_cast<Type>(min_pos_[1] * s);
  max_pos_[0] = static_cast<Type>(max_pos_[0] * s);
  max_pos_[1] = static_cast<Type>(max_pos_[1] * s);
}

template <class Type>
void vgl_box_2d<Type>::setmin_position(Type const min_position[2])
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  if (max_pos_[0] < min_pos_[0]) {
    max_pos_[0]=min_pos_[0];
  }
  if (max_pos_[1] < min_pos_[1]) {
    max_pos_[1]=min_pos_[1];
  }
}

template <class Type>
void vgl_box_2d<Type>::setmax_position(Type const max_position[2])
{
  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
  if (max_pos_[0] < min_pos_[0])
    min_pos_[0]=max_pos_[0];
  if (max_pos_[1] < min_pos_[1])
    min_pos_[1]=max_pos_[1];
}

template <class Type>
void vgl_box_2d<Type>::set_min_point(vgl_point_2d<Type> const& min_pt)
{
  min_pos_[0]=min_pt.x(); if (max_pos_[0]<min_pos_[0]) max_pos_[0]=min_pos_[0];
  min_pos_[1]=min_pt.y(); if (max_pos_[1]<min_pos_[1]) max_pos_[1]=min_pos_[1];
}

template <class Type>
void vgl_box_2d<Type>::set_max_point(vgl_point_2d<Type> const& max_pt)
{
  max_pos_[0]=max_pt.x(); if (max_pos_[0]<min_pos_[0]) min_pos_[0]=max_pos_[0];
  max_pos_[1]=max_pt.y(); if (max_pos_[1]<min_pos_[1]) min_pos_[1]=max_pos_[1];
}

template <class Type>
std::ostream& vgl_box_2d<Type>::print(std::ostream& s) const
{
  if (is_empty())
    return s << "<vgl_box_2d (empty)>";
  else
    return s << "<vgl_box_2d "
             << min_pos_[0] << ',' << min_pos_[1] << " to "
             << max_pos_[0] << ',' << max_pos_[1] << '>';
}

template <class Type>
std::ostream& vgl_box_2d<Type>::write(std::ostream& s) const
{
  return s << min_pos_[0] << ' ' << min_pos_[1] << ' '
           << max_pos_[0] << ' ' << max_pos_[1] << '\n';
}

template <class Type>
std::istream& vgl_box_2d<Type>::read(std::istream& s)
{
  return s >> min_pos_[0] >> min_pos_[1]
           >> max_pos_[0] >> max_pos_[1];
}

//: Add a point to this box.
// Do this by possibly enlarging the box so that the point just falls within the box.
// Adding a point to an empty box makes it a size zero box only containing p.
template <class Type>
void vgl_box_2d<Type>::add(vgl_point_2d<Type> const& p)
{
  if (is_empty())
  {
    min_pos_[0] = max_pos_[0] = p.x();
    min_pos_[1] = max_pos_[1] = p.y();
  }
  else
  {
    if (p.x() > max_pos_[0]) max_pos_[0] = p.x();
    if (p.x() < min_pos_[0]) min_pos_[0] = p.x();
    if (p.y() > max_pos_[1]) max_pos_[1] = p.y();
    if (p.y() < min_pos_[1]) min_pos_[1] = p.y();
  }
}

//: Make the convex union of two boxes
// Do this by possibly enlarging this box so that the corner points of the
// given box just fall within the box.
// Adding an empty box does not change the current box.
template <class Type>
void vgl_box_2d<Type>::add(vgl_box_2d<Type> const& b)
{
  if (b.is_empty()) return;
  add(b.min_point());
  add(b.max_point());
}

//: Return true iff the point p is inside this box
template <class Type>
bool vgl_box_2d<Type>::contains(vgl_point_2d<Type> const& p) const
{
  return contains(p.x(), p.y());
}

//: Return true iff the corner points of b are inside this box
template <class Type>
bool vgl_box_2d<Type>::contains(vgl_box_2d<Type> const& b) const
{
  return
    contains(b.min_x(), b.min_y()) &&
    contains(b.max_x(), b.max_y());
}

//: Make the box empty
template <class Type>
void vgl_box_2d<Type>::empty()
{
  min_pos_[0]=min_pos_[1]=(Type)1;
  max_pos_[0]=max_pos_[1]=(Type)0;
}

//: Print to stream
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_box_2d<Type> const& p)
{
  return p.print(s);
}

//: Read from stream
template <class Type>
std::istream& operator>>(std::istream& is, vgl_box_2d<Type>& p)
{
  return p.read(is);
}

#undef VGL_BOX_2D_INSTANTIATE
#define VGL_BOX_2D_INSTANTIATE(Type) \
template class vgl_box_2d<Type >;\
template std::istream& operator>>(std::istream&, vgl_box_2d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_box_2d<Type > const&)

#endif // vgl_box_2d_hxx_
